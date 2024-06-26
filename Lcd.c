#include <reg52.h>
#include <absacc.h>
#include <string.h>
#include "inc/types.h"
#include "inc/hdres.h"
#include "inc/timer.h"
#include "inc/lcd.h"
#include "inc/ctrl.h"
#include "inc/menu.h"
#include "inc/key.h"
#include "inc/system.h"
#include "inc/vf040.h"
#include "hzk.c"
//#include "inc/icon.h"

struct MESSAGE xdata __msg;
unsigned char xdata  msg_index[MESSAGE_NUM];
extern bit __start_show;
unsigned char xdata new_img_msg;        //有新的图像

struct HOUSE_PIC xdata house_pic;
//bit new_img_msg;        //有新的图像

void crt_init(void)
{

    unsigned char i;
    for(i=0;i<MESSAGE_NUM;i++){
        msg_index[i] = i;
    }
}

//void mem_cpy(

void show_room_pic(void)
{
     unsigned char xdata *pic;
     unsigned char xdata *xp,*vp;
     unsigned char high,wide,i,tmp,page; 
     unsigned char rol_data_len;
     bit b8; 
     unsigned int data len;
     unsigned char xdata rol_data[50];

     if(house_pic.used != 0x55)         //使用标志
        return;
     rol_data_len=0;
     XBYTE[VRAM_PAGE_ADDR] = page =1;   //PAGE ADDR
     vp= xp= VRAM_ADDR+5;
     pic=house_pic.dat;	
     wide = house_pic.wide;
     high = house_pic.high;;
     while(high--){
	  len =0;
	  for(i=0;i<50;i++){
              tmp = *pic;
	      if(tmp==0x00){
		 pic++;
		 break; 	
              } 
    	      rol_data[i] = tmp;
	      len += tmp&0x7F;  	   	     
              rol_data_len=i;	 
	      pic++;
	      if(len>=wide)	
		 break;
          } 	    	

	  for(i=0;i<rol_data_len+1;i++){
       	      tmp=rol_data[i];               		       
	      b8 = tmp&0x80;
              tmp &= 0x7F;  	   	     	       
              if(b8){
	         while(tmp--)
		    *xp++ = LCD_YELLOW;		 
	      }
	      else
                 xp += tmp; 	    		
          }
	  vp += 512;
          if(vp >= VRAM_PAGE_SIZE+VRAM_ADDR){
             vp = VRAM_ADDR+5; 
             XBYTE[VRAM_PAGE_ADDR] = ++page;   //VRAM PAGE ADDR
          } 
	  xp = vp;
      }
}
/*  ACC  */
/*
sbit ACC_7   = ACC^7;
sbit ACC_6   = ACC^6;
sbit ACC_5   = ACC^5;
sbit ACC_4   = ACC^4;
sbit ACC_3   = ACC^3;
sbit ACC_2   = ACC^2;
sbit ACC_1   = ACC^1;
sbit ACC_0   = ACC^0;
*/
static unsigned char bdata mask_color;
sbit ACC_7   = mask_color^7;
sbit ACC_6   = mask_color^6;
sbit ACC_5   = mask_color^5;
sbit ACC_4   = mask_color^4;
sbit ACC_3   = mask_color^3;
sbit ACC_2   = mask_color^2;
sbit ACC_1   = mask_color^1;
sbit ACC_0   = mask_color^0;

#define TST_COLOR  LCD_WHITE                  
static void crt_outline(U_INT_8 xdata *vp, U_INT_8 mask) 
{
 
     mask_color = mask;
     if(ACC_7)
        *vp = TST_COLOR;
     if(ACC_6)
        *(vp+1) = TST_COLOR;
     if(ACC_5)
        *(vp+2) = TST_COLOR;
     if(ACC_4)
        *(vp+3) = TST_COLOR;
     if(ACC_3)
        *(vp+4) = TST_COLOR;
     if(ACC_2)
        *(vp+5) = TST_COLOR;
     if(ACC_1)
        *(vp+6) = TST_COLOR;
     if(ACC_0)
        *(vp+7) = TST_COLOR;

/*
     if(mask & 0x80)
        *vp = TST_COLOR;
     if(mask & 0x40)
        *(vp+1) = TST_COLOR;
     if(mask & 0x20)
        *(vp+2) = TST_COLOR;
     if(mask & 0x10)
        *(vp+3) = TST_COLOR;
     if(mask & 0x08)
        *(vp+4) = TST_COLOR;
     if(mask & 0x04)
        *(vp+5) = TST_COLOR;
     if(mask & 0x02)
        *(vp+6) = TST_COLOR;
     if(mask & 0x01)
        *(vp+7) = TST_COLOR;
*/
}
 	
void crt_clear(void)
{
    unsigned char page,j;
    unsigned char xdata *vp;

    for(page=0; page<VRAM_PAGE_NUM; page++){
        XBYTE[VRAM_PAGE_ADDR] = page;   //PAGE ADDR
	vp = VRAM_ADDR;
	for(j=0;j<16;j++){
  	    memset(vp, LCD_SHOW_BK_COLOR, 290);      //背景设为蓝色
            vp += 512;
        }        
    }
}
/*
void crt_out_smallicon(U_INT_8 page,U_INT_8 col,U_INT_8 icon_no) 
{
	U_INT_8	i;
        unsigned char xdata *vp;
	U_INT_8	code  *p = up_down;
        
   
//        XBYTE[EEPROM_PAGE_ADDR] = ICON_ADDR_PAGE+(icon_no>>2);  //页选择
//        p = EEPROM_ADDR+_ICON_SIZE*(icon_no%4);
        XBYTE[VRAM_PAGE_ADDR] = page;     //VRAM PAGE ADDR
        vp =  VRAM_ADDR +col;
	for(i=0;i<26;i++){	  
            crt_outline(vp, *p++);
            crt_outline(vp+8, *p++);
            crt_outline(vp+16, *p++);
            vp += 512;
            if(vp >= 8192+VRAM_ADDR){
               vp -= 8192; 
               XBYTE[VRAM_PAGE_ADDR] = ++page;   //VRAM PAGE ADDR
            } 
	}
}
*/


/**************************************************************************/
/*                 crt_outicon  LCD上显示图标                             */
/* 参数:  page=显示页号                                                   */     
/*        col=显示列                                                      */     
/*        iicon_no=图标编号（从0开始）                                    */
/* 返回：无                                                               */  
/* 日期：2001/02/15                                                       */   
/*------------------------------------------------------------------------*/
/* 修改记录:		                                                  */
/*    							                  */
/**************************************************************************/                   
void crt_outicon(U_INT_8 page,U_INT_8 col,U_INT_8 icon_no) 
{
	U_INT_8	i;
        unsigned long tmp32;
        unsigned char xdata *vp;
	U_INT_8	xdata *p;//=icon+288*icon_no;	
        unsigned char xdata icon_data[_ICON_SIZE];	
        
        tmp32 = ICON_DATA_ADDR+(_ICON_SIZE/2)*icon_no;

        Read_hzk(tmp32,_ICON_SIZE/2,icon_data);   //一个字节保存两个点的数据	

        p = icon_data; 
        XBYTE[VRAM_PAGE_ADDR] = page;     //VRAM PAGE ADDR
        vp =  VRAM_ADDR + col;
	for(i=0;i<_ICON_HIGH;i++){	  
            //memcpy(vp, p, _ICON_WIDTH/2);
            fast_write_icon(vp, p, _ICON_WIDTH/2);
            p += _ICON_WIDTH/2; 	    		
            vp += 512;
            if(vp >= 8192+VRAM_ADDR){
               vp -= 8192; 
               XBYTE[VRAM_PAGE_ADDR] = ++page;   //VRAM PAGE ADDR
            } 
	}
}

void crt_outchar(U_INT_8 page, U_INT_16 col, U_INT_8 qm) 
{
    unsigned char i;
    unsigned long tmp32;

    unsigned char xdata *p;
    unsigned char xdata *vp;
    unsigned char xdata rom[100];
    if(qm <= 0x20)    //控制字符，包括回车换行,空格
       return;	
    if(qm < 0x7F){   //可显示的ASC_II
//       XBYTE[EEPROM_PAGE_ADDR] = (0x6E/2);    //页选择
       qm -= 0x20;
       tmp32 = ASCII_DATA_ADDR+qm*36;
       Read_hzk(tmp32,36,rom);  	
//       p = (EEPROM_ADDR+0x1000)+qm*36;
//       memcpy(rom, p, 36); //_ASCII_WIDE*2);     //read ASCII data		
       p = rom; 	
       XBYTE[VRAM_PAGE_ADDR] = page;      //VRAM PAGE ADDR
       vp = VRAM_ADDR;
       vp += col;
       for(i=0;i<_ASCII_HIGH;i++){
           crt_outline(vp, *p++);
           crt_outline(vp+8, *p++);
           vp += 512;
           if(vp >= 8192+VRAM_ADDR){
              vp -= 8192; 
              XBYTE[VRAM_PAGE_ADDR] = ++page;   //VRAM PAGE ADDR
           } 
       }
    }	
}

void crt_outhz(U_INT_8 page, U_INT_8 col, U_INT_8 *pChar, U_INT_8 len) 
{
    unsigned char xdata *p,*vp;
    unsigned long  tmp32;
    unsigned char i,qm,wm;
    unsigned char xdata rom[100];
    unsigned int  col_int=col;
     while(len--){
        qm = *pChar++;
        if(qm < 0x20){    //控制字符，包括回车换行
  	   if(qm=='\0')   //结束符号
 	      return;
           continue; 
        }             
        if(qm < 0xA1){
	   crt_outchar(page,col_int,qm);
	   col_int += _ASCII_WIDE;  
           continue;     
        }
        if(len)len--;          //@20070719
        qm=(qm-(0xA1))&0x7F;   //节省空间，字库4区--15区未使用	 
        if(qm>12)              //1区--3区,地址不变
           qm-=12;
        wm =(*pChar++ - 0xA1)&0x7F;
        tmp32 =(qm*94)+wm;
        tmp32 <<= 6;
        Read_hzk(tmp32,63,rom);  //3666
 
//        XBYTE[EEPROM_PAGE_ADDR] = ((tmp16>>7))&0x7F; //页地址   //加一是ASCII在前一页I
//        p = EEPROM_HZK_ADDR|((tmp16<<6)&0x1FFF);     //取低12位数据    
//        memcpy(rom, p, 63);     //read HZK data		
        p = rom;
        XBYTE[VRAM_PAGE_ADDR] = qm=page;   //VRAM PAGE ADDR
        vp = VRAM_ADDR;
        vp += col_int;//+_WORD_WIDE;           //0x100;
        for(i=0;i<_WORD_HIGH;i++){
            crt_outline(vp,*p++);
            crt_outline(vp+8,*p++);
            crt_outline(vp+16,*p++);
            vp +=512;
            if(vp >= 8192+VRAM_ADDR){
               vp -= 8192; 
               XBYTE[VRAM_PAGE_ADDR] = ++qm;   //VRAM PAGE ADDR
            } 
        }
        col_int+=_WORD_WIDE;  
    }
}

void crt_outnull_num(U_INT_8 page,U_INT_8 col,U_INT_8 num)    //显示1个空数字
{
   unsigned char xdata *vp;
   unsigned char i,j;  

     XBYTE[VRAM_PAGE_ADDR] = page;   //VRAM PAGE ADDR
     vp = VRAM_ADDR+col;   
     for(i=0; i<_ASCII_HIGH; i++){
       for(j=0;j<_NUMBER_WIDE*num;j++)	
           *(vp+j) = LCD_SHOW_BK_COLOR;
       vp += 512;
       if(vp >= VRAM_PAGE_SIZE+VRAM_ADDR){
          vp -= VRAM_PAGE_SIZE; 
          XBYTE[VRAM_PAGE_ADDR] = ++page;   //VRAM PAGE ADDR
       } 
    }     
}
	
                 
void crt_outnull(U_INT_8 page,U_INT_8 col,U_INT_8 num)    //显示num个空字符
{
   unsigned char xdata *vp;
   unsigned char i,j;  

     XBYTE[VRAM_PAGE_ADDR] = page;   //VRAM PAGE ADDR
     vp = VRAM_ADDR+col;
     for(i=0; i<_WORD_HIGH; i++){
         for(j=0;j<_WORD_WIDE*num;j++)	
             *(vp+j) = LCD_SHOW_BK_COLOR;
         vp += 512;
         if(vp >= VRAM_PAGE_SIZE+VRAM_ADDR){
            vp -= VRAM_PAGE_SIZE; 
            XBYTE[VRAM_PAGE_ADDR] = ++page;   //VRAM PAGE ADDR
         } 
     }
}
	
//显示16进制数据
void crt_out_hex(U_INT_8 page, U_INT_8 col, U_INT_8 hex_num)
{
   unsigned char tmp;
   	
   tmp = (hex_num>>4)+0x30;      
   if(tmp>0x39) tmp+=7;	
   crt_outchar(page, col, tmp);   //显示16进制高位       
   tmp = (hex_num & 0x0F)+0x30;      
   if(tmp>0x39) tmp+=7;	
   crt_outchar(page, col+_NUMBER_WIDE , tmp); //显示16进制低位
}

void crt_outdigital(U_INT_8 page, U_INT_8 col, U_INT_32 dgt, U_INT_8  length,U_INT_8 float_pos)
{
	U_INT_8 	i, num;
	U_INT_32 	data den=1;

        float_pos = length-float_pos;	
	for(i=1;i<length;i++)       
	    den*=10;	
	for(i=0; i<length; i++) {         //length=8 长整形 4=整形 2=字符  输出宽度
	    num = dgt/den+0x30;	           
	    dgt = dgt%den;
	    den /= 10;
	    if(i==float_pos){            //有小数位数输出
	       crt_outchar(page, col, '.');         /* 写小数点 */               	       	
	       col += _NUMBER_WIDE;
	    }
	    crt_outchar(page, col, num);        
            col += _NUMBER_WIDE;
	}
}
/*
void crt_clear_hand(unsigned char page,unsigned char col)
{       
      unsigned char xdata *vp;
      unsigned char i;
      XBYTE[VRAM_PAGE_ADDR] = page;     //VRAM PAGE ADDR
      vp=VRAM_ADDR+col;
      for(col=0;col<12;col++){	           //手型图标用16*10横向取模
	  for(i=0;i<24;i++)	
              *(vp+i) = 0x00;
          vp += 512;
          if(vp >= (4096+VRAM_ADDR)){
             vp -= 4096; 
             XBYTE[VRAM_PAGE_ADDR] = page+1;   //VRAM PAGE ADDR
          } 
      }
}
*/
/*
void crt_show_hand(unsigned char page,unsigned char col)
{
      unsigned char xdata *vp;
      unsigned char code  *cp=head;
      XBYTE[VRAM_PAGE_ADDR] = page;     //VRAM PAGE ADDR
      vp =  VRAM_ADDR + col;
      for(col=0;col<12;col++){	         //手型图标用16*10横向取模
          crt_outline(vp,  *cp++);
          crt_outline(vp+8,*cp++);
          crt_outline(vp+16,*cp++);
          vp +=512;
          if(vp >= (4096+VRAM_ADDR)){
             vp -= 4096; 
             XBYTE[VRAM_PAGE_ADDR] = page+1;   //VRAM PAGE ADDR
          } 
      }
}
*/      
   
/**************************************************************************/
/*                 show_crt_number 输入密码时在LCD上显示10个数字键�       */
/* 参数：无                                                               */   
/* 返回： 无                                                              */      
/* 日期：2002/12/01                                                       */      
/*------------------------------------------------------------------------*/
/* 修改记录:		                                                  */
/*    							                                  */   
/**************************************************************************/
void show_crt_number(void)
{
      unsigned char code *p;
      unsigned char i,col;

      col =18*2;
      for(i=0;i<5;i++){              
          crt_outchar(4, col, i+0x30);   //01234 
          crt_outchar(7, col, i+0x35);   //56789 
	  col += 48;
      }
      crt_outchar(11, 18*2, '*');    //*
      crt_outchar(11, 18*2+40, '#'); //#

      p = ok_del_ret;
      col = 18*2+40+40;
//      col = 30;
      for(i=0; i<3; i++){ 
          crt_outhz(11, col, p, 4);
//          col += 60;
          col += 50;
          p += 4;
      }     
}

void draw_screen(U_INT_8 num)
{
        U_INT_8 i;
	unsigned char code *p;

	crt_clear();
	switch(num) {
//	case 11:				//网络连接…正常
//		crt_outhz(4, SPACE_COL, net_ok_menu,14);
//		break;
//	case 12:				//网络连接…异常
//		crt_outhz(4, SPACE_COL, net_fail_menu,14);
//		break;
	case 13:				//网络连接
		crt_outhz(2, SPACE_COL,"网络连接",8);
		break;
	case 20:
		for(i=0; i<3; i++){	
		   crt_outicon(2,  i*90+20, i);	   //第一行图标
		   crt_outicon(6,  i*90+20, i+3);	   //第二行图标
		   crt_outicon(10, i*90+20, i+6);	   //第二行图标
		}
		break;				/* 2.主菜单  布防查询读表求助消警设置 */
	case 31:	
		p = defend_menu;
		for(i=0; i<4*63; i+=63){ 
           	    crt_outhz(2, i+SPACE_COL+2, p, 4); 
		    p+=4;
		} 
		for(i=0; i<3*63; i+=63){ 
           	    crt_outhz(5, i+SPACE_COL+2, p, 4); 
		    p+=4;
		} 
		break;		
	case 32:				/*3.2 ‘密码：’*/
                show_crt_number();
		crt_outhz(2,SPACE_COL*2,enter_passwd_menu,5);	
		break;		
	case 33:				//3.3   '操作成功'
		crt_outhz(2, _WORD_WIDE, op_ok_menu,8);
		break;
	case 34:				//3.4   操作失败
		crt_outhz(2, _WORD_WIDE, op_fail_menu, 8);
		break;
	case 35:				
                show_crt_number();
		crt_outhz(2,SPACE_COL*2,"新数据:",7);	
		break;		
	case 40:
		p = query_menu;
		for(i=0; i<4*63; i+=63){ 
           	    crt_outhz(2, i+SPACE_COL+2, p, 4); 
		    p+=4;
		} 
//        	crt_outhz(5, SPACE_COL+2, p, 4); 	
		break;
	case 43:	
                p = msg_list_menu;		         //１条２条３条４条５条６条７条留影
		for(i=0; i<8; i++){ 
		    crt_outhz(i<4?2:6, 64*(i%4)+SPACE_COL, p, 4);
                    p += 4;
                }     
                crt_outhz(10, SPACE_COL, "返回", 4);

                for(i=0;i<7;i++){
                    if(__msg.msg_newness & (1<<i)) 
                       crt_outchar(i<4?2:6, 16-5+64*(i%4), '*');                           
                } 
                if(new_img_msg)
                   crt_outchar(6, 8+64*3, '*');                           
		break;
	case 46:				//年xx月xx日
		crt_outhz(2, SPACE_COL+40, ymd_menu, 10);
		crt_outhz(4, SPACE_COL+20, tm_menu, 6); //xx时xx分
		break;
	case 80:			       //密码静音防区房号锁密调节开灯返回
		p = set_menu;
		for(i=0; i<4*63; i+=63){ 
           	    crt_outhz(2, i+SPACE_COL+2, p, 4); 
		    p+=4;
		} 
		for(i=0; i<4*63; i+=63){ 
           	    crt_outhz(5, i+SPACE_COL+2, p, 4); 
		    p+=4;
		} 
		crt_outhz(8, SPACE_COL+2, "返回", 4);
		break;
	case 81:			       //旧密码:
                show_crt_number();
		crt_outhz(2, SPACE_COL*2, old_passwd_menu, 7);
		break;
	case 82:			       //新密码:
                show_crt_number();
		crt_outhz(2, SPACE_COL*2, new_passwd_menu, 7);
		break;
	case 83:			       //请重复
                show_crt_number();
		crt_outhz(2, SPACE_COL*2, retry_passwd_menu, 7);   //请重复
		break;
	case  85:
                p = system_menu;		         //网络读数配置地址复位清除返回
		for(i=0; i<4; i++){ 
		    crt_outhz(2, 64*i+SPACE_COL, p, 4);
                    p += 4;
                }     
		for(i=0; i<3; i++){     
		    crt_outhz(6, 64*i+SPACE_COL, p, 4); //第二行
                    p += 4;
                }                        
		break;           
	case  115:
		crt_outhz(2,SPACE_COL,now_delay_menu,10);  //正在延时。。。    
		break;	
	case  120:
		crt_outhz(2,SPACE_COL,com_fail_menu,8);   //通讯失败
		break;	
	case  123:
                p = del_device;	                     //下移禁用安装返回
		for(i=0; i<4*63; i+=63){ 
		    crt_outhz(10, i+SPACE_COL+2, p, 4);
                    p += 4;
                }     
                break;
	case  124:
                p = defend_diy;	                     //在家休息出门其他确定下移返回
		for(i=0; i<4*63; i+=63){ 
		    crt_outhz(8, i+SPACE_COL+2, p, 4);
                    p += 4;
                }     
		for(i=0; i<3*63; i+=63){     
		    crt_outhz(10, i+SPACE_COL+2, p, 4); //第二行
                    p += 4;
                }                        
                break;
	case  125:
                p = sys_adjust_menu;	                     //音小音大低亮高亮返回
		for(i=0; i<4*63; i+=63){ 
		    crt_outhz(2, i+SPACE_COL+2, p, 4);
                    p += 4;
                }                     
		for(i=0; i<2*63; i+=63){ 
		    crt_outhz(5, i+SPACE_COL+2, p, 4);
                    p += 4;
                }                     
//                crt_outhz(5, SPACE_COL+2, p, 4);
                break;
	case 126:			       //旧密码:
                show_crt_number();
		crt_outhz(2, SPACE_COL*2, "房号:", 5);
		break;
	case 127:			       //旧密码:
                show_crt_number();
		crt_outhz(2, SPACE_COL*2, "锁密码:", 7);
		break;
	case 128:	
		crt_outhz(2, SPACE_COL*2, "撤防开灯", 8);
		crt_outhz(5, SPACE_COL*2, "报警开灯", 8);
		crt_outhz(8, SPACE_COL*2, "刷卡撤防", 8);
		crt_outhz(11, SPACE_COL*2, "返回", 4);
		break;
	case 130:			       //系统忙
		crt_outhz(2, SPACE_COL*2, "系统忙", 6);
		break;
	case 131:			       //
		crt_outhz(2, SPACE_COL*2, "没有图像", 8);
		break;

	}
}

//unsigned char idata start_line;
/**************************************************************************/
/*                 screen_saves    屏幕保护程序  �                        */
/* 参数:�*无            �                                                  */   
/* 返回： 无                                                               */     
/* 日期：2002/03/07                                                        */      
/*------------------------------------------------------------------------*/
/* 修改记录:		                                                  */
/*    							                  */   
/**************************************************************************/
/*
void screen_saves(void)
{
    if(timer_out(_SCREEN_SAVE_TIMER)){
       set_start_line(start_line);
        
       if(++start_line > 0x3F){
          start_line = 0;
          timer_start(_SCREEN_SAVE_TIMER,0xFF);  
       } 
       else
          timer_start(_SCREEN_SAVE_TIMER,30);  //等待300ms
    } 
}
*/

unsigned char xdata * xdata msg_xp;
void show_msg(void)
{
    U_INT_8  i,tmp,bm;
    unsigned char col;
    unsigned char char_wide;

    if(__start_show && tm[MSG_TIMER]==0){  
       if(__msg.start>=__msg.end){
  	  __start_show = 0;
	  if(__menu_layer==SUB_MENU_2){
	     draw_screen(43);
	  }
          key_in(AUTO_RETURN_KEY);   //
	  return;
       } 
       crt_clear();
       for(i=1; i<_CRT_MAX_ROL; i+=2){
           if(__msg.start>=__msg.end)
              break;
           col = SPACE_COL;
           while(1){  
               bm = *msg_xp;   
               if(bm<0xA1){          //ASCII
                  char_wide=1;
                  tmp =_ASCII_WIDE;
               }else{                //汉字
                  char_wide=2;
                  tmp = _WORD_WIDE;
               }
               if(col+tmp > (_CRT_MAX_COL+SPACE_COL)){
                  break;    //正常换行
               }                        
               crt_outhz(i,col,msg_xp++,char_wide);     
               if(char_wide==2){
                  msg_xp++;         
                  col+=_WORD_WIDE;   
               }
               else 
                  col+=_ASCII_WIDE;                                     
               if(++__msg.start>=__msg.end)
                  break;   
               if(bm==0x0A){             //换行符号
                  break;
               }
           }     
       }         
       tm[MSG_TIMER]=120; 		//显示120S
    } 
}
/**************************************************************************/
/*                 message_in 在消息对列中增加新消息�                        */
/* 参数:�*type——消息类型                                                  */   
/*       msg_no——消息编号                                                */
/* 返回： 无                                                               */     
/* 日期：2001/06/18                                                        */      
/*------------------------------------------------------------------------*/
/* 修改记录:		                                                  */
/*    							                  */   
/**************************************************************************/
void message_in(unsigned char type,unsigned int msg_no)
{
    unsigned char i,lost_index,first_msg_no;
    
    first_msg_no =  msg_index[0] & 0x07;
    if(msg_no == __msg.msg_no[first_msg_no]){   //消息号相同�  
       return;
    }
    i=MESSAGE_NUM-1;
    lost_index = msg_index[i] & 0x07;    //最后一个INDEX,低3位可用
    do{
       i--;
       msg_index[i+1] = msg_index[i];  //index 向后移动
    }while(i != 0);

    if(lost_index < MESSAGE_NUM){
       __msg.type[lost_index] = type;
       __msg.msg_no[lost_index] = msg_no;
    }
    msg_index[0] = lost_index | 0x80;   //新消息总放在第一个,做标记 
    __msg.msg_newness <<=1;      //新消息标志右移动
    __msg.msg_newness |=0x01;    //10 0000
    __msg.msg_newness &=0x7F; 
    buzzer_start(15,3);
}   

/**************************************************************************/
/*            message_reindex 移动后面的新消息                              */
/* 参数:�*key——当前读消息的按键                                            */    
/* 返回： 无                                                               */     
/* 日期：2001/06/18                                                        */      
/*------------------------------------------------------------------------*/
/* 修改记录:		                                                  */
/*    							                  */   
/**************************************************************************/
void  message_reindex(unsigned char key)
{
    unsigned char curr_msg;        
    if(key < MESSAGE_NUM){
       curr_msg = msg_index[key];
       if(curr_msg & 0x80){   //新消息已读    
          do{
             if(key+1 < MESSAGE_NUM && (msg_index[key+1] & 0x80)){  //下一个消息是新消息
                msg_index[key] = msg_index[key+1];    //新消息向前移动
                __msg.msg_newness |=(1<<key);
                key++;
             }               
             else
                break;      //后面没有新消息
          }while(key<MESSAGE_NUM);
          if(key < MESSAGE_NUM)
             msg_index[key] =  curr_msg & 0x07;   //当前消息后移
       }
       __msg.msg_newness &=(~(1<< key));  //当前消息为旧消息         
    }
}    

