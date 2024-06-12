#include <reg52.h>
#include <absacc.h>
#include <string.h>
#include <intrins.h>
#include "inc/types.h"
#include "inc/hdres.h"
#include "inc/sst_spi.h"
#include "inc/isd4000.h"

void sst_spi_1(unsigned char w_data)
{
      unsigned char tmp;
      SPDR = w_data;
      do{
         tmp = SPSR & 0x80;
      }while(tmp !=0x80);         //busy
      SPSR = 0;
}
/*      
void  isd4000_cmd(unsigned char add0,unsigned char add1,unsigned char cmd)
{
      unsigned int data i;

      SPCR = 0x70;//0x72;     //SPI低位在前   
      ISD_4000_CS_L;   //片选
      sst_spi_1(0);      //ISD上电
      sst_spi_1(ISD_POWER_UP_CMD);
      ISD_4000_CS_H;
      for(i=0;i<10000;i++);  //延迟25ms
//      MC34119_CD_ON;       //本地暂时不响
      ISD_4000_CS_L;   //片选
      sst_spi_1(add0);
      sst_spi_1(add1|cmd);
      ISD_4000_CS_H;
}
*/
void  isd4000_cmd(unsigned char add0)
{
      unsigned int data i;

      SPCR = 0x70;//0x72;     //SPI低位在前   
      ISD_4000_CS_L;   //片选
      sst_spi_1(0);      //ISD上电
      sst_spi_1(ISD_POWER_UP_CMD);
      ISD_4000_CS_H;
      for(i=0;i<10000;i++);  //延迟25ms
//      MC34119_CD_ON;       //本地暂时不响
      ISD_4000_CS_L;   //片选
      sst_spi_1(add0);
      sst_spi_1(ISD_SET_PLAY_CMD);
      ISD_4000_CS_H;
}

/*
void  isd4000_command(unsigned char cmd)
{
      SPCR = 0x70;     //0x72;     //SPI低位在前   
      ISD_4000_CS_L;   //片选 
      sst_spi_1(0);    //ISD上电
      sst_spi_1(cmd);
      ISD_4000_CS_H;
}

void isd4000_init(void)
{
      isd4000_command(ISD_STOP_POWERDOWN_CMD);
}
*/
