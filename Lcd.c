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
unsigned char xdata new_img_msg;        //ÓÐÐÂµÄÍ¼Ïñ

struct HOUSE_PIC xdata house_pic;
//bit new_img_msg;        //ÓÐÐÂµÄÍ¼Ïñ

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

     if(house_pic.used != 0x55)         //Ê¹ÓÃ±êÖ¾
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
  	    memset(vp, LCD_SHOW_BK_COLOR, 290);      //±³¾°ÉèÎªÀ¶É«
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
        
   
//        XBYTE[EEPROM_PAGE_ADDR] = ICON_ADDR_PAGE+(icon_no>>2);  //Ò³Ñ¡Ôñ
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
/*                 crt_outicon  LCDÉÏÏÔÊ¾Í¼±ê                             */
/* ²ÎÊý:  page=ÏÔÊ¾Ò³ºÅ                                                   */     
/*        col=ÏÔÊ¾ÁÐ                                                      */     
/*        iicon_no=Í¼±ê±àºÅ£¨´Ó0¿ªÊ¼£©                                    */
/* ·µ»Ø£ºÎÞ                                                               */  
/* ÈÕÆÚ£º2001/02/15                                                       */   
/*------------------------------------------------------------------------*/
/* ÐÞ¸Ä¼ÇÂ¼:		                                                  */
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

        Read_hzk(tmp32,_ICON_SIZE/2,icon_data);   //Ò»¸ö×Ö½Ú±£´æÁ½¸öµãµÄÊý¾Ý	

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
    if(qm <= 0x20)    //¿ØÖÆ×Ö·û£¬°üÀ¨»Ø³µ»»ÐÐ,¿Õ¸ñ
       return;	
    if(qm < 0x7F){   //¿ÉÏÔÊ¾µÄASC_II
//       XBYTE[EEPROM_PAGE_ADDR] = (0x6E/2);    //Ò³Ñ¡Ôñ
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
        if(qm < 0x20){    //¿ØÖÆ×Ö·û£¬°üÀ¨»Ø³µ»»ÐÐ
  	   if(qm=='\0')   //½áÊø·ûºÅ
 	      return;
           continue; 
        }             
        if(qm < 0xA1){
	   crt_outchar(page,col_int,qm);
	   col_int += _ASCII_WIDE;  
           continue;     
        }
        if(len)len--;          //@20070719
        qm=(qm-(0xA1))&0x7F;   //½ÚÊ¡¿Õ¼ä£¬×Ö¿â4Çø--15ÇøÎ´Ê¹ÓÃ	 
        if(qm>12)              //1Çø--3Çø,µØÖ·²»±ä
           qm-=12;
        wm =(*pChar++ - 0xA1)&0x7F;
        tmp32 =(qm*94)+wm;
        tmp32 <<= 6;
        Read_hzk(tmp32,63,rom);  //3666
 
//        XBYTE[EEPROM_PAGE_ADDR] = ((tmp16>>7))&0x7F; //Ò³µØÖ·   //¼ÓÒ»ÊÇASCIIÔÚÇ°Ò»Ò³I
//        p = EEPROM_HZK_ADDR|((tmp16<<6)&0x1FFF);     //È¡µÍ12Î»Êý¾Ý    
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

void crt_outnull_num(U_INT_8 page,U_INT_8 col,U_INT_8 num)    //ÏÔÊ¾1¸ö¿ÕÊý×Ö
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
	
                 
void crt_outnull(U_INT_8 page,U_INT_8 col,U_INT_8 num)    //ÏÔÊ¾num¸ö¿Õ×Ö·û
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
	
//ÏÔÊ¾16½øÖÆÊý¾Ý
void crt_out_hex(U_INT_8 page, U_INT_8 col, U_INT_8 hex_num)
{
   unsigned char tmp;
   	
   tmp = (hex_num>>4)+0x30;      
   if(tmp>0x39) tmp+=7;	
   crt_outchar(page, col, tmp);   //ÏÔÊ¾16½øÖÆ¸ßÎ»       
   tmp = (hex_num & 0x0F)+0x30;      
   if(tmp>0x39) tmp+=7;	
   crt_outchar(page, col+_NUMBER_WIDE , tmp); //ÏÔÊ¾16½øÖÆµÍÎ»
}

void crt_outdigital(U_INT_8 page, U_INT_8 col, U_INT_32 dgt, U_INT_8  length,U_INT_8 float_pos)
{
	U_INT_8 	i, num;
	U_INT_32 	data den=1;

        float_pos = length-float_pos;	
	for(i=1;i<length;i++)       
	    den*=10;	
	for(i=0; i<length; i++) {         //length=8 ³¤ÕûÐÎ 4=ÕûÐÎ 2=×Ö·û  Êä³ö¿í¶È
	    num = dgt/den+0x30;	           
	    dgt = dgt%den;
	    den /= 10;
	    if(i==float_pos){            //ÓÐÐ¡ÊýÎ»ÊýÊä³ö
	       crt_outchar(page, col, '.');         /* Ð´Ð¡Êýµã */               	       	
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
      for(col=0;col<12;col++){	           //ÊÖÐÍÍ¼±êÓÃ16*10ºáÏòÈ¡Ä£
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
      for(col=0;col<12;col++){	         //ÊÖÐÍÍ¼±êÓÃ16*10ºáÏòÈ¡Ä£
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
/*                 show_crt_number ÊäÈëÃÜÂëÊ±ÔÚLCDÉÏÏÔÊ¾10¸öÊý×Ö¼üé       */
/* ²ÎÊý£ºÎÞ                                                               */   
/* ·µ»Ø£º ÎÞ                                                              */      
/* ÈÕÆÚ£º2002/12/01                                                       */      
/*------------------------------------------------------------------------*/
/* ÐÞ¸Ä¼ÇÂ¼:		                                                  */
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
//	case 11:				//ÍøÂçÁ¬½Ó¡­Õý³£
//		crt_outhz(4, SPACE_COL, net_ok_menu,14);
//		break;
//	case 12:				//ÍøÂçÁ¬½Ó¡­Òì³£
//		crt_outhz(4, SPACE_COL, net_fail_menu,14);
//		break;
	case 13:				//ÍøÂçÁ¬½Ó
		crt_outhz(2, SPACE_COL,"ÍøÂçÁ¬½Ó",8);
		break;
	case 20:
		for(i=0; i<3; i++){	
		   crt_outicon(2,  i*90+20, i);	   //µÚÒ»ÐÐÍ¼±ê
		   crt_outicon(6,  i*90+20, i+3);	   //µÚ¶þÐÐÍ¼±ê
		   crt_outicon(10, i*90+20, i+6);	   //µÚ¶þÐÐÍ¼±ê
		}
		break;				/* 2.Ö÷²Ëµ¥  ²¼·À²éÑ¯¶Á±íÇóÖúÏû¾¯ÉèÖÃ */
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
	case 32:				/*3.2 ¡®ÃÜÂë£º¡¯*/
                show_crt_number();
		crt_outhz(2,SPACE_COL*2,enter_passwd_menu,5);	
		break;		
	case 33:				//3.3   '²Ù×÷³É¹¦'
		crt_outhz(2, _WORD_WIDE, op_ok_menu,8);
		break;
	case 34:				//3.4   ²Ù×÷Ê§°Ü
		crt_outhz(2, _WORD_WIDE, op_fail_menu, 8);
		break;
	case 35:				
                show_crt_number();
		crt_outhz(2,SPACE_COL*2,"ÐÂÊý¾Ý:",7);	
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
                p = msg_list_menu;		         //£±Ìõ£²Ìõ£³Ìõ£´Ìõ£µÌõ£¶Ìõ£·ÌõÁôÓ°
		for(i=0; i<8; i++){ 
		    crt_outhz(i<4?2:6, 64*(i%4)+SPACE_COL, p, 4);
                    p += 4;
                }     
                crt_outhz(10, SPACE_COL, "·µ»Ø", 4);

                for(i=0;i<7;i++){
                    if(__msg.msg_newness & (1<<i)) 
                       crt_outchar(i<4?2:6, 16-5+64*(i%4), '*');                           
                } 
                if(new_img_msg)
                   crt_outchar(6, 8+64*3, '*');                           
		break;
	case 46:				//ÄêxxÔÂxxÈÕ
		crt_outhz(2, SPACE_COL+40, ymd_menu, 10);
		crt_outhz(4, SPACE_COL+20, tm_menu, 6); //xxÊ±xx·Ö
		break;
	case 80:			       //ÃÜÂë¾²Òô·ÀÇø·¿ºÅËøÃÜµ÷½Ú¿ªµÆ·µ»Ø
		p = set_menu;
		for(i=0; i<4*63; i+=63){ 
           	    crt_outhz(2, i+SPACE_COL+2, p, 4); 
		    p+=4;
		} 
		for(i=0; i<4*63; i+=63){ 
           	    crt_outhz(5, i+SPACE_COL+2, p, 4); 
		    p+=4;
		} 
		crt_outhz(8, SPACE_COL+2, "·µ»Ø", 4);
		break;
	case 81:			       //¾ÉÃÜÂë:
                show_crt_number();
		crt_outhz(2, SPACE_COL*2, old_passwd_menu, 7);
		break;
	case 82:			       //ÐÂÃÜÂë:
                show_crt_number();
		crt_outhz(2, SPACE_COL*2, new_passwd_menu, 7);
		break;
	case 83:			       //ÇëÖØ¸´
                show_crt_number();
		crt_outhz(2, SPACE_COL*2, retry_passwd_menu, 7);   //ÇëÖØ¸´
		break;
	case  85:
                p = system_menu;		         //ÍøÂç¶ÁÊýÅäÖÃµØÖ·¸´Î»Çå³ý·µ»Ø
		for(i=0; i<4; i++){ 
		    crt_outhz(2, 64*i+SPACE_COL, p, 4);
                    p += 4;
                }     
		for(i=0; i<3; i++){     
		    crt_outhz(6, 64*i+SPACE_COL, p, 4); //µÚ¶þÐÐ
                    p += 4;
                }                        
		break;           
	case  115:
		crt_outhz(2,SPACE_COL,now_delay_menu,10);  //ÕýÔÚÑÓÊ±¡£¡£¡£    
		break;	
	case  120:
		crt_outhz(2,SPACE_COL,com_fail_menu,8);   //Í¨Ñ¶Ê§°Ü
		break;	
	case  123:
                p = del_device;	                     //ÏÂÒÆ½ûÓÃ°²×°·µ»Ø
		for(i=0; i<4*63; i+=63){ 
		    crt_outhz(10, i+SPACE_COL+2, p, 4);
                    p += 4;
                }     
                break;
	case  124:
                p = defend_diy;	                     //ÔÚ¼ÒÐÝÏ¢³öÃÅÆäËûÈ·¶¨ÏÂÒÆ·µ»Ø
		for(i=0; i<4*63; i+=63){ 
		    crt_outhz(8, i+SPACE_COL+2, p, 4);
                    p += 4;
                }     
		for(i=0; i<3*63; i+=63){     
		    crt_outhz(10, i+SPACE_COL+2, p, 4); //µÚ¶þÐÐ
                    p += 4;
                }                        
                break;
	case  125:
                p = sys_adjust_menu;	                     //ÒôÐ¡Òô´óµÍÁÁ¸ßÁÁ·µ»Ø
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
	case 126:			       //¾ÉÃÜÂë:
                show_crt_number();
		crt_outhz(2, SPACE_COL*2, "·¿ºÅ:", 5);
		break;
	case 127:			       //¾ÉÃÜÂë:
                show_crt_number();
		crt_outhz(2, SPACE_COL*2, "ËøÃÜÂë:", 7);
		break;
	case 128:	
		crt_outhz(2, SPACE_COL*2, "³··À¿ªµÆ", 8);
		crt_outhz(5, SPACE_COL*2, "±¨¾¯¿ªµÆ", 8);
		crt_outhz(8, SPACE_COL*2, "Ë¢¿¨³··À", 8);
		crt_outhz(11, SPACE_COL*2, "·µ»Ø", 4);
		break;
	case 130:			       //ÏµÍ³Ã¦
		crt_outhz(2, SPACE_COL*2, "ÏµÍ³Ã¦", 6);
		break;
	case 131:			       //
		crt_outhz(2, SPACE_COL*2, "Ã»ÓÐÍ¼Ïñ", 8);
		break;

	}
}

//unsigned char idata start_line;
/**************************************************************************/
/*                 screen_saves    ÆÁÄ»±£»¤³ÌÐò  é                        */
/* ²ÎÊý:£*ÎÞ            Í                                                  */   
/* ·µ»Ø£º ÎÞ                                                               */     
/* ÈÕÆÚ£º2002/03/07                                                        */      
/*------------------------------------------------------------------------*/
/* ÐÞ¸Ä¼ÇÂ¼:		                                                  */
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
          timer_start(_SCREEN_SAVE_TIMER,30);  //µÈ´ý300ms
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
               }else{                //ºº×Ö
                  char_wide=2;
                  tmp = _WORD_WIDE;
               }
               if(col+tmp > (_CRT_MAX_COL+SPACE_COL)){
                  break;    //Õý³£»»ÐÐ
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
               if(bm==0x0A){             //»»ÐÐ·ûºÅ
                  break;
               }
           }     
       }         
       tm[MSG_TIMER]=120; 		//ÏÔÊ¾120S
    } 
}
/**************************************************************************/
/*                 message_in ÔÚÏûÏ¢¶ÔÁÐÖÐÔö¼ÓÐÂÏûÏ¢é                        */
/* ²ÎÊý:£*type¡ª¡ªÏûÏ¢ÀàÐÍ                                                  */   
/*       msg_no¡ª¡ªÏûÏ¢±àºÅ                                                */
/* ·µ»Ø£º ÎÞ                                                               */     
/* ÈÕÆÚ£º2001/06/18                                                        */      
/*------------------------------------------------------------------------*/
/* ÐÞ¸Ä¼ÇÂ¼:		                                                  */
/*    							                  */   
/**************************************************************************/
void message_in(unsigned char type,unsigned int msg_no)
{
    unsigned char i,lost_index,first_msg_no;
    
    first_msg_no =  msg_index[0] & 0x07;
    if(msg_no == __msg.msg_no[first_msg_no]){   //ÏûÏ¢ºÅÏàÍ¬¬  
       return;
    }
    i=MESSAGE_NUM-1;
    lost_index = msg_index[i] & 0x07;    //×îºóÒ»¸öINDEX,µÍ3Î»¿ÉÓÃ
    do{
       i--;
       msg_index[i+1] = msg_index[i];  //index ÏòºóÒÆ¶¯
    }while(i != 0);

    if(lost_index < MESSAGE_NUM){
       __msg.type[lost_index] = type;
       __msg.msg_no[lost_index] = msg_no;
    }
    msg_index[0] = lost_index | 0x80;   //ÐÂÏûÏ¢×Ü·ÅÔÚµÚÒ»¸ö,×ö±ê¼Ç 
    __msg.msg_newness <<=1;      //ÐÂÏûÏ¢±êÖ¾ÓÒÒÆ¶¯
    __msg.msg_newness |=0x01;    //10 0000
    __msg.msg_newness &=0x7F; 
    buzzer_start(15,3);
}   

/**************************************************************************/
/*            message_reindex ÒÆ¶¯ºóÃæµÄÐÂÏûÏ¢                              */
/* ²ÎÊý:£*key¡ª¡ªµ±Ç°¶ÁÏûÏ¢µÄ°´¼ü                                            */    
/* ·µ»Ø£º ÎÞ                                                               */     
/* ÈÕÆÚ£º2001/06/18                                                        */      
/*------------------------------------------------------------------------*/
/* ÐÞ¸Ä¼ÇÂ¼:		                                                  */
/*    							                  */   
/**************************************************************************/
void  message_reindex(unsigned char key)
{
    unsigned char curr_msg;        
    if(key < MESSAGE_NUM){
       curr_msg = msg_index[key];
       if(curr_msg & 0x80){   //ÐÂÏûÏ¢ÒÑ¶Á    
          do{
             if(key+1 < MESSAGE_NUM && (msg_index[key+1] & 0x80)){  //ÏÂÒ»¸öÏûÏ¢ÊÇÐÂÏûÏ¢
                msg_index[key] = msg_index[key+1];    //ÐÂÏûÏ¢ÏòÇ°ÒÆ¶¯
                __msg.msg_newness |=(1<<key);
                key++;
             }               
             else
                break;      //ºóÃæÃ»ÓÐÐÂÏûÏ¢
          }while(key<MESSAGE_NUM);
          if(key < MESSAGE_NUM)
             msg_index[key] =  curr_msg & 0x07;   //µ±Ç°ÏûÏ¢ºóÒÆ
       }
       __msg.msg_newness &=(~(1<< key));  //µ±Ç°ÏûÏ¢Îª¾ÉÏûÏ¢         
    }
}    

