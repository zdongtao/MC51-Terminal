#include <reg52.h>
#include <intrins.h>
#include <absacc.h>
#include "inc/eth.h"
#include "inc/mac.h"
#include "inc/skbuff.h"
#include "inc/hdres.h"
#include "inc/system.h"
#include "inc/dhcp.h"


static unsigned char data next_frame;
bit nic_need_init_bit;
unsigned char bdata  mark_isr;

sbit  mark_isr_Mark_PRX = mark_isr ^ 0;
sbit  mark_isr_Mark_PTX = mark_isr ^ 1;
sbit  mark_isr_Mark_RXE = mark_isr ^ 2;
sbit  mark_isr_Mark_TXE = mark_isr ^ 3;
sbit  mark_isr_Mark_OVW = mark_isr ^ 4;
sbit  mark_isr_Mark_CNT = mark_isr ^ 5;
sbit  mark_isr_Mark_RDC = mark_isr ^ 6;
sbit  mark_isr_Mark_RST = mark_isr ^ 7;

void nic_headware_reset(void)
{ 
     unsigned int j;
	
     NIC_RESET_PIN = 1; 	  //UM9008硬件复位(最小205us)
     for(j=0;j<2000;j++);
     NIC_RESET_PIN = 0; 
     for(j=0;j<2000;j++);
}

unsigned char xdata r8019_eth_addr[ETH_ALEN];// = {0x00,0xAE,0x28,0x12,0x34,0x56};
void get_mac_addr(void)
{
	unsigned char  i;
	unsigned char  idata  buf[ETH_ALEN*2];
	outb(IO_BASE + NIC_CMD, 0x21);

        outb(IO_BASE + PG0_DCFG, NIC_DCR);
        outb(IO_BASE + PG0_RCNTLO, 0);
        outb(IO_BASE + PG0_RCNTHI, 0);
        outb(IO_BASE + PG0_IMR, 0);
        outb(IO_BASE + PG0_ISR, 0xFF);
        outb(IO_BASE + PG0_RXCR, NIC_RXOFF);
        outb(IO_BASE + PG0_TXCR, NIC_TXOFF);
        outb(IO_BASE + PG0_RCNTLO, 12);
        outb(IO_BASE + PG0_RCNTHI, 0);
        outb(IO_BASE + PG0_RSARLO, 0);
        outb(IO_BASE + PG0_RSARHI, 0);
        outb(IO_BASE + NIC_CMD, NIC_RREAD|NIC_START);

        for(i=0;i<12;i++){
            buf[i] = inb(IO_BASE + NE_DATAPORT);               
        }
        for(i=0;i<6;i++)
            r8019_eth_addr[i] = buf[i<<1];
}

void r8019_eth_init(bit reset_flag)
{
        unsigned int i;
        EX1 = 0;		
//        IT1 = 1; //沿触发

        if(reset_flag){   
           nic_headware_reset();     
        }
#if __DEBUG==1
//        P11 = ~P11;     //TEST
#endif 
//	for(i=0;i<3000;i++);
//        P14 = P12 = 0; 
//        outb(IO_BASE, NE_RESET);
        /* 网络适配器芯片重置。*/
//	for(i=0;i<3000;i++);

        outb(IO_BASE, 0x21);
//        if((inb(IO_BASE) & 0xF9) != (NIC_NODMA|NIC_STOP)){  //寄存器不能写，硬件复位
//            nic_need_init_bit=1;
//            return;    
//        }

        /* 远程字节数寄存器清零。 */
        outb(IO_BASE+PG0_DCFG, NIC_DCR);  //0x48

        outb(IO_BASE+PG0_RCNTLO,0);
        outb(IO_BASE+PG0_RCNTHI,0);    //0

        outb(IO_BASE+PG0_RXCR, NIC_RXOFF);
        outb(IO_BASE+PG0_TXCR, NIC_TXOFF);

        /* 设置接收缓冲环。 */
        outb(IO_BASE+PG0_TPSR, TX_START_PAGE);
        outb(IO_BASE+PG0_STARTPG, RX_START_PAGE);
//        outb(IO_BASE+PG0_BOUNDARY,STOP_PAGE - 1);
        outb(IO_BASE+PG0_BOUNDARY,RX_START_PAGE);
        outb(IO_BASE+PG0_STOPPG, STOP_PAGE);
	    //清除中断屏蔽
        outb(IO_BASE+PG0_ISR, 0xFF);
        outb(IO_BASE+PG0_IMR, 0x00);     

        /* 把MAC地址拷贝到地址寄存器，
           并把多目地址设置成可以接收本机包。*/
        outb(IO_BASE+NIC_CMD, NIC_PAGE1|NIC_STOP|NIC_NODMA);
        for(i=0; i<6; i++)
            outb(IO_BASE + PG1_PHYS + i, r8019_eth_addr[i]);

        for(i=0; i<8; i++)
            outb(IO_BASE + PG1_MULT + i, 0x00);

        /* 设置当前页寄存器。 */
        outb(IO_BASE+PG1_CURPAG, RX_START_PAGE+1);
        next_frame = RX_START_PAGE+1;


        /* 寄存器设置完后，起动NIC， 开始响应中断、接收包、发送包 */
        outb(IO_BASE+NIC_CMD, NIC_PAGE0|NIC_STOP|NIC_NODMA);

        outb(IO_BASE+PG0_ISR, 0xFF);
	outb(IO_BASE+PG0_IMR, ISR_MASK); //ENISR_ALL);
	    //if(startp){
 	outb(IO_BASE+PG0_TXCR, NIC_TXCONFIG); /* xmit on (0x00). */
  	outb(IO_BASE+PG0_RXCR, NIC_RXCONFIG); /* rx on (0x04|0x08),accept-all packet. */
	    //}       
        outb(IO_BASE+NIC_CMD, NIC_PAGE0|NIC_START|NIC_NODMA);
        mark_isr = 0;
        nic_need_init_bit = 0;
        EX1 = 1;		
}

void eth_init(void)
{       
//      IT1 = 1; //TEST
      nic_headware_reset();
      get_mac_addr();
      r8019_eth_init(0);        
}

void r8019_eth_send(unsigned char xdata *xp, unsigned int len)
{
        unsigned int  i;           
        if(len < 60)
           len = 60;             
	EX1 = 0;	
        outb(IO_BASE+NIC_CMD, NIC_PAGE0|NIC_NODMA);   // 7.5
        outb(IO_BASE + PG0_RCNTLO, len & 0xff);
        outb(IO_BASE + PG0_RCNTHI, len >> 8);
        outb(IO_BASE + PG0_RSARLO, 0x00);
        outb(IO_BASE + PG0_RSARHI, TX_START_PAGE);   //4000H
        outb(IO_BASE + NIC_CMD, NIC_RWRITE | NIC_START);         
        
        mark_isr_Mark_RDC = 0;          	
        i = len;	             
        while(i--){        
            outb(IO_BASE+NE_DATAPORT, *(xp++));
        }
	EX1 = 1;
        i=0;	         
	do{
	    if(mark_isr_Mark_RDC) goto SEND;	
        }while(i++ < 1024);  
        goto FAIL;     
SEND:
        outb(IO_BASE+PG0_TCNTLO, len & 0xff);
        outb(IO_BASE+PG0_TCNTHI, len >> 8);
        outb(IO_BASE+PG0_TPSR, TX_START_PAGE);
        mark_isr_Mark_PTX = 0;
        outb(IO_BASE+NIC_CMD,  NIC_NODMA|NIC_TRANS|NIC_START);//0x26);    	
        i=0;	         
	do{
	    if(mark_isr_Mark_PTX) return;	
        }while(i++ < 1024);  
FAIL:
        nic_need_init_bit=1;                
}


struct ETH{
    	unsigned char stat;
        unsigned char next;
        unsigned int  count;
};

static struct ETH idata __ethHdr;
static void  get_hdr(unsigned char offset)
{
        unsigned char data *p;
        p = (unsigned char *)&__ethHdr; 			//(char *)&__ethHdr;
         outb(IO_BASE+NIC_CMD, NIC_PAGE0|NIC_NODMA);
         outb(IO_BASE+PG0_RCNTLO, sizeof(__ethHdr));
         outb(IO_BASE+PG0_RCNTHI, 0);
         outb(IO_BASE+PG0_RSARLO, 0);
         outb(IO_BASE+PG0_RSARHI, offset);  //   this_frame);  	 
         outb(IO_BASE+NIC_CMD, NIC_RREAD|NIC_START);         
         offset = sizeof(__ethHdr);
         while(offset--){
             *(p++) = inb(IO_BASE + NE_DATAPORT);
         } 
         /* 调换__ethHdr.count的字节顺序。 */
         __ethHdr.count = __ethHdr.count>>8 |__ethHdr.count<<8;
}         

void r8019_eth_rcv() 
{
        unsigned char curr_page;
        unsigned int  i,len;        
        unsigned char pkt_count = 0;
        struct sk_buff xdata *skb;
        
    while(pkt_count++ < 10){
        outb(IO_BASE+NIC_CMD, NIC_PAGE1|NIC_NODMA);
        curr_page = inb(IO_BASE + PG1_CURPAG);                
        if(curr_page==next_frame)
           break;
        else{
             if(curr_page < RX_START_PAGE){
//	        EX1 = 0;                                 //关中断     
//        	nic_busy=5;
                nic_need_init_bit = 1;
                return;
             }
        }         
                                 
        /* 读包头和包。 */
        get_hdr(next_frame);//this_frame);
	if((__ethHdr.stat & 0x4E) || (__ethHdr.count> 1518)){  //接收到错误的包
//            EX1 = 0;                                 //关中断     
//            nic_busy=5;
            nic_need_init_bit = 1;
            return;
        }    


        if((__ethHdr.stat & 0x20) && (__ethHdr.count>70) && (__msgno == DHCPACK)){ //不接收大的广播报文
//            #if __DEBUG
//                total.ethRxCnt1++;
//            #endif    
            goto next_package;    
        }

        if((__ethHdr.stat & 0x01) && (__ethHdr.count>=60)){
            if(skb_fifo_full()==0){
               skb = alloc_recbuf();
               len= __ethHdr.count - sizeof(__ethHdr);					
//               if(ei_dmaing) return;
//               ei_dmaing = 1;	
               outb(IO_BASE+NIC_CMD, NIC_PAGE0|NIC_NODMA);
               outb(IO_BASE+PG0_RCNTLO, len & 0xff);
               outb(IO_BASE+PG0_RCNTHI, len  >> 8);
               outb(IO_BASE+PG0_RSARLO, sizeof(__ethHdr));
               outb(IO_BASE+PG0_RSARHI, next_frame);//this_frame);      //this_frame);			
               outb(IO_BASE+NIC_CMD, NIC_RREAD|NIC_START);			
               for(i=0; i<len; i++){
                   *(skb->dat+i) = inb(IO_BASE+NE_DATAPORT);
               } 
//             ei_dmaing = 0;	
               skb->len = len;
               skb_fifo_in(skb);
      	    }     
	    else{
               EX1 = 0;            //NIC忙，停止中断
	    }
        } 
next_package:	
        next_frame=curr_page=__ethHdr.next;
        if(next_frame < RX_START_PAGE){
           nic_need_init_bit = 1;
           return;
        }      
        curr_page--;
        if(curr_page < RX_START_PAGE){
           curr_page = STOP_PAGE;
        }
        outb(IO_BASE+PG0_BOUNDARY, curr_page);//next_frame-1 ); 	                                                                            
   }
}


void  ethOverTreat(void) {

    //unsigned char was_txing, must_resend = 0;
    unsigned int i;	
  
    
     // Record whether a Tx was in progress and then issue the
     // stop command.
     
    //was_txing = inb(IO_BASE) & NIC_TRANS;
//    EA = 0;		

    outb(IO_BASE, NIC_PAGE0|NIC_STOP|NIC_NODMA);
    
     
    // Wait a full Tx time (1.2ms) + some guard time, NS says 1.6ms total.
    //  Early datasheets said to poll the reset bit, but now they say that
    //  it "is not a reliable indicator and subsequently should be ignored."
    // We wait at least 10ms.
     
     //delay(2) ;
      for(i=0;i<1000;i++);	

    
     // Reset RBCR[01] back to zero as per magic incantation.     
     
     outb(IO_BASE+PG0_RCNTLO,0);
     outb(IO_BASE+PG0_RCNTHI,0);

    
     // See if any Tx was interrupted or not. According to NS, this
     // step is vital, and skipping it will cause no end of havoc.
     
    //if (was_txing) { 
    // 	must_resend = !(inb(IO_BASE+PG0_ISR) & (ENISR_TX|ENISR_TX_ERR));
    //}

    
    // Have to enter loopback mode and then restart the NIC before
     //you are allowed to slurp packets up off the ring.    
    outb(IO_BASE+PG0_TXCR, NIC_TXOFF);
    //outb(IO_BASE+NIC_CMD, NIC_PAGE0|NIC_START);

    //Clear the Rx ring of all the debris, and ack the interrupt.
    
    r8019_eth_rcv();
    //outb(IO_BASE+PG0_ISR,ENISR_OVER|ENISR_RX|ENISR_RX_ERR);
    
    // Leave loopback mode, and resend any packet that got stopped.

    outb(IO_BASE+PG0_TXCR, NIC_TXCONFIG);          
//    outb(IO_BASE+PG0_ISR, 0xFF);         //清除OVW_ISR  
    outb(IO_BASE+PG0_IMR, ISR_MASK); //ENISR_ALL);
//    outb(IO_BASE+NIC_CMD, NIC_PAGE0|NIC_START|NIC_NODMA);    
//    EA = 1;		
}


void etherInterrupt(void) interrupt 2   //INT1
{

#if __DEBUG==1
//        P11 = ~P11;     //TEST
#endif 

     outb(IO_BASE+NIC_CMD, NIC_PAGE0|NIC_NODMA);   //choose page0              
     mark_isr |= inb(IO_BASE+PG0_ISR);             //2004.10.09
     if(mark_isr_Mark_OVW){                        //接收满	    
        ethOverTreat();
//      outb(IO_BASE+NIC_CMD, NIC_PAGE0|NIC_STOP|NIC_NODMA);//NIC_PAGE0|NIC_STOP|NIC_NODMA); //关闭NIC，停止接收数据
        EX1 = 0;                                   //关中断     
     }
     else{
        if(mark_isr & (ENISR_RX|ENISR_RX_ERR)){
           r8019_eth_rcv(); 	
        } 
     } 	
     outb(IO_BASE+NIC_CMD, NIC_PAGE0|NIC_START|NIC_NODMA);    	
     outb(IO_BASE+PG0_ISR, 0xFF);        //清除ISR          
}

