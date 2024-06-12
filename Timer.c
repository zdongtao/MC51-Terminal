#include <reg52.h>
#include <string.h>
#include "inc/types.h"
#include "inc/timer.h"
#include "inc/system.h"
#include "inc/asyn.h"
#include "inc/hdres.h"
#include "inc/key.h"
#include "inc/ctrl.h"
#include "inc/MT8888.h"

static BUZZER_CTRL xdata __buzzer_ctrl;
YMD  xdata  __ymd={1,12,7,1,8,2004};
TIMESTAMP xdata  __timestamp;
unsigned char xdata tm[TIMER_MAX_NUM];
bit wait_second_bit;
bit request_time_bit;

void timer_init(void)
{
     TMOD = 0x21;
     TH0 = TH0_VAL;
     TL0 = TL0_VAL;     
     ET0 = 1;
     TR0 = 1;
//     PT0 = 1;   //¸ßÓÅÏÈ¼¶
     request_time_bit = 1;
     tm[KEYBOARD_TIMEOUT] = KEY_TIMEOUT; 
}
static unsigned char data msel_counter;   
void TimerInterrupt(void) interrupt 1  using 1
{
     unsigned char i;
     extern bit wait_10ms_bit; 
     static bit buzz_status;
     extern unsigned int xdata timer_monitor;
 	
      TH0 = TH0_VAL;
      TL0 = TL0_VAL;

      timer_monitor=0;
//      WATCHDOG = 1;            

//#if   __DEBUG==1
//      WATCHDOG = 0;            
//#endif

      wait_10ms_bit = 1;
      for(i=0; i<MS_MAX_SUM;i++){  
         if(tm[i])              
            tm[i]--;
      }
      if(asyn_timeout)
         asyn_timeout--;

      if(msel_counter++ >= SECOND){
         msel_counter = 0;	   
         wait_second_bit = 1;
         if(phone_alm_timerout)phone_alm_timerout--;
      }         
	     
      if(__buzzer_ctrl.times){   //ÓÃ¶¨Ê±Æ÷¿ØÖÆ·äÃùÆ÷µÄ´ÎÊý
         if(__buzzer_ctrl.curr_sec) 
            __buzzer_ctrl.curr_sec--;
         else{
	    if(buzz_status) 
               buzzer_off(_BIT_DELAY);
	    else
               buzzer_on(_BIT_DELAY);
	    buzz_status = ~buzz_status;
            if(--__buzzer_ctrl.times==0){
               buzz_status = 0;
               buzzer_off(_BIT_DELAY);	       	
            } 
            else
               __buzzer_ctrl.curr_sec = __buzzer_ctrl.frequency;        
         }
      }     		        

      if(touch.timeout){
         if(--touch.timeout==0)            
            XICON = 0x55;
            //EX0 =1;
      }    
//      key_timer();	        //¼üÅÌÊý¾ÝÈë¶ÓÁÐ
      ad_process();      
}

/*************************************************************************/
/*             ÅÐ¶ÏÊÇ·ñÈòÄê                                              */
/* ²ÎÊý£ºÎÞ                                                              */       
/* ·µ»Ø£º1--ÊÇÈòÄê£¬0--²»ÊÇÈòÄêÞ                                         */     
/* ÈÕÆÚ£º2003/08/28                                                      */        
/*-----------------------------------------------------------------------*/
/* ÐÞ¸Ä¼ÇÂ¼:		                                                 */
/*    							                 */   
/*************************************************************************/
bit leap_year(void)
{
   if((__ymd.Year%4==0 && (__ymd.Year%100!=0)) || (__ymd.Year%400==0))
      return 1;
   return 0;
}       
/*************************************************************************/
/*             ÓÃ¶¨Ê±Æ÷²úÉúÊ±ÖÓ                                 */
/* ²ÎÊý£º ÎÞ                                                              */ 
/* ·µ»Ø£º ÎÞ                                                              */     
/* ÈÕÆÚ£º2001/06/01                                                     */        
/*-----------------------------------------------------------------------*/
/* ÐÞ¸Ä¼ÇÂ¼:		                                                     */
/*    							                                 */   
/*************************************************************************/
unsigned char code days_month[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
void get_real_clock(void)
{
    if(++__ymd.Minite>=60){
         __ymd.Minite=0;
       if(++__ymd.Hour>=24){
          __ymd.Hour=0;
          request_time_bit = 1;  //24HOUR ÇëÇóÒ»´ÎÊ±¼ä
          if(++__ymd.Week>7)
             __ymd.Week=1;           
          if(++__ymd.Date > days_month[__ymd.Month]){
             if(__ymd.Month==2 && leap_year() && __ymd.Date<=29);
             else{ 
                __ymd.Date=1;
                if(++__ymd.Month>12){
                   __ymd.Month=1;
                   ++__ymd.Year; 
                }
             }           
          }
       }  
    }
}   

//unsigned char idata pre_remote_ctrl_status;
//unsigned char idata remote_status;
/*
void remote_control(void)    //´¦ÀíÒ£¿ØÆ÷
{
    unsigned char curr_status,i;

    curr_status =(P1 ^ pre_remote_ctrl_status) & 0x0F;
    if(curr_status){
       for(i=1; i<=8; i<<=1){
           if(curr_status & i & P1){                                                        
              remote_status = curr_status;                
              break;
           }
       }    
       pre_remote_ctrl_status = P1;
    }    
}
 
*/

void get_timestamp(TIMESTAMP xdata *xp)
{
   extern U_INT_8 idata second_counter;
//   TIMESTAMP xdata *xp;
//   xp = &__timestamp;
   xp->Year =  __ymd.Year;
   xp->Month =  __ymd.Month;
   xp->Day =  __ymd.Date;
   xp->Hour =  __ymd.Hour;
   xp->Minite =  __ymd.Minite;
   xp->Sec =  second_counter;
   xp->Msel =  msel_counter;
}

/*************************************************************************/
/*             buzzer_start ·äÃùÆ÷¿ØÖÆ¿ªÊ¼                               */
/* ²ÎÊý£º times¡ª¡ª·äÃùÆ÷Ãù½Ð´ÎÊý                                          */ 
/*       frequency¡ª¡ªÆµÂÊ                                                 */
/* ·µ»Ø£º ÎÞ                                                             */     
/* ÈÕÆÚ£º2000/12/13                                                      */      
/*-----------------------------------------------------------------------*/
/* ÐÞ¸Ä¼ÇÂ¼:		                                                 */
/*    							                 */   
/*************************************************************************/
void  buzzer_start(unsigned char frequency,unsigned char times)
{
     if(__buzzer_ctrl.times==0){    //Î´Ê¹ÓÃ
        __buzzer_ctrl.frequency = frequency;
        __buzzer_ctrl.times = 1<<times;   //times*2
     }   
}  

void delay(U_INT_8 val)
{
     tm[DELAY_TIMER] = val;
     while(tm[DELAY_TIMER])
	   proc_msg();
}


