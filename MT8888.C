#include <reg52.h>
#include <absacc.h>
#include <string.h>
#include <intrins.h>
#include "inc/types.h"
#include "inc/hdres.h"
#include "inc/sst_spi.h"
#include "inc/key.h"
#include "inc/timer.h"
#include "inc/tool.h"
#include "inc/MT8888.h"
#include "inc/alarm.h"
#include "inc/isd4000.h"
#include "inc/lcd.h"
#include "inc/menu.h"

unsigned char xdata mt8888_status;

void  init_mt8888(void)
{
      unsigned char tmp;
      unsigned int i;
      for(i=0;i<10000;i++);            //上电后必须延迟100ms 

      tmp = XBYTE[MT8888_RS0_ADDR];    //用读的方法清空状态寄存器
      XBYTE[MT8888_RS0_ADDR] = 0x00;   //CRA=0000
      XBYTE[MT8888_RS0_ADDR] = 0x00;   
      XBYTE[MT8888_RS0_ADDR] = 0x08;   
      XBYTE[MT8888_RS0_ADDR] = 0x00;   //CRB=0000
      tmp = XBYTE[MT8888_RS0_ADDR];    //用读的方法清空状态寄存器

      XBYTE[MT8888_RS0_ADDR] = 0x0C;   //设置：DTMF、中断、下一次访问CRB
      XBYTE[MT8888_RS0_ADDR] = 0x00;   //CRB=0000
      /**************启动INT3中断,MT8888*************/ 
//      XICON |= 0x50;  //EX3=1,IT3=0
      EX0 = 1;                  //开中断   INT0
}

unsigned char xdata curr_dtmf_send_ptr;
unsigned char xdata send_dtmf_buf[PHONE_NO_LEN];
unsigned char xdata rece_dtmf_ptr;
#define DTMF_RECE_BUF_LEN    0x0F
unsigned char xdata rece_dtmf_buf[DTMF_RECE_BUF_LEN];

//unsigned char code dtmf_code[]={10,1,2,3,4,5,6,7,8,9};
void INT_MT8888(void) interrupt  0    //INT0  外部中断0   //8    //for SST INT3
{
      unsigned char idata tmp,dat; 
      unsigned int i;

      tmp =  XBYTE[MT8888_RS0_ADDR]; //读中断状态寄存器
      if(tmp & 0x02){                //发送数据
         BUZZER_ON;
         for(i=0;i<10000;i++); 
         BUZZER_OFF;
         tmp = send_dtmf_buf[curr_dtmf_send_ptr++];
         if(tmp != 0){
            if(tmp==0x30) tmp=0x3A;   //0
            XBYTE[MT8888_ADDR] = tmp-0x30;
         }
         else
            curr_dtmf_send_ptr=0;
         return;      //发送时不接收，可能是收到自己的数据
      }
      if(curr_dtmf_send_ptr==0 && (tmp & 0x04)){ //发送状态，不接收；接收数据
         dat = XBYTE[MT8888_ADDR] & 0x0F;
         if(dat==10)dat = 0;
         if(dat==NUM_JING){       //全局结束,检测到一个#号
            stop_phone_alm();
            key_in(dat);
         } 
         else  
            rece_dtmf_buf[rece_dtmf_ptr++]=dat;
         rece_dtmf_ptr &= DTMF_RECE_BUF_LEN;
         buzzer_start(1,1);
      }  
}

#if 0
void send_dtmf(void)
{
      EX0 = 0;                  //开中断   INT0
      curr_dtmf_send_ptr=0;
      buzzer_start(1,1);
      PHONE_CTRL_ON;      //电话摘机
      delay(50000);
      delay(50000);
      XBYTE[MT8888_RS0_ADDR] = 0x0D;   //CRA=0000
      XBYTE[MT8888_RS0_ADDR] = 0x00;   //写控制寄存器A,中断EN
      XBYTE[MT8888_ADDR]=0x0B;
      delay(50000);
      XBYTE[MT8888_ADDR]=0x02;
      delay(50000);
      XBYTE[MT8888_ADDR]=0x08;
      delay(50000);
      delay(50000);
      PHONE_CTRL_OFF;      //电话摘机
      EX0 = 1;                  //开中断   INT0
}
#endif


static unsigned char code vol_seg[]={VOICE_ADDR_ALM_DOOR_0,
                                     VOICE_ADDR_ALM_YANGAN_0,
                                     VOICE_ADDR_ALM_MEIQI_0,
                                     VOICE_ADDR_ALM_WINDOW_0,
				     VOICE_ADDR_ALM_HELP_0,
				     VOICE_ADDR_ALM_HONGWAI_0,
				     VOICE_ADDR_ALM_OUTSIDE_0,
				     VOICE_ADDR_ALM_UNKNOWN_0
                                    };

unsigned char get_alm_voice(void)
{

     unsigned char type = __alm.alm_device;

     if(type >= ALM_DOOR_TYPE && type <=ALM_MAX_TYPE){
        type -= ALM_DOOR_TYPE;             //语音从门磁开始
        return vol_seg[type];         //查找地址

     }
     else{  //没有语音文件
        return VOICE_ADDR_ALM_UNKNOWN_0; //未知报警
     }
} 

//拨出的电话序号
extern void show_phone(unsigned char rol, unsigned char col, unsigned char xdata *p);
void alm_to_phone(unsigned char no)
{   
     unsigned char tmp;
     unsigned char xdata *xp;
     extern unsigned char xdata curr_dtmf_send_ptr;

     xp = other_item.phone_id0+PHONE_NO_LEN*no;
     buzzer_start(1,1);
     XBYTE[MT8888_RS0_ADDR] = 0x0D;   //CRA=0000
     XBYTE[MT8888_RS0_ADDR] = 0x00;   //写控制寄存器A,中断EN
     memcpy(send_dtmf_buf,xp,PHONE_NO_LEN);  

     crt_outnull_num(13,SPACE_COL+_WORD_WIDE*5,PHONE_NO_LEN);
     crt_outhz(13,SPACE_COL, "正在拨号..",10);      
     show_phone(13,SPACE_COL+_WORD_WIDE*5, send_dtmf_buf);

     tmp = xp[0];
     if(tmp){
//        if(tmp==0x39) tmp=NUM_XING+0x30; //9
        XBYTE[MT8888_ADDR] = tmp-0x30;
     }
     curr_dtmf_send_ptr=1;
}




unsigned char xdata phone_alm_status;
unsigned char idata phone_alm_timerout;
unsigned char xdata phone_total_times;
unsigned char xdata alm_phone_no;  //拨打的电话号码顺序
unsigned char xdata loop_play_times;

unsigned char xdata rece_dtmf_timeout;
unsigned char xdata dev_type,dev_sn,curr_counter;
extern bit __return_ok,ring_bit_ok;

void phone_alm_task(void)          //报警拨号播放语音任务状态
{
//      unsigned char tmp;
      unsigned char volatile tmp_p4 = P4;
      unsigned char op;

      if(phone_alm_timerout)
         return;  
      if(phone_alm_status >= PHONE_CALL_IN){
         if(rece_dtmf_timeout++ > WAIT_DTMF_IN_TIMEOUT){  //等待输入超时
            rece_dtmf_timeout=0;
            stop_phone_alm();
         }  
      }
      switch(phone_alm_status){
          case PHONE_IDLE:
               alm_phone_no=0;     
               rece_dtmf_ptr=0;
               if(_testbit_(ring_bit_ok)){
                  if(curr_counter++ > other_item.hangup_times){
                     curr_counter=0;
                     PHONE_CTRL_ON;           //电话摘机
                     phone_alm_timerout=2;
                     rece_dtmf_timeout=0;
                     phone_alm_status = PHONE_CALL_IN;
                  }
               }   
               break;
          case PHONE_HUANG_UP:              
               if(phone_total_times++ < 100){      //最多次数              
                  PHONE_CTRL_ON;                   //电话摘机  
                  phone_alm_timerout=2;
                  phone_alm_status = PHONE_DIAL;
               }   
               else{
		  stop_phone_alm();
               } 
               break;
          case PHONE_DIAL:
               alm_to_phone(alm_phone_no++);
               if(alm_phone_no>0x02)alm_phone_no=0; //电话号码位置
               phone_alm_timerout=5;                //拨号时间+播放声音时间
               phone_alm_status = PHONE_PLAY_VOICE;  
               loop_play_times=0;
               break;
          case PHONE_PLAY_VOICE:
               isd4000_cmd(get_alm_voice());        //播放"XX报警"
               phone_alm_timerout=3;
               if(loop_play_times++>4)
                  phone_alm_status = PHONE_PLAY_END;
               else
                  phone_alm_status = PHONE_PLAY_VOICE;  //报警声音重复5次
               break;
          case PHONE_PLAY_END:
               PHONE_CTRL_OFF;           
               phone_alm_timerout=2;
               phone_alm_status = PHONE_HUANG_UP_WAIT;
               break;
          case PHONE_HUANG_UP_WAIT:
               PHONE_CTRL_OFF;           
               phone_alm_status = PHONE_HUANG_UP;
               break;

//呼入状态转换
          case PHONE_CALL_IN:
   	       isd4000_cmd(0);      //播放欢迎词
               phone_alm_status = PHONE_CALL_WAIT_PIN;
               break;
          case PHONE_CALL_WAIT_PIN:
               if(rece_dtmf_ptr >= 4){
                  if(memcmp(rece_dtmf_buf,"\x1\x2\x3\x4",PHONE_PIN_LEN)==0){
                     phone_alm_status = PHONE_CALL_PLAY_MAIN_PAGE;
   	             rece_dtmf_timeout=0;
                  }
                  else{
                     stop_phone_alm();
                  } 
               } 
               break;  
          case PHONE_CALL_PLAY_MAIN_PAGE:
               isd4000_cmd(VOICE_ADDR_CTRL_0);                                          
               phone_alm_status = PHONE_CALL_WAIT_TYPE;
	       rece_dtmf_timeout=0;
               break; 
          case PHONE_CALL_WAIT_TYPE:
               if(rece_dtmf_ptr >= 5){
                  dev_type = rece_dtmf_buf[4];
//                  crt_outnull_num(13,SPACE_COL,2);
//                  crt_out_hex(13,SPACE_COL,dev_type);                  
                  if(dev_type==9){  //全开
                     op = 0x81; 
                     goto ALL_OP;  
                  } 
                  if(dev_type==0){  //全关
                      op = 0x80; 
                      goto ALL_OP;  
                  } 
 	          rece_dtmf_timeout=0;
                  isd4000_cmd(VOICE_ADDR_ROOM_NAME_0);//播放"房间名称"
                  phone_alm_status = PHONE_CALL_WAIT_SN;
               }
               break;
          case PHONE_CALL_WAIT_SN:  
               if(rece_dtmf_ptr >= 6){
                  dev_sn = rece_dtmf_buf[5]; //设备序号
                  isd4000_cmd(VOICE_ADDR_ONOFF_0);        //播放"开关操作"
                  phone_alm_status = PHONE_CALL_WAIT_OP;
 	          rece_dtmf_timeout=0;
               }
               break;
          case PHONE_CALL_WAIT_OP:  
               if(rece_dtmf_ptr >= 7){
                  op = rece_dtmf_buf[6];
ALL_OP:  
		  __return_ok=0;
                  phone_dev_ctrl(dev_type,dev_sn,op);
                  isd4000_cmd(VOICE_ADDR_WAITING_0);    //播放"请稍后"
	          rece_dtmf_timeout=0;
                  phone_alm_status= PHONE_CALL_SEND_CMD;
               }
               break;
          case PHONE_CALL_SEND_CMD:                              
               if(__return_ok){ 
                  //isd4000_cmd(VOICE_ADDR_OP_OK_0,VOICE_ADDR_OP_OK_1,ISD_SET_PLAY_CMD);     //播放"开关操作" 
                  op = VOICE_ADDR_OP_OK_0;
                  goto END; 
               }
               else{  
                  phone_dev_ctrl(dev_type,dev_sn,op);
               }     
               if(rece_dtmf_timeout > 4){
                  op = VOICE_ADDR_OP_FAIL_0; //播放"操作失败"
END:
                  isd4000_cmd(op); 
  	          rece_dtmf_timeout=0;
		  phone_alm_timerout=3;      //3秒后播主菜单
                  rece_dtmf_ptr = 4;
		  phone_alm_status = PHONE_CALL_PLAY_MAIN_PAGE;  //重复主菜单
               }
               break;
          default:
               stop_phone_alm();
               break;  
      }
}                

void enter_phone_alm(void)
{
     phone_alm_status = PHONE_HUANG_UP;
     phone_total_times=0;
}

void stop_phone_alm(void)
{
     PHONE_CTRL_OFF;           
     phone_alm_status = PHONE_IDLE;  //停止电话拨号
}

