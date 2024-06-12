#ifndef  _ICMP_H_
#define  _ICMP_H_     

#define  ICMP     1

#define  ICT_ECHORP      0       //Echo reply
#define  ICT_ECHORQ      8       //Echo request


typedef struct {
    unsigned char  ic_type;             //信息类型
    unsigned char  ic_code;             //code 
    unsigned int   ic_cksum;            //校验ICMP报文的头+数据
    unsigned int   ic_id;         //回送类型，是一个消息ID
    unsigned int   ic_seq;        //回送类型，是一个报文顺序号
    unsigned char  ic_data[1];	        //报文数据区，长度<1500
}icmphdr;

void icmp_rcv_packet(struct sk_buff xdata *skb);

#endif