/**************************************************************/
/*           ϵͳ�������ͷ�ļ�SYSTEM.H                       */
/*                                                            */
/* ����: 2001/09/30				              */
/* ����: ����                                                 */
/*                                                            */
/* (c) Copyright reserved by HJKJ, 2000.                      */
/*                                                            */
/* ---------------------------------------------------------- */
/* �޸ļ�¼:		                                      */
/*    							      */   
/**************************************************************/
#ifndef _SYSTEM_
#define _SYSTEM_ 

#define __DEBUG            1           //ϵͳ���Ժͷ���ʱʹ�ã�1=���ԣ�0=����
#define __PHONE_DEBUG      0           //�绰���ֵ�������
#define SYS_CLOCK_11M      0	       //11.059Mhz		
#define SYS_CLOCK_22M      1	       //22.118Mhz	 
#define SYS_CLOCK_18M      0	       //18.432Mhz

#define SYS_LINK_CTRL      0           //������������ 

#define ALM_OPEN_LIGHT     1           //���屨������
#define NO_DEFEND_OPEN_LIGHT  1           //���峷������

#define GBK_SYSTEM         0           //GKB���Ƶƿز���
#define SHOW_SELF_FLAG     0           //��ʾ�۾ӱ�־

#define LOW_VOL_TET        0           //��ѹ���		

#define SHN_TYPE           0x04        //NDTCRT
#define INSTALL_LOCK       1           //ϵͳ�����˵�������
#define SCU_HARDWARE_VERSION     0x30  //���嵱ǰCCUӲ���İ汾��
#define SCU_SOFTWARE_VERSION     0x30  //���嵱ǰCCU����İ汾��

#define SCU_SHOW_SOFTWARE_VERSION  300  //������LCD����ʾ��ǰSCU����İ汾��
#define SCU_SHOW_HARDWARE_VERSION  300  //������LCD����ʾ��ǰSCUӲ���İ汾��


#define SCU_PROTO_VERSION  0x30  //���嵱ǰSCU�����õ�Ӧ�ò�Э��汾��
#define APP_HEAD_LEN       9     //
#define APP_TYPE_LEN       2     //���ͳ���

#define SEND_TO_CCC        0x01  //���͵�CCC 
#define SEND_TO_SCU        0x02  //���͵�SCU 
#define SEND_TO_RIU        0x04  //���͵�RIU 

#define NORMAL_TYPE        0x00  //��Ӧ����
#define QUERY_TYPE         0x40  //NDT������ѯ����
#define ALM_TYPE           0x80  //NDT������������

#define SENDER_IS_NDT      0xC0  //������ΪNDT

#define SEND_ONES          0x00   //1
//#define SEND_TWICE       0x10   //2     
//#define SEND_THRICE      0x20   //3
//#define SEND_ALWAYS      0x30   //255


//#define REPLY_SEND_TIMES   0x01  //NDT��Ӧ���ķ��ʹ���
//#define QUERY_SEND_TIMES   0x03  //NDT�������Ͳ�ѯ���Ĵ���
//#define ALM_SEND_TIMES     0xFF  //NDT�������Ͳ�ѯ���Ĵ���

typedef struct{
     unsigned char version;        //�汾��
     unsigned int  length;         //���ĳ���
     unsigned char tcbNum;         //������ƺ�
     unsigned char check_sum;      //У��
     unsigned char RIU_Addr;       //RIU��ַ
     unsigned char forward_flag;   //ת����־
     unsigned char RIU_Type;       //RIU���� 
     unsigned char reserved;       //���� 
     unsigned char type;           //����������
     unsigned char type_add;       //����������
     unsigned char buf[1];         //������
}APP_HEAD;

#endif