#ifndef _TIMER_H_
#define _TIMER_H_   
#include "inc/system.h"

 #define TIMER_INTERUPT  10         //10ms   //max 65.5ms
// #define _TH0_VAL         -((_TIMER_INTERUPT*11059.2/12)/256)   
// #define _TL0_VAL         -((_TIMER_INTERUPT*11059.2/12)%256)   
 #if SYS_CLOCK_11M
   #define TH0_VAL        -(9216/256)   
   #define TL0_VAL        0x70                  //0x50
 #endif

 #if SYS_CLOCK_22M
   #define TH0_VAL        -(18432/256)   
   #define TL0_VAL        0xD0  //0xE0 f1
 #endif

 #if SYS_CLOCK_18M   	  //18.432 
   #define TH0_VAL        -(15360/256)   
   #define TL0_VAL        0xA0          //70 慢4m        
 #endif

 #define SECOND         (1000/TIMER_INTERUPT)
 #define MINUTE         (60 * SECOND)

 enum  TIMER{
       DELAY_TIMER,
       ROM_TIMER,
       SECOND_TIMER,

       NDT_STATUS_TIMER,
       NDT_RESET_WAIT,
       KEYBOARD_TIMEOUT,
       MSG_TIMER,
       MOTI_INFO_TIMER,
       TIMER_MAX_NUM 
 };   
 #define MS_MAX_SUM          3      //毫秒最大数量

 typedef struct{
      unsigned char times;
      unsigned char frequency;
      unsigned char curr_sec;  
 }BUZZER_CTRL;     

 typedef struct{                         //时钟结构
    unsigned char Minite;
    unsigned char Hour;
    unsigned char Week;
    unsigned char Date;
    unsigned char Month;
    unsigned int  Year;
 }YMD;
 extern YMD xdata  __ymd;

 typedef struct{                         //时间戳结构
    unsigned int  Year;
    unsigned char Month;
    unsigned char Day;
    unsigned char Hour;
    unsigned char Minite;
    unsigned char Sec;
    unsigned char Msel;
 }TIMESTAMP;
 extern TIMESTAMP xdata  __timestamp;


 #define REMOTE_BUF_NUM     0x08
 typedef struct{
     unsigned char head;
     unsigned char tail;
     unsigned char dat[REMOTE_BUF_NUM];
 }REMOTE_FIFO; 

 extern unsigned char idata __main_run,__timer_run;
 extern unsigned char xdata tm[]; 
 extern bit wait_second_bit; 
 extern bit request_time_bit;
 extern unsigned char idata pre_remote_ctrl_status;
 extern unsigned char idata remote_status;

 
 void timer_init(void);
 void get_real_clock(void);
 void remote_control(void);
// void get_timestamp(void);
 void get_timestamp(TIMESTAMP xdata *xp);
 void  buzzer_start(unsigned char frequency,unsigned char times);
 void delay(U_INT_8 val);
 void timer_start(U_INT_8 tid, U_INT_8 val);
 void timer_stop(U_INT_8 tid);


#endif /* _TIMER_H_  */


