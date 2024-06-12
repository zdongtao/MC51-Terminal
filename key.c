#include <reg52.h>
#include <absacc.h>
#include <string.h>
#include <intrins.h>
#include "inc/types.h"
#include "inc/hdres.h"
#include "inc/key.h"
#include "inc/timer.h"
#include "inc/asyn.h"
#include "inc/ctrl.h"
#include "inc/system.h"
#include "inc/menu.h"
#include "inc/lcd.h"
#include "inc/dhcp.h"
#include "inc/skbuff.h"
#include "inc/tool.h"
#include "inc/app.h"
#include "inc/alarm.h"
#include "inc/vi2c.h"
#include "inc/sst_spi.h"
#include "inc/mt8888.h"
#include "inc/isd4000.h"
#include "inc/vf040.h"


unsigned char xdata lcd_display_status;
struct TOHUCH idata touch;
struct IIC_SEND_FIFO xdata iic_send_fifo;
struct IIC_RECE_FIFO xdata iic_rece_fifo;

bit talk_belling_status_bit;   //当前是否呼叫状态
void key_init(void)
{
       unsigned char volatile tmp_p4 = P4;

       SCREEN_SWITCH_CHAR;
       tm[KEYBOARD_TIMEOUT] = KEY_TIMEOUT;   //定时
       LCD_POWER_ON;          //打开LCD电源
       /****************启动INT0中断******************/ 
//       EX0 = 1;
       XICON = 0x55;  //EX3=1,IT3=1;//EX2=1,IT2=1
       /**************启动INT2中断,LPC930*************/ 
       //电话振铃输出P41
//       tmp_p4 &= 0x02; 
//       pre_ring = tmp_p4;    //读I/O初始状态
}

struct KEY_CTRL idata __key_ctrl;
/*
void remote_ctrl()
{
       unsigned char status;
       static unsigned char idata pre;	
       if(((status=XBYTE[REMOTE_CTRL_ADDR])&0x10)){  //MAX 200ms
	  if(pre==0){  //只检测1次,高敏感度
	     pre=0x01;
 	     buzzer_start(20,1);
             switch(status & 0x0F){
               case 0x01:
		   defend_save_and_light(1,0x8F,DEFEND_MODE_REMOTE_CTRL);
		   break;
               case 0x02:
		   defend_save_and_light(1,0x80,DEFEND_MODE_REMOTE_CTRL);
		   break;
	       case 0x04:
		   do_help(_REMOTE_CTROL_ALM);
		   break;  
	       case 0x08:
                   no_alm(0x02);
		   break;
             }
          }
       }
       else
          pre = 0x00;
}
*/
//unsigned char idata pre_bell_status;
//unsigned char xdata belling_count;
bit wait_10ms_bit;
unsigned int xdata timer_monitor;
extern void alm_to_phone(void);
void proc_msg(void)
{  		
        extern unsigned char xdata no_sound;
        extern unsigned char xdata img_status,show_img_end;
        extern bit  __return_ok;
        unsigned char xdata buf[8];
        unsigned char msg_no;
        unsigned long xdata  msg_data;



//        unsigned char tmp;
        
        WATCHDOG;    
        if(timer_monitor++ >5000){      //定时器监控
           while(1);
        }  
        net_handle();
        delay_ctrl();
        asyn_controler();
	show_msg();
        input_ctrl();

#if 1
        if(_testbit_(wait_10ms_bit)){   //10ms执行一次
//           if(iic_send_out(buf)){
//              EA = 0;
//              ISendStrExt(buf,IIC_MSG_LEN+1);
//              EA = 1;
//           } 
           ringcheck();
	}
#endif

        if(_testbit_(wait_second_bit)){ //1秒执行依次
#if 1
           if(iic_send_out(buf)){
              EA = 0;
              ISendStrExt(buf,IIC_MSG_LEN+1);
              EA = 1;
           } 
#endif
           other_task();
           app_other_task();
           dhcp_task();
           retrans_out();
#if LOW_VOL_TET==1
           crt_outnull(13,250,5);
           crt_out_hex(13,250,read_vol_status());
#endif
           phone_alm_task();
//           if(XBYTE[TEAR_ADD]==0){
           if(P12==0){
              //send_dtmf();
//              buzzer_start(1,1);
//              isd4000_cmd(0x0,0x0,ISD_PLAY_CMD);
//              alm_to_phone();
              //send_dtmf();
           }  
//
//           if(ISD_4000_INT==0){
//              MC34119_CD_OFF;
//              isd4000_command(ISD_STOP_POWERDOWN_CMD);
//           }

#if 0
           tmp =  XBYTE[MT8888_RS0_ADDR];   //读中断状态寄存器
           if(tmp & 0x04){   //接收数据
              tmp = XBYTE[MT8888_ADDR] & 0x0F;
              key_in(tmp);
              buzzer_start(1,1);
           }
#endif
        }
#if 1
        if(iic_rece_out(&msg_no,&msg_data)){   //处理键盘消息
          switch(msg_no){   
              case MSG_TEAR:           //外壳被打开
                   __alm.port = _BACKOUT_ALM_PORT;
                   __alm.riu_addr = 0;
//                   buzzer_on(_BIT_ALM);
                   ALM_LIGHT_ON;
//                   alm(1);
                   buf[APP_HEAD_LEN]=0xA5;
   	           buf[APP_HEAD_LEN+1]=0x00;
  	           buf[APP_HEAD_LEN+2]=_BACKOUT_ALM_PORT;   //打开机壳报警
	           buf[APP_HEAD_LEN+3]=0x00;
                   retrans_in(buf,APP_HEAD_LEN+4);
//                   alm_to_phone();    //test
                   key_in(ALM_SHOW_KEY);
                   __alm.alm_device = 34;
                   break;
              case MSG_MONITOR_RDY:    //监视中
              case MSG_TALK_CALL:
                   talk_belling_status_bit = 1;    //置系统呼叫标志
              case MSG_TALK:
                   SCREEN_SWITCH_VIDEO;
   	           tm[KEYBOARD_TIMEOUT] = KEY_TIMEOUT;  //定时
                   LCD_POWER_ON;          //打开LCD电源
                   video_sw(0);           //关闭视频切换
                   break;
              case MSG_DOOR_BUSY:         //门口机忙
                   img_status = 0x01;     //图像浏览忙
                   __return_ok = 1;                                                        
                   break;
              case MSG_TALK_END:          //对讲结束  
                   SCREEN_SWITCH_CHAR;    //显示字符
                   talk_belling_status_bit = 0;
                   if(show_img_end)       //正在显示图片
                      key_in(AUTO_RETURN_KEY); //RETURN TO MAIN MENU
                   break;
              case MSG_SAVE_DATA_OK:        
                   __return_ok = 1;                                                        
                   break;
              case MSG_IC_CARD_NODEFEND:   //刷卡撤防
                   if(other_item.ic_card_no_defend){
                      defend_save_and_light(1,0x80,DEFEND_MODE_REMOTE_CTRL); 
                   }
                   break;
              case MSG_READ_IMG_RETURN:
                   img_status = msg_data; //返回状态
                   __return_ok = 1;                                                        
                   break;       
              case MSG_NEW_IMG_RETURN:    //有新的图像通知            
                   new_img_msg = 1;
                   break;
          }   
       } 
#endif
}

/*
void key_press_in(void) 
{        
	register U_INT_8 tmp = (__key_ctrl.head + 1) & _KEY_FIFO_MASK;
	if(tmp != __key_ctrl.tail){
	   __key_ctrl.dat[__key_ctrl.head] = __key_ctrl.key_code;
	   __key_ctrl.head = tmp;
	} 
}
*/

void key_in(U_INT_8 item) 
{        
	U_INT_8 tmp = (__key_ctrl.head + 1) & _KEY_FIFO_MASK;
	if(tmp != __key_ctrl.tail){
	   __key_ctrl.dat[__key_ctrl.head] = item;
	   __key_ctrl.head = tmp;
	} 
}

bit key_out(U_INT_8 idata *item) 
{
	if(__key_ctrl.tail != __key_ctrl.head){
	   *item = __key_ctrl.dat[__key_ctrl.tail];
	   __key_ctrl.tail = (__key_ctrl.tail + 1) & _KEY_FIFO_MASK;
	   return 1;
	} 
	return 0;
}

void key_free(void)
{
        __key_ctrl.tail = __key_ctrl.head;
}

unsigned char xdata loop_sw_video;
bit pre_fix_key(void)
{
       unsigned char code fix_xy[]={0x20,15, 0x39,10, 0x55,10, 0x73,10, 0x90,10,
                                    0xAC,10, 0xCA,10, 0xE4,15, 0x00,0,  0x00,0 };
       unsigned char code  *pre;  
       unsigned char idata x,i;
       extern bit no_sound_bit;

       pre = fix_xy;                 

       if(touch.y >= FIX_HOT_KEY_ROL)  
          x =touch.y - FIX_HOT_KEY_ROL;            
       else 
          x =FIX_HOT_KEY_ROL - touch.y;                       
       if(x < FIX_HOT_KEY_WID){    
#if __DEBUG
       crt_outnull(13,200,5);
       crt_out_hex(13,200,touch.x);
       crt_out_hex(13,230,touch.y);
#endif     
          for(i=0;i<8;i++){                  
              x = *pre++;              
              if(touch.x >= x)  
                 x =touch.x - x;            
              else 
                 x =x - touch.x;                       
              if(x < *pre++){   //找到
                 buzzer_start(4,1);
                 switch(i){
                     case 0:           //对讲,监视
                         iic_send_in_msgno(MSG_HOST_TALK);
//                         talk_belling_status_bit = 0;    //清对讲标志
                         no_sound_bit = 0;
                         break;       
                     case 1:           //开锁
                         iic_send_in_msgno(MSG_HOST_OPENLK);
                         no_sound_bit = 0;
                         break;       
                     case 2:           //呼叫 
                         iic_send_in_msgno(MSG_HOST_CALL );
                         no_sound_bit = 0;
                         break;       
                     case 3:           //布防
                         defend_save_and_light(1,0x8F,DEFEND_MODE_KEY);
                         break;       
                     case 4:           //撤防，需要密码验证,安全要求
                         if(__menu_layer==MAIN_MENU){
                            key_in(FAST_NO_DEFEND_KEY); 
                         }
                         else{   
                            key_in(AUTO_RETURN_KEY);     
                            key_in(FAST_NO_DEFEND_KEY); 
                         } 
                         break;       
                     case 5:           //报警
                         do_help(_KEYBOARD_ALM_PORT);    //求助
                         break;       
                     case 6:           //消警,需要密码验证,安全要求
                         if(__menu_layer==MAIN_MENU){
                            key_in(FAST_NO_ALM_KEY); 
                         }
                         else{   
                            key_in(CANCL_ALM_RETURN_KEY); 
                            key_in(CANCL_ALM_RETURN_KEY);         
                            key_in(FAST_NO_ALM_KEY); 
                         } 
                 	 //no_alm(0x01);
                         break;            
                     case 7:           //功能，现定义为视频切换
                         if(loop_sw_video++ > 3){
                            loop_sw_video=0;
                            SCREEN_SWITCH_CHAR;
                            video_sw(0);          //关闭
                         }
                         else{
                            SCREEN_SWITCH_VIDEO; //顺序切换
                            video_sw(1);        
                         }   
                         LCD_POWER_ON;           //打开LCD电源
                         break;
                     default:
                         return 0;
                }
             }
         }
         return 1;  
      }
      return 0;  
}
//unsigned int key_delay;
U_INT_8 key_get(void)
{
	U_INT_8 idata key;

        do{
           proc_msg();
           if(touch.flag==0x5A){
              touch.flag = 0;
 	      tm[KEYBOARD_TIMEOUT] = KEY_TIMEOUT;       //不按键返回主菜
              if(lcd_display_status==CRTDISPLAY_OFF){   //开显示，触点无效  
                 SCREEN_SWITCH_CHAR;
                 LCD_POWER_ON;
		 continue;
              } 
              if(pre_fix_key()==0){
                 if(_testbit_(talk_belling_status_bit)){//呼叫状态中，按屏幕上方空白处任意地方接听 
                    iic_send_in_msgno(MSG_HOST_TALK);
   	            continue;
                 }
//                 SCREEN_SWITCH_CHAR;       //看图时不能显示字符
                 return TOUCH_KEY;
              } 
           }  
	   if(key_out(&key))
              return key;
       }while(1);									         	        
}
#if 0
void spi_start() //SPI开始
{
//       SCK=0;
       ADS7846_CS=1;
       SI=1;
       SO=1;
       SCK=0;
       ADS7846_CS=0;
       _nop_();
       _nop_();
       _nop_();
       _nop_();
       _nop_();
       _nop_();
       _nop_();
       _nop_();
}

void write_spi(unsigned char w_data) //SPI写数据
{
    unsigned char i=0;      
    while(i++ < 8){
         w_data<<=1;
         SI=CY;
         _nop_();
         _nop_();
         _nop_();
         _nop_();
         _nop_();
         _nop_();

         SCK=0;
         _nop_();
         _nop_();
         _nop_();
         _nop_();
         _nop_();
         _nop_();
         SCK=1;
    }
}

unsigned char read_spi(void) //SPI读数据
{
    unsigned char i;
    unsigned char  r_data=0;

    for(i=0;i<8;i++){
        r_data <<=1;
        SCK=1;
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();

        SCK=0;	 	 
        _nop_();
         _nop_();
         _nop_();
         _nop_();
        _nop_();
        _nop_();
        if(SO)
           r_data++;         
    }
    return r_data;
}

#endif
/*
struct TOUCH_CTRL idata __touch_ctrl;
void touch_in(U_INT_16 item) 
{        
	U_INT_8 tmp = (__touch_ctrl.head + 1) & _TOUCH_FIFO_MASK;
	if(tmp != __touch_ctrl.tail){
	   __touch_ctrl.x_y[__touch_ctrl.head] = item;
	   __touch_ctrl.head = tmp;
	} 
}

bit touch_out(U_INT_8 idata *x,U_INT_8 idata *y) 
{
	if(__touch_ctrl.tail != __touch_ctrl.head){
	   *x = __touch_ctrl.x_y[__touch_ctrl.tail]>>8;
	   *y = __touch_ctrl.x_y[__touch_ctrl.tail]&0xFF;
	   __touch_ctrl.tail = (__touch_ctrl.tail + 1) & _TOUCH_FIFO_MASK;
	   return 1;
	} 
	return 0;
}
*/
//IIC接收队列,键盘->主机
bit iic_rece_out(unsigned char *msg_no,unsigned long *dat) 
{
      bit tmp=0;
      unsigned char tail=iic_rece_fifo.tail;

      if(tail != iic_rece_fifo.head){
         *msg_no = iic_rece_fifo.msg_no[tail];         
         *dat = iic_rece_fifo.msg_dat[tail];            
         iic_rece_fifo.tail = (tail + 1) & IIC_RECE_FIFO_MASK;
         tmp = 1;
      } 
      return tmp;
}

void  iic_rece_in(unsigned char *buf) 
{
      unsigned char idata tmp,i;

      tmp = iic_rece_fifo.head;
      i = (tmp + 1) & IIC_RECE_FIFO_MASK;
      if(i != iic_rece_fifo.tail){
         iic_rece_fifo.head = i;       
         iic_rece_fifo.msg_no[tmp] = buf[0];
     	 iic_rece_fifo.msg_dat[tmp] =*(unsigned long *)(buf+1);
      } 
}

#if 1
//IIC发送队列,主机->键盘
bit iic_send_out(unsigned char xdata *buf)
{

      bit tmp=0;
      unsigned char tail;

      tail = iic_send_fifo.tail;
      if(tail != iic_send_fifo.head){
         buf[0] = iic_send_fifo.iic_msg[tail].msg_no;         
         memcpy(buf+1, iic_send_fifo.iic_msg[tail].msg_dat,IIC_MSG_LEN);
         iic_send_fifo.tail = (tail + 1) & IIC_SEND_FIFO_MASK;
         tmp = 1;
      } 
      return tmp;
}
#endif

///消息号和数据
void  iic_send_in(unsigned char msg_no,unsigned char *dat) 
{
      unsigned char tmp,i;

      tmp = iic_send_fifo.head;
      i = (tmp + 1) & IIC_SEND_FIFO_MASK;
      if(i != iic_send_fifo.tail){
         iic_send_fifo.head = i;       
         iic_send_fifo.iic_msg[tmp].msg_no = msg_no;
     	 memcpy(iic_send_fifo.iic_msg[tmp].msg_dat,dat,IIC_MSG_LEN);
      } 
}

//只发送一个消息号，不含数据
void  iic_send_in_msgno(unsigned char msg_no) 
{
      unsigned char tmp,i;

      tmp = iic_send_fifo.head;
      i = (tmp + 1) & IIC_SEND_FIFO_MASK;
      if(i != iic_send_fifo.tail){
         iic_send_fifo.head = i;       
         iic_send_fifo.iic_msg[tmp].msg_no = msg_no;
      } 
}

#define TOUCH_SAMPLE_TIMES   32
//unsigned char xdata touch_x[SAMPLE_TIMES],touch_y[SAMPLE_TIMES];
void TouchInterrupt(void) interrupt 8    //SST P42 INT3 外部中断3 //0    //INT0  外部中断0
{

     unsigned char idata i;
     unsigned int idata tmp_x,tmp_y;
     unsigned char volatile idata tmp_p4;

     if(vf040_busy_bit)  //SPI总线忙(读写VF040字库中)
        return;

     XICON = 0x05;       //关中断 EX3=0,IT3=0
     for(tmp_x=0;tmp_x<20000;tmp_x++);
     tmp_x = 0;
     tmp_y = 0;
     SPCR = 0x53;
     for(i=0;i<TOUCH_SAMPLE_TIMES;i++){
          tmp_p4 = P4;
          if((tmp_p4&0x4)){       //lost
            goto END; 
         }   

         ADS7846_CS=0;
         _nop_();_nop_();_nop_();_nop_();
         sst_spi(0x98);
         SPCR = 0x10;
         SCK=1; _nop_();_nop_();_nop_();_nop_();  //busy
         SCK=0; _nop_();_nop_();_nop_();_nop_();
         SPCR = 0x53;
         tmp_x += sst_spi(0x00);   //~(read_spi());// & 0x0FFF;

         sst_spi(0xd8); 
         SPCR = 0x10;
         SCK=1; _nop_();_nop_();_nop_();_nop_();
         SCK=0; _nop_();_nop_();_nop_();_nop_();
         SPCR = 0x53;
         tmp_y += sst_spi(0x00);//~(read_spi()) ;//& 0x0FFF;
         ADS7846_CS=1;
     }
     touch.x =  ~(tmp_x/TOUCH_SAMPLE_TIMES);  //取平均值
     touch.y =  ~(tmp_y/TOUCH_SAMPLE_TIMES);
     touch.flag = 0x5A;
END:
     touch.timeout = 10;
//     EX0 = 0;                  //关中断   
//     XICON = 0x55;               // EX3=0,IT3=0
}

void LPC_921(void) interrupt 7     //for SST;INT2外部中断2,协处理器中断,STC=6
{
     unsigned char xdata iic_buf[10];
     unsigned char i;

     for(i=0;i<250;i++);           //延时读数
     if(IRcvStrExt(iic_buf,5)){            
        iic_rece_in(iic_buf); 
     }           
}

//读电压状态
#if LOW_VOL_TET==1
unsigned char  read_vol_status(void)  
{
     unsigned char ad;

     EX0 = 0;                  //关中断   
     ADS7846_CS=0;
     sst_spi(0xAC);            //电压检测通道  
     SPCR = 0x10;
     SCK=1; 
     SCK=0; 
     SPCR = 0x53;
     ad = sst_spi(0x00);
     ADS7846_CS=1;
     EX0 = 1;                  //关中断   
     return ad;   
}     
#endif
//电话振铃检测
unsigned char xdata ring_counter;
bit pre_ring;
bit ring_bit_ok;
void ringcheck(void)
{
     unsigned char volatile tmp_p4 = P4;
     bit curr_bit;

     curr_bit=tmp_p4 & 0x02; 
     if((curr_bit ^ pre_ring) ){
        pre_ring = curr_bit;
        if(ring_counter++>10){
           ring_counter=0;
           ring_bit_ok = 1;
//           key_in(PHONE_CALL_MSG);
        }
     }   
}

/*
unsigned char xdata phone_status;  
unsigned char xdata phone_timeout;     
void phone_task(void)
{
      switch(phone_status){
         case PHONE_MSG_IDLE:
              break;
         case PHONE_MSG_HANGUP:
              phone_status=PHONE_MSG_WAIT_PIN;
              phone_timeout = PHONE_TIMEOUT_MAX;
              break;
         case PHONE_MSG_WAIT_PIN:
              
*/ 
       
