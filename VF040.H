#ifndef _VF040_H_
#define _VF040_H_  1

#define VF040_CE   P14
#define CE_Low()   (VF040_CE=0)
#define CE_High()  (VF040_CE=1)

extern bit vf040_busy_bit;

void Read_hzk(unsigned long des, unsigned int no_bytes, unsigned char xdata *DataArray);
void read_sector(unsigned char sector, unsigned int no_bytes, unsigned char xdata *DataArray);
void write_sector(unsigned char sec,unsigned int len,unsigned char xdata *buf);

#endif



