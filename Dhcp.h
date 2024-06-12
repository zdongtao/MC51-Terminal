/**************************************************************/
/*           DHCP客户端程序头文件——DHCP.H                     */
/*                                                            */
/* 日期: 2000/11/20				                           */
/* 作者: 董涛                                                  */
/*                                                            */
/* (c) Copyright reserved by JDKJ, 2000.                      */
/*                                                            */
/* ---------------------------------------------------------- */
/* 修改记录:		                                          */
/*    							                      */   
/**************************************************************/
#ifndef  _DHCP_H_
#define  _DHCP_H_    1

#define  DHCP_CLIENT_PORT    68
#define  DHCP_SERVER_PORT    67

#define  BOOTREQUEST         1
#define  BOOTREPLY           2

#define  DHCPDISCOVER        1
#define  DHCPOFFER           2
#define  DHCPREQUEST         3
#define  DHCPDECLINE         4
#define  DHCPACK             5
#define  DHCPNACK            6
#define  DHCPRELEASE         7

#define  OTHER_PARA_LEN               64
typedef  struct 
{
     unsigned  char  op;           //操作
     unsigned  char  htype;        //硬件类型
     unsigned  char  hlen;         //硬件长度
     unsigned  char  hops;         //跳数
     unsigned  char  xid[4];          //事务ID
     unsigned  int   secs;         //秒数
     unsigned  int   flags;        //标志
     unsigned  long  ciaddr;       //客户IP地址
     unsigned  long  yiaddr;       //你的IP地址 
     unsigned  long  siaddr;       //服务器IP地址
     unsigned  long  giaddr;       //网关IP地址
     unsigned  char  chaddr[16];      //客户硬件地址
     unsigned  char  server_name[64]; //服务器名
     unsigned  char  boot_file[128];        //引导文件名
     unsigned  char  other_parameter[OTHER_PARA_LEN];  //其他参数
} DHCP;

extern unsigned char idata __msgno;


bit  dhcp_receive(unsigned char xdata *xp);
void dhcp_send(unsigned char msg_no);
unsigned char xdata *find_field(unsigned char type, unsigned char xdata *xp);
void dhcp_task(void);

#endif
