#include <reg52.h>
#include <absacc.h>
#include <string.h>
#include <intrins.h>
#include "inc/types.h"
#include "inc/hdres.h"
#include "inc/sst_spi.h"

/*
void init_spi(void)
{
      P1 = 0xFF;
      SPCR = 0x52;
}
*/
unsigned char sst_spi(unsigned char w_data)
{
      unsigned char tmp;
      SPDR = w_data;
      do{
         tmp = SPSR & 0x80;
      }while(tmp !=0x80);        //busy
      SPSR = 0;
      _nop_();
      _nop_();
      return SPDR;               //返回数据也在此
}
