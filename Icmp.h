#ifndef  _ICMP_H_
#define  _ICMP_H_     

#define  ICMP     1

#define  ICT_ECHORP      0       //Echo reply
#define  ICT_ECHORQ      8       //Echo request


typedef struct {
    unsigned char  ic_type;             //��Ϣ����
    unsigned char  ic_code;             //code 
    unsigned int   ic_cksum;            //У��ICMP���ĵ�ͷ+����
    unsigned int   ic_id;         //�������ͣ���һ����ϢID
    unsigned int   ic_seq;        //�������ͣ���һ������˳���
    unsigned char  ic_data[1];	        //����������������<1500
}icmphdr;

void icmp_rcv_packet(struct sk_buff xdata *skb);

#endif