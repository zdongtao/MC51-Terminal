#ifndef _MT8888_H_
#define _MT8888_H_  

#define  XING    11
#define  JING    12

#define WAIT_DTMF_IN_TIMEOUT    30

/* MT8888状态定义 */
enum MT8888_STATUS {
     MT8888_IDLE,
     MT8888_SEND,
     MT8888_RECE
};

enum ALM_DIAG_STATUS{
	PHONE_IDLE,
	PHONE_HUANG_UP,
	PHONE_DIAL,
	PHONE_PLAY_VOICE,
	PHONE_PLAY_END,
	PHONE_HUANG_UP_WAIT,
        //接收状态转换 
        PHONE_CALL_IN,
	PHONE_CALL_WAIT_PIN,
	PHONE_CALL_PLAY_MAIN_PAGE,
	PHONE_CALL_WAIT_TYPE,
	PHONE_CALL_WAIT_SN,
	PHONE_CALL_WAIT_OP,
        PHONE_CALL_SEND_CMD
};

extern unsigned char idata phone_alm_timerout;
extern unsigned char xdata send_dtmf_buf[];

void  init_mt8888(void);
void send_dtmf(void);
void phone_alm_task(void);
void enter_phone_alm(void);
void stop_phone_alm(void);

#endif