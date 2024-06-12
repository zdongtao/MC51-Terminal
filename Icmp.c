/**************************************************************************/
/*                 ICMP 客户端程序——ICMP.C                                */
/*              					                  */
/* 功能: 1. 从CCC管理主机获取NDT自己的IP地址，网关地址，                      */
/*          子网掩码，广播地址		 		                   */
/* 日期: 2000/11/21				                          */
/* 作者: 董涛                                                             */
/*                                                                        */
/* (c) Copyright reserved by JDKJ, 2000.                                  */
/*------------------------------------------------------------------------*/
/* 修改记录:		                                                  */
/*    							                  */   
/**************************************************************************/
#include <reg52.h>
#include <string.h>
#include "inc/ip.h"
#include "inc/icmp.h"
#include "inc/skbuff.h"

/**************************************************************************/
/*                 icmp_receive ICMP报文接收处理                           */
/* 参数：bufNo——当前报文缓冲区编号                                         */   
/* 返回：无                                                                */     
/* 日期：2000/11/21                                                        */      
/*------------------------------------------------------------------------*/
/* 修改记录:		                                                      */
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
       if(sum == chksum){      //校验OK
     	  skb = alloc_skb();
          skb->dat += 34;
	  skb->len = icmp_len;
          memcpy(skb->dat, pIcmp, icmp_len);   
	  pIcmp = (icmphdr *)skb->dat;	
          pIcmp->ic_type  = ICT_ECHORP;           
          pIcmp->ic_cksum = 0;
          pIcmp->ic_cksum = ip_fast_csum((unsigned int *)pIcmp, icmp_len); //计算校验和
          udp_server.src_addr = __netif.ni_ip;
          ip_send(skb, ICMP);
       }
    }
}    

    
    