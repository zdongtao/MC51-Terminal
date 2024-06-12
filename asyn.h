 //   处理SCU与RIU之间的异步口通讯，实现主从多机异步口通讯协议，和
 //   NDT系统应用层协议。
 
#ifndef _ASYN_H_
#define _ASYN_H_   
#include "inc/system.h"

 #define MULTICAST              0xFE   //CCU组播地址 

 #if  SYS_CLOCK_11M
       #define BAUD_RATE        0xFD  //19200BPS
 #endif
 #if  SYS_CLOCK_22M
       #define BAUD_RATE        0xFA  //19200BPS
 #endif
 #if  SYS_CLOCK_18M
       #define BAUD_RATE        0xFB  //19200BPS
 #endif

 #define ASYN_WAIT_TIMEOUT      4     //异步通讯超时 

 #define VIDEO_SW_ADDR          0x8   //视频切换模块的地址固定为8

 enum ASYN_COMM_STATUS{          //主机状态定义
      ASYN_IDLE,
      SEND_ADDR,
      WAIT_ANSWER,
      SEND_CMD_END,
      WAIT_STATUS,
      SEND_DATA,
      RECEIVE_DATA
 };
/*
 #define ASYN_IDLE     0        //主机状态定义
 #define SEND_ADDR     1       
 #define WAIT_ANSWER   2
 #define SEND_CMD_END  3
 #define WAIT_STATUS   4
 #define SEND_DATA     5
 #define RECEIVE_DATA  6
*/  
 #define RESET         0xF9      //从机复位 
 #define ASYN_SEND     0xF1      //NDT发送
 #define ASYN_RECE     0xF0      //NDT接收
 #define RX_RDY        0x01      //从机接收就绪
 #define TX_RDY        0x02      //从机发送准备就绪
 #define NO_RDY        0x03      //从机没有就绪 

 #define RECE_485      RS485_CTRL=0
 #define SEND_485      RS485_CTRL=1

 #define ASYN_RECE_MAX_BUF   (1518-42)    //接收处理最大BUF,port+head+port_no
 #define ASYN_SEND_MAX_BUF   (1518-42)    //       

 typedef struct{ 
        unsigned int  rece_len;
        unsigned int  send_len;
	unsigned char rece_buf[ASYN_RECE_MAX_BUF];
	unsigned char send_buf[ASYN_SEND_MAX_BUF];
 }ASYN_BUF;

 extern  ASYN_BUF xdata asyn;
// extern  unsigned int idata asyn_len;

 typedef struct{
     unsigned int rece_count;
     unsigned int send_count;
     unsigned int error_count;
 }TEST_COUNTER;
 extern TEST_COUNTER xdata test_counter;


 extern unsigned char data asyn_timeout;  
 extern bit asyn_send;
 extern unsigned char xdata riu_status[];


 void asyn_controler(void);
 void asyn_init(void);

#endif 



