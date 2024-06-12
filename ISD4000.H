#ifndef _ISD4000_H_
#define _ISD4000_H_  


#define ISD_SET_PLAY_CMD  	0xE0 
#define ISD_PLAY_CMD  		0xF0 
#define ISD_REC_CMD   		0xA0
#define ISD_STOP_POWERDOWN_CMD  0x10
#define ISD_READ_INT_CMD  	0x30
#define ISD_POWER_UP_CMD  	0x20    //上电

//欢迎词
#define VOICE_ADDR_WELCOME_0    0x00
#define VOICE_ADDR_WELCOME_1    0x00

//控制语音
#define VOICE_ADDR_CTRL_0       0x11     
#define VOICE_ADDR_CTRL_1       0x00

//房间名称
#define VOICE_ADDR_ROOM_NAME_0  0x5D  
#define VOICE_ADDR_ROOM_NAME_1  0x00     

//操作名称
#define VOICE_ADDR_ONOFF_0      0x86
#define VOICE_ADDR_ONOFF_1      0x00

//请稍后
#define VOICE_ADDR_WAITING_0    0x91
#define VOICE_ADDR_WAITING_1    0x00

//操作成功
#define VOICE_ADDR_OP_OK_0      0x96
#define VOICE_ADDR_OP_OK_1      0x00

//操作失败
#define VOICE_ADDR_OP_FAIL_0    0x9B
#define VOICE_ADDR_OP_FAIL_1    0x00

//以下是电话报警的语音,高位地址都为0
//门磁报警,设备类型33
#define ALM_DOOR_TYPE    33
#define VOICE_ADDR_ALM_DOOR_0    0xA0

//烟感报警,设备类型34
#define VOICE_ADDR_ALM_YANGAN_0  0xA5

//煤气报警,设备类型35
#define VOICE_ADDR_ALM_MEIQI_0   0xA9

//窗磁报警,设备类型36
#define VOICE_ADDR_ALM_WINDOW_0   0xAD

//求助报警,设备类型37
#define VOICE_ADDR_ALM_HELP_0   0xB2

//红外报警,设备类型38
#define VOICE_ADDR_ALM_HONGWAI_0   0xB6

//周界报警,设备类型39
#define VOICE_ADDR_ALM_OUTSIDE_0   0xBA

//未知报警,设备类型40
#define ALM_MAX_TYPE    36
#define VOICE_ADDR_ALM_UNKNOWN_0   0xBF


//void  isd4000_cmd(unsigned char add0,unsigned char add1,unsigned char cmd);
void  isd4000_cmd(unsigned char add0);
void  isd4000_command(unsigned char cmd);
void isd4000_init(void);


#endif
