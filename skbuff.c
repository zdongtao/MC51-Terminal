#include  <string.h>
#include  "inc/skbuff.h"
#include  "inc/app.h"
#include  "inc/system.h"


struct sk_buff xdata skbuff; //_at_ 0x3000;

struct sk_buff xdata rec_buff[SK_FIFO_NUM+1]; //_at_ (0x3000+sizeof(skbuff));
struct sk_fifo xdata skfifo;
RETRANS xdata retrans[RETRANS_BUF_NUM];

//static unsigned char idata skb_cur;
static unsigned char idata rec_cur;
static unsigned char idata retrans_cur;

struct sin  xdata udp_server;       

//int skb_init(void)
//{
//	return 0;
//}

//多次发送的报文用此函数
static unsigned char idata out_ptr;
void retrans_in(unsigned char xdata *xp,unsigned char len)
{
     RETRANS xdata *rp;
     if(len < RETRANS_BUF_LEN){       
        rp = &retrans[retrans_cur];
        memcpy(rp->dat,xp,len);
        rp->len = len;
        out_ptr = retrans_cur;
        retrans_cur = (retrans_cur + 1) % RETRANS_BUF_NUM;
     }
}

void retrans_out(void)
{
     RETRANS xdata *rp;
     unsigned char len;
     rp = &retrans[out_ptr];
     len = rp->len;
     if(len){
        make_send_package(0,out_ptr,ALM_TYPE,rp->dat,len);         
     } 
     out_ptr = (out_ptr + 1) % RETRANS_BUF_NUM;
}      
      
void retrans_free(unsigned char ptr)
{
     if(ptr < RETRANS_BUF_NUM){ 
        retrans[ptr].len=0;
     } 
}    

void skb_fifo_in(struct sk_buff xdata *skb)
{    	
     unsigned char tmp=(skfifo.head+1) & SK_FIFO_NUM;
     if(tmp !=  skfifo.tail){
        skfifo.dat[skfifo.head] = skb;
        skfifo.head = tmp;
     }
}

struct sk_buff xdata *skb_fifo_out(void)//struct sk_buff xdata *skb)
{   

     unsigned char tmp = skfifo.tail;
     if(skfifo.head != tmp){
//        skb = skfifo.dat[tmp];        
        skfifo.tail = (tmp+1) & SK_FIFO_NUM;
        return skfifo.dat[tmp];
     }      
     return NULL;
}

bit skb_fifo_full(void)
{
     if(((skfifo.head+1)&SK_FIFO_NUM) == skfifo.tail)
        return 1;
     return 0;
}  


/*
void skb_headerinit(struct sk_buff xdata *skb)
{
	memset(skb, 0, sizeof(struct sk_buff));
	skb->dat = skb->buf;
	skb->len = 0;
	skb->truesize = ETH_FRAME_LEN;
}
*/
struct sk_buff xdata *alloc_skb(void)
{
//	struct sk_buff xdata *skb;
//	skb = &skbuff[skb_cur];
//	skb_cur = (skb_cur + 1) % SK_SEND_NUM;
//	memset(skb, 0, sizeof(struct sk_buff));
//	skb->dat = skb->buf;
//	skb->len = 0;
//	skb_headerinit(skb);
        skbuff.dat = (unsigned char *)&skbuff;
        skbuff.len = 0;
	return &skbuff;
}

struct sk_buff xdata *alloc_recbuf(void)
{
	struct sk_buff xdata *skb;
	skb = &rec_buff[rec_cur];
	rec_cur = (rec_cur + 1) & SK_FIFO_NUM;
	skb->dat = skb->buf;
	skb->len = 0;
//	skb_headerinit(skb);
	return skb;
}

//void free_skb(struct sk_buff xdata *skb)
//{
//}

unsigned char xdata *skb_put(struct sk_buff xdata *skb, unsigned int len)
{
	skb->len += len;
	return skb->dat;
}


unsigned char xdata *skb_pull(struct sk_buff xdata *skb, unsigned int len)
{
	skb->dat += len;
	skb->len -= len;
	return skb->dat;
}


unsigned char xdata *skb_push(struct sk_buff xdata *skb,unsigned int len)
{
	skb->dat -= len;
	skb->len += len;
	return skb->dat;
}


void skb_reserve(struct sk_buff xdata *skb, unsigned int len)
{
	skb->dat += len;
}


