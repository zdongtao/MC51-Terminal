#ifndef _ISD4000_H_
#define _ISD4000_H_  


#define ISD_SET_PLAY_CMD  	0xE0 
#define ISD_PLAY_CMD  		0xF0 
#define ISD_REC_CMD   		0xA0
#define ISD_STOP_POWERDOWN_CMD  0x10
#define ISD_READ_INT_CMD  	0x30
#define ISD_POWER_UP_CMD  	0x20    //�ϵ�

//��ӭ��
#define VOICE_ADDR_WELCOME_0    0x00
#define VOICE_ADDR_WELCOME_1    0x00

//��������
#define VOICE_ADDR_CTRL_0       0x11     
#define VOICE_ADDR_CTRL_1       0x00

//��������
#define VOICE_ADDR_ROOM_NAME_0  0x5D  
#define VOICE_ADDR_ROOM_NAME_1  0x00     

//��������
#define VOICE_ADDR_ONOFF_0      0x86
#define VOICE_ADDR_ONOFF_1      0x00

//���Ժ�
#define VOICE_ADDR_WAITING_0    0x91
#define VOICE_ADDR_WAITING_1    0x00

//�����ɹ�
#define VOICE_ADDR_OP_OK_0      0x96
#define VOICE_ADDR_OP_OK_1      0x00

//����ʧ��
#define VOICE_ADDR_OP_FAIL_0    0x9B
#define VOICE_ADDR_OP_FAIL_1    0x00

//�����ǵ绰����������,��λ��ַ��Ϊ0
//�Ŵű���,�豸����33
#define ALM_DOOR_TYPE    33
#define VOICE_ADDR_ALM_DOOR_0    0xA0

//�̸б���,�豸����34
#define VOICE_ADDR_ALM_YANGAN_0  0xA5

//ú������,�豸����35
#define VOICE_ADDR_ALM_MEIQI_0   0xA9

//���ű���,�豸����36
#define VOICE_ADDR_ALM_WINDOW_0   0xAD

//��������,�豸����37
#define VOICE_ADDR_ALM_HELP_0   0xB2

//���ⱨ��,�豸����38
#define VOICE_ADDR_ALM_HONGWAI_0   0xB6

//�ܽ籨��,�豸����39
#define VOICE_ADDR_ALM_OUTSIDE_0   0xBA

//δ֪����,�豸����40
#define ALM_MAX_TYPE    36
#define VOICE_ADDR_ALM_UNKNOWN_0   0xBF


//void  isd4000_cmd(unsigned char add0,unsigned char add1,unsigned char cmd);
void  isd4000_cmd(unsigned char add0);
void  isd4000_command(unsigned char cmd);
void isd4000_init(void);


#endif