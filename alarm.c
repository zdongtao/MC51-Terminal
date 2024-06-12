#include <reg51.h>
#include <absacc.h>
#include <string.h>
#include "inc/types.h"
#include "inc/hdres.h"
#include "inc/timer.h"
#include "inc/alarm.h"
#include "inc/ctrl.h"
#include "inc/tool.h"
#include "inc/system.h"
#include "inc/app.h"
#include "inc/skbuff.h"
#include "inc/key.h"
#include "inc/menu.h"
#include "inc/MT8888.h"

struct ALM idata __alm={0xFF,0,0};
struct ALM_LOG xdata __alm_log;   //报警记录
DEFEND_LOG xdata __defend_log;

void alm_init(void)
{	
        //ALM_LIGHT = LED_OFF;
        ALM_LIGHT_OFF;
	if(__security_class & 0x0F){
//	   DEFEND_LIGHT = LED_ON;
           DEFEND_LIGHT_ON;
        }
	else{
	   //DEFEND_LIGHT =  LED_OFF;	
           DEFEND_LIGHT_OFF;
           __security_class = 0x80;
        }
}


void alm_log(unsigned char riu,unsigned char port)  
{
    unsigned char curr_ptr;
    unsigned char xdata *xp; 

    extern U_INT_8 idata second_counter;

        curr_ptr = __alm_log.curr_ptr;
	if(curr_ptr >= _ALM_LOG_MAX) 
           curr_ptr = 0;	
        xp =(unsigned char *)&__alm_log.dat[curr_ptr];
        *xp++ = 0xAA;             //used              
        *xp++ = riu;              //alm_riu        
        *xp++ = port;             //alm_port        
        *xp++ = __alm.alm_device; //alm_device        
        *xp++ = __ymd.Minite;    //alm_minite        
        *xp++ = __ymd.Hour;     //alm_hour        
        *xp++ = __ymd.Date;     //alm_day        
        *xp++ = __ymd.Month;    //alm_month        
        *xp   = second_counter; //alm_sec        
/*
	__alm_log.dat[curr_ptr].alm_riu=riu;         //记录报警
	__alm_log.dat[curr_ptr].alm_port=port;
	__alm_log.dat[curr_ptr].alm_device = __alm.alm_device;
	__alm_log.dat[curr_ptr].alm_minite=__ymd.Minite;
	__alm_log.dat[curr_ptr].alm_hour=__ymd.Hour;
	__alm_log.dat[curr_ptr].alm_day=__ymd.Date;
	__alm_log.dat[curr_ptr].alm_month=__ymd.Month;	
	__alm_log.dat[curr_ptr].alm_sec = second_counter;	
	__alm_log.dat[curr_ptr].used = 0xAA;     //使用
*/
	__alm_log.curr_ptr = curr_ptr+1;
	eeprom_in_queue(SAVE_ALM_LOG);
        return;
}

//保存布防信息
void defend_log(unsigned char type,unsigned char mode)  
{
    unsigned char curr_ptr;
    unsigned char xdata *xp; 
    extern U_INT_8 idata second_counter;

        curr_ptr = __defend_log.curr_ptr;
	if(curr_ptr >= DEFEND_LOG_MAX) 
           curr_ptr = 0;	
        xp =(unsigned char *)&__defend_log.dat[curr_ptr];
        *xp++= 0xAA;    //used
        *xp++= type;    //defend_type
        *xp++= mode;    //defend_mode
        *xp++= __ymd.Month;//
        *xp++= __ymd.Date;
        *xp++= __ymd.Hour;
        *xp++= __ymd.Minite;
        *xp  = second_counter;	
/*
	__defend_log.dat[curr_ptr].defend_type = type;         //记录报警
	__defend_log.dat[curr_ptr].defend_mode = mode;
	__defend_log.dat[curr_ptr].defend_month = __ymd.Month;
	__defend_log.dat[curr_ptr].defend_day=__ymd.Date;
	__defend_log.dat[curr_ptr].defend_hour=__ymd.Hour;
	__defend_log.dat[curr_ptr].defend_minite=__ymd.Minite;
	__defend_log.dat[curr_ptr].defend_sec = second_counter;	
	__defend_log.dat[curr_ptr].used = 0xAA;     //使用
*/
	__defend_log.curr_ptr = curr_ptr+1;
	eeprom_in_queue(SAVE_DEFEND_LOG);
        return;
}


void alm(bit status)      //status=1 报警  0 关闭报警  loca_alm=1 响应非端口触发的报警
{
	U_INT_8 i;
	unsigned char idata Op_Code,port;
        unsigned char idata Normal_Status,type,xy;
	U_INT_8 xdata *p;
        unsigned char xdata buf[45];
        extern unsigned char xdata lcd_display_status;
        extern unsigned char xdata lcd_sw_status;


        //cs6_b2_CRTdisplay = CRTDISPLAY_ON;       //开显示 
        LCD_POWER_ON;
	//cs6_b1_switching = SHOW_CHAR;		 //切换到字符
        SCREEN_SWITCH_CHAR;
	tm[KEYBOARD_TIMEOUT] = KEY_TIMEOUT;	 //关显示延时

        port = __alm.port;
	if(!__alm.riu_addr && port<_SCU_PORT_MAX_NUM){		//9.25
	   Op_Code = __port[port].type.In_Port.Op_Code;	
           type = __port[port].Device_Type;
           xy = __port[port].type.In_Port.Curr_xy;
           __alm.port += 1;
	}	
	else{ 
	   Op_Code = 0x02;		      //RIU报警，只产生本地报警
	}	 
	for(i=16; i<_SCU_PORT_MAX_NUM; i++){  //控制警灯或其它报警输出设备
  	    if(__port[i].Device_Type>=0x80 && __port[i].Device_Type<=0x9F){   //安防报警输出
	       Normal_Status = __port[i].type.Out_Port.Normal_Status & 0x01;
               if(status) //报警
                  start_ctrl(Normal_Status,i,_CTRL_TYPE_ALM_ON);
               else
                  stop_ctrl(Normal_Status, i, _CTRL_TYPE_ALM_OFF);
	    }
	}
	if(status){ 
#if ALM_OPEN_LIGHT==1
           if(other_item.alm_open_light){   //发送状态
                buf[APP_HEAD_LEN]   = 0x16;
                buf[APP_HEAD_LEN+1] = 0x02;  
                buf[APP_HEAD_LEN+2] = 0x05;  
                buf[APP_HEAD_LEN+3] = 13;     //报警固定开13号灯  
                buf[APP_HEAD_LEN+4] = 0x01; 
                make_send_package(0xFF,0,QUERY_TYPE,buf,APP_HEAD_LEN+5);   //发送一个开灯命令

           }
#endif
  	   alm_log(__alm.riu_addr,port);   //记录报警  2000.9.21
	   if(Op_Code & 0x01){  //本地报警 
//    	      buzzer_on(_BIT_ALM);
              ALM_LIGHT_ON;
	      if(alm_info.ptr<ALM_INFO_LEN){	
                 i=alm_info.ptr++;
                 alm_info.alm[i].x = xy & 0xF0;             
                 alm_info.alm[i].y = ((xy&0x0F)+1);  //显示从第二页开始+1            
                 alm_info.alm[i].device_type = type;
              }
	   }	
	   if(Op_Code & 0x02){  //远程报警
              buf[APP_HEAD_LEN]   = 0xA5;
   	      buf[APP_HEAD_LEN+1] = 0x00;
	      buf[APP_HEAD_LEN+2] = __alm.port; //port + 1;   //报警的端口
	      p=(U_INT_8 *)&__port;
	      memcpy(buf+APP_HEAD_LEN+3, p+port*sizeof(PORT), sizeof(PORT));	
              get_timestamp((TIMESTAMP xdata *)(buf+APP_HEAD_LEN+3+sizeof(PORT)));
              retrans_in(buf,3+sizeof(PORT)+sizeof(TIMESTAMP)+APP_HEAD_LEN);
//              make_send_package(0,0,ALM_TYPE|SEND_ALWAYS,buf,3+sizeof(PORT)+sizeof(TIMESTAMP));                
	   }
	}  			
	else{   //取消报警 ,关警灯或其它报警输出设备
  	    buzzer_off(_BIT_ALM|_BIT_WARN_OFF);
            delay_status_bit = 0;
            continue_status_bit = 0;
            ALM_LIGHT_OFF;
            stop_phone_alm();
	}
}


