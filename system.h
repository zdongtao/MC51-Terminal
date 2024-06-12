/**************************************************************/
/*           系统定义程序头文件SYSTEM.H                       */
/*                                                            */
/* 日期: 2001/09/30				              */
/* 作者: 董涛                                                 */
/*                                                            */
/* (c) Copyright reserved by HJKJ, 2000.                      */
/*                                                            */
/* ---------------------------------------------------------- */
/* 修改记录:		                                      */
/*    							      */   
/**************************************************************/
#ifndef _SYSTEM_
#define _SYSTEM_ 

#define __DEBUG            1           //系统调试和发行时使用，1=调试，0=发行
#define __PHONE_DEBUG      0           //电话部分调试条件
#define SYS_CLOCK_11M      0	       //11.059Mhz		
#define SYS_CLOCK_22M      1	       //22.118Mhz	 
#define SYS_CLOCK_18M      0	       //18.432Mhz

#define SYS_LINK_CTRL      0           //定义联动控制 

#define ALM_OPEN_LIGHT     1           //定义报警开灯
#define NO_DEFEND_OPEN_LIGHT  1           //定义撤防开灯

#define GBK_SYSTEM         0           //GKB定制灯控部分
#define SHOW_SELF_FLAG     0           //显示慧居标志

#define LOW_VOL_TET        0           //电压检测		

#define SHN_TYPE           0x04        //NDTCRT
#define INSTALL_LOCK       1           //系统定义了电子门锁
#define SCU_HARDWARE_VERSION     0x30  //定义当前CCU硬件的版本号
#define SCU_SOFTWARE_VERSION     0x30  //定义当前CCU软件的版本号

#define SCU_SHOW_SOFTWARE_VERSION  300  //定义在LCD上显示当前SCU软件的版本号
#define SCU_SHOW_HARDWARE_VERSION  300  //定义在LCD上显示当前SCU硬件的版本号


#define SCU_PROTO_VERSION  0x30  //定义当前SCU所采用的应用层协议版本号
#define APP_HEAD_LEN       9     //
#define APP_TYPE_LEN       2     //类型长度

#define SEND_TO_CCC        0x01  //发送到CCC 
#define SEND_TO_SCU        0x02  //发送到SCU 
#define SEND_TO_RIU        0x04  //发送到RIU 

#define NORMAL_TYPE        0x00  //响应报文
#define QUERY_TYPE         0x40  //NDT主动查询报文
#define ALM_TYPE           0x80  //NDT主动报警报文

#define SENDER_IS_NDT      0xC0  //发送者为NDT

#define SEND_ONES          0x00   //1
//#define SEND_TWICE       0x10   //2     
//#define SEND_THRICE      0x20   //3
//#define SEND_ALWAYS      0x30   //255


//#define REPLY_SEND_TIMES   0x01  //NDT响应报文发送次数
//#define QUERY_SEND_TIMES   0x03  //NDT主动发送查询报文次数
//#define ALM_SEND_TIMES     0xFF  //NDT主动发送查询报文次数

typedef struct{
     unsigned char version;        //版本号
     unsigned int  length;         //报文长度
     unsigned char tcbNum;         //传输控制号
     unsigned char check_sum;      //校验
     unsigned char RIU_Addr;       //RIU地址
     unsigned char forward_flag;   //转发标志
     unsigned char RIU_Type;       //RIU类型 
     unsigned char reserved;       //保留 
     unsigned char type;           //主报文类型
     unsigned char type_add;       //主报文类型
     unsigned char buf[1];         //数据区
}APP_HEAD;

#endif