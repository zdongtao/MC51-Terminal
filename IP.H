#ifndef __IP_H
#define __IP_H


#define UDP  0x11

#define IP_USED_FLAG       0xA5        
typedef struct {                       //��������ӿ�
        unsigned char use_flag;        //ʹ�ñ�־
        unsigned long ni_ip;        //IP��ַ
        unsigned long ni_gateway;   //����
        unsigned long ni_mask;      //��������        
        unsigned long ni_net;       //�����ַ
        unsigned char ni_group;        //������Ϣ
        unsigned char reserved[3];     //����
        unsigned long sys_ip0;      //ϵͳ����IP��ַ
        unsigned long sys_ip1;      //ϵͳ����IP��ַ
        unsigned long sys_ip2;      //ϵͳ����IP��ַ
        unsigned long sys_ip3;      //ϵͳ����IP��ַ           
        unsigned char chk_xor_not;     //У������
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