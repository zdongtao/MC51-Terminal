/*****************************************************************/
/*                 Òì²½Í¨Ñ¶ÖÐ¶Ï´¦ÀíÄ£¿é                            */
/*        		                                         */
/* ¹¦ÄÜ: Òì²½ÖÐ¶Ï×´Ì¬×ª»»                                          */     
/* ÈÕÆÚ: 2001/07/03				                 */
/* ×÷Õß: ¶­ÌÎ                                                     */
/*                                                               */
/* (c) Copyright reserved by JDKJ, 2001.                         */
/*---------------------------------------------------------------*/
/* ÐÞ¸Ä¼ÇÂ¼:		                                         */
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
     TH1 = BAUD_RATE;   // ÉèÖÃÒì²½¿Ú²¨ÌØÂÉ
     TMOD = 0x21;       // timer 1 mode 2: 8-Bit reload          */
     SCON = 0xD0;       // 11010000 
     PCON = 0x80;       // ³õÊ¼»¯PCON¼Ä´æÆ÷£¬SMOD=1
//     PS = 1;          // ÉèÖÃÒì²½¿ÚÖÐ¶ÏÓÅÏÈ¼¶Îª¸ß
     TR1 = 1;           // Æð¶¯1ºÅ¶¨Ê±Æ÷¡
     RECE_485;
}


static void rec_ready(void)     //½ÓÊÕ×¼±¸
{
     RECE_485;
     asyn_count = 0;
     asyn_len = 10;             //ÏÈ½ÓÊÕ10¸öÊý¾Ý  
     asyn_comm_status = RECEIVE_DATA;
}
    

static void asyn_comm_end(void) 
{
     ES = 0;
     asyn_timeout = 2;     //Òì²½·¢ËÍºóÑÓ³Ù>10ms£¬ÔÙ¶Ô¶ÔRIUÂÖÑ²½ÓÊÕ
     asyn_comm_status = ASYN_IDLE;
     RECE_485;	
}

void asyn_handle1(void) interrupt 4  using 2   //²»Ê¹ÓÃ¼Ä´æÆ÷×é£¬°ÑÒªÊ¹ÓÃµÄR0-R7Ñ¹Õ»£¬¶àÒ»Ð©¶ÑÕ»¿Õ¼ä£¬£¨×ÊÔ´½ôÕÅ£©
{

       switch(asyn_comm_status){
       case SEND_ADDR:
            if(TI){
               if(riu_addr >= MULTICAST){   //·¢ËÍ¹ã²¥±¨ÎÄ
                  asyn_comm_status = SEND_DATA;  
               } 
               else {      ////·¢ËÍÖ¸¶¨±¨ÎÄ
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
               if(SBUF == riu_addr){       //µØÖ·Ïà·û
                  SEND_485;
                  if(asyn_send_bit)        //·¢ËÍÊý¾Ý
                     SBUF = ASYN_SEND;     //´Ó»ú½ÓÊÕ,Ö÷»ú·¢ËÍÊý¾Ý
                  else                        
                     SBUF = ASYN_RECE;     //ÂÖÑ¯RIU£¬NIU ÊÇ·ñÓÐÊý¾Ý·¢ËÍ
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
                   case RX_RDY:      //´Ó»ú½ÓÊÕ¾ÍÐ÷ 
                        SEND_485;     
                        TI = 1;      //Æô¶¯·¢ËÍ
                        asyn_timeout = ASYN_WAIT_TIMEOUT;
                        asyn_comm_status = SEND_DATA;  
                        break;
//                   case MULTICAST:
//                      TB8 = 0;
                   case TX_RDY:      //´Ó»ú·¢ËÍ¾ÍÐ÷
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
                if(asyn_count < asyn_len){      //Êý¾Ý+Ð£ÑéºÍ
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
                  asyn_len = *((unsigned int *)(asyn.rece_buf+1));  //È¡Êý¾Ý±¨³¤¶È     
                  asyn_len += 2; //chk_sum      
	          if(asyn_len > ASYN_RECE_MAX_BUF){          //³¬¹ý³¤¶È£¬²»½ÓÊÕ
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
/*                 rec_check_sum ±¨ÎÄÐ£ÑéºÍ                               */
/* ²ÎÊý£º*pBuf¡ª¡ªµ±Ç°Êý¾Ý»º³åÇøÖ¸Õë                                      */   
/*       len¡ª¡ªÊý¾Ý³¤¶È                                                  */
/* ·µ»Ø: 1¡ª¡ª³É¹¦,0¡ª¡ªÊ§°Ü                                              */     
/* ÈÕÆÚ£º2001/04/10                                                       */      
/*------------------------------------------------------------------------*/
/* ÐÞ¸Ä¼ÇÂ¼:		                                                  */
/*    							                  */   
/**************************************************************************/
bit rec_check_sum(void)
{
   unsigned int i;
   unsigned int chk_sum1,chk_sum2;
   
   chk_sum1 = *(unsigned int *)(asyn.rece_buf + asyn.rece_len);  //»º³åÇøºóÁ½¸ö×Ö½ÚÎªÐ£ÑéºÍ
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

    if(_testbit_(asyn_bufr_bit)){   //Òì²½Í¨Ñ¶½ÓÊÕµ½Êý¾Ý
       asyn.rece_len -= 2; 
       if(rec_check_sum()){
	  if((flag=asyn.rece_buf[6]) & SEND_TO_CCC)   //ÓÅÏÈ·¢ËÍµ½CCC,SCU²»´¦Àí
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
          riu_addr=next_riu_addr=asyn.send_buf[5]; //ÏÂ´ÎÂÖÑ²µ±Ç°RIU
       }
       else{
          if(asyn_timeout)        //ÂÖÑ²¼ä¸ô¼¸Ê®¸öºÁÃë
             return;   
          asyn_send_bit = 0;
          riu_addr = next_riu_addr;
          if(++next_riu_addr > 0x1F){
             next_riu_addr = 1;        
             err_send_bit = 1;        //Ò»¸öÑ­»·ÖÃÎ»
          }       
          times = riu_link_counter[riu_addr];
          if(times >= 5){
             if(err_send_bit && (times>0x1f+5)){  //RIU²»´æÔÚ
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
       SBUF = riu_addr;    //·¢ËÍ´Ó»úµØÖ·                 
       ES = 1;           
    } 
    else{      //³¬Ê±´¦Àí 
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
