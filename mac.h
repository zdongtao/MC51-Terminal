#ifndef __MAC_H
#define __MAC_H


        #define IO_BASE           0xC000    //0x300
     
        #define outb(base,x)      (XBYTE[base]=x)
        #define inb(base)         (XBYTE[base])

        #define NIC_TX_IRQ_MASK   0xa      /* For register PG0_ISR */
        #define NIC_RX_IRQ_MASK   0x5
        #define NIC_RXCONFIG      0x04     /* 0x04PG0_RXCR: broadcasts, no multicast,errors */
        #define NIC_TXOFF         0x02     /* PG0_TXCR: Transmitter off */
        #define NIC_RXOFF         0x01     /* PG0_RXCR: Accept no packets */
        #define NIC_TXCONFIG      0x00     /* PG0_TXCR: Normal transmit mode */
        #define NIC_DCR           0x68  
        
        /*  Register accessed at EN_CMD, the 8390 base addr.  */
        #define NIC_STOP      0x01    /* Stop and reset the chip */
        #define NIC_START     0x02    /* Start the chip, clear reset */
        #define NIC_TRANS     0x04    /* Transmit a frame */
        #define NIC_RREAD     0x08    /* Remote read */
        #define NIC_RWRITE    0x10    /* Remote write  */
        #define NIC_NODMA     0x20    /* Remote DMA */
        #define NIC_PAGE0     0x00    /* Select page chip registers */
        #define NIC_PAGE1     0x40    /* using the two high-order bits */
        #define NIC_PAGE2     0x80    /* Page 3 is invalid. */
        
        
        #define NIC_CMD         0x00    /* The command register (for all pages) */
        /* Page 0 register offsets. */
        #define PG0_CLDALO      0x01    /* Low byte of current local dma addr  RD */
        #define PG0_STARTPG     0x01    /* Starting page of ring bfr WR */
        #define PG0_CLDAHI      0x02    /* High byte of current local dma addr  RD */
        #define PG0_STOPPG      0x02    /* Ending page +1 of ring bfr WR */
        #define PG0_BOUNDARY    0x03    /* Boundary page of ring bfr RD WR */
        #define PG0_TSR         0x04    /* Transmit status reg RD */
        #define PG0_TPSR        0x04    /* Transmit starting page WR */
        #define PG0_NCR         0x05    /* Number of collision reg RD */
        #define PG0_TCNTLO      0x05    /* Low  byte of tx byte count WR */
        #define PG0_FIFO        0x06    /* FIFO RD */
        #define PG0_TCNTHI      0x06    /* High byte of tx byte count WR */
        #define PG0_ISR         0x07    /* Interrupt status reg RD WR */
        #define PG0_CRDALO      0x08    /* low byte of current remote dma address RD */
        #define PG0_RSARLO      0x08    /* Remote start address reg 0 */
        #define PG0_CRDAHI      0x09    /* high byte, current remote dma address RD */
        #define PG0_RSARHI      0x09    /* Remote start address reg 1 */
        #define PG0_RCNTLO      0x0a    /* Remote byte count reg WR */
        #define PG0_RCNTHI      0x0b    /* Remote byte count reg WR */
        #define PG0_RSR         0x0c    /* rx status reg RD */
        #define PG0_RXCR        0x0c    /* RX configuration reg WR */

        #define PG0_TXCR        0x0d    /* TX configuration reg WR */
        #define PG0_COUNTER0    0x0d    /* Rcv alignment error counter RD */
        #define PG0_DCFG        0x0e    /* Data configuration reg WR */
        #define PG0_DCR         0x0e    /* Data configuration reg WR */
        #define PG0_COUNTER1    0x0e    /* Rcv CRC error counter RD */
        #define PG0_IMR         0x0f    /* Interrupt mask reg WR */
        #define PG0_COUNTER2    0x0f    /* Rcv missed frame error counter RD */
        
        /* Bits in PG0_ISR - Interrupt status register */
        #define ENISR_RX        0x01    /* Receiver, no error */
        #define ENISR_TX        0x02    /* Transmitter, no error */
        #define ENISR_RX_ERR    0x04    /* Receiver, with error */
        #define ENISR_TX_ERR    0x08    /* Transmitter, with error */
        #define ENISR_OVER      0x10    /* Receiver overwrote the ring */
        #define ENISR_COUNTERS  0x20    /* Counters need emptying */
        #define ENISR_RDC       0x40    /* remote dma complete */
        #define ENISR_RESET     0x80    /* Reset completed */
        /* next line is modi by yao to disable TX interrupt*/
        #define ENISR_ALL       0xFF    /* Interrupts we will enable */
        #define ISR_MASK        0x57    //0x53    // 
        /* next line is add by yao to disable RX interrupt*/
        #define ENISR_EXCEPT_RX 0x20


        
        /* Bits in PG0_DCFG - Data config register */
        #define ENDCFG_WTS      0x01    /* word transfer mode selection */
        
        /* Page 1 register offsets. */
        #define PG1_PHYS   0x01 /* This board's physical enet addr RD WR */
        #define PG1_CURPAG 0x07 /* Current memory page RD WR */
        #define PG1_MULT   0x08 /* Multicast filter mask array (8 bytes) RD WR */
        
        /* Bits in received packet status byte and PG0_RSR*/
        #define ENRSR_RXOK      0x01    /* Received a good packet */
        #define ENRSR_CRC       0x02    /* CRC error */
        #define ENRSR_FAE       0x04    /* frame alignment error */
        #define ENRSR_FO        0x08    /* FIFO overrun */
        #define ENRSR_MPA       0x10    /* missed pkt */
        #define ENRSR_PHY       0x20    /* physical/multicase address */
        #define ENRSR_DIS       0x40    /* receiver disable. set in monitor mode */
        #define ENRSR_DEF       0x80    /* deferring */
        
        /* Transmitted packet status, PG0_TSR. */
        #define ENTSR_PTX 0x01  /* Packet transmitted without error */
        #define ENTSR_ND  0x02  /* The transmit wasn't deferred. */
        #define ENTSR_COL 0x04  /* The transmit collided at least once. */
        #define ENTSR_ABT 0x08  /* The transmit collided 16 times, and was deferred. */
        #define ENTSR_CRS 0x10  /* The carrier sense was lost. */
        #define ENTSR_FU  0x20  /* A "FIFO underrun" occurred during transmit. */
        #define ENTSR_CDH 0x40  /* The collision detect "heartbeat" signal was lost. */
        #define ENTSR_OWC 0x80  /* There was an out-of-window collision. */
        
        
        #define NE_RESET  0x1f  /* Issue a read to reset, a write to clear. */
        #define NE_DATAPORT 0x10

        #define TX_START_PAGE 0x40
        #define RX_START_PAGE 0x48
        #define STOP_PAGE     0x7F          //0x7F		
        
        #define TRUE    1  /* Integer value of TRUE */
        #define FALSE   0

//        void outb(unsigned char port,unsigned char val);
//        unsigned char inb(unsigned char port);

        extern unsigned char xdata r8019_eth_addr[];

        void r8019_eth_init(bit);
        void r8019_eth_send(unsigned char xdata *dat, unsigned int len);
        void r8019_eth_rcv();
        void eth_init(void);
        //void eth_get_address(unsigned char *address);
#endif /* __MAC_H */
