 //   ����SCU��RIU֮����첽��ͨѶ��ʵ�����Ӷ���첽��ͨѶЭ�飬��
 //   NDTϵͳӦ�ò�Э�顣
 
#ifndef _ASYN_H_
#define _ASYN_H_   
#include "inc/system.h"

 #define MULTICAST              0xFE   //CCU�鲥��ַ 

 #if  SYS_CLOCK_11M
       #define BAUD_RATE        0xFD  //19200BPS
 #endif
 #if  SYS_CLOCK_22M
       #define BAUD_RATE        0xFA  //19200BPS
 #endif
 #if  SYS_CLOCK_18M
       #define BAUD_RATE        0xFB  //19200BPS
 #endif

 #define ASYN_WAIT_TIMEOUT      4     //�첽ͨѶ��ʱ 

 #define VIDEO_SW_ADDR          0x8   //��Ƶ�л�ģ��ĵ�ַ�̶�Ϊ8

 enum ASYN_COMM_STATUS{          //����״̬����
      ASYN_IDLE,
      SEND_ADDR,
      WAIT_ANSWER,
      SEND_CMD_END,
      WAIT_STATUS,
      SEND_DATA,
      RECEIVE_DATA
 };
/*
 #define ASYN_IDLE     0        //����״̬����
 #define SEND_ADDR     1       
 #define WAIT_ANSWER   2
 #define SEND_CMD_END  3
 #define WAIT_STATUS   4
 #define SEND_DATA     5
 #define RECEIVE_DATA  6
*/  
 #define RESET         0xF9      //�ӻ���λ 
 #define ASYN_SEND     0xF1      //NDT����
 #define ASYN_RECE     0xF0      //NDT����
 #define RX_RDY        0x01      //�ӻ����վ���
 #define TX_RDY        0x02      //�ӻ�����׼������
 #define NO_RDY        0x03      //�ӻ�û�о��� 

 #define RECE_485      RS485_CTRL=0
 #define SEND_485      RS485_CTRL=1

 #define ASYN_RECE_MAX_BUF   (1518-42)    //���մ������BUF,port+head+port_no
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



