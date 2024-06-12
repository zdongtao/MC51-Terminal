#include <reg51.h>
#include <intrins.h>
#include <string.h>
#include <absacc.h>
#include "inc/asyn.h"
#include "inc/hdres.h"
#include "inc/skbuff.h"
#include "inc/vf040.h"

bit vf040_busy_bit;

/***************************************************************/                                         
/* PROCEDURE:          SST_MasterIO                            */                                                        
/*                                                             */                                                         
/* This procedure handles byte transfer to and from            */                                         
/* the slave device.                                           */                                                   
/*                                                             */                                                         
/* Input:      None                                            */                                                      
/*                                                             */                                                         
/* Returns: Nothing                                            */                                                     
/*                                                             */                                                         
/***************************************************************/                                         
unsigned char SST_MasterIO(unsigned char HW_SPI_out)
{                                                                                                                         
       unsigned char temp;                                                                                            
       SPDR = HW_SPI_out;
       do                                                                                                             
       {                                                                                                              
           temp = SPSR & 0x80;                                                                                     
       }while(temp != 0x80);                                                                                        
       return SPDR;                                                                                                   
}                                                                                                                         


/*************************************************************/                                         
/* PROCEDURE:          Read_Status_Register                  */                                                      
/*                                                           */                                                         
/* This procedure reads fom Read_Stratus_Register.           */                                                     
/*                                                           */                                                         
/* Input:      None                                          */                                                      
/*                                                           */                                                         
/* Returns: status byte                                      */                                                   
/*                                                           */                                                         
/*************************************************************/                                         
unsigned char Read_Status_Register()                                                                                      
{                                                                                                                         
       unsigned char byte = 0;                                                                                        
       CE_Low();                                  /* enable device */                                                 
       SST_MasterIO(0x05);                        /* send RDSR command */                                           
       byte = SST_MasterIO(0x00);                 /* receive byte */                                             
       CE_High();                                 /* disable device */                                               
       return byte;                                                                                                   
}           

/**************************************************************/                                         
/* PROCEDURE:          Wait_Busy                              */                                                        
/*                                                            */                                                         
/* This procedure waits until device is no longer busy.       */                                       
/*                                                            */                                                         
/* Input:      None                                           */                                                      
/*                                                            */                                                         
/* Returns: Nothing                                           */                                                     
/*                                                            */                                                         
/**************************************************************/                                         
void Wait_Busy()                                                                                                          
{                                                                                                                         
      while((Read_Status_Register() & 0x03) == 0x03);
}                                                                                                                         
                                                                                                              
/*************************************************************/                                         
/* PROCEDURE:          EWSR                                  */                                                           
/*                                                           */                                                         
/* This procedure enables the Write Status Register.         */                                         
/*                                                           */                                                         
/* Input:      None                                          */                                                      
/*                                                           */                                                         
/* Returns: Nothing                                          */                                                     
/*                                                           */                                                         
/*************************************************************/                                         
void EWSR()                                                                                                               
{                                                                                                                         
        CE_Low();                              /* enable device */                                                 
        SST_MasterIO(0x50);                    /* enable writing to the status register */                       
        CE_High();                             /* disable device */                                               
}                                                                                                                         

/*************************************************************/                                         
/* PROCEDURE:          WRSR                                  */                                                           
/*                                                           */                                                         
/* This procedure writes a byte to the Status Register.      */                                       
/*                                                           */                                                         
/* Input:      data byte                                     */                                                   
/*                                                           */                                                         
/* Returns: Nothing                                          */                                                     
/*                                                           */                                                         
/*************************************************************/                                         
void WRSR(unsigned char byte)                                                                                             
{                                                                                                                         
        CE_Low();                                  /* enable device */                                                 
        SST_MasterIO(0x01);                        /* select write to status register */                             
        SST_MasterIO(byte);                        /* data that will change the status of BPx or BPL                
                                                         (only bits 2,3,7 can be written) */                              
        CE_High();                                 /* disable the device */                                           
        Wait_Busy();                                                                                                   
}                                                                                                                         

/*******************************************************************************/                                         
/* PROCEDURE:          WREN                                  */                                                           
/*                                                             */                                                         
/* This procedure enables the Write Enable Latch.                            */                                           
/*                                                             */                                                         
/* Input:      None                                               */                                                      
/*                                                             */                                                         
/* Returns: Nothing                                                */                                                     
/*                                                             */                                                         
/*******************************************************************************/                                         
void WREN()                                                                                                               
{                                                                                                                         
        CE_Low();                                  /* enable device */                                                 
        SST_MasterIO(0x06);                        /* send WREN command */                                           
        CE_High();                                 /* disable device */                                               
}                                                                                                                         

/*******************************************************************************/                                         
/* PROCEDURE:          WRDI                                   */                                                          
/*                                                             */                                                         
/* This procedure disables the Write Enable Latch.                            */                                          
/*                                                             */                                                         
/* Input:      None                                               */                                                      
/*                                                             */                                                         
/* Returns: Nothing                                                */                                                     
/*                                                             */                                                         
/*******************************************************************************/                                         
void WRDI()                                                                                                               
{                                     
        CE_Low();                                  /* enable device */                                                 
        SST_MasterIO(0x04);                        /* send WRDI command */                                           
        CE_High();                                 /* disable device */                                               
}                                                                                                                         

/*******************************************************************************/                                         
/* PROCEDURE:          Read_ID                                 */                                                         
/*                                                             */                                                         
/* This procedure reads the manufacturer抯 ID and device ID.                    */                                        
/* It will use 90h as the command to read the ID.  It is up to                       */                                   
/* the user to give the last byte ID_addr to determine whether                     */                                     
/* the device outputs manufacturer抯 ID first, or device ID first.                     */                                 
/* Review the datasheets for details.                                      */                                             
/* Returns ID in variable byte.                                          */                                               
/*                                                             */                                                         
/* Input:      ID_addr                                             */                                                     
/*                                                             */                                                         
/* Returns: byte:           ID1                                     */                                                    
/*                                                             */                                                         
/*******************************************************************************/                                         
/*
unsigned char Read_ID (unsigned char ID_addr)                                                                           
{                                                                                                                         
        unsigned char byte;                                                                                            
         CE_Low();                                  // enable device
         SST_MasterIO(0x90);                        // send read ID command
         SST_MasterIO(0x00);                        // send address
         SST_MasterIO(0x00);                        // send address
         SST_MasterIO(ID_addr);                     // send address - either 00H or 01H 
         byte = SST_MasterIO(0x00);                 // receive byte 
         CE_High();                                 // disable device 
         return byte;                                                                                                   
}                                                                                                                         
*/
    /*******************************************************************************/                                         
    /* PROCEDURE:          Read                                   */                                                          
    /*                                                             */                                                         
    /* This procedure reads one address of the device.                           */                                           
    /* It will return the byte read in variable byte.                                 */                                      
    /*                                                             */                                                         
    /* Input:      Dst:         Destination Address 000000H - 07FFFFH */                                                      
    /*                                                             */                                                         
    /* Returns: byte                                                   */                                                     
    /*                                                             */                                                         
    /*******************************************************************************/                                         
/*
    unsigned char Read(unsigned long Dst)                                                                                     
    {                                                                                                                         
               unsigned char byte = 0;                                                                                        
               CE_Low();                                  // enable device 
               SST_MasterIO(0x03);                        // read command                                                
               SST_MasterIO(((Dst & 0xFFFFFF) >> 16));    // send 3 address bytes
               SST_MasterIO(((Dst & 0xFFFF) >> 8));                                                                        
               SST_MasterIO(Dst & 0xFF);                                                                                      
               byte = SST_MasterIO(0x00);                                                                                     
               CE_High();                                //disable device                                         
               return byte;                              // return one byte read                                      
    }                                                                                                                         
*/
/*******************************************************************************/                                         
/* PROCEDURE:          Read_Cont                               */                                                         
/*                                                             */                                                         
/* This procedure reads mulle consecuttipive addresses of             */                                                  
/* the device and stores the data into DataArray.                             */                                          
/*                                                             */                                                         
/* Input:   Dst:         Destination Address 000000H - 07FFFFH */                                                      
/*          no_bytes     Number of bytes to read                       */                                                 
/*          DataArray     Array storing read data                       */                                                
/*                                                             */                                                         
/* Returns: Nothing                                                */                                                     
/*                                                             */                                                         
/*******************************************************************************/                                         
void read_sector(unsigned char sector, unsigned int no_bytes, unsigned char xdata *DataArray)                                      
{                    
       vf040_busy_bit=1;
       SPCR = 0x50;                                                                                                     
       CE_Low();                     /* enable device */                                                 
       SST_MasterIO(0x03);           /* read command */                                               
       SST_MasterIO(sector >> 4);    /* send 3 address bytes */                                  
       SST_MasterIO(sector << 4);                                                                           
       SST_MasterIO(0);                                                                                      
       while(no_bytes--)
       {                                                                                                              
           *DataArray++ = SST_MasterIO(0x00);   /* receive byte and store in DataArray */                            
       }                                                                                                              
       CE_High();                                /* disable device */                                               
       vf040_busy_bit=0;
}                                                                                                                         

void Read_hzk(unsigned long des, unsigned int no_bytes, unsigned char xdata *DataArray)                                      
{                  
       vf040_busy_bit=1;    
       SPCR = 0x50;
       CE_Low();                     /* enable device */
       SST_MasterIO(0x03);           /* read command */
       SST_MasterIO(des >> 16);      /* send 3 address bytes */
       SST_MasterIO(des >> 8);
       SST_MasterIO(des);	
       while(no_bytes--)
       {                                                                                                              
           *DataArray++ = SST_MasterIO(0x00);   /* receive byte and store in DataArray */                            
       }                                                                                                              
       CE_High();                                /* disable device */                                               
       vf040_busy_bit=0;
}                                                                                                                         

    /*******************************************************************************/                                         
    /* PROCEDURE:          Byte_Program                             */                                                        
    /*                                                             */                                                         
    /* This procedure programs one address of the device.                         */                                          
    /* Assumption:  Address being programmed is already                         */                                            
    /* erased and is NOT block protected.                                    */                                               
    /*                                                             */                                                         
    /* Input:      Dst:         Destination Address 000000H - 07FFFFH */                                                      
    /*           byte:        byte to be programmed                       */                                                  
    /*                                                             */                                                         
    /* Returns: Nothing                                                */                                                     
    /*                                                             */                                                         
    /*******************************************************************************/                                         
/*
    void Byte_Program(unsigned char sector, unsigned char dat)                                                                 
    {                                                                                                                         
               WREN();                                                                                                        
               CE_Low();                     // enable device                                                 
               SST_MasterIO(0x02);           // send Byte Program command                                   
               SST_MasterIO(sector >> 4);    // send 3 address bytes                                
               SST_MasterIO(sector << 4);                                                                           
               SST_MasterIO(0);                                                                                      
               SST_MasterIO(dat);            // send byte to be programmed                                
               CE_High();                    // disable device                                                
               Wait_Busy();                                                                                                   
    }                                                                                                                         
*/
    /*******************************************************************************/                                         
    /* PROCEDURE:          Auto_Add_IncA                            */                                                        
    /*                                                             */                                                         
    /* This procedure programs consecutive addresses of                         */                                            
    /* the device. This is used to start the AAI process.                             */                                      
    /* It should be followed by Auto_Add_IncB.                                 */                                             
    /* Assumption:  Address being programmed is already                         */                                            
    /* erased and is NOT block protected.                                    */                                               
    /*                                                             */                                                         
    /* Input:      Dst:         Destination Address 000000H - 07FFFFH */                                                      
    /*          byte:         byte to be programmed                      */                                                   
    /*                                                             */                                                         
    /* Returns: Nothing                                                */                                                     
    /*                                                             */                                                         
    /*******************************************************************************/                                         

    void Auto_Add_IncA(unsigned char sector, unsigned char dat)                                                                 
    {                                                                                                                         
               WREN();                                                                                                        
               CE_Low();                     // enable device                                         
               SST_MasterIO(0xAF);           // send AAI command 
               SST_MasterIO(sector >> 4);    // send 3 address bytes
               SST_MasterIO(sector << 4);                                                                           
               SST_MasterIO(0);                                                                                      
               SST_MasterIO(dat);            // send byte to be programmed 
               CE_High();                    // disable device               
               Wait_Busy();  
    }                                                                                                                         

    /*******************************************************************************/                                         
    /* PROCEDURE:          Auto_Add_IncB                            */                                                        
    /*                                                             */                                                         
    /* This procedure programs consecutive addresses of                         */                                            
    /* the device. This is used after Auto_Address_IncA.                           */                                         
    /* Assumption:  Address being programmed is already                         */                                            
    /* erased and is NOT block protected.                                    */                                               
    /*                                                             */                                                         
    /* Input:      byte:        byte to be programmed                       */                                                
    /*                                                             */                                                         
    /* Returns: Nothing                                                */                                                     
    /*                                                             */                                                         
    /*******************************************************************************/                                         

    void Auto_Add_IncB(unsigned char byte)                                                                                    
    {                                                                                                                         
               CE_Low();                               // enable device                                            
               SST_MasterIO(0xAF);                     // send AAI command 
               SST_MasterIO(byte);                     // send byte to be programmed 
               CE_High();                              // disable device      
               Wait_Busy();                                                                                                   
    }                                                                                                                         

    /*******************************************************************************/                                         
    /* PROCEDURE: Chip_Erase                                         */                                                       
    /*                                                             */                                                         
    /* This procedure erases the entire Chip.                                  */                                             
    /*                                                             */                                                         
    /* Input:      None                                               */                                                      
    /*                                                             */                                                         
    /* Returns: Nothing                                                */                                                     
    /*                                                             */                                                         
    /*******************************************************************************/                                         
/*
    void Chip_Erase()                                                                                                         
    {                                                                                                                         
               WREN();                                                                                                        
               CE_Low();                                // enable device
               SST_MasterIO(0x60);                      // send Chip Erase command
               CE_High();                               // disable device
               Wait_Busy();                                                                                                   
    }                                                                                                                         
*/
    /*******************************************************************************/                                         
    /* PROCEDURE:          Sector_Erase                              */                                                       
    /*                                                             */                                                         
    /* This procedure Sector Erases the Chip.                                 */                                              
    /*                                                             */                                                         
    /* Input:      Dst:         Destination Address 000000H - 07FFFFH */                                                      
    /*                                                             */                                                         
    /* Returns: Nothing                                                */                                                     
    /*                                                             */                                                         
    /*******************************************************************************/                                         
    void Sector_Erase(unsigned char sector)                                                                                      
    {                                                                                                                         
               WREN();                                                                                                        
               CE_Low();                     // enable device
               SST_MasterIO(0x20);           // send Sector Erase command 
               SST_MasterIO(sector >> 4);    // send 3 address bytes
               SST_MasterIO(sector << 4);                                                                           
               SST_MasterIO(0);                                                                                      
               CE_High();                    // disable device                                             
               Wait_Busy();                                                                                                   
    }                                                                                                                         

void write_sector(unsigned char sec,unsigned int len,unsigned char xdata *buf)
{
      unsigned int i;

      if(sec >= SAVE_DATA_SECTOR && len<0x1000){//用户数据扇区，//限制4096个字节
         vf040_busy_bit=1;
         SPCR = 0x50;
         EWSR();
         WRSR(0x00);
         Sector_Erase(sec);              //擦除一个扇区
         Auto_Add_IncA(sec, buf[0]);     //编程一个扇区
         for(i=1;i<len;i++){
             Auto_Add_IncB(buf[i]);                                                                            
         }    
         WRDI();      
         EWSR();
         WRSR(0x0C);                     //BP0,BP1=1  所有块保护 @20060621
         vf040_busy_bit=0;
      }
}

                                                                                  
