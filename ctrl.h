#ifndef _CTRL_H_
#define _CTRL_H_	1

//sbit __alm_defend_line	= P1^3;   	/* 布防 */
//sbit __alm_light_line	= P1^5;   	/* 报警 */

#define INPUT_PORT_PHY_NUM     8      //输入物理端口数量

#define _INPUT_PORT_NUM        16      //输入软件端口数量
#define _OUTPUT_PORT_NUM       4       //输出端口数量
#define _SCU_PORT_MAX_NUM      (_INPUT_PORT_NUM + _OUTPUT_PORT_NUM)  //SCU最多端口数量
#define _MAX_METER_RANGE       999999L 

#define DEFEND_MODE_KEY            0x01    //键盘
#define DEFEND_MODE_REMOTE_CTRL    0x02    //遥控
#define DEFEND_MODE_DOORLOCK       0x03    //门锁
#define DEFEND_MODE_CENTER         0x04    //管理中心布防 
#define DEFEND_MODE_OTHER          0x05    //其他


#define ALM_VOLTAGE_VALUES         0x40    //小于10v 


#define _DOOR_DELAY	60*_SECOND

#define _CTRL_TYPE_NOT           0x00
#define _CTRL_TYPE_TIMER         0x01
#define _CTRL_TYPE_DIRECT_OFF    0x02
#define _CTRL_TYPE_DIRECT_ON     0x04

#define _CTRL_TYPE_ALM_ON        0x08
#define _CTRL_TYPE_ALM_OFF       0x10

//定义蜂鸣器通道
#define _BIT_KEY         0x01
#define _BIT_DELAY       0x02
#define _BIT_ALM         0x04
#define _BIT_CTRL        0x08
#define _BIT_ERROR       0x10 
#define _BIT_IIMER_CTRL  0x20      
#define _BIT_WARN_OFF    0x40    //预警

//操作码
#define _LOCAL_ALM      0x01     //本地报警
#define _REMOTE_ALM     0x02     //远程报警
#define _ADVANCE_CTRL   0x04     //智能控制
#define _SEND_TO_CCC    0x08     //发送到CCC
#define _BYPASS         0x10     //被旁路 

#define _RIU_NUM        40       //SCU最多RIU数量30个
#define _HEAD_LEN       5        //数据包头长度
#define _SYS_PASSWD_LEN 6
#define _INIT_PIN       "\x01\x01\x01\x01\x01\x01"
#define _SYS_PIN        "\x01\x02\x03\x04\x05\x06"
#define _USER_PIN_LEN   8

typedef struct{
   unsigned char Pass_Num;          //用户密码长度
   unsigned char Password[8];
   unsigned char Syspasswd[_SYS_PASSWD_LEN];  //系统密码
}PASSWORD;
extern PASSWORD xdata __sys_passwd;

struct S_RIU{
     unsigned char RIU_Num;
     struct{                      
          unsigned char Usable;        //可用状态
          unsigned char RIU_Type;      //类型
          unsigned char RIU_Addr;      //RIU地址
          unsigned char Curr_Status;   //当前状态
     }riu_no[_RIU_NUM];                //30
     unsigned char Security_Password;
};
extern struct S_RIU  xdata __riu;

typedef struct {
     unsigned char Curr_Status;   //量程
     unsigned int  Unitage;       //脉冲数/单位量
     unsigned long Curr_Data;     //当前读数
     unsigned char reserved[2];   //保留         
     unsigned char Curr_ad;       //当前AD值
}METER_TYPE;

typedef struct{
    unsigned char Open;           //布防开启状态，D0---D5可用于6种布防状态，D8用于永久布防
    unsigned char Op_Code;        //操作码 D0=1本地报警,D1=1远程报警,D2=1双防区,D3=1上传到CCC，D4=1被旁路
    unsigned char Normal_Status;  //正常状态 
    unsigned char Prev_Status;    //前一个状态
    unsigned char Curr_Status;    //当前状态
    unsigned int  Delay;	  //延时响应时间（秒）
    unsigned int  Respond;        //响应持续时间
    unsigned char Curr_xy;        //传感器当前位置，高四位表示横向坐标，低四位表示纵向坐标
}INPUT_TYPE;

typedef struct{
    unsigned char Normal_Status;  //正常状态 
    unsigned char Curr_Status;    //当前状态
    unsigned char Op_Status;      //操作状态
    unsigned char Op_Mode;        //操作模式，=0延时启动 
    unsigned int  Start_Time;     //延时时间（秒）OR 启动时间（分）
    unsigned int  End_Time;       //持续时间（秒）OR 结束时间（分）
    unsigned char Week;           //D0--D6表示一周七天，D8当天
    unsigned char Ctrl_Status;    //控制状态
}OUTPUT_TYPE;

#define SEND_FLAG          0x01
#define OPEN_ALM_FLAG      0x02
#define SHORT_ALM_FLAG     0x04

typedef struct{
        unsigned char Usable;        //0不可用
        unsigned char Device_Type;   //0不可用，255种设备
        unsigned char Other_Flag;    //D0=上传变化，D1=开路报警，D2=短路报警
        unsigned char Sample_Times;
        unsigned char Low_level;     //低电平
        unsigned char High_level;    //高电平
        unsigned char Voice_No;      //语音编号
        union {
            METER_TYPE  Meter;
            INPUT_TYPE  In_Port;
            OUTPUT_TYPE Out_Port;
        }type;    
}PORT;           

extern PORT xdata  __port[_SCU_PORT_MAX_NUM];

//比较器状态
#define SHORT_LEVEL      0x00        //比较器输出全0
#define OPEN_LEVEL       0x08        //比较器输出最高位为1

#define LOW_FLAG         0x01
#define HIGH_FLAG        0x02
#define SHORT_FLAG       0x04
#define OPEN_FLAG        0x08
#define ERROR_FLAG       0x10        //其他电平

#define STOP_SEND_SHORT  0x40
#define STOP_SEND_OPEN   0x80 

#define SAMPLE_MIN_TIMES 0x04		 //每个端口最小采样次数 

typedef struct{
    unsigned char curr_flag;
    unsigned char curr_times;
    unsigned char curr_level;
}AD_PORT;
/*
#define METER_MAX_NUM      12               //定义表最大数量
struct QUERY_FEE{
     unsigned char Meter_Num; 
     struct {            //费用查询
         unsigned char  Device_Type;       //设备类型
         unsigned int   Meter_data;        //表计量数据
         unsigned int   Rate_Fee;          //费率 
         unsigned int   Fee;               //收费
     }meter_no[METER_MAX_NUM];
}; 
extern struct QUERY_FEE xdata __query_fee;

typedef struct{				   //查询表读数，新增	
     unsigned char Meter_Num; 
     struct {            //费用查询
         unsigned char  Device_Type;       //设备类型
         unsigned long  Meter_data;        //表读数,(999999.99) 前六位为整数，后两位为小数  
     }meter_no[METER_MAX_NUM];
}QUERY_METER; 
extern  QUERY_METER xdata __query_meter;  //查询表读数，FROM CCC TO NDT
//add end
*/

#define MENU_MAX_LEN      30		     //菜单项数量		
#define MENU_MATNIX_LEN   MENU_MAX_LEN*10    //汉字编码空间长度
struct MENU{
    struct{
       unsigned char Used;                   //使用标记
       unsigned char Device_Type;        
       unsigned char Menu_Item_Pointer;      //开始位置
       unsigned char Menu_Length;            //字符数量
    }device[MENU_MAX_LEN];
    unsigned char  Hz_bm[MENU_MATNIX_LEN];   //汉字编码   
}; 	
extern struct MENU xdata __menu_item;
   
typedef struct{
//     unsigned char used;
     unsigned char port;
     unsigned int  ctrl_time;
}CTRL_TIME_TYPE;  
extern CTRL_TIME_TYPE idata __delay_ctrl,__continue_ctrl;

#define REPAIR_MENU_ITEM   40
#define REPAIR_MENU_MATNIX_LEN REPAIR_MENU_ITEM*10  //汉字编码空间长度
typedef struct{                                     //设备维修菜单 
    struct{
       unsigned char seq_no;                        //设备编号从1开始，没有使用填零 
       unsigned char Menu_Item_Pointer;             //开始位置
       unsigned char Menu_Length;                   //字符数量
    }device_name[REPAIR_MENU_ITEM];
    unsigned char Hz_bm[REPAIR_MENU_MATNIX_LEN];    //汉字编码
}REPAIR_MENU;
extern REPAIR_MENU xdata __repair_menu;

#define LINK_CTRL_NUM      10
#define CTRL_VERSION       0x10         //1.0            
#define DELAY_CTRL         0x01
#define SYN_CTRL           0x02
#define MAN_CTRL           0x03
#define TWO_ARM            0x10

typedef struct{
       unsigned char used;	     //可用= 0x01 
       unsigned char type;           //0x01=延时控制，0x02=同步，0x03=手动恢复
 				     //0x10双防区处理(报警滤除)
       unsigned char in_port1;	     //第一路输入，端口0-15
       union {
           unsigned char out_port;   //用于输出控制时端口16-19
           unsigned char in_port2;   //用于双防区时，第二路输入，端口0-15
       }sec_port;
       union {
           unsigned char out_status; //0x00=关闭，0x01=打开
           unsigned char alm_op;     //b0=1 in_port1触发预警状态，b1=1 in_port2触发预警状态
			             //b2=1 in_port1被触发，b3=1 in_port2被触发
       }out_type;
       unsigned char in_status;	     //输入状态触发状态，0x00(低电平触发),0x01(高电平触发)
       unsigned char delay_times;    //延时时间(秒),在延时控制有效,其他为0;防区延迟时低8位有效
       unsigned char reserved[2];    //保留2个字节
}CTRL_ITEM;
/*
typedef struct{
       unsigned char used;
       unsigned char type;               //0x01=延时控制，0x02=同步，0x03=手动恢复
       unsigned char in_port;
       unsigned char out_port;
       unsigned char in_status;
       unsigned char out_status;         //0x00=关闭，0X01=打开
       unsigned int  delay_time;
}CTRL_ITEM;
*/
typedef struct{
    unsigned char version;            //版本号
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
//防误报措施
#define TWO_ARM_NUM          4
typedef struct{              //双触发防区,主要用于红外  
    unsigned char used;      //使用标志 
    unsigned char in_port1;  //防区1� 
    unsigned char in_port2;  //防区2� 
    unsigned char delay;     //两个防区触发有效时间(秒) 
//    unsigned char curr_time; //结束时间
    unsigned char op;	     //b0=1 in_port1预警，b1=1 in_port2预警
			     //b2=1 in_port1被触发，b3=1 in_port2被触发
}TWO_ARM;
*/    

#define PORT_BUF_LEN    0x1FF
typedef struct{
    unsigned int  head;
    unsigned int  tail;
    unsigned char dat[PORT_BUF_LEN+1];
}PORT_FIFO;

#define LIGHT_CTRL_ON     0x08        //开状态
#define LIGHT_CTRL_OFF    0x09        //关状态
#define LIGHT_CTRL_NUM    112         //64路   @20060724 增加到112
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
     unsigned char used;                   //0x55 =使用     
     unsigned char scene[5][SCENE_MAX_NUM];  //场景内的设备编号，共5个场景，0=无效
}HJ_SCENE_CTRL;
extern HJ_SCENE_CTRL xdata hj_scene_ctrl;

extern unsigned char xdata  __bypass[];              //旁路标志
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
void get_float(U_INT_8 i);     //脉冲数转换成小数
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


