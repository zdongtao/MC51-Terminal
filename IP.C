#include <string.h>
#include "inc/eth.h"
#include "inc/arp.h"
#include "inc/ip.h"
#include "inc/udp.h"
#include "inc/icmp.h"
#include "inc/skbuff.h"

NETIF xdata __netif;
//static unsigned long local_ip = 0;
unsigned short ip_fast_csum(unsigned int xdata *xp, unsigned int len)
{
    unsigned long sum=0;    
    if(len & 0x1){      //调整为偶数长度
       ((unsigned char *)xp)[len]=0;
       len++;           //len += 1
    }		
    len >>= 1;          //len /= 2
    while(len--){
       sum += (*xp>>8) | (*xp<<8);
       xp++;
    }
    sum = (sum >> 16) +(sum & 0xFFFF);
    sum += (sum >> 16);
    len = ~sum & 0xffff; 
    return(len>>8 | len<<8);
} 

//void ip_init(unsigned long  ip)
//{
//	local_ip = ip;
//        netif.ni_ip = ip;
//        netif.ni_mask = 0xFFFFFF00;
//        netif.ni_gateway = 0xC009C901;
//        netif.ni_net = netif.ip & netif.mask;
//}

#ifndef INADDR_ANY
#define INADDR_ANY      (0xFFFFFFFFUL)
#endif

void ip_rcv_packet(struct sk_buff xdata *skb)
{
	struct iphdr xdata *ip_hdr = (struct iphdr *)(skb->dat);
	if(ip_hdr->daddr==__netif.ni_ip || ip_hdr->daddr==INADDR_ANY)  //0xFFFFFFFF=>dhcp
	{
		skb->len = ip_hdr->tot_len;
		skb_pull(skb, sizeof(struct iphdr));
                udp_server.des_addr = ip_hdr->saddr;
		if(ip_hdr->protocol == UDP){
		   udp_rcv_packet(skb);
                } 
		else
                   if(ip_hdr->protocol == ICMP){
               	      icmp_rcv_packet(skb);		
		   }
	}
}

unsigned short xdata ip_id;
void ip_send(struct sk_buff xdata *skb, unsigned char proto)
{
	struct iphdr xdata *ip_hdr;
        unsigned char xdata dest_eth_addr[ETH_ALEN];

	if (udp_server.des_addr == INADDR_ANY) {
	    memset(dest_eth_addr,0xFF,6);
        } 
	else{
            if(arp_get_eth_addr(dest_eth_addr))
	       return;
        }
	ip_hdr = (struct iphdr *)skb_push(skb, sizeof(struct iphdr));
//	ip_hdr->ihl = 5;
	ip_hdr->ver_hl = 0x45;
	ip_hdr->tos = 0;      
	ip_hdr->tot_len = skb->len;// + 20;
	ip_hdr->id = ip_id++;
	ip_hdr->frag_off = 0;
	ip_hdr->ttl = 0x20;
	ip_hdr->protocol = proto;
	ip_hdr->saddr = udp_server.src_addr;
	ip_hdr->daddr = udp_server.des_addr;
        ip_hdr->check = 0;
	ip_hdr->check = ip_fast_csum((unsigned int *)ip_hdr, sizeof(struct iphdr));
	eth_send(skb, dest_eth_addr);
}


void ip_skb_reserve(struct sk_buff xdata *skb)
{
	eth_skb_reserve(skb);
	skb_reserve(skb, sizeof(struct iphdr));
}

/*
unsigned long ip_get_source_ip(struct sk_buff *skb)
{

	struct iphdr *ip_hdr;
	ip_hdr = (struct iphdr *)(skb->buf + ETH_HLEN);
	return ip_hdr->saddr;
}
*/
