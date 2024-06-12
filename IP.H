#ifndef __IP_H
#define __IP_H


#define UDP  0x11

#define IP_USED_FLAG       0xA5        
typedef struct {                       //定义网络接口
        unsigned char use_flag;        //使用标志
        unsigned long ni_ip;        //IP地址
        unsigned long ni_gateway;   //网关
        unsigned long ni_mask;      //子网掩码        
        unsigned long ni_net;       //网络地址
        unsigned char ni_group;        //分组信息
        unsigned char reserved[3];     //保留
        unsigned long sys_ip0;      //系统主机IP地址
        unsigned long sys_ip1;      //系统主机IP地址
        unsigned long sys_ip2;      //系统主机IP地址
        unsigned long sys_ip3;      //系统主机IP地址           
        unsigned char chk_xor_not;     //校验数据
}NETIF;
extern NETIF xdata __netif;

/*
typedef struct {
      unsigned long ip;
      unsigned long mask;
      unsigned long gateway;
      unsigned long net;
}NETIF;
*/
/*

 *	This structure defines an ip header.

 */


struct iphdr {


//	unsigned char	ihl:4,

//			version:4;
	unsigned char	ver_hl;

	unsigned char	tos;

	unsigned short	tot_len;

	unsigned short	id;

	unsigned short	frag_off;

	unsigned char	ttl;

	unsigned char	protocol;

	unsigned short	check;

	unsigned long	saddr;

	unsigned long	daddr;

	/*The options start here. */

};


void ip_init(unsigned long ip);
void ip_rcv_packet(struct sk_buff xdata *skb);
void ip_send(struct sk_buff xdata *skb, unsigned char proto);
void ip_skb_reserve(struct sk_buff xdata *skb);
unsigned long ip_get_source_ip(struct sk_buff xdata *skb);
unsigned short ip_fast_csum(unsigned int xdata *iph, unsigned int len);

#endif /* __IP_H */
