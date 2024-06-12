#include <string.h>
#include "inc/skbuff.h"
#include "inc/eth.h"
#include "inc/arp.h"
#include "inc/ip.h"
#include "inc/udp.h"
#include "inc/mac.h"

#define ARP_CACHE_SIZE   8

typedef struct arp_entry {
	unsigned char ar_ha[ETH_ALEN];
	unsigned long ar_ip;
}arp_entry;
static arp_entry xdata arp_cache[ARP_CACHE_SIZE];

static unsigned char idata arp_chche_cur;
/*
void arp_init(void)
{
	memset(&arp_entrys, 0, sizeof(arp_entry));
	memset(arp_cache, 0, sizeof(arp_entry) * ARP_CACHE_SIZE);
}
*/
/*
int arp_add_entry(unsigned char xdata *ha, unsigned long ip)
{
	memcpy(arp_entrys.ar_ha, ha, ETH_ALEN);
	arp_entrys.ar_ip = ip;
	return 0;
}
*/
void arp_add_cache(struct arphdr *arp_hdr)//unsigned char xdata *ha, unsigned long ip)
{
	unsigned char i;

	for (i = 0; i < ARP_CACHE_SIZE; i++) {
		if (arp_hdr->ar_sip == arp_cache[i].ar_ip)
			break;
	}
	if(i == ARP_CACHE_SIZE) {
	   i = arp_chche_cur;
	   arp_chche_cur = (arp_chche_cur + 1) % ARP_CACHE_SIZE;
	}
	memcpy(arp_cache[i].ar_ha, arp_hdr->ar_sha, ETH_ALEN);
	arp_cache[i].ar_ip = arp_hdr->ar_sip;
}

void arp_send_req(unsigned long ip) 
{
	struct arphdr xdata *arp_req;
        unsigned char xdata arp_buf[80];

        memset(arp_buf, 0xFF, ETH_ALEN);
        memcpy(arp_buf+ETH_ALEN, r8019_eth_addr, ETH_ALEN); 
        memcpy(arp_buf + 12, "\x08\x06\x00\x01\x08\x00\x06\x04\x00\x01", 10);
	arp_req =(struct arphdr *)(arp_buf+14);
	memcpy(arp_req->ar_sha, r8019_eth_addr, ETH_ALEN);
	arp_req->ar_sip = __netif.ni_ip;
	memset(arp_req->ar_tha, 0x00, ETH_ALEN);
	arp_req->ar_tip = ip;
        r8019_eth_send(arp_buf,sizeof(struct arphdr)+14);
}

void arp_send_rsp(struct arphdr xdata *arp_hdr)
{
	struct arphdr xdata *arp_rsp;
        unsigned char xdata arp_buf[80];

        memcpy(arp_buf, arp_hdr->ar_sha, ETH_ALEN);   //des
        memcpy(arp_buf+ETH_ALEN, r8019_eth_addr, ETH_ALEN); //src 
        memcpy(arp_buf + 12, "\x08\x06\x00\x01\x08\x00\x06\x04\x00\x02", 10);
	arp_rsp =(struct arphdr *)(arp_buf+14);

	memcpy(arp_rsp->ar_sha, r8019_eth_addr, ETH_ALEN);
	arp_rsp->ar_sip =__netif.ni_ip;//arp_hdr->ar_tip;
	memcpy(arp_rsp->ar_tha, arp_hdr->ar_sha, ETH_ALEN);
	arp_rsp->ar_tip = arp_hdr->ar_sip;
        r8019_eth_send(arp_buf,sizeof(struct arphdr)+14);
}

bit look_up_eth_addr(unsigned long ip, unsigned char xdata *ha)
{
	unsigned char  i;
	for(i = 0; i < ARP_CACHE_SIZE; i++){
	    if(ip == arp_cache[i].ar_ip){
	       memcpy(ha, arp_cache[i].ar_ha, ETH_ALEN);
	       return 1;
	    }
	}
	return 0;
}

bit arp_get_eth_addr(unsigned char xdata *ha)
{
        unsigned int delay;
	struct sk_buff xdata *skb;
	struct ethhdr xdata *eth_hdr;
        unsigned long idata ip; 
        bit first=0;

        ip = udp_server.des_addr;         
FIND:
        if(look_up_eth_addr(ip, ha))
           return 0;       
	if((ip & __netif.ni_mask)!=__netif.ni_net){  //不在同一个网络
           ip = __netif.ni_gateway;
           if(look_up_eth_addr(ip, ha))
              return 0;  
        }        
	if(first) return 1;
	arp_send_req(ip);
        delay=0;
        do{
            if(skb=skb_fifo_out()){
		eth_hdr = (struct ethhdr *)(skb->dat);
		skb_pull(skb, ETH_HLEN);
		if(eth_hdr->h_proto == ETH_P_ARP){
		   arp_rcv_packet(skb);
		   first = 1;
	           goto FIND;
                }			
//		else
//		   first = 1;
                   
            }  
        }while(delay++ <0x200);                                      
	return 1;
}


void arp_rcv_packet(struct sk_buff xdata *skb)
{
	struct arphdr xdata *arp_hdr = (struct arphdr *)(skb->dat);
	if (arp_hdr->ar_tip != __netif.ni_ip)
		return;
	if (arp_hdr->ar_op == ARPOP_REQUEST)
		arp_send_rsp(arp_hdr);
	arp_add_cache(arp_hdr);//arp_hdr->ar_sha, arp_hdr->ar_sip);
}

