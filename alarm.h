#ifndef _ALARM_H_
#define _ALARM_H_ 1

#define _ALM_PORT _P8155_A
#define _ALM_LOG_MAX   0x0F

#define _KEYBOARD_ALM_PORT     0x40     //键盘报警逻辑端口
#define _INTIMIDATE_ALM_PORT   0x41     //胁持报警
#define _BACKOUT_ALM_PORT      0x44     //机壳防拆报警
#define _REMOTE_CTROL_ALM      0x45     //遥控器报警
#define _LOW_POWER_12V         0x46     //12V低压报警
#define _BOARDCASE_ALM_PORT    0x50     //广播报警端口

struct ALM
{
     unsigned char port;
     unsigned char riu_addr;
     unsigned char alm_device;
};
extern struct ALM idata __alm;


struct ALM_LOG
{  
     unsigned char curr_ptr;     
     struct {
     	unsigned char used;
        unsigned char alm_riu;    //riu地址
        unsigned char alm_port;   //端口
        unsigned char alm_device; 
        unsigned char alm_minite;
        unsigned char alm_hour;
        unsigned char alm_day;
        unsigned char alm_month;
        unsigned char alm_sec;
     }dat[_ALM_LOG_MAX];   
};
extern struct ALM_LOG xdata __alm_log;

#define DEFEND_LOG_MAX   0x1F
typedef struct {  
     unsigned char curr_ptr;     
     struct {
     	unsigned char used;
        unsigned char defend_type;    //布防类型
        unsigned char defend_mode;    //布防方式
        unsigned char defend_month; 
        unsigned char defend_day; 
        unsigned char defend_hour; 
        unsigned char defend_minite; 
        unsigned char defend_sec; 
     }dat[DEFEND_LOG_MAX];   
}DEFEND_LOG;
extern  DEFEND_LOG xdata __defend_log;

void alm_log(unsigned char riu,unsigned char port);
void alm_init(void);
void alm(bit status);
void send_alm_to_mmu(void);
void defend_log(unsigned char type,unsigned char mode);  

#endif
