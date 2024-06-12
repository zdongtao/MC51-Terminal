/**************************************************************************/
/*                 DHCP�ͻ��˳��򡪡�DHCP.C                               */
/*              					                  */
/* ����: 1. ��CCC����������ȡNDT�Լ���IP��ַ�����ص�ַ,�������룬�㲥��ַ */
/*          �������룬�㲥��ַ		 		                  */
/* ����: 2000/11/20				                          */
/* ����: ����                                                             */
/*                                                                        */
/* (c) Copyright reserved by JDKJ, 2000.                                  */
/*------------------------------------------------------------------------*/
/* �޸ļ�¼:		                                                  */
/*    							                  */   
/**************************************************************************/
#include <reg51.h>
#include <string.h>
#include <stdlib.h>
#include "inc/ip.h"
#include "inc/arp.h"
#include "inc/udp.h"   
#include "inc/dhcp.h"
#include "inc/tool.h"
#include "inc/skbuff.h"
#include "inc/mac.h"
#include "inc/eth.h"


unsigned char xdata __server_ip[4];
static unsigned char xdata __local_ip[4];
unsigned char idata __msgno = DHCPDISCOVER;

unsigned int xdata  seconds;
/**************************************************************************/
/*                 dhcp_send  DHCP������װ����                            */
/* ������msg_no������ǰDHCP���ĵ���Ϣ��                                   */   
/* ���أ���                                                               */     
/* ���ڣ�2000/11/20                                                       */      
/*------------------------------------------------------------------------*/
/* �޸ļ�¼:		                                                  */
/*    							                  */   
/**************************************************************************/
void dhcp_send(unsigned char msg_no)
{
     DHCP xdata *pDhcp; 
     struct sk_buff xdata *skb;

     skb = alloc_skb();
     udp_skb_reserve(skb);
     skb_put(skb, sizeof(DHCP));         
     pDhcp =(DHCP *)skb->dat;
     memset(pDhcp,0x00,sizeof(DHCP));    //��ʼ��������
     pDhcp->op    = BOOTREQUEST; 
     pDhcp->htype = 1;
     pDhcp->hlen  = 6;
     pDhcp->secs = seconds;
     memcpy(pDhcp->xid, r8019_eth_addr+2, 4);
     memcpy(pDhcp->chaddr, r8019_eth_addr, 6);
     memcpy(pDhcp->other_parameter, "\x63\x82\x53\x63", 4);        
     memcpy(pDhcp->other_parameter + 7, "\x3D\x7\x01", 3);
     memcpy(pDhcp->other_parameter + 7 + 3, r8019_eth_addr, 6);
     switch(msg_no)          
     {
         case  DHCPDISCOVER:
               memcpy(pDhcp->other_parameter + 4, "\x35\x1\x1", 3); //MESSAGE NO                               
               pDhcp->other_parameter[10+6] = 0xFF;                 //END
               memcpy(pDhcp->other_parameter+10+6+1,"NDT_310",7);
               seconds += 0x1000;                     //4096ms
               break;
         case  DHCPREQUEST:
               memcpy(pDhcp->other_parameter + 4, "\x35\x1\x3", 3);
               memcpy(pDhcp->other_parameter + 10 + 6, "\x32\x04", 2); 
               memcpy(pDhcp->other_parameter + 10 + 6 + 2, __local_ip, 4); 
               memcpy(pDhcp->other_parameter + 10 + 6 + 2+4,"\x36\x4",2);
               memcpy(pDhcp->other_parameter + 10 + 6 + 4+4, __server_ip, 4); 
               pDhcp->other_parameter[10+6+4+4+4] = 0xFF;  
               memcpy(pDhcp->other_parameter+10+6+4+4+4+1,"NDT_310",7);
               break;                 
         default:
               __msgno = DHCPDISCOVER;
               return;                                                     
     }
     udp_server.des_addr = 0xFFFFFFFF;
     udp_server.src_addr = 0x00000000;
     udp_server.des_port = DHCP_SERVER_PORT;
     udp_server.src_port = DHCP_CLIENT_PORT;
     udp_send(skb);	
}

/**************************************************************************/
/*                 dhcp_receive  DHCP���Ľ���                             */
/* ������xp����ǰ���Ļ�������ַ                                      */   
/* ���أ�0����DHCP����ɹ���1��������ʧ��                                 */     
/* ���ڣ�2000/11/20                                                       */      
/*------------------------------------------------------------------------*/
/* �޸ļ�¼:		                                                  */
/*   ���Σ�2001/09/11�Զ����������ӵ����ź�3���ֽ�                      */
/*        ����ϵͳIP��ַ��SCU�� 				          */   
/**************************************************************************/
bit dhcp_receive(unsigned char xdata *xp)
{     
     unsigned char xdata *pOther_para;
     DHCP xdata *pDhcp; 

     pDhcp= (DHCP *)xp ;
     pOther_para = pDhcp->other_parameter;
     if(memcmp(pDhcp->chaddr, r8019_eth_addr,6)==0) 
     {
         if(memcmp(pOther_para, "\x63\x82\x53\x63", 4))    //DHCP Magic Number ����
            return 0;        
         pOther_para += 4;
         xp = find_field(0xFF, pOther_para);    //END
         if(xp==NULL)
            return 0;    
         if(memcmp(xp,"SERVER",6))            //NDT DHCP SERVER FLAG
            return 0;                    

         xp = find_field(0x35, pOther_para);              //��Ϣ����
         if(xp != NULL){
            if(*xp == 0x02){ //DHCPOFFER
               xp = find_field(0x36, pOther_para);        //DHCP SERVER ID
               if(xp != NULL){  
                  memcpy(__server_ip, xp, 4); 
                  memcpy(__local_ip, &pDhcp->yiaddr, 4);    //NDT IP��ַ
                  seconds -= 0x1000;
                  dhcp_send(DHCPREQUEST);
                  __msgno = DHCPREQUEST;
                  return 1;
               }
               return 0;
            }          
            if(*xp != 0x05) //DHCPACK
               return 0;
         }
         else
            return 0;
       
         __netif.ni_ip = pDhcp->yiaddr; //NDT IP��ַ
    
         xp = find_field(0x01, pOther_para);      //SUBNET MASK
         if(xp != NULL)  
            memcpy(&__netif.ni_mask, xp, 4); 
         if(pDhcp->giaddr)   //·������д������
            __netif.ni_gateway = pDhcp->giaddr;  //NDT ���ص�ַ                  
         else{               //�û���д
            xp = find_field(0x03, pOther_para);      //ROUTE
            if(xp != NULL)  
               memcpy((unsigned char *)&__netif.ni_gateway, xp, 4); 
         }
         xp = find_field(0xFF, pOther_para);      //END
         if(xp != NULL){  
            xp += 8;    //8�ֽڱ�־
            memcpy((unsigned char *)&__netif.sys_ip0, xp, 4); 
            memcpy((unsigned char *)&__netif.sys_ip1, xp+4, 4); 
            memcpy((unsigned char *)&__netif.sys_ip2, xp+8, 4); 
            memcpy((unsigned char *)&__netif.sys_ip3, xp+12, 4); 
            __netif.ni_group = *(xp+12+4);
         }
//         if(pDhcp->giaddr[0] && pDhcp->giaddr[3])       //·����������Ч    
//         __netif.ni_gateway = pDhcp->giaddr;  //NDT ���ص�ַ                  

         __netif.ni_net = __netif.ni_ip & __netif.ni_mask;
         __netif.use_flag = IP_USED_FLAG;

         __msgno = DHCPACK;                    
         eeprom_in_queue(SAVE_SYS_IP);     //����IP��ַ 
         return 0;
     }
}

unsigned char xdata *find_field(unsigned char type, unsigned char xdata *xp)
{
   unsigned char len,curr_type,j;
//   unsigned char xdata *p;
   
   j = 0;     
//   p = xp;
   do{       
       curr_type = *xp;
       if(curr_type == 0xFF && type != 0xFF)   //FIND END
          break;
       if(curr_type == type){
          if(curr_type == 0xFF)
             xp++;
          else 
             xp += 2;
          return xp;          //DATA
       } 
       else{
          len = *(xp + 1) + 2;  //TYPE + LEN + DATA               
          xp += len;
       }          
       j += len;
   }while(j < OTHER_PARA_LEN);

   return NULL;
} 

/* dhcp���� */
static unsigned char xdata request_times;
static unsigned char xdata delay_times;
#define MYIP     0xC0A800FA
#define MASK     0xFFFFFF00
#define NET_ADDR (MYIP & MASK)
void dhcp_task(void)
{
     unsigned char i;

     if(__msgno != DHCPACK){
        if(request_times>5 && __netif.use_flag==IP_USED_FLAG){
           __msgno = DHCPACK;
           return;     
        }
        if((__netif.ni_ip & 0xFF)==0xFF || (__netif.ni_ip & 0xFF)==0x00){   //û��IP��ַ
//           memcpy((unsigned char *)&__netif.ni_ip,"\xC0\xA8\x00\xFA",4);    //������������,ϵͳȱʡ
           __netif.ni_ip =  MYIP;      //192.168.0.250
//           memcpy((unsigned char *)&__netif.ni_mask,"\xFF\xFF\xFF\x00",4);  //�����������ԣ�ȱʡ
           __netif.ni_mask = MASK;     //255.255.255.0
           __netif.ni_net = NET_ADDR;  //�����ַ
        } 
#if 0
	i = 5;                //�ӵ��ӳ�5�룬������������
  	if(request_times!=0)
           i += (r8019_eth_addr[5]&0x3F);       //����1���ӷ���һ��
        if(delay_times++ >= i){  //�ӳ�ʱ�䷢��		          		     
           delay_times = 0;
           switch(__msgno){
	       case DHCPDISCOVER:
                    dhcp_send(DHCPDISCOVER);
                    request_times++;
	            break;
    	       case DHCPREQUEST:
	            dhcp_send(DHCPREQUEST);
                    break;
               default:
                  __msgno = DHCPDISCOVER; 
	   }       	                  
        }
#endif
     } 
}


