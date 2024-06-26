/*****************************************************************
            MCS-51单片机模拟I2C软件包头文件VI2C_C51.H

    这个头文件对应的库是VI2C_C51.LIB,库中有模拟I2C的操作函数,加入
此文件即可使用软件包中的函数，函数是对LPC764的I2C的I／O口实现,即
其P1.3(SDA),P1.2(SCL),MCS－51系列机型可以通用.
           
    注意: 函数是采用软件延时的方法产生SCL脉冲,固对高晶振频率要作 
一定的修改....(本例是1us机器周期,即晶振频率要小于12MHZ).
    (函数的使用可参考给出的事例程序.)
*****************************************************************/

#ifndef  VI2C_C51_H
#define  VI2C_C51_H

#ifndef uchar 
#define uchar unsigned char
#endif


/*******************************************************************
                     无子地址发送字节数据函数               

功能:     从启动总线到发送地址，数据，结束总线的全过程,从器件地址sla.
          如果返回1表示操作成功，否则操作有误。
********************************************************************/
extern bit ISendByte(uchar sla,uchar c);
 




/*******************************************************************
                     有子地址发送多字节数据函数               

功能:     从启动总线到发送地址，子地址,数据，结束总线的全过程,从器件
          地址sla，子地址suba，发送内容是s指向的内容，发送no个字节。
          如果返回1表示操作成功，否则操作有误。
********************************************************************/
extern bit ISendStr(uchar sla,uchar suba,uchar *s,uchar no) ;



/*******************************************************************
                    无子地址发送多字节数据函数   
             
功能:     从启动总线到发送地址，子地址,数据，结束总线的全过程,从器件
          地址sla，发送内容是s指向的内容，发送no个字节。
          如果返回1表示操作成功，否则操作有误。
********************************************************************/
extern bit ISendStrExt(uchar xdata *s,uchar no);



/*******************************************************************
                    无子地址读字节数据函数               

功能:     从启动总线到发送地址，读数据，结束总线的全过程,从器件地
          址sla，返回值在c.
          如果返回1表示操作成功，否则操作有误。
********************************************************************/
extern bit IRcvByte(uchar sla,uchar *c);





/*******************************************************************
                    有子地址读取多字节数据函数               
   
功能:     从启动总线到发送地址，子地址,读数据，结束总线的全过程,从器件
          地址sla，子地址suba，读出的内容放入s指向的存储区，读no个字节。
          如果返回1表示操作成功，否则操作有误。
********************************************************************/
extern bit IRcvStr(uchar sla,uchar suba,uchar *s,uchar no);



/*******************************************************************
                    无子地址读取多字节数据函数               
  
功能:     从启动总线到发送地址,读数据,结束总线的全过程.
          从器件地址sla，读出的内容放入s指向的存储区，
          读no个字节。如果返回1表示操作成功，否则操作有误。
********************************************************************/
extern bit IRcvStrExt(uchar xdata *s,uchar no);

void I_init(void);

#endif