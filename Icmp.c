/**************************************************************************/
/*                 ICMP �ͻ��˳��򡪡�ICMP.C                                */
/*              					                  */
/* ����: 1. ��CCC����������ȡNDT�Լ���IP��ַ�����ص�ַ��                      */
/*          �������룬�㲥��ַ		 		                   */
/* ����: 2000/11/21				                          */
/* ����: ����                                                             */
/*                                                                        */
/* (c) Copyright reserved by JDKJ, 2000.                                  */
/*------------------------------------------------------------------------*/
/* �޸ļ�¼:		                                                  */
/*    							                  */   
/**************************************************************************/
#include <reg52.h>
#include <string.h>
#include "inc/ip.h"
#include "inc/icmp.h"
#include "inc/skbuff.h"

/**************************************************************************/
/*                 icmp_receive ICMP���Ľ��մ���                           */
/* ������bufNo������ǰ���Ļ��������                                         */   
/* ���أ���                                                                */     
/* ���ڣ�2000/11/21                                                        */      
/*------------------------------------------------------------------------*/
/* �޸ļ�¼:		                                                      */
/*    							                                  */   
/**************************************************************************/
void icmp_rcv_packet(struct sk_buff xdata *skb)
{   
    unsigned int icmp_len,sum,chksum;   
    icmphdr xdata *pIcmp;
//    unsigned char xdata desAddr[5];
//    struct sin  xdata server;       

    pIcmp = (icmphdr *)(skb->dat);    
    if(pIcmp->ic_type == ICT_ECHORQ){               
       chksum = pIcmp->ic_cksum; 
       pIcmp->ic_cksum = 0;
       icmp_len = skb->len;
       sum = ip_fast_csum((unsigned int *)pIcmp, icmp_len);
       if(sum == chksum){      //У��OK
     	  skb = alloc_skb();
          skb->dat += 34;
	  skb->len = icmp_len;
          memcpy(skb->dat, pIcmp, icmp_len);   
	  pIcmp = (icmphdr *)skb->dat;	
          pIcmp->ic_type  = ICT_ECHORP;           
          pIcmp->ic_cksum = 0;
          pIcmp->ic_cksum = ip_fast_csum((unsigned int *)pIcmp, icmp_len); //����У���
          udp_server.src_addr = __netif.ni_ip;
          ip_send(skb, ICMP);
       }
    }
}    

    
    