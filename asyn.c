/*****************************************************************/
/*                 异步通讯中断处理模块                            */
/*        		                                         */
/* 功能: 异步中断状态转换                                          */     
/* 日期: 2001/07/03				                 */
/* 作者: 董涛                                                     */
/*                                                               */
/* (c) Copyright reserved by JDKJ, 2001.                         */
/*---------------------------------------------------------------*/
/* 修改记录:		                                         */
/*    					                         */
/*****************************************************************/
#include <reg52.h>
#include <intrins.h>
#include <string.h>
#include "inc/types.h"
#include "inc/asyn.h"
#include "inc/timer.h"
#include "inc/system.h"
#include "inc/ctrl.h"
#include "inc/hdres.h"
#include "inc/app.h"

unsigned char data asyn_timeout;  
bit  asyn_send;
unsigned char idata asyn_comm_status;
unsigned int  idata asyn_count,asyn_len;
bit asyn_send_bit;
static bit asyn_bufr_bit;
static unsigned char data riu_addr;

ASYN_BUF xdata asyn;
unsigned char xdata riu_status[0x100];
unsigned char xdata riu_link_counter[0x100];

#if __DEBUG
    TEST_COUNTER xdata test_counter;
#endif

void asyn_init(void) 
{
     TH1 = BAUD_RATE;   // 设置异步口波特律
     TMOD = 0x21;       // timer 1 mode 2: 8-Bit reload          */
     SCON = 0xD0;       // 11010000 
     PCON = 0x80;       // 初始化PCON寄存器，SMOD=1
//     PS = 1;          // 设置异步口中断优先级为高
     TR1 = 1;           // 起动1号定时器�
     RECE_485;
}


static void rec_ready(void)     //接收准备
{
     RECE_485;
     asyn_count = 0;
     asyn_len = 10;             //先接收10个数据  
     asyn_comm_status = RECEIVE_DATA;
}
    

static void asyn_comm_end(void) 
{
     ES = 0;
     asyn_timeout = 2;     //异步发送后延迟>10ms，再对对RIU轮巡接收
     asyn_comm_status = ASYN_IDLE;
     RECE_485;	
}

void asyn_handle1(void) interrupt 4  using 2   //不使用寄存器组，把要使用的R0-R7压栈，多一些堆栈空间，（资源紧张）
{

       switch(asyn_comm_status){
       case SEND_ADDR:
            if(TI){
               if(riu_addr >= MULTICAST){   //发送广播报文
                  asyn_comm_status = SEND_DATA;  
               } 
               else {      ////发送指定报文
                  RECE_485;
                  TI = 0;
                  asyn_timeout = ASYN_WAIT_TIMEOUT;
                  asyn_comm_status = WAIT_ANSWER;
               } 
            }    
            RI=0;
            TB8=0;      
            break;     
       case WAIT_ANSWER:
            TI = 0;    
            if(_testbit_(RI)){
               if(SBUF == riu_addr){       //地址相符
                  SEND_485;
                  if(asyn_send_bit)        //发送数据
                     SBUF = ASYN_SEND;     //从机接收,主机发送数据
                  else                        
                     SBUF = ASYN_RECE;     //轮询RIU，NIU 是否有数据发送
                  asyn_timeout = ASYN_WAIT_TIMEOUT;
                  asyn_comm_status = SEND_CMD_END;
                  riu_status[riu_addr] = 0;
                  riu_link_counter[riu_addr]=0;
               }
            }    
            break;
       case SEND_CMD_END:
            if(_testbit_(TI)){
               RECE_485;
               asyn_timeout = ASYN_WAIT_TIMEOUT;
               asyn_comm_status = WAIT_STATUS;
            }   
            RI = 0;  
            break;   
       case WAIT_STATUS:
            TI = 0;
            if(_testbit_(RI)){
               switch(SBUF){
                   case RX_RDY:      //从机接收就绪 
                        SEND_485;     
                        TI = 1;      //启动发送
                        asyn_timeout = ASYN_WAIT_TIMEOUT;
                        asyn_comm_status = SEND_DATA;  
                        break;
//                   case MULTICAST:
//                      TB8 = 0;
                   case TX_RDY:      //从机发送就绪
                        rec_ready();
                        break;
                   case NO_RDY:      
                   default:    
                        asyn_comm_end();
               }
            }
            break;                
        case SEND_DATA:
            RI = 0;
            if(_testbit_(TI)){
                if(asyn_count < asyn_len){      //数据+校验和
                    SBUF = asyn.send_buf[asyn_count++];		
                    asyn_timeout = ASYN_WAIT_TIMEOUT;
                }else{ 
                    #if __DEBUG      
                      test_counter.send_count++;
                    #endif      
		    asyn_comm_end();
             	}
            }
            break;
        case RECEIVE_DATA:
            TI = 0;
            if(_testbit_(RI)){
               asyn.rece_buf[asyn_count] = SBUF;
               if(asyn_count == 5){
                  asyn_len = *((unsigned int *)(asyn.rece_buf+1));  //取数据报长度     
                  asyn_len += 2; //chk_sum      
	          if(asyn_len > ASYN_RECE_MAX_BUF){          //超过长度，不接收
	             asyn_comm_end();
                     asyn_bufr_bit = 0;
	             return;
	          }	
	       }                	
               asyn_timeout = ASYN_WAIT_TIMEOUT;
               if(++asyn_count >= asyn_len){
                  asyn.rece_len = asyn_len;
                  asyn_bufr_bit = 1;
                  #if __DEBUG      
                      test_counter.rece_count++;
                  #endif      
                  asyn_comm_end();
               }                 
            }  
            break;
        default:
            TI=0;    
            RI=0;
	    asyn_comm_end(); 
    }
}
/**************************************************************************/
/*                 rec_check_sum 报文校验和                               */
/* 参数：*pBuf——当前数据缓冲区指针                                      */   
/*       len——数据长度                                                  */
/* 返回: 1——成功,0——失败                                              */     
/* 日期：2001/04/10                                                       */      
/*------------------------------------------------------------------------*/
/* 修改记录:		                                                  */
/*    							                  */   
/**************************************************************************/
bit rec_check_sum(void)
{
   unsigned int i;
   unsigned int chk_sum1,chk_sum2;
   
   chk_sum1 = *(unsigned int *)(asyn.rece_buf + asyn.rece_len);  //缓冲区后两个字节为校验和
   chk_sum2 = 0;
   for(i=0;i<asyn.rece_len;i++){    
       chk_sum2 += asyn.rece_buf[i];
   }
   if(chk_sum1 == chk_sum2)
      return 1;     //ok
   return 0;      
}   

void asyn_controler(void)
{
    static unsigned char idata next_riu_addr=1;
    unsigned char times,flag;    
    static bit err_send_bit;

    if(_testbit_(asyn_bufr_bit)){   //异步通讯接收到数据
       asyn.rece_len -= 2; 
       if(rec_check_sum()){
	  if((flag=asyn.rece_buf[6]) & SEND_TO_CCC)   //优先发送到CCC,SCU不处理
             send_to_ccc(flag,asyn.rece_buf,asyn.rece_len);  //RIU->CCC   	
          else 
             app_process_received(asyn.rece_buf);  //RIU->NDT
       }  
       return; 	
    }        

    if(asyn_comm_status == ASYN_IDLE){
       if(_testbit_(asyn_send)){ 
          asyn_send_bit = 1;
          asyn_count = 0;
          asyn_len = asyn.send_len; 
          riu_addr=next_riu_addr=asyn.send_buf[5]; //下次轮巡当前RIU
       }
       else{
          if(asyn_timeout)        //轮巡间隔几十个毫秒
             return;   
          asyn_send_bit = 0;
          riu_addr = next_riu_addr;
          if(++next_riu_addr > 0x1F){
             next_riu_addr = 1;        
             err_send_bit = 1;        //一个循环置位
          }       
          times = riu_link_counter[riu_addr];
          if(times >= 5){
             if(err_send_bit && (times>0x1f+5)){  //RIU不存在
                err_send_bit=0; 
                riu_link_counter[riu_addr] = 5;
             }   
             else{ 
                riu_link_counter[riu_addr]++;
                asyn_timeout = ASYN_WAIT_TIMEOUT;
                return; 
             }     
          }                      
       } 
       SEND_485;               
       asyn_comm_status = SEND_ADDR;
       asyn_timeout = ASYN_WAIT_TIMEOUT;
       TB8= 1;
       SBUF = riu_addr;    //发送从机地址                 
       ES = 1;           
    } 
    else{      //超时处理 
       if(asyn_timeout==0){
          ES  = 0;   
          TH1 = BAUD_RATE;     
          SCON= 0xD0;  
          TR1 = 1;           
          asyn_comm_status = ASYN_IDLE;
	  if(riu_addr < _RIU_NUM){                  //2002.10.10 ADD
             riu_link_counter[riu_addr]++;
             riu_status[riu_addr] = riu_addr;         
          }
       } 
    }    
}
