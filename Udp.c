#include "inc/skbuff.h"
#include "inc/eth.h"
#include "inc/ip.h"
#include "inc/udp.h"
#include "inc/dhcp.h"
#include "inc/app.h"

//struct sin  xdata to;       

void udp_rcv_packet(struct sk_buff xdata *skb)
{
	struct udphdr xdata *udp_hdr = (struct udphdr *)(skb->dat);
         
	skb->len = udp_hdr->len;
	skb_pull(skb, sizeof(struct udphdr));
	if(udp_hdr->dest == NDT_PORT){
           udp_server.des_port = udp_hdr->source;
           app_process_received(skb->dat);
        }  
	else
	   if(udp_hdr->dest == DHCP_CLIENT_PORT){
	      dhcp_receive(skb->dat);
           }
}

void udp_send(struct sk_buff xdata *skb)//, struct sin xdata *to)
{
	struct udphdr xdata *udp_hdr;
        UDP_PSEUDO_HEAD xdata *phdr;

	udp_hdr = (struct udphdr *)skb_push(skb, sizeof(struct udphdr));
        phdr =(UDP_PSEUDO_HEAD *)udp_hdr - 1;

        phdr->src_ip = udp_server.src_addr;
        phdr->dst_ip = udp_server.des_addr;//to->sin_addr;
        phdr->zero = 0;
        phdr->protocol = UDP;
        phdr->len = skb->len;//len;

	udp_hdr->source = udp_server.src_port;
	udp_hdr->dest = udp_server.des_port;//to->sin_port;
	udp_hdr->len = skb->len;//+sizeof(struct udphdr);

	udp_hdr->check = 0;
	udp_hdr->check = ip_fast_csum((unsigned int *)phdr, skb->len+12);

	ip_send(skb, UDP); //to->sin_addr
}

void udp_skb_reserve(struct sk_buff xdata *skb)
{
	ip_skb_reserve(skb);
	skb_reserve(skb, sizeof(struct udphdr));
}
/*
unsigned short udp_get_source_port(struct sk_buff xdata *skb)
{
	struct udphdr xdata *udp_hdr;
	udp_hdr = (struct udphdr *)(skb->buf + ETH_HLEN + sizeof(struct iphdr));
	return ntohs(udp_hdr->source);
}
*/
