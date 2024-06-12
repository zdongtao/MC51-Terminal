#ifndef __UDP_H
#define __UDP_H

#define BROADCASE_PROXY_PORT  20005    //广播代理端口 
#define UDP_INFO_SERVER_PORT  20007    //后台查询服务端口
#define UDP_SERVER_PORT       20008    /* UDP 服务器端口       */
#define NDT_PORT              20009    /* UDP NIU 请求端口     */
#define UDP_NDT_ALARM_PORT    20119    /* NDT 主动报警端口     */
/*
 *	This structure defines an udp header.
 */
struct udphdr {
	unsigned short	source;
	unsigned short	dest;
	unsigned short	len;
	unsigned short	check;
};

typedef struct {                      /*定义UDP 伪头结构    */
    unsigned long   src_ip;      /*源NIU IP地址        */
    unsigned long   dst_ip;      /*目的IP地址          */
    unsigned char   zero;             
    unsigned char   protocol;         /*17  UDP PROTOCOL    */
    unsigned int    len;              /*UDP头标长度+ 数据报长度(不包含伪头长度) */
}UDP_PSEUDO_HEAD;

void udp_init(void);
void udp_rcv_packet(struct sk_buff xdata *skb);
void udp_send(struct sk_buff xdata *skb);
void udp_skb_reserve(struct sk_buff xdata *skb);
unsigned short udp_get_source_port(struct sk_buff xdata *skb);

#endif /* __UDP_H */
