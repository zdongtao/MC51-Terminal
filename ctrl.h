#ifndef _CTRL_H_
#define _CTRL_H_	1

//sbit __alm_defend_line	= P1^3;   	/* ²¼·À */
//sbit __alm_light_line	= P1^5;   	/* ±¨¾¯ */

#define INPUT_PORT_PHY_NUM     8      //ÊäÈëÎïÀí¶Ë¿ÚÊıÁ¿

#define _INPUT_PORT_NUM        16      //ÊäÈëÈí¼ş¶Ë¿ÚÊıÁ¿
#define _OUTPUT_PORT_NUM       4       //Êä³ö¶Ë¿ÚÊıÁ¿
#define _SCU_PORT_MAX_NUM      (_INPUT_PORT_NUM + _OUTPUT_PORT_NUM)  //SCU×î¶à¶Ë¿ÚÊıÁ¿
#define _MAX_METER_RANGE       999999L 

#define DEFEND_MODE_KEY            0x01    //¼üÅÌ
#define DEFEND_MODE_REMOTE_CTRL    0x02    //Ò£¿Ø
#define DEFEND_MODE_DOORLOCK       0x03    //ÃÅËø
#define DEFEND_MODE_CENTER         0x04    //¹ÜÀíÖĞĞÄ²¼·À 
#define DEFEND_MODE_OTHER          0x05    //ÆäËû


#define ALM_VOLTAGE_VALUES         0x40    //Ğ¡ÓÚ10v 


#define _DOOR_DELAY	60*_SECOND

#define _CTRL_TYPE_NOT           0x00
#define _CTRL_TYPE_TIMER         0x01
#define _CTRL_TYPE_DIRECT_OFF    0x02
#define _CTRL_TYPE_DIRECT_ON     0x04

#define _CTRL_TYPE_ALM_ON        0x08
#define _CTRL_TYPE_ALM_OFF       0x10

//¶¨Òå·äÃùÆ÷Í¨µÀ
#define _BIT_KEY         0x01
#define _BIT_DELAY       0x02
#define _BIT_ALM         0x04
#define _BIT_CTRL        0x08
#define _BIT_ERROR       0x10 
#define _BIT_IIMER_CTRL  0x20      
#define _BIT_WARN_OFF    0x40    //Ô¤¾¯

//²Ù×÷Âë
#define _LOCAL_ALM      0x01     //±¾µØ±¨¾¯
#define _REMOTE_ALM     0x02     //Ô¶³Ì±¨¾¯
#define _ADVANCE_CTRL   0x04     //ÖÇÄÜ¿ØÖÆ
#define _SEND_TO_CCC    0x08     //·¢ËÍµ½CCC
#define _BYPASS         0x10     //±»ÅÔÂ· 

#define _RIU_NUM        40       //SCU×î¶àRIUÊıÁ¿30¸ö
#define _HEAD_LEN       5        //Êı¾İ°üÍ·³¤¶È
#define _SYS_PASSWD_LEN 6
#define _INIT_PIN       "\x01\x01\x01\x01\x01\x01"
#define _SYS_PIN        "\x01\x02\x03\x04\x05\x06"
#define _USER_PIN_LEN   8

typedef struct{
   unsigned char Pass_Num;          //ÓÃ»§ÃÜÂë³¤¶È
   unsigned char Password[8];
   unsigned char Syspasswd[_SYS_PASSWD_LEN];  //ÏµÍ³ÃÜÂë
}PASSWORD;
extern PASSWORD xdata __sys_passwd;

struct S_RIU{
     unsigned char RIU_Num;
     struct{                      
          unsigned char Usable;        //¿ÉÓÃ×´Ì¬
          unsigned char RIU_Type;      //ÀàĞÍ
          unsigned char RIU_Addr;      //RIUµØÖ·
          unsigned char Curr_Status;   //µ±Ç°×´Ì¬
     }riu_no[_RIU_NUM];                //30
     unsigned char Security_Password;
};
extern struct S_RIU  xdata __riu;

typedef struct {
     unsigned char Curr_Status;   //Á¿³Ì
     unsigned int  Unitage;       //Âö³åÊı/µ¥Î»Á¿
     unsigned long Curr_Data;     //µ±Ç°¶ÁÊı
     unsigned char reserved[2];   //±£Áô         
     unsigned char Curr_ad;       //µ±Ç°ADÖµ
}METER_TYPE;

typedef struct{
    unsigned char Open;           //²¼·À¿ªÆô×´Ì¬£¬D0---D5¿ÉÓÃÓÚ6ÖÖ²¼·À×´Ì¬£¬D8ÓÃÓÚÓÀ¾Ã²¼·À
    unsigned char Op_Code;        //²Ù×÷Âë D0=1±¾µØ±¨¾¯,D1=1Ô¶³Ì±¨¾¯,D2=1Ë«·ÀÇø,D3=1ÉÏ´«µ½CCC£¬D4=1±»ÅÔÂ·
    unsigned char Normal_Status;  //Õı³£×´Ì¬ 
    unsigned char Prev_Status;    //Ç°Ò»¸ö×´Ì¬
    unsigned char Curr_Status;    //µ±Ç°×´Ì¬
    unsigned int  Delay;	  //ÑÓÊ±ÏìÓ¦Ê±¼ä£¨Ãë£©
    unsigned int  Respond;        //ÏìÓ¦³ÖĞøÊ±¼ä
    unsigned char Curr_xy;        //´«¸ĞÆ÷µ±Ç°Î»ÖÃ£¬¸ßËÄÎ»±íÊ¾ºáÏò×ø±ê£¬µÍËÄÎ»±íÊ¾×İÏò×ø±ê
}INPUT_TYPE;

typedef struct{
    unsigned char Normal_Status;  //Õı³£×´Ì¬ 
    unsigned char Curr_Status;    //µ±Ç°×´Ì¬
    unsigned char Op_Status;      //²Ù×÷×´Ì¬
    unsigned char Op_Mode;        //²Ù×÷Ä£Ê½£¬=0ÑÓÊ±Æô¶¯ 
    unsigned int  Start_Time;     //ÑÓÊ±Ê±¼ä£¨Ãë£©OR Æô¶¯Ê±¼ä£¨·Ö£©
    unsigned int  End_Time;       //³ÖĞøÊ±¼ä£¨Ãë£©OR ½áÊøÊ±¼ä£¨·Ö£©
    unsigned char Week;           //D0--D6±íÊ¾Ò»ÖÜÆßÌì£¬D8µ±Ìì
    unsigned char Ctrl_Status;    //¿ØÖÆ×´Ì¬
}OUTPUT_TYPE;

#define SEND_FLAG          0x01
#define OPEN_ALM_FLAG      0x02
#define SHORT_ALM_FLAG     0x04

typedef struct{
        unsigned char Usable;        //0²»¿ÉÓÃ
        unsigned char Device_Type;   //0²»¿ÉÓÃ£¬255ÖÖÉè±¸
        unsigned char Other_Flag;    //D0=ÉÏ´«±ä»¯£¬D1=¿ªÂ·±¨¾¯£¬D2=¶ÌÂ·±¨¾¯
        unsigned char Sample_Times;
        unsigned char Low_level;     //µÍµçÆ½
        unsigned char High_level;    //¸ßµçÆ½
        unsigned char Voice_No;      //ÓïÒô±àºÅ
        union {
            METER_TYPE  Meter;
            INPUT_TYPE  In_Port;
            OUTPUT_TYPE Out_Port;
        }type;    
}PORT;           

extern PORT xdata  __port[_SCU_PORT_MAX_NUM];

//±È½ÏÆ÷×´Ì¬
#define SHORT_LEVEL      0x00        //±È½ÏÆ÷Êä³öÈ«0
#define OPEN_LEVEL       0x08        //±È½ÏÆ÷Êä³ö×î¸ßÎ»Îª1

#define LOW_FLAG         0x01
#define HIGH_FLAG        0x02
#define SHORT_FLAG       0x04
#define OPEN_FLAG        0x08
#define ERROR_FLAG       0x10        //ÆäËûµçÆ½

#define STOP_SEND_SHORT  0x40
#define STOP_SEND_OPEN   0x80 

#define SAMPLE_MIN_TIMES 0x04		 //Ã¿¸ö¶Ë¿Ú×îĞ¡²ÉÑù´ÎÊı 

typedef struct{
    unsigned char curr_flag;
    unsigned char curr_times;
    unsigned char curr_level;
}AD_PORT;
/*
#define METER_MAX_NUM      12               //¶¨Òå±í×î´óÊıÁ¿
struct QUERY_FEE{
     unsigned char Meter_Num; 
     struct {            //·ÑÓÃ²éÑ¯
         unsigned char  Device_Type;       //Éè±¸ÀàĞÍ
         unsigned int   Meter_data;        //±í¼ÆÁ¿Êı¾İ
         unsigned int   Rate_Fee;          //·ÑÂÊ 
         unsigned int   Fee;               //ÊÕ·Ñ
     }meter_no[METER_MAX_NUM];
}; 
extern struct QUERY_FEE xdata __query_fee;

typedef struct{				   //²éÑ¯±í¶ÁÊı£¬ĞÂÔö	
     unsigned char Meter_Num; 
     struct {            //·ÑÓÃ²éÑ¯
         unsigned char  Device_Type;       //Éè±¸ÀàĞÍ
         unsigned long  Meter_data;        //±í¶ÁÊı,(999999.99) Ç°ÁùÎ»ÎªÕûÊı£¬ºóÁ½Î»ÎªĞ¡Êı  
     }meter_no[METER_MAX_NUM];
}QUERY_METER; 
extern  QUERY_METER xdata __query_meter;  //²éÑ¯±í¶ÁÊı£¬FROM CCC TO NDT
//add end
*/

#define MENU_MAX_LEN      30		     //²Ëµ¥ÏîÊıÁ¿		
#define MENU_MATNIX_LEN   MENU_MAX_LEN*10    //ºº×Ö±àÂë¿Õ¼ä³¤¶È
struct MENU{
    struct{
       unsigned char Used;                   //Ê¹ÓÃ±ê¼Ç
       unsigned char Device_Type;        
       unsigned char Menu_Item_Pointer;      //¿ªÊ¼Î»ÖÃ
       unsigned char Menu_Length;            //×Ö·ûÊıÁ¿
    }device[MENU_MAX_LEN];
    unsigned char  Hz_bm[MENU_MATNIX_LEN];   //ºº×Ö±àÂë   
}; 	
extern struct MENU xdata __menu_item;
   
typedef struct{
//     unsigned char used;
     unsigned char port;
     unsigned int  ctrl_time;
}CTRL_TIME_TYPE;  
extern CTRL_TIME_TYPE idata __delay_ctrl,__continue_ctrl;

#define REPAIR_MENU_ITEM   40
#define REPAIR_MENU_MATNIX_LEN REPAIR_MENU_ITEM*10  //ºº×Ö±àÂë¿Õ¼ä³¤¶È
typedef struct{                                     //Éè±¸Î¬ĞŞ²Ëµ¥ 
    struct{
       unsigned char seq_no;                        //Éè±¸±àºÅ´Ó1¿ªÊ¼£¬Ã»ÓĞÊ¹ÓÃÌîÁã 
       unsigned char Menu_Item_Pointer;             //¿ªÊ¼Î»ÖÃ
       unsigned char Menu_Length;                   //×Ö·ûÊıÁ¿
    }device_name[REPAIR_MENU_ITEM];
    unsigned char Hz_bm[REPAIR_MENU_MATNIX_LEN];    //ºº×Ö±àÂë
}REPAIR_MENU;
extern REPAIR_MENU xdata __repair_menu;

#define LINK_CTRL_NUM      10
#define CTRL_VERSION       0x10         //1.0            
#define DELAY_CTRL         0x01
#define SYN_CTRL           0x02
#define MAN_CTRL           0x03
#define TWO_ARM            0x10

typedef struct{
       unsigned char used;	     //¿ÉÓÃ= 0x01 
       unsigned char type;           //0x01=ÑÓÊ±¿ØÖÆ£¬0x02=Í¬²½£¬0x03=ÊÖ¶¯»Ö¸´
 				     //0x10Ë«·ÀÇø´¦Àí(±¨¾¯ÂË³ı)
       unsigned char in_port1;	     //µÚÒ»Â·ÊäÈë£¬¶Ë¿Ú0-15
       union {
           unsigned char out_port;   //ÓÃÓÚÊä³ö¿ØÖÆÊ±¶Ë¿Ú16-19
           unsigned char in_port2;   //ÓÃÓÚË«·ÀÇøÊ±£¬µÚ¶şÂ·ÊäÈë£¬¶Ë¿Ú0-15
       }sec_port;
       union {
           unsigned char out_status; //0x00=¹Ø±Õ£¬0x01=´ò¿ª
           unsigned char alm_op;     //b0=1 in_port1´¥·¢Ô¤¾¯×´Ì¬£¬b1=1 in_port2´¥·¢Ô¤¾¯×´Ì¬
			             //b2=1 in_port1±»´¥·¢£¬b3=1 in_port2±»´¥·¢
       }out_type;
       unsigned char in_status;	     //ÊäÈë×´Ì¬´¥·¢×´Ì¬£¬0x00(µÍµçÆ½´¥·¢),0x01(¸ßµçÆ½´¥·¢)
       unsigned char delay_times;    //ÑÓÊ±Ê±¼ä(Ãë),ÔÚÑÓÊ±¿ØÖÆÓĞĞ§,ÆäËûÎª0;·ÀÇøÑÓ³ÙÊ±µÍ8Î»ÓĞĞ§
       unsigned char reserved[2];    //±£Áô2¸ö×Ö½Ú
}CTRL_ITEM;
/*
typedef struct{
       unsigned char used;
       unsigned char type;               //0x01=ÑÓÊ±¿ØÖÆ£¬0x02=Í¬²½£¬0x03=ÊÖ¶¯»Ö¸´
       unsigned char in_port;
       unsigned char out_port;
       unsigned char in_status;
       unsigned char out_status;         //0x00=¹Ø±Õ£¬0X01=´ò¿ª
       unsigned int  delay_time;
}CTRL_ITEM;
*/
typedef struct{
    unsigned char version;            //°æ±¾ºÅ
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
//·ÀÎó±¨´ëÊ©
#define TWO_ARM_NUM          4
typedef struct{              //Ë«´¥·¢·ÀÇø,Ö÷ÒªÓÃÓÚºìÍâ  
    unsigned char used;      //Ê¹ÓÃ±êÖ¾ 
    unsigned char in_port1;  //·ÀÇø1» 
    unsigned char in_port2;  //·ÀÇø2» 
    unsigned char delay;     //Á½¸ö·ÀÇø´¥·¢ÓĞĞ§Ê±¼ä(Ãë) 
//    unsigned char curr_time; //½áÊøÊ±¼ä
    unsigned char op;	     //b0=1 in_port1Ô¤¾¯£¬b1=1 in_port2Ô¤¾¯
			     //b2=1 in_port1±»´¥·¢£¬b3=1 in_port2±»´¥·¢
}TWO_ARM;
*/    

#define PORT_BUF_LEN    0x1FF
typedef struct{
    unsigned int  head;
    unsigned int  tail;
    unsigned char dat[PORT_BUF_LEN+1];
}PORT_FIFO;

#define LIGHT_CTRL_ON     0x08        //¿ª×´Ì¬
#define LIGHT_CTRL_OFF    0x09        //¹Ø×´Ì¬
#define LIGHT_CTRL_NUM    112         //64Â·   @20060724 Ôö¼Óµ½112
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
     unsigned char used;                   //0x55 =Ê¹ÓÃ     
     unsigned char scene[5][SCENE_MAX_NUM];  //³¡¾°ÄÚµÄÉè±¸±àºÅ£¬¹²5¸ö³¡¾°£¬0=ÎŞĞ§
}HJ_SCENE_CTRL;
extern HJ_SCENE_CTRL xdata hj_scene_ctrl;

extern unsigned char xdata  __bypass[];              //ÅÔÂ·±êÖ¾
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
void get_float(U_INT_8 i);     //Âö³åÊı×ª»»³ÉĞ¡Êı
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


