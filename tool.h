#ifndef _TOOL_H_
#define _TOOL_H_ 


 #define _FIFO_MASK  0x0F
 #define _FIFO_LEN   (_FIFO_MASK + 1)


 #define _PORT_BUF_NUM_MASK   0x3F                  //端口队列数量 0011 1111
 #define _PORT_BUF_LEN   (_PORT_BUF_NUM_MASK+1)     //端口缓冲长度

 #define _SAVE_EEPROM_FIFO_MASK 0x0F    //EEPROM写队列长度  0000 1111
 #define _SAVE_EEPROM_BUF_LEN   (_SAVE_EEPROM_FIFO_MASK + 1)  //EEPROM缓冲长度
 
 enum SAVE_ITEM{
      SAVE_SYS_PASSWD,
      SAVE_DEFEND,
      SAVE_PORT,
      SAVE_MENU_ITEM,
      SAVE_SYS_IP,
      SAVE_ALM_LOG,
      SAVE_DEFEND_LOG,
      SAVE_SHN_CTRL,
      SAVE_LIGHT_CTRL,
      SAVE_IR_CTRL,
      SAVE_HOUSE_PIC,	
      SAVE_HJ_SCENE,
      SAVE_USED_FLAG,
      SAVE_MAX_ITEM 
};

//每个项保存在EEPROM 1个扇区中
#define SYS_PASSWD_ROM_ADDR     (SAVE_DATA_SECTOR) 
#define SECURITY_ROM_ADDR       (SAVE_DATA_SECTOR+1)
#define PORT_ROM_ADDR           (SAVE_DATA_SECTOR+2)  
#define MENU_ROM_ADDR           (SAVE_DATA_SECTOR+3)  
#define NETIF_ROM_ADDR          (SAVE_DATA_SECTOR+4)
#define ALM_LOG_ROM_ADDR        (SAVE_DATA_SECTOR+5)
#define DEFEND_LOG_ROM_ADDR     (SAVE_DATA_SECTOR+6)
#define LINK_CTRL_ROM_ADDR      (SAVE_DATA_SECTOR+7)
#define LIGHT_CTRL_ROM_ADDR     (SAVE_DATA_SECTOR+8)
#define IR_CTRL_ROM_ADDR        (SAVE_DATA_SECTOR+9) 
#define HOUSE_PIC_ROM_ADDR      (SAVE_DATA_SECTOR+10) 
#define HJ_SCENE_ADDR           (SAVE_DATA_SECTOR+11) 
#define OTHER_SAVE_ROM_ADDR     (SAVE_DATA_SECTOR+12) 

struct FIFO {
     unsigned char  head;
     unsigned char  tail;
     unsigned char  dat[_FIFO_LEN];
};

struct EEPROM_ITEM {
     unsigned char xdata *src_add;    //源数据地址     
     unsigned int  len;
     unsigned char sector_addr;       //对应的EEPROM扇区地址
};

typedef struct{
     unsigned char head;		
     unsigned char tail;		
     unsigned char dat[_SAVE_EEPROM_BUF_LEN];
}EEPROM_WRITE_QUEUE;

#define PHONE_NO_LEN   15
typedef struct{
       unsigned char rom_used;
       unsigned char on_display;
       unsigned char no_defend_open_light;    
       unsigned char alm_open_light;
       unsigned char ic_card_no_defend;    //刷卡撤防
       unsigned char phone_pin[8];         //电话密码
       unsigned char phone_id0[PHONE_NO_LEN]; //电话号码1
       unsigned char phone_id1[PHONE_NO_LEN]; //电话号码2
       unsigned char phone_id2[PHONE_NO_LEN]; //电话号码3
       unsigned char hangup_times;            //电话自动摘机时间

}OTHER_ITEM;
extern OTHER_ITEM  xdata other_item;

void restore_from_rom(void);
void eeprom_in_queue(unsigned char item);

#endif /* _TOOL_H_ */ 
