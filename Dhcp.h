/**************************************************************/
/*           DHCP�ͻ��˳���ͷ�ļ�����DHCP.H                     */
/*                                                            */
/* ����: 2000/11/20				                           */
/* ����: ����                                                  */
/*                                                            */
/* (c) Copyright reserved by JDKJ, 2000.                      */
/*                                                            */
/* ---------------------------------------------------------- */
/* �޸ļ�¼:		                                          */
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
     unsigned  char  op;           //����
     unsigned  char  htype;        //Ӳ������
     unsigned  char  hlen;         //Ӳ������
     unsigned  char  hops;         //����
     unsigned  char  xid[4];          //����ID
     unsigned  int   secs;         //����
     unsigned  int   flags;        //��־
     unsigned  long  ciaddr;       //�ͻ�IP��ַ
     unsigned  long  yiaddr;       //���IP��ַ 
     unsigned  long  siaddr;       //������IP��ַ
     unsigned  long  giaddr;       //����IP��ַ
     unsigned  char  chaddr[16];      //�ͻ�Ӳ����ַ
     unsigned  char  server_name[64]; //��������
     unsigned  char  boot_file[128];        //�����ļ���
     unsigned  char  other_parameter[OTHER_PARA_LEN];  //��������
} DHCP;

extern unsigned char idata __msgno;


bit  dhcp_receive(unsigned char xdata *xp);
void dhcp_send(unsigned char msg_no);
unsigned char xdata *find_field(unsigned char type, unsigned char xdata *xp);
void dhcp_task(void);

#endif
