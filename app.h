#ifndef _APP_H_
#define _APP_H_   

#include "inc/eth.h"

#define APP_DATA_LEN   (ETH_FRAME_LEN-42)

#define MODIFY_VAR_PASSWD      "\x08\x26\x01\x00"  

#define UPDATE_MMU_ALL          0xFF
#define UPDATE_MMU_DEFEND       0x01
#define UPDATE_MMU_PIN          0x02
#define UPDATE_MMU_PORT         0x03
#define UPDATE_MMU_NO_ALM       0x12

typedef struct{
     unsigned char passwd[4];
     unsigned char access_flag;
              void *address;
     unsigned int  len;
     unsigned char buf[1];
}MODIFY_VAR;

extern unsigned char xdata __security_class;
extern bit pre_open_bit;


unsigned char  rec_check_xor(unsigned int len,unsigned char xdata *pBuf);
void app_process_received(unsigned char xdata *xp);
void make_send_package(unsigned char riu_addr,unsigned char tcbNum, unsigned char type, unsigned char xdata *xp,unsigned int len);
bit  rece_chksum(unsigned char len);
void call_checksum(unsigned int len,unsigned char xdata *p);
void app_other_task(void);
void net_handle(void);
void send_to_ccc(unsigned char type,unsigned char xdata *xp,unsigned int len);
void _do_isp(void);

#endif
