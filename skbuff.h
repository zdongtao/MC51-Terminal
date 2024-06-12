#ifndef _SKBUFF_H_         
#define _SKBUFF_H_         

#include "inc/eth.h"

struct sk_buff{
//	unsigned char pad[2];
	unsigned char buf[0x600- 4];		//ETH_FRAME_LEN
//	unsigned int truesize;			/* Buffer size 	        */
	unsigned char xdata *dat;		/* Data head pointer	*/
	unsigned int len;			/* Length of actual data*/
};

#define SK_SEND_NUM    0x02
#define SK_FIFO_NUM    0x07          //0000 0111
struct sk_fifo
{
    unsigned char  head;
    unsigned char  tail;
    struct sk_buff xdata *dat[SK_FIFO_NUM+1];
};


struct sin{
       unsigned long   src_addr;
       unsigned long   des_addr;
       unsigned int    src_port;
       unsigned int    des_port;
};
extern struct sin  xdata udp_server;       

#define RETRANS_BUF_NUM     10
#define RETRANS_BUF_LEN     40

typedef struct {
       unsigned char len;
       unsigned char dat[RETRANS_BUF_LEN];
}RETRANS;

int skb_init(void);
struct sk_buff xdata *alloc_skb(void);
void free_skb(struct sk_buff xdata *skb);
unsigned char xdata *skb_put(struct sk_buff xdata *skb, unsigned int len);
unsigned char xdata *skb_push(struct sk_buff xdata *skb, unsigned int len);
unsigned char xdata *skb_pull(struct sk_buff xdata*skb, unsigned int len);
void skb_reserve(struct sk_buff xdata *skb, unsigned int len);
struct sk_buff xdata *alloc_recbuf(void);
bit skb_fifo_full(void);
struct sk_buff xdata *skb_fifo_out(void); //struct sk_buff xdata *skb);
void skb_fifo_in(struct sk_buff xdata *skb);
void retrans_in(unsigned char xdata *xp,unsigned char len);
void retrans_out(void);
void retrans_free(unsigned char ptr);


#endif	/* __SKBUFF_H */
