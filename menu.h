#ifndef _MENU_
#define _MENU_ 1

#define POS_X_MAX	    4	
#define POS_Y_MAX	    0x40   //��4λ��Ч	

#define REQUEST_BUF_LEN      100
typedef struct{
      unsigned char len;
      unsigned char riu_addr;
      unsigned char dat[REQUEST_BUF_LEN];
}RESEND_BUF; 

#define ALM_INFO_LEN  9
typedef struct{
      unsigned char ptr;
      struct{
        unsigned char x;
        unsigned char y;
        unsigned char device_type;
      }alm[ALM_INFO_LEN];
}ALM_INFO;
extern ALM_INFO xdata alm_info;

//���⣬���߿��Ʋ˵�
#define IR_MAIN_MENU_HZ_LEN 12
#define IR_SUB_MENU_HZ_LEN  6
struct IR_MAIN_MENU{      
      unsigned char hz[IR_MAIN_MENU_HZ_LEN];      //main menu
      unsigned int  ptr;        //�Ӳ˵���ַ
      unsigned char num;        //�Ӳ˵�����
};

struct IR_SUB_MENU{      
      unsigned char hz[IR_SUB_MENU_HZ_LEN];      //sub menu
      unsigned int  ptr;        //��������ַ
      unsigned char code_len;   //������볤��
      unsigned char device_no;  //�豸��ţ��˽ṹ���豸���Ψһ
      unsigned char riu_addr;	//RIU��ַ
};

struct IR_MENU{
      unsigned char used;            //0x55 
      unsigned char main_item_num;   //���˵�����
      unsigned char sub_item_num;    //�Ӳ˵�����
      unsigned char reserved[2];
      struct IR_MAIN_MENU main_menu[1];
      struct IR_SUB_MENU  sub_menu[1];      
      unsigned char ir_buf[4*256-5-sizeof(struct IR_MAIN_MENU)-sizeof(struct IR_SUB_MENU)];
};
extern struct IR_MENU xdata ir_ctrl;

#define  LIGHT_CTRL_TYPE      0x01
#define  AIR_CTRL_TYPE        0x02
#define  WINDOW_CTRL_TYPE     0x03
#define  ELEC_CTRL_TYPE       0x04
#define  OTHER_CTRL_TYPE      0x10
struct LIGHT_TYPE{  
      unsigned char light_name[12];        //���6������, δ����0
      unsigned char light_addr;            //��������ַ
      unsigned char light_no;              //�ƺ�
      unsigned char light_type;            //�豸���ͣ�1�ƹ⣬2�յ���3������4������10����
};
struct LIGHT_CTRL_MENU{                    //�˽ṹȫ��<=1024���ֽ�
      unsigned char used;                  //0x55 
      unsigned char light_num;             //������
      struct LIGHT_TYPE light_type[1];
      unsigned char fill[1024*2-sizeof(struct LIGHT_TYPE)-2];//���,@20060724�������ӵ�2k,112���ƿ��豸
};
extern struct LIGHT_CTRL_MENU xdata light_ctrl;


#define _TRUE		    1
#define _FALSE		    0
		
#define START_MENU          0
#define SCREEN_SAVE_MENU    1
#define MAIN_MENU           5
#define SUB_MENU_1          6
#define SUB_MENU_2          7 
#define SUB_MENU_3          8   //no 
//#define SHOW_MESSAGE        9 

#define FLASH_MESSAGE    0x01
#define FLASH_WEEK       0x02
#define FLASH_DEFEND     0x04

extern U_INT_8  idata __menu_layer;    //�˵����־  
extern bit __flash_msgno;

void  do_show_meter_data(void);
void  do_modify_meter_data(unsigned char show_type);
void  defend_save_and_light(bit save_flag, unsigned char defend_class,unsigned char mode);
unsigned char xdata *get_config_menu(U_INT_8 Device_type,U_INT_8 data *len);
bit wait_reply(void);
void  do_repair_set(void);
void  other_task(void);
void  no_alm(unsigned char type);
void  do_help(unsigned char type);
void light_ctrl_op(void);
void phone_dev_ctrl(unsigned char dev_type,unsigned char sn,unsigned char op);

#endif