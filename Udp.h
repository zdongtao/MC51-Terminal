#ifndef __UDP_H
#define __UDP_H

#define BROADCASE_PROXY_PORT  20005    //�㲥����˿� 
#define UDP_INFO_SERVER_PORT  20007    //��̨��ѯ����˿�
#define UDP_SERVER_PORT       20008    /* UDP �������˿�       */
#define NDT_PORT              20009    /* UDP NIU ����˿�     */
#define UDP_NDT_ALARM_PORT    20119    /* NDT ���������˿�     */
/*
 *	This structure defines an udp header.
 */
struct udphdr {
	unsigned short	source;
	unsigned short	dest;
	unsigned short	len;
	unsigned short	check;
};

typedef struct {                      /*����UDP αͷ�ṹ    */
    unsigned long   src_ip;      /*ԴNIU IP��ַ        */
    unsigned long   dst_ip;      /*Ŀ��IP��ַ          */
    unsigned char   zero;             
    unsigned char   protocol;         /*17  UDP PROTOCOL    */
    unsigned int    len;              /*UDPͷ�곤��+ ���ݱ�����(������αͷ����) */
}UDP_PSEUDO_HEAD;

void udp_init(void);
void udp_rcv_packet(struct sk_buff xdata *skb);
void udp_send(struct sk_buff xdata *skb);
void udp_skb_reserve(struct sk_buff xdata *skb);
unsigned short udp_get_source_port(struct sk_buff xdata *skb);

#endif /* __UDP_H */
