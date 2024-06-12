#include <string.h>
#include "inc/skbuff.h"
#include "inc/eth.h"
#include "inc/mac.h"

void eth_send(struct sk_buff xdata *skb, unsigned char xdata *dest_addr)
{
	struct ethhdr xdata *eth_hdr;
        extern unsigned char xdata r8019_eth_addr[];

	eth_hdr = (struct ethhdr *)skb_push(skb, ETH_HLEN);
	memcpy(eth_hdr->h_dest, dest_addr, ETH_ALEN);
	memcpy(eth_hdr->h_source, r8019_eth_addr, ETH_ALEN);
	eth_hdr->h_proto = ETH_P_IP;
	r8019_eth_send(skb->dat, skb->len);
}
/*
bit eth_rcv(struct sk_buff xdata *skb)
{
	return r8019_eth_rcv(skb->dat);
}
*/
/*
int eth_get_addr(unsigned char *addr)
{
//	return s3c4510_eth_get_addr(addr);

}
*/

void eth_skb_reserve(struct sk_buff xdata *skb)
{
	skb_reserve(skb, ETH_HLEN);
}

