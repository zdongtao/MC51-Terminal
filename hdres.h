#ifndef _HDRES_H_
#define _HDRES_H_ 

 #define CRTDISPLAY_ON		1
 #define CRTDISPLAY_OFF		0

 #define LED_ON			1
 #define LED_OFF		0

 #define LOCK			0
 #define UNLOCK			1

 #define WAITING_RING		1
// #define SWITCH_INTER_TALK	1

 #define SCENS                  0
 #define SHOW_CHAR              1

 
 #define VOICE_ON		1        //2003.8.12 硬件改动
 #define VOICE_OFF		0

 #define BUZZER_OFF		(XBYTE[0xD700]=0)
 #define BUZZER_ON		(XBYTE[0xD700]=1)
 #define LCD_POWER_ON           (XBYTE[0xDD00]=1);(lcd_display_status=CRTDISPLAY_ON)                                    
 #define LCD_POWER_OFF          (XBYTE[0xDD00]=0);(lcd_display_status=CRTDISPLAY_OFF)                                      
 #define SCREEN_SWITCH_CHAR     (XBYTE[0xDC00]=1);//(lcd_sw_status=0)
 #define SCREEN_SWITCH_VIDEO    (XBYTE[0xDC00]=0);//(lcd_sw_status=1)

// #define SCREEN_SWITCH_CHAR     XBYTE[0xDE00]=1;delay_sw();XBYTE[0xDC00]=1//(lcd_sw_status=0)
// #define SCREEN_SWITCH_VIDEO    XBYTE[0xDC00]=0;delay_sw();XBYTE[0xDE00]=0//(lcd_sw_status=1)

// #define SCREEN_SWITCH_CHAR     (XBYTE[0xDC00]=3);(lcd_sw_status=0) //for展板2004/10/12
// #define SCREEN_SWITCH_VIDEO    (XBYTE[0xDC00]=0);(lcd_sw_status=1)
 
 #define CLOSE_LOCK             _nop_();//P14=0;//(XBYTE[0xDA00]=1)  
 #define OPEN_LOCK              _nop_();//P14=1;//(XBYTE[0xDA00]=0)  

 #define SWITCH_INTER_TALK      (XBYTE[0xD300]=1)  
 #define WAITING_BELL           (XBYTE[0xD300]=0)  

 #define ALM_LIGHT_ON           iic_send_in_msgno(MSG_HOST_ONALM)        
 #define ALM_LIGHT_OFF          iic_send_in_msgno(MSG_HOST_OFFALM)        
  
 #define DEFEND_LIGHT_ON        iic_send_in_msgno(MSG_HOST_ONDEF) //(XBYTE[0xDF00]=0)        //10.15
 #define DEFEND_LIGHT_OFF       iic_send_in_msgno(MSG_HOST_OFFDEF)  

// #define INT_STATUS_REG         0xF400         //INT0 中断状态寄存器      
// #define CLR_INT_REG            0xDE00         //请中断INT0 中断状态寄存器       

 #define BRIGHT_UP              XBYTE[0xE000]
 #define BRIGHT_DOWN            XBYTE[0xE100]

 #define VOICE_ADJ_UP           XBYTE[0xDE00]
 #define VOICE_ADJ_DOWN         XBYTE[0xDF00]

 //摘机控制 
 #define PHONE_CTRL_ON          P10=1//XBYTE[0xE500]=1  
 #define PHONE_CTRL_OFF         P10=0//XBYTE[0xE500]=0  

 //声音控制
 #define MC34119_CD_ON          XBYTE[0xE300]=1  
 #define MC34119_CD_OFF         XBYTE[0xE300]=0  

 
 //SPI INTERFACE
 #define  ADS7846_CS            T1
 #define  SCK                   P17
// #define  SI                    P15
// #define  SO                    P16
 #define  ISD_4000_CS_L         P11 = 0   //P40=0
 #define  ISD_4000_CS_H         P11 = 1   //P40=1
// #define  ISD_4000_INT          P13
   
 #define RS485_CTRL             T0
 #define WATCHDOG		(WDTC = 0x1B)
 #define NIC_RESET_PIN          XBYTE[0xEA00]  //T1
// #define POWER_LOW		T0

 #define OUT_PORT               0xD000

 #define OUT_PORT0              0xD000
 #define OUT_PORT1              0xD100
 #define OUT_PORT2              0xD200
 #define OUT_PORT3              0xD300

 #define AD_CHANNEL             0xD900
 #define READ_AD                0xF000

 //语音段选地址
 #define VOICE_SEG              0xD800               
 #define VOICE_IRP              0xD400               
 //vram page choose add
 #define VRAM_ADDR              0x8000
 #define VRAM_PAGE_ADDR         0xD600
 #define EEPROM_PAGE_ADDR       0xD500 

 #define TEAR_ADD               0xF100

 //定义EEPROM存储结构
 #define EEPROM_ADDR            0xA000 

 #define ASCII_START_ADDR       EEPROM_ADDR //定义ASCII码开始地址
 #define ASCII_LENGTH           4096        //定义ASCII码长度

 #define EEPROM_HZK_ADDR        EEPROM_ADDR 
 #define HZK_START_ADDR         0x0000      //汉字库开始地址 

 #define ICON_ADDR_PAGE         (0x70/2)

 #define COMPANY_FLAG_PAGE      (0x7C/2)       //页地址
 #define COMPANY_FLAG_ADDR      (EEPROM_ADDR+0x0000)     //公司标志开始地址

 #define SYSTEM_DATA_PAGE       (0x7E/2)       //页地址
 #define SYSTEM_DATA            (&XBYTE[EEPROM_ADDR])         
 #define INIT_FLAG              (EEPROM_ADDR+0x1F00)    /* 系统初始化标志，当第一次启动时不从EEPROM中恢复数据 */
 #define SYSDATA_GOOD           0xA5 


 #define TEST_ROM_PAGE          (0x7E/2)       //出厂测试区 
 #define TEST_ROM_ADDR          (EEPROM_ADDR+0x0000)     //0x7F000
 

 //键盘地址
 #define KEYBOARD_IN_ADDR       0xF200
// #define KEYBOARD_OUT_ADDR      0xE900

 //遥控地址
 #define REMOTE_CTRL_ADDR       0xF300
 //防撤开关
 #define TEAR_SW_ADDR           0xF100
 //MT8888地址空间
 #define MT8888_ADDR            0xA000
 #define MT8888_RS0_ADDR        (0xA000|0x100)

 //定义EEPROM存储结构
 #define ASCII_DATA_ADDR        0x6F000 

 #define ICON_DATA_ADDR         0x70000


 #define SECTOR_SIZE            4096 
 #define SAVE_DATA_ADDR         0x73000
 #define SAVE_DATA_SECTOR       (SAVE_DATA_ADDR/SECTOR_SIZE)    


#define STC89C_58RD             0   //stc 89c58rd+
#define SST89E58RD2             1   //sst 89e58rd2

/*  P1  */
sbit P10 = P1^0;
sbit P11 = P1^1;
sbit P12 = P1^2;
sbit P13 = P1^3;
sbit P14 = P1^4;
sbit P15 = P1^5;
sbit P16 = P1^6;
sbit P17 = P1^7;

#if SST89E58RD2==1
/*  89x5xxRD2 Extensions */
   sfr IPA   = 0xF8;
   sfr IEA   = 0xE8;
   sfr SADDR = 0xA9;
   sfr SADEN = 0xB9;
   sfr SPSR  = 0xAA;
   sfr AUXR1 = 0xA2;
   sfr AUXR  = 0x8E;
   sfr WDTD  = 0x85;
   sfr WDTC  = 0xC0;
   sfr SPDR  = 0x86;
   sfr SFCF  = 0xB1;
   sfr SFCM  = 0xB2;
   sfr SFAL  = 0xB3;
   sfr SFAH  = 0xB4;
   sfr SFDT  = 0xB5;
   sfr SFST  = 0xB6;
   sfr IPH   = 0xB7;
   sfr SPCR  = 0xD5;
   sfr IPAH  = 0xF7;
   sfr P4    = 0xA5;
   sfr XICON = 0xAE;
#endif


#if STC89C_58RD==1
  sfr AUXR = 0x8e;
  sfr AUXR1 = 0xa2;
  sfr XICON = 0xc0;
  sfr IPH = 0xb7;
  sfr P4 = 0xe8;
  sfr WDT_CONTR = 0xe1;
  sfr ISP_DATA =  0xe2;
  sfr ISP_ADDRH = 0xe3;
  sfr ISP_ADDRL = 0xe4;
  sfr ISP_CMD =   0xe5;
  sfr ISP_TRIG =  0xe6;
  sfr ISP_CONTR = 0xe7;

  /* XICON */
  sbit PX3    = XICON^7;
  sbit EX3    = XICON^6;
  sbit IE3    = XICON^5;
  sbit IT3    = XICON^4;
  sbit PX2    = XICON^3;
  sbit EX2    = XICON^2;
  sbit IE2    = XICON^1;
  sbit IT2    = XICON^0;
 
  sbit P40 = 0xe8;
  sbit P41 = 0xe9;
  sbit P42 = 0xea;
  sbit P43 = 0xeb;
  // P4 
  sbit INT2    = P4^3;
  sbit INT3    = P4^2;

#endif


#endif
