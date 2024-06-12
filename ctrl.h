#ifndef _CTRL_H_
#define _CTRL_H_	1

//sbit __alm_defend_line	= P1^3;   	/* ���� */
//sbit __alm_light_line	= P1^5;   	/* ���� */

#define INPUT_PORT_PHY_NUM     8      //��������˿�����

#define _INPUT_PORT_NUM        16      //��������˿�����
#define _OUTPUT_PORT_NUM       4       //����˿�����
#define _SCU_PORT_MAX_NUM      (_INPUT_PORT_NUM + _OUTPUT_PORT_NUM)  //SCU���˿�����
#define _MAX_METER_RANGE       999999L 

#define DEFEND_MODE_KEY            0x01    //����
#define DEFEND_MODE_REMOTE_CTRL    0x02    //ң��
#define DEFEND_MODE_DOORLOCK       0x03    //����
#define DEFEND_MODE_CENTER         0x04    //�������Ĳ��� 
#define DEFEND_MODE_OTHER          0x05    //����


#define ALM_VOLTAGE_VALUES         0x40    //С��10v 


#define _DOOR_DELAY	60*_SECOND

#define _CTRL_TYPE_NOT           0x00
#define _CTRL_TYPE_TIMER         0x01
#define _CTRL_TYPE_DIRECT_OFF    0x02
#define _CTRL_TYPE_DIRECT_ON     0x04

#define _CTRL_TYPE_ALM_ON        0x08
#define _CTRL_TYPE_ALM_OFF       0x10

//���������ͨ��
#define _BIT_KEY         0x01
#define _BIT_DELAY       0x02
#define _BIT_ALM         0x04
#define _BIT_CTRL        0x08
#define _BIT_ERROR       0x10 
#define _BIT_IIMER_CTRL  0x20      
#define _BIT_WARN_OFF    0x40    //Ԥ��

//������
#define _LOCAL_ALM      0x01     //���ر���
#define _REMOTE_ALM     0x02     //Զ�̱���
#define _ADVANCE_CTRL   0x04     //���ܿ���
#define _SEND_TO_CCC    0x08     //���͵�CCC
#define _BYPASS         0x10     //����· 

#define _RIU_NUM        40       //SCU���RIU����30��
#define _HEAD_LEN       5        //���ݰ�ͷ����
#define _SYS_PASSWD_LEN 6
#define _INIT_PIN       "\x01\x01\x01\x01\x01\x01"
#define _SYS_PIN        "\x01\x02\x03\x04\x05\x06"
#define _USER_PIN_LEN   8

typedef struct{
   unsigned char Pass_Num;          //�û����볤��
   unsigned char Password[8];
   unsigned char Syspasswd[_SYS_PASSWD_LEN];  //ϵͳ����
}PASSWORD;
extern PASSWORD xdata __sys_passwd;

struct S_RIU{
     unsigned char RIU_Num;
     struct{                      
          unsigned char Usable;        //����״̬
          unsigned char RIU_Type;      //����
          unsigned char RIU_Addr;      //RIU��ַ
          unsigned char Curr_Status;   //��ǰ״̬
     }riu_no[_RIU_NUM];                //30
     unsigned char Security_Password;
};
extern struct S_RIU  xdata __riu;

typedef struct {
     unsigned char Curr_Status;   //����
     unsigned int  Unitage;       //������/��λ��
     unsigned long Curr_Data;     //��ǰ����
     unsigned char reserved[2];   //����         
     unsigned char Curr_ad;       //��ǰADֵ
}METER_TYPE;

typedef struct{
    unsigned char Open;           //��������״̬��D0---D5������6�ֲ���״̬��D8�������ò���
    unsigned char Op_Code;        //������ D0=1���ر���,D1=1Զ�̱���,D2=1˫����,D3=1�ϴ���CCC��D4=1����·
    unsigned char Normal_Status;  //����״̬ 
    unsigned char Prev_Status;    //ǰһ��״̬
    unsigned char Curr_Status;    //��ǰ״̬
    unsigned int  Delay;	  //��ʱ��Ӧʱ�䣨�룩
    unsigned int  Respond;        //��Ӧ����ʱ��
    unsigned char Curr_xy;        //��������ǰλ�ã�����λ��ʾ�������꣬����λ��ʾ��������
}INPUT_TYPE;

typedef struct{
    unsigned char Normal_Status;  //����״̬ 
    unsigned char Curr_Status;    //��ǰ״̬
    unsigned char Op_Status;      //����״̬
    unsigned char Op_Mode;        //����ģʽ��=0��ʱ���� 
    unsigned int  Start_Time;     //��ʱʱ�䣨�룩OR ����ʱ�䣨�֣�
    unsigned int  End_Time;       //����ʱ�䣨�룩OR ����ʱ�䣨�֣�
    unsigned char Week;           //D0--D6��ʾһ�����죬D8����
    unsigned char Ctrl_Status;    //����״̬
}OUTPUT_TYPE;

#define SEND_FLAG          0x01
#define OPEN_ALM_FLAG      0x02
#define SHORT_ALM_FLAG     0x04

typedef struct{
        unsigned char Usable;        //0������
        unsigned char Device_Type;   //0�����ã�255���豸
        unsigned char Other_Flag;    //D0=�ϴ��仯��D1=��·������D2=��·����
        unsigned char Sample_Times;
        unsigned char Low_level;     //�͵�ƽ
        unsigned char High_level;    //�ߵ�ƽ
        unsigned char Voice_No;      //�������
        union {
            METER_TYPE  Meter;
            INPUT_TYPE  In_Port;
            OUTPUT_TYPE Out_Port;
        }type;    
}PORT;           

extern PORT xdata  __port[_SCU_PORT_MAX_NUM];

//�Ƚ���״̬
#define SHORT_LEVEL      0x00        //�Ƚ������ȫ0
#define OPEN_LEVEL       0x08        //�Ƚ���������λΪ1

#define LOW_FLAG         0x01
#define HIGH_FLAG        0x02
#define SHORT_FLAG       0x04
#define OPEN_FLAG        0x08
#define ERROR_FLAG       0x10        //������ƽ

#define STOP_SEND_SHORT  0x40
#define STOP_SEND_OPEN   0x80 

#define SAMPLE_MIN_TIMES 0x04		 //ÿ���˿���С�������� 

typedef struct{
    unsigned char curr_flag;
    unsigned char curr_times;
    unsigned char curr_level;
}AD_PORT;
/*
#define METER_MAX_NUM      12               //������������
struct QUERY_FEE{
     unsigned char Meter_Num; 
     struct {            //���ò�ѯ
         unsigned char  Device_Type;       //�豸����
         unsigned int   Meter_data;        //���������
         unsigned int   Rate_Fee;          //���� 
         unsigned int   Fee;               //�շ�
     }meter_no[METER_MAX_NUM];
}; 
extern struct QUERY_FEE xdata __query_fee;

typedef struct{				   //��ѯ�����������	
     unsigned char Meter_Num; 
     struct {            //���ò�ѯ
         unsigned char  Device_Type;       //�豸����
         unsigned long  Meter_data;        //�����,(999999.99) ǰ��λΪ����������λΪС��  
     }meter_no[METER_MAX_NUM];
}QUERY_METER; 
extern  QUERY_METER xdata __query_meter;  //��ѯ�������FROM CCC TO NDT
//add end
*/

#define MENU_MAX_LEN      30		     //�˵�������		
#define MENU_MATNIX_LEN   MENU_MAX_LEN*10    //���ֱ���ռ䳤��
struct MENU{
    struct{
       unsigned char Used;                   //ʹ�ñ��
       unsigned char Device_Type;        
       unsigned char Menu_Item_Pointer;      //��ʼλ��
       unsigned char Menu_Length;            //�ַ�����
    }device[MENU_MAX_LEN];
    unsigned char  Hz_bm[MENU_MATNIX_LEN];   //���ֱ���   
}; 	
extern struct MENU xdata __menu_item;
   
typedef struct{
//     unsigned char used;
     unsigned char port;
     unsigned int  ctrl_time;
}CTRL_TIME_TYPE;  
extern CTRL_TIME_TYPE idata __delay_ctrl,__continue_ctrl;

#define REPAIR_MENU_ITEM   40
#define REPAIR_MENU_MATNIX_LEN REPAIR_MENU_ITEM*10  //���ֱ���ռ䳤��
typedef struct{                                     //�豸ά�޲˵� 
    struct{
       unsigned char seq_no;                        //�豸��Ŵ�1��ʼ��û��ʹ������ 
       unsigned char Menu_Item_Pointer;             //��ʼλ��
       unsigned char Menu_Length;                   //�ַ�����
    }device_name[REPAIR_MENU_ITEM];
    unsigned char Hz_bm[REPAIR_MENU_MATNIX_LEN];    //���ֱ���
}REPAIR_MENU;
extern REPAIR_MENU xdata __repair_menu;

#define LINK_CTRL_NUM      10
#define CTRL_VERSION       0x10         //1.0            
#define DELAY_CTRL         0x01
#define SYN_CTRL           0x02
#define MAN_CTRL           0x03
#define TWO_ARM            0x10

typedef struct{
       unsigned char used;	     //����= 0x01 
       unsigned char type;           //0x01=��ʱ���ƣ�0x02=ͬ����0x03=�ֶ��ָ�
 				     //0x10˫��������(�����˳�)
       unsigned char in_port1;	     //��һ·���룬�˿�0-15
       union {
           unsigned char out_port;   //�����������ʱ�˿�16-19
           unsigned char in_port2;   //����˫����ʱ���ڶ�·���룬�˿�0-15
       }sec_port;
       union {
           unsigned char out_status; //0x00=�رգ�0x01=��
           unsigned char alm_op;     //b0=1 in_port1����Ԥ��״̬��b1=1 in_port2����Ԥ��״̬
			             //b2=1 in_port1��������b3=1 in_port2������
       }out_type;
       unsigned char in_status;	     //����״̬����״̬��0x00(�͵�ƽ����),0x01(�ߵ�ƽ����)
       unsigned char delay_times;    //��ʱʱ��(��),����ʱ������Ч,����Ϊ0;�����ӳ�ʱ��8λ��Ч
       unsigned char reserved[2];    //����2���ֽ�
}CTRL_ITEM;
/*
typedef struct{
       unsigned char used;
       unsigned char type;               //0x01=��ʱ���ƣ�0x02=ͬ����0x03=�ֶ��ָ�
       unsigned char in_port;
       unsigned char out_port;
       unsigned char in_status;
       unsigned char out_status;         //0x00=�رգ�0X01=��
       unsigned int  delay_time;
}CTRL_ITEM;
*/
typedef struct{
    unsigned char version;            //�汾��
    CTRL_ITEM item[LINK_CTRL_NUM]; 
}LINK_CTRL;
extern LINK_CTRL xdata __link_ctrl;

typedef struct{
    unsigned char port;
    unsigned char end_status;
    unsigned char delay_time;    
}LINK_DELAY;
extern LINK_DELAY xdata link_delay[];

/*
//���󱨴�ʩ
#define TWO_ARM_NUM          4
typedef struct{              //˫��������,��Ҫ���ں���  
    unsigned char used;      //ʹ�ñ�־ 
    unsigned char in_port1;  //����1� 
    unsigned char in_port2;  //����2� 
    unsigned char delay;     //��������������Чʱ��(��) 
//    unsigned char curr_time; //����ʱ��
    unsigned char op;	     //b0=1 in_port1Ԥ����b1=1 in_port2Ԥ��
			     //b2=1 in_port1��������b3=1 in_port2������
}TWO_ARM;
*/    

#define PORT_BUF_LEN    0x1FF
typedef struct{
    unsigned int  head;
    unsigned int  tail;
    unsigned char dat[PORT_BUF_LEN+1];
}PORT_FIFO;

#define LIGHT_CTRL_ON     0x08        //��״̬
#define LIGHT_CTRL_OFF    0x09        //��״̬
#define LIGHT_CTRL_NUM    112         //64·   @20060724 ���ӵ�112
typedef struct{
    unsigned char light_no;
    unsigned char status;
}LIGHT_CTRL_STATUS;     
extern LIGHT_CTRL_STATUS xdata light_ctrl_status[LIGHT_CTRL_NUM];

/*
#define HEAD_FLAG   0x7E
typedef struct{
    unsigned char head_flag;          //0x7E
    unsigned long Curr_Data;
    unsigned char magic_cookie;
    unsigned char check_xor;
    unsigned char status;
}METER_COUNTER;
*/

#define SCENE_MAX_NUM        10
typedef struct{
     unsigned char used;                   //0x55 =ʹ��     
     unsigned char scene[5][SCENE_MAX_NUM];  //�����ڵ��豸��ţ���5��������0=��Ч
}HJ_SCENE_CTRL;
extern HJ_SCENE_CTRL xdata hj_scene_ctrl;

extern unsigned char xdata  __bypass[];              //��·��־
extern unsigned char xdata pre_status[];
//extern unsigned char data ws5_drive;
extern bit delay_status_bit,continue_status_bit;
extern AD_PORT xdata ad_port[];             

void load_data(void);
void ctrl_init(void);
bit  port_check(unsigned char defend_type,unsigned char *port_no);
void buzzer_on(U_INT_8 channal);
void buzzer_off(U_INT_8 channal);
void delay_ctrl(void);
void continue_ctrl(void);
void input_ctrl(void);
void device_ctrl(void);
void start_ctrl(U_INT_8 Normal_Status,U_INT_8 i,U_INT_8 ctrl_type);
void stop_ctrl(U_INT_8 Normal_Status,U_INT_8 i,U_INT_8 ctrl_type);
void reload_pulse(U_INT_8 i);
void get_float(U_INT_8 i);     //������ת����С��
void output_ctrl(unsigned char status,unsigned char i);
unsigned int get_delay_time(void);
void ad_process(void);    
void init_port(void);
bit proce_ctrl(unsigned char port_no,unsigned char curr_status,unsigned char);
void send_defend_to_ccc(unsigned char defend_class);
void ad_process(void);

void play_voice(unsigned char seg);
void stop_voice(void);
void video_sw(bit flag);
void refuse_light_status(unsigned char light_no,bit status);
void proce_light_refuse(unsigned char xdata *p);
void set_all_light_status(bit flag);
void delay_sw(void);


#endif


