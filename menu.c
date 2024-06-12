#include <reg51.h>
#include <string.h>
#include <intrins.h>
#include <absacc.h>
#include <stdlib.h>
#include <math.h>
#include "inc/types.h"
#include "inc/hdres.h"
#include "inc/timer.h"
#include "inc/lcd.h"
#include "inc/key.h"
#include "inc/alarm.h"
#include "inc/ctrl.h"
#include "inc/asyn.h"
#include "inc/menu.h"
#include "inc/system.h"
#include "inc/app.h"
#include "inc/mac.h"
#include "inc/ip.h"
#include "inc/skbuff.h"
#include "inc/tool.h"
#include "inc/isd4000.h"
#include "inc/mt8888.h"

//bit __flash_msgno=0;
bit __defend_fail_beep;
bit __start_show;
bit __return_ok;
//bit __ndt_reset;

//bit init_data_flag=0;
//bit bit_screen_save_run;

static unsigned char code * idata pre_cp;

struct IR_MENU xdata ir_ctrl;
struct LIGHT_CTRL_MENU xdata light_ctrl;

RESEND_BUF xdata resend_buf;

U_INT_8 idata __menu_layer=START_MENU;    //²Ëµ¥²ã±êÖ¾  0=MAIN 1=sub1 2=sub2

unsigned char get_xy(unsigned char num,unsigned char col_limit,unsigned char code *cp);


void op_status(bit ok)         //ÏÔÊ¾²Ù×÷³É¹¦ ²¢´¦Àí·µ»Ø  1=ok,0=fail
{
    if(ok){
        draw_screen(33);     
    }
    else{
        draw_screen(34);     
    }     
//    delay(100);       //±ØÐëÊÇ1s
    key_get();
}     
#if 0
void network_status(bit ok)         //ÏÔÊ¾ÍøÂç×´Ì¬³É¹¦ ²¢´¦Àí·µ»Ø  1=ok,0=fail
{
    extern unsigned char code net_status_menu[];
    unsigned char code *cp =net_status_menu;
    if(ok==0)
       cp+=4;
    crt_outhz(4, SPACE_COL, cp,4);
    key_get();
}
#endif
/**************************************************/
//ÏÔÊ¾Êý×Ö¼üÅÌ£¬ÊäÈëÊý×Ö0--9
//   pin  ÃÜÂëÊäÈë±êÖ¾Î»
//   xp   Êý¾Ý´æ·ÅµØÖ· 
//   len  ´ýÊäÈëµÄ×Ö·ûÊýÁ¿
//   ·µ»Ø: x    ÒÑ¾­ÊäÈëµÄ×Ö·ûÊýÁ¿
//         0xff ÓÐ·µ»Ø¼ü 
/*************************************************/
unsigned char modify_data(bit pin,unsigned char xdata *xp,unsigned char len)
{
    unsigned char idata i,key;
    char idata show_char;
/*
    unsigned char code xy[]={0x4F,10,       0x38,10, 0x54,10, 0x7C,10, 0xA5,10, 0xCC,10,
                             0x74,10|0x50,  0x38,10, 0x54,10, 0x7C,10, 0xA5,10, 0xCC,10,
                             0x9D,10|0xa0,  0x37,15, 0x67,15 ,0x9A,15, 0,0,};
*/
    unsigned char code xy[]={0x4F,10,       0x32,15, 0x58,15, 0x80,15, 0xA6,15, 0xCE,15,
                             0x74,10|0x50,  0x32,15, 0x58,15, 0x80,15, 0xA6,15, 0xCE,15,
                             0x9D,10|0xA0,  0x38,15, 0x5C,15 ,0x85,15, 0xAC,15, 0xD5,15};

    i=0;
    if(len==0)len=6;
    do{ 
       key=get_xy(3,5,xy);             
       if(key < 12 && i<len){
          switch(key){
             case 10:
               key=XING;      //*->11  
               show_char = '*';
               break;
             case 11:
               key=JING;      //*->12  
               show_char = '#';
               break;
             default:      //0~9  
               show_char=pin?'*':key+0x30;
               break; 
          }  
          xp[i] = pin?key:key+0x30;    
          crt_outchar(2,16+64+32+i*_NUMBER_WIDE,show_char);
	  i++;
          if(pin && (i==len))
             return i;  
	  continue;
       }  
       if(key==12){   //È·¶¨
          return i;
       } 
       if((key==13) && i){  //É¾³ý
          xp[--i]=0x30;
          crt_outnull_num(2,16+64+32+i*_NUMBER_WIDE,1);
       }                 
    }while(key < 14);
    return 0xFF;
}

unsigned char  pw_verify(U_INT_8 type) //0=ÐÞ¸ÄÓÃ»§ÃÜÂë; 1=ºË¶ÔÃÜÂë; 3=ºË¶ÔÏµÍ³ÃÜÂë
{
    U_INT_8 data pin_len;
    U_INT_8 xdata buf[40],tmp[_USER_PIN_LEN],in_pw[_USER_PIN_LEN];
    
    if(type<2){ 
        pin_len=__sys_passwd.Pass_Num;   //ÓÃ»§ÃÜÂë³¤¶È
        if(pin_len==0 && type==1)
           return 1;           //ÓÃ»§ÃÜÂë³¤¶ÈÎªÁã£¬²»Ð£Ñé¿ÚÁî
        if(pin_len > _USER_PIN_LEN)
           pin_len = _USER_PIN_LEN;
        memcpy(tmp,__sys_passwd.Password,pin_len);
    }
    else{
        pin_len=_SYS_PASSWD_LEN;         //ÏµÍ³ÃÜÂë³¤¶È
        memcpy(tmp,__sys_passwd.Syspasswd,pin_len);
    }
    if(type) draw_screen(32);      //ÃÜÂë
    else     draw_screen(81);      //¾ÉÃÜÂë£º

    memset(in_pw,0xA5,_USER_PIN_LEN);
    if(modify_data(1,in_pw,pin_len) > _USER_PIN_LEN)
       return _FALSE;
#if SHOW_SELF_FLAG==1
    if(!memcmp(in_pw, "\x8\x2\x6\x1\x0\x1", 6))
       return 5;       
#endif 

    if(!memcmp(in_pw, tmp, pin_len)){
        if(!type){ 
           memset(tmp,  0x5A,_USER_PIN_LEN);
           draw_screen(82);    //ÐÂÃÜÂë
           pin_len = modify_data(1,tmp,_USER_PIN_LEN);     //first
           if(pin_len == 0xff)
              return _FALSE;    //return 
           draw_screen(83);     //retry 
           if(modify_data(1,in_pw,pin_len)> _USER_PIN_LEN) //second
	      return _FALSE;    //·µ»Ø             

           if(memcmp(tmp,in_pw, pin_len)) goto fail;    
           memcpy(__sys_passwd.Password,in_pw,pin_len);
	   __sys_passwd.Pass_Num = pin_len;
           eeprom_in_queue(SAVE_SYS_PASSWD);
           op_status(1);
        }        
        return _TRUE;
    } else {                
fail:   if(type < 2){    //´¦Àí·ÀÐ²³Ö
           pin_len--;
           type = tmp[pin_len]+1;    //×îºóÒ»Î»¼Ó1
           if(type > 9) type = 0;
           if(!memcmp(in_pw, tmp, pin_len) && type==in_pw[pin_len]){  //ÅÐ¶ÏÇ°NÎ»£¬ºÍN+1Î»
              buf[APP_HEAD_LEN] = 0xA5;   //¿ÚÁî³ö´í£¬·ÀÐ²³Ö±¨¾¯
	      buf[APP_HEAD_LEN+1] = 0x00;
              buf[APP_HEAD_LEN+2] = _INTIMIDATE_ALM_PORT;            
              retrans_in(buf,APP_HEAD_LEN+3);
              alm_log(0,_INTIMIDATE_ALM_PORT);
              return _TRUE;
           } 
        } 
        op_status(0);                        /* ÃÜÂë´íÎó£¬ÖØÐÂÊäÈë¡£*/
        return _FALSE;
    }
}

static defend_fail(unsigned char port)  //²¼·ÀÊ§°Ü
{
    unsigned char Menu_Length;
    unsigned char xdata *xp;    

    crt_clear();
    crt_outhz(2,SPACE_COL,"¶Ë¿ÚÃû:",7);    
    crt_outhz(5,SPACE_COL, "ÅÔÂ·",4);     //ÅÔÂ·     
    crt_outhz(5,SPACE_COL+64, "È¡Ïû",4);  //È¡Ïû
    crt_outdigital(2,SPACE_COL+_WORD_WIDE*4,port+1, 2,0);
    xp = get_config_menu(__port[port].Device_Type, &Menu_Length); 
    if(xp){    
       crt_outhz(2, SPACE_COL+_WORD_WIDE*6, xp, Menu_Length);
    }
}

void defend_save_and_light(bit save_flag, unsigned char defend_class,unsigned char mode)
{
    unsigned char xdata buf[25];
    bit no_defend_flag=0;

    __security_class = defend_class;
    if(defend_class & 0x0F){
       DEFEND_LIGHT_ON;
       buzzer_start(50,1);
    }
    else{
       no_defend_flag = 1;
       DEFEND_LIGHT_OFF;
       buzzer_start(20,3);
    }
    if(save_flag){
       eeprom_in_queue(SAVE_DEFEND);
       defend_log(defend_class, mode);
#if NO_DEFEND_OPEN_LIGHT==1                //³··À¿ªµÆ
       if(no_defend_flag){
          if(other_item.no_defend_open_light){
             buf[APP_HEAD_LEN]   = 0x16;
             buf[APP_HEAD_LEN+1] = 0x02;  
             buf[APP_HEAD_LEN+2] = 0x05;  
             buf[APP_HEAD_LEN+3] = 14;     //³··À¹Ì¶¨¿ª14ºÅµÆ  
             buf[APP_HEAD_LEN+4] = 0x01; 
             make_send_package(0xFF,0,QUERY_TYPE,buf,APP_HEAD_LEN+5); //·¢ËÍÃüÁî
          }
       }  
#endif
    } 
    __defend_fail_beep = 0;
}

void do_defend(void)
{
    U_INT_8 tmp_defend,port,old_port;
    unsigned char key; 
    unsigned char idata key1,old_defend; 
    unsigned char idata delay_times;
//    unsigned char code xy[]={0x51,0x53,0x55,0x57,
//			     0x91,0x93,0x95}; 
    unsigned char code xy[]={0x37,10,      0x2D,15, 0x64,15 ,0x93,15 ,0xC0,15,
                             0x60,10|0x40, 0x2D,15, 0x64,15 ,0x93,15}; 

    unsigned char code xy_pass[]={0x63,10, 0x23,15, 0x56,15};

    draw_screen(31);
    tmp_defend=__security_class & 0x0F;
//    for(i=0;i<6 	
    switch(tmp_defend){	
       case 0x0F:
           port=5;         //page
	   old_port=19-_NUMBER_WIDE;  //col
           break;
       case 0x00:  
           port=5;         //page
	   old_port=19-_NUMBER_WIDE+63;  //col
           break;
       case 0x08:  	    
           tmp_defend -= 2;
       case 0x01:
       case 0x02:  
       case 0x04:  
           tmp_defend >>= 1;
           port=2;         //page
	   old_port=19-_NUMBER_WIDE+tmp_defend*63;  //col
           break;
       default:	
           goto NEXT;
     }
     crt_outchar(port,old_port,'*');   	

NEXT:
    do{ 	
        key=get_xy(2,4,xy);             
        switch(key){
           case 0:      
           case 1:      
           case 2:      
           case 3:      
               tmp_defend = 0x80 | (1<<key);  //ÖÃÎ»²¼·ÀÀàÐÍ
               break;
           case 4:      
               tmp_defend = 0x8F;             //È«²¿²¼·À    
               break;
           case 5:   
               tmp_defend = 0x80;             //È«³·
               break;
           default:                                 
	       return;	
        }              
        if(__security_class == 0x8F){        //½µµÍ·ÀÇøµÈ¼¶Ê±ÐèÒªÊäÈëÃÜÂë
           if(key != 4 && !pw_verify(1)) 
              return;
        }
        else{        
           if(__security_class & 0x0F){      //ÓÐ²¼·À×´Ì¬£¬×ªÈë³··ÀÊ±ÐèÒªÃÜÂë 
              if(key==5 && !pw_verify(1)) 
                 return;
           }                                 
        }        
        old_defend = __security_class;  
        __security_class = 0x80;                        //ÔÝÊ±È«²¿Ê§·À
        //DEFEND_LIGHT =  LED_OFF;
        DEFEND_LIGHT_OFF;
        memset(__bypass,0x00,_INPUT_PORT_NUM);
        for(port=0; port<_INPUT_PORT_NUM; port++){ 
            __port[port].type.In_Port.Op_Code &= 0xEF;     //MASK D4=0; 
        }
        if(key<5){                  //°´²»Í¬²¼·ÀÀàÐÍ¼ì²â¶Ë¿Ú
            delay_times = 0;
            old_port=0xFF; 
            while(1){    //µÈ´ý¼üÅÌ 
                if(port_check(tmp_defend,&port)){       //ÓÐ¶Ë¿Ú´¦ÓÚ²»Õý³£×´Ì¬£¬²»ÄÜ²¼·À          
                   __defend_fail_beep = 1;
		   if(old_port!=port){	    //¶Ë¿Ú²»Í¬£¬Ë¢ÐÂÏÔÊ¾
                      defend_fail(port);
    		      old_port = port;
                   }
                   key=get_xy(1,2,xy_pass);             
                   switch(key){    
                       case 0:     //ÅÔÂ· 
                         __bypass[port] = 0x1;      //ÅÔÂ·                                                                                                  
                         __port[port].type.In_Port.Op_Code |= 0x10;
                         break;
                       case 1:     //·µ»Ø
                       case ALM_RETURN_KEY:  //ÓÐ±¨¾¯£¬ÍË³ö
                         goto Exit;       
                   }    
                   if(delay_times++ == 0xFF)    //Ã»ÓÐ´¦Àí£¬×Ô¶¯ÍË³ö
    		      goto  Exit;
                }
                else{  //ok
                    __defend_fail_beep = 0;
                    delay_times = get_delay_time(); //µÃµ½µ±Ç°ËùÓÐ´«¸ÐÆ÷µÄ×î´óÑÓ³ÙÊ±¼ä
		    if(delay_times>0)
                       draw_screen(115);      // ÕýÔÚÑÓÊ±                    
                    while(1){
                       if(key_out(&key1)){    //ÈÎÒâ¼üÍË³ö
                          if(key1 != AUTO_RETURN_KEY){        //²»ÄÜ×Ô¶¯ÍË³ö     
Exit:			     defend_save_and_light(0,old_defend,DEFEND_MODE_KEY);
                             op_status(0);
                             return;
                          }      
                       }  
                       if(delay_times)                                              
			  delay_times--;
		       else
			  goto ok;			
                       delay(SECOND);       //±ØÐëÊÇ1s
		       crt_outnull_num(2,SPACE_COL+_WORD_WIDE*6,3);
                       crt_outdigital(2,SPACE_COL+_WORD_WIDE*6,delay_times,3,0);                    
                   }
                }
            }
        }                       
ok:     //buzzer_start(20,3);
        send_defend_to_ccc(tmp_defend);
        defend_save_and_light(1,tmp_defend,DEFEND_MODE_KEY);
        op_status(1);
        break;
    }while(key < 6);
}
#if 0
void do_get_data(unsigned char type)    //0=´ÓCCCÈ¡µ±Ç°NDTµÄ±íÊý¾Ý, 1=ÏÔÊ¾¹«¸æ
{
     extern unsigned char xdata * xdata msg_xp;
//     unsigned char xdata buf[40];

//     draw_screen(13);
     resend_buf.dat[APP_HEAD_LEN]=0xa7;
     resend_buf.dat[APP_HEAD_LEN+1]=0x03;
     resend_buf.dat[APP_HEAD_LEN+2]=type;
     resend_buf.dat[APP_HEAD_LEN+3]=0;
     resend_buf.dat[APP_HEAD_LEN+4]=0;
     resend_buf.len =APP_HEAD_LEN+5; 
     resend_buf.riu_addr = 0;
//     make_send_package(0,0,QUERY_TYPE|SEND_THRICE,buf,5);                
     if(wait_reply()){        //Êý¾Ý·µ»
        if(__msg.word_num){   //×Ö·ûÊý²»ÎªÁã         
           __msg.start=0;  
           __msg.end = __msg.word_num; 
           msg_xp = __msg.msg_data;
           __start_show = 1;
           while(1){
              key_get();
              if(__start_show){    //ÒÑ¾­¿ªÊ¼ÏÔÊ¾ÏûÏ¢,ÏÔÊ¾µÚ¶þÒ³
                 tm[MSG_TIMER]=0;
                 continue;
              }
              break;          
           }   
        }               
     }
     else{
        draw_screen(120);         //Í¨Ñ¶Ê§°Ü
//        while(key_get()<7);
        key_get();
     }   
     __msg.word_num = 0;
}
#endif

bit wait_iic_reply(void)
{
    unsigned char i=0,j=1;

    draw_screen(13);      //ÍøÂçÁ¬½Ó
    __return_ok = _FALSE;
    do{
       if(i%10==0){
          crt_outchar(2,j*16+24*4,'>');
          j++;
       }  
       delay(15);       //100ms
       if(__return_ok)
          return 1;
    }while(i++ <= 20);
    return 0;
}

unsigned char xdata img_status,show_img_end;
void read_img_msg(void)
{
    unsigned char xdata buf[10];
    unsigned char key;
    unsigned char code xy[]={0x30,10,      0xDA,15,
                             0x50,10|0x10, 0xDA,15,
                             0x70,10|0x20, 0xDA,15};

     iic_send_in(MSG_READ_IMG,buf);
     if(wait_iic_reply()==0) return;
     show_img_end = 1;
     switch(img_status){
          case 0x01:
               draw_screen(130);  //ÏµÍ³Ã¦                 
               key_get();     
               break;
          case 0x00:              //ÕÒµ½
               new_img_msg = 0;
               while(1){
                   SCREEN_SWITCH_VIDEO; //ÏÔÊ¾ÊÓÆµÉÏµÄ½çÃæ£¬"ÉÏÒ³","ÏÂÒ³"£¬"·µ»Ø"
                   key=get_xy(3,1,xy);
                   switch(key){
                     case 0:      //up
                     case 1:      //down
                          iic_send_in(key+MSG_READ_IMG_UP,buf); //0x21,0x22                        
                          continue;
                     default:     //exit
                          iic_send_in_msgno(MSG_READ_IMG_END);  //·¢Ò»¸ö½áÊøµ½Í¼Ïñ´¦ÀíÆ÷
    	                  SCREEN_SWITCH_CHAR;
                          return;
                   }                       
              }
              break;
          case 0x02:             //Ã»ÓÐÏûÏ¢
              new_img_msg = 0;
              draw_screen(131);  
              key_get();     
              break;
       }         
}

void do_query_msg(void) 
{
    U_INT_8 key,tmp;
    extern unsigned char xdata  msg_index[];
    extern unsigned char xdata * xdata msg_xp;

//    unsigned char code xy[]={0x51,0x53,0x55,0x57,
//			     0x91,0x93,0x95,0x97}; 
    unsigned char code xy[]={0x3A,10,      0x2A,15, 0x5F,15 ,0x95,15 ,0xCA,15,
                             0x6B,10|0x40, 0x2A,15, 0x5F,15 ,0x95,15 ,0xCA,15,
                             0x9C,10|0x80, 0x2A,15};



    __menu_layer=SUB_MENU_2;
    resend_buf.dat[APP_HEAD_LEN] = 0xA7;
    resend_buf.dat[APP_HEAD_LEN+1] = 0x03;
    resend_buf.riu_addr = 0;

    draw_screen(43);
    do{	
        if(__start_show){    //ÒÑ¾­¿ªÊ¼ÏÔÊ¾ÏûÏ¢,ÏÔÊ¾µÚ¶þÒ³
	   key_get();        //ÈÎÒâ¼ü·­Ò³
           tm[MSG_TIMER]=0;
           continue;
        }                 
        key=get_xy(3,4,xy);             
        if(key<7){
            tmp = msg_index[key] & 0x07;
            resend_buf.dat[APP_HEAD_LEN+2]=__msg.type[tmp];
            (unsigned int )resend_buf.dat[APP_HEAD_LEN+3]=__msg.msg_no[tmp];
            resend_buf.len = APP_HEAD_LEN+5;
            if(wait_reply() && __msg.word_num){   //Êý¾Ý·µ»Ø£¬×Ö·ûÊý²»ÎªÁã         
               message_reindex(key);
               __msg.start=0;  
               __msg.end = __msg.word_num; 
               msg_xp = __msg.msg_data;
               __start_show=1;
            }
            else{    
               draw_screen(43);  //Á¬½ÓÊ§°Ü·µ»Ø
            }    
            __msg.word_num = 0;
        }  
        if(key==7){   //¶ÁÍ¼ÏñÏûÏ¢
           read_img_msg();
           show_img_end=0;
           draw_screen(43);              //·µ»Ø
        }
    }while(key<8);
}
#if 0
void do_query_net(void)
{
    resend_buf.dat[APP_HEAD_LEN]=0xA6;
    resend_buf.dat[APP_HEAD_LEN+1]=0x00;
    resend_buf.len = APP_HEAD_LEN+2; 
    resend_buf.riu_addr = 0;
    network_status(wait_reply());
}
#endif

void do_query_alm(void)     //±¨¾¯²éÑ¯
{
    U_INT_8 port,Menu_Length;
    U_INT_8 xdata *xp;  
    unsigned char key;       
    char tmp_ptr,first_rec;
    unsigned char code xy[]={0x38,10, 0x3D,20, 0x8E,20};


    if(__alm_log.curr_ptr > _ALM_LOG_MAX)     //·Ç·¨Ö¸Õë
       __alm_log.curr_ptr = 0;

    if(__alm_log.curr_ptr == 0 && __alm_log.dat[0].used != 0xAA)    //ÎÞ±¨¾¯¼ÇÂ¼
       return;    

    if(__alm_log.curr_ptr==0)
       tmp_ptr = _ALM_LOG_MAX-1;
    else
       tmp_ptr = __alm_log.curr_ptr - 1;
    first_rec = tmp_ptr;
    do{ 
        crt_clear();
        crt_outhz(2,50,"ÏÂÒ³",4);  
        crt_outhz(2,150,"·µ»Ø",4);  
#define NEXT_PAGE  5
//        next=3;
//SHOW_NEXT_PAGE:
        crt_outhz(0+NEXT_PAGE,SPACE_COL,"ÐòºÅ:",5);  
        crt_outdigital(0+NEXT_PAGE,SPACE_COL+3*_WORD_WIDE,tmp_ptr,2,0); 
        crt_outhz(0+NEXT_PAGE,SPACE_COL+6*_WORD_WIDE,"ÔÂ  ÈÕ",6);   //ÔÂxxÈÕÖ       
        crt_outdigital(0+NEXT_PAGE,SPACE_COL+5*_WORD_WIDE,__alm_log.dat[tmp_ptr].alm_month, 2,0);  //month
        crt_outdigital(0+NEXT_PAGE,SPACE_COL+7*_WORD_WIDE,__alm_log.dat[tmp_ptr].alm_day, 2,0);    //Date
        crt_outhz(2+NEXT_PAGE,SPACE_COL+4*_WORD_WIDE,"Ê±  ·Ö  Ãë",10);   //ÔÂxxÈÕxxÊ±xx·Ö       
        crt_outdigital(2+NEXT_PAGE,SPACE_COL+3*_WORD_WIDE,__alm_log.dat[tmp_ptr].alm_hour, 2,0);  //Hour
        crt_outdigital(2+NEXT_PAGE,SPACE_COL+5*_WORD_WIDE,__alm_log.dat[tmp_ptr].alm_minite, 2,0); //Minite
        crt_outdigital(2+NEXT_PAGE,SPACE_COL+7*_WORD_WIDE,__alm_log.dat[tmp_ptr].alm_sec, 2,0);    //Second

        crt_outhz(6+NEXT_PAGE,SPACE_COL,"µØÖ·:",5);    
        crt_outdigital(6+NEXT_PAGE,SPACE_COL+_WORD_WIDE*3,__alm_log.dat[tmp_ptr].alm_riu,2,0);     //show RIU_ADDR 

        crt_outhz(4+NEXT_PAGE,SPACE_COL,"ÀàÐÍ:",5);    
        port=__alm_log.dat[tmp_ptr].alm_port;        //´Ó1¿ªÊ¼ÏÔÊ¾
        crt_outdigital(6+NEXT_PAGE,SPACE_COL+5*_WORD_WIDE,port+1,2,0);   //ÏÔÊ¾±¨¾¯¶Ë¿Ú
        if(port<_INPUT_PORT_NUM ){  
           xp = get_config_menu(__alm_log.dat[tmp_ptr].alm_device, &Menu_Length);
           if(xp) 
              crt_outhz(4+NEXT_PAGE,SPACE_COL+3*_WORD_WIDE,xp, Menu_Length);
           else
              crt_outhz(4+NEXT_PAGE,SPACE_COL+3*_WORD_WIDE,"¶Ë¿Ú",4);     
        }      
        else {
            switch(port){
            case _BOARDCASE_ALM_PORT:     //¹ã²¥±¨¾¯¶Ë¿Ú
                crt_outhz(4+NEXT_PAGE,SPACE_COL+3*_WORD_WIDE,"¹ÜÀíÖÐÐÄ",8);    
                break;
            case _KEYBOARD_ALM_PORT:      //¼üÅÌ±¨¾¯
                crt_outhz(4+NEXT_PAGE,SPACE_COL+3*_WORD_WIDE,"¼üÅÌ",4);    
                break;
            case _INTIMIDATE_ALM_PORT:    //Ð²³Ö±¨¾¯
                crt_outhz(4+NEXT_PAGE,SPACE_COL+3*_WORD_WIDE,"Ð²³ÖÃÜÂë",8);    
                break;
            case _BACKOUT_ALM_PORT:       //²ð»ú¿Ç
                crt_outhz(4+NEXT_PAGE,SPACE_COL+3*_WORD_WIDE,"»úÏä·À²ð",8);    
                break;
            case _REMOTE_CTROL_ALM:       //Ò£¿Ø
                crt_outhz(4+NEXT_PAGE,SPACE_COL+3*_WORD_WIDE,"Ò£¿Ø",4);    
                break;
            case _LOW_POWER_12V:
                crt_outhz(4+NEXT_PAGE,SPACE_COL+3*_WORD_WIDE,"µçÔ´µÍÑ¹",8);    
                break;
            }
        }                  
        if(tmp_ptr <= 0){   //ÏÔÊ¾ÁÙÊ±Ö¸Õë
           for(tmp_ptr=_ALM_LOG_MAX-1;tmp_ptr>=0;tmp_ptr--){    //²éÕÒÏÂÒ»¸ö±¨¾¯¼ÇÂ¼
               if(__alm_log.dat[tmp_ptr].used == 0xAA)              	   
                  break;              
           }                    
           if(tmp_ptr<0)     
              tmp_ptr = 0;
        } 
        else{            
           tmp_ptr--;
        }
//        if(next==3 && tmp_ptr!=first_rec){              	   
//           next = 15; 
//           goto SHOW_NEXT_PAGE;
//        } 
        key=get_xy(1,2,xy);
        if(key==0x01) 
           break;               
//    }while(key<ENTER_KEY);                                       
      }while(key<AUTO_RETURN_KEY);
}               

void do_query_defend(void)     //²¼·À²éÑ¯
{
    U_INT_8 type,mode;
    char tmp_ptr,first_rec;
    unsigned char key;       
    unsigned char code xy[]={0x38,10, 0x3D,20, 0x8E,20};

    if(__defend_log.curr_ptr > DEFEND_LOG_MAX)     //·Ç·¨Ö¸Õë
       __defend_log.curr_ptr = 0;

    if(__defend_log.curr_ptr == 0 && __defend_log.dat[0].used != 0xAA)    //ÎÞ²¼·À¼ÇÂ¼
       return;    

    if(__defend_log.curr_ptr==0)
       tmp_ptr = DEFEND_LOG_MAX-1;
    else
       tmp_ptr = __defend_log.curr_ptr - 1;
    first_rec = tmp_ptr;
    do{ 
        crt_clear();
//        next=2;
        crt_outhz(2,50,"ÏÂÒ³",4);  
        crt_outhz(2,150,"·µ»Ø",4);  

//        write_return();      //Ð´¡¯·µ»Ø¡®
//SHOW_NEXT_PAGE:
        crt_outhz(0+NEXT_PAGE,SPACE_COL,"ÐòºÅ:",5);  
        crt_outdigital(0+NEXT_PAGE,SPACE_COL+3*_WORD_WIDE,tmp_ptr,2,0); 
        crt_outhz(0+NEXT_PAGE,SPACE_COL+6*_WORD_WIDE,"ÔÂ  ÈÕ",6);   //ÔÂxxÈÕÖ       
        crt_outdigital(0+NEXT_PAGE,SPACE_COL+5*_WORD_WIDE,__defend_log.dat[tmp_ptr].defend_month, 2,0);  //month
        crt_outdigital(0+NEXT_PAGE,SPACE_COL+7*_WORD_WIDE,__defend_log.dat[tmp_ptr].defend_day, 2,0);    //Date
        crt_outhz(2+NEXT_PAGE,SPACE_COL+4*_WORD_WIDE,"Ê±  ·Ö  Ãë",10);   //ÔÂxxÈÕxxÊ±xx·Ö       
        crt_outdigital(2+NEXT_PAGE,SPACE_COL+3*_WORD_WIDE,__defend_log.dat[tmp_ptr].defend_hour, 2,0);  //Hour
        crt_outdigital(2+NEXT_PAGE,SPACE_COL+5*_WORD_WIDE,__defend_log.dat[tmp_ptr].defend_minite, 2,0); //Minite
        crt_outdigital(2+NEXT_PAGE,SPACE_COL+7*_WORD_WIDE,__defend_log.dat[tmp_ptr].defend_sec, 2,0);    //Second

        crt_outhz(4+NEXT_PAGE,SPACE_COL,"·½Ê½:",5);    
        mode=__defend_log.dat[tmp_ptr].defend_mode;        
        switch(mode){
            case DEFEND_MODE_KEY:     //¼üÅÌ
                crt_outhz(4+NEXT_PAGE,SPACE_COL+_WORD_WIDE*3,"¼üÅÌ",4);    
                break;
            case DEFEND_MODE_REMOTE_CTRL: //Ò£¿Ø
                crt_outhz(4+NEXT_PAGE,SPACE_COL+_WORD_WIDE*3,"Ò£¿Ø",4);  
		break; 
            case DEFEND_MODE_CENTER: //ÖÐÐÄ
                crt_outhz(4+NEXT_PAGE,SPACE_COL+_WORD_WIDE*3,"ÖÐÐÄ",4);   
                break;
            case DEFEND_MODE_OTHER:  //ÆäËü
                crt_outhz(4+NEXT_PAGE,SPACE_COL+_WORD_WIDE*3,"ÆäËü",4);   
                break;
        }
        crt_outhz(6+NEXT_PAGE,SPACE_COL,"ÀàÐÍ:",5);    
        type=__defend_log.dat[tmp_ptr].defend_type & 0x0F;        
        switch(type){
             case 0x00:
                crt_outhz(6+NEXT_PAGE,SPACE_COL+_WORD_WIDE*3,"È«³·",4);    
                break;  
             case 0x01:
                crt_outhz(6+NEXT_PAGE,SPACE_COL+_WORD_WIDE*3,"ÔÚ¼Ò",4);    
                break;  
             case 0x02:
                crt_outhz(6+NEXT_PAGE,SPACE_COL+_WORD_WIDE*3,"ÐÝÏ¢",4);    
                break;  
             case 0x04:
                crt_outhz(6+NEXT_PAGE,SPACE_COL+_WORD_WIDE*3,"³öÃÅ",4);    
                break;  
             case 0x08:
                crt_outhz(6+NEXT_PAGE,SPACE_COL+_WORD_WIDE*3,"ÆäËû",4);    
                break;  
             case 0x0F:
                crt_outhz(6+NEXT_PAGE,SPACE_COL+_WORD_WIDE*3,"È«²¼",4);    
                break;  
        }
        if(tmp_ptr <= 0){   //ÏÔÊ¾ÁÙÊ±Ö¸Õë
           for(tmp_ptr=DEFEND_LOG_MAX-1;tmp_ptr>=0;tmp_ptr--){    //²éÕÒÏÂÒ»¸ö±¨¾¯¼ÇÂ¼
               if(__defend_log.dat[tmp_ptr].used == 0xAA)              	   
                  break;              
           }                    
           if(tmp_ptr<0)     
              tmp_ptr = 0;
        } 
        else{            
           tmp_ptr--;
        }
//        if(next==3 && tmp_ptr!=first_rec){
//           next = 15; 
//           goto SHOW_NEXT_PAGE;
//        } 
        key=get_xy(1,2,xy);
        if(key==0x01) 
           break;               

//    }while(key<ENTER_KEY);                                       
      }while(key<AUTO_RETURN_KEY);
}               

void do_query_time(void)    //²éÑ¯Ê±¼ä
{
    extern unsigned char code week_day[];

    draw_screen(46);  
    crt_outdigital(2,SPACE_COL,__ymd.Year, 4,0);                 //year
    crt_outdigital(2,SPACE_COL+40+21,__ymd.Month, 2,0);   //month
    crt_outdigital(2,SPACE_COL+40+21+20+21,__ymd.Date, 2,0);    //Date
    crt_outdigital(4,SPACE_COL,__ymd.Hour, 2,0);                 //Hour
    crt_outdigital(4,SPACE_COL+20+21,__ymd.Minite, 2,0);    //Minite
    crt_outhz(4,SPACE_COL+4*_WORD_WIDE,"ÐÇÆÚ",4);
    crt_outhz(4,SPACE_COL+6*_WORD_WIDE,week_day+(__ymd.Week-1)*2,2);   
    key_get();
}

void show_mac_addr(void)
{
    unsigned char i;
    unsigned char col = SPACE_COL+_WORD_WIDE*3;

    crt_outhz(12,SPACE_COL,"MAC:",4);      //Ð´'MAC'
    for(i=0; i<6; i++){
        crt_out_hex(12,col,r8019_eth_addr[i]);                  
        col += 2*_NUMBER_WIDE; 
    }
}               

void do_query_version(void)
{
    crt_clear();
//    if(flag)
//       crt_outhz(2,SPACE_COL,"¹ã¶«ÕæÉÆÃÀ¼¯ÍÅÓÐÏÞ¹«Ë¾",24);   //°æ±¾
//    else
    crt_outhz(2,SPACE_COL,"ÉÏº£»Û¾ÓÖÇÄÜµç×ÓÓÐÏÞ¹«Ë¾",24); //°æ±¾
    crt_outhz(4,SPACE_COL,"Ó²¼þ:",5);  //°æ±¾
    crt_outhz(6,SPACE_COL,"Èí¼þ:",5);  //°æ±¾
    crt_outdigital(4,SPACE_COL+_WORD_WIDE*3,SCU_SHOW_HARDWARE_VERSION, 3,2);  //Ó²¼þ°æ±¾
    crt_outdigital(6,SPACE_COL+_WORD_WIDE*3,SCU_SHOW_SOFTWARE_VERSION, 3,2); //Èí¼þ°æ±¾
    crt_outhz(8,SPACE_COL,"ÈÕÆÚ:",5); 
    crt_outhz(8,SPACE_COL+_WORD_WIDE*3,__DATE__, 11);   //·¢²¼ÈÕÆÚ
    crt_outhz(10,SPACE_COL+_WORD_WIDE*3,__TIME__, 8);   //·¢²¼ÈÕÆÚ
    show_mac_addr();
    key_get();
}

void show_ip_addr(unsigned char page,unsigned char col,unsigned char xdata *xp)
{
    unsigned char i;
    for(i=0;i<4;i++){
        crt_outdigital(page,col,*(xp++),3,0);          
	if(i<3){
           crt_outchar(page, col+28, '.'); 
           col += 36;
        } 
    }
}         

unsigned char atoc(unsigned char xdata *xp)
{
    unsigned char tmp;

    tmp = (*xp++ -0x30)*100;
    tmp += (*xp++ -0x30)*10;
    tmp += (*xp -0x30);
    return tmp;
}
//asc to hex    
unsigned char ascTohex(unsigned char xdata *xp)
{
    unsigned char tmp;

    tmp = (*xp++ -0x30)*10;
    tmp += (*xp -0x30);
    return tmp;
}

    
//²éÑ¯IPºÍMACµØÖ·
void do_query_ip_mac(void) 
{
//    unsigned char code  xy[]={0xD1,0xD3,0xD5,0xD7}; 
    unsigned char code  xy[]={0x2F,10,      0x30,30,
                              0x42,10|0x10, 0x30,30,
                              0x5B,10|0x20, 0x30,30,
                              0x79,10|0x30, 0x30,30,
                              0x89,10|0x40, 0x30,30,
                              0xB3,10|0x50, 0x30,30};

    unsigned char xdata buf[40];
    unsigned char i,key;
    unsigned char xdata *xp;

    do{
       //draw_screen(125);
       crt_clear();
       crt_outhz(1,SPACE_COL,"[IP]",4);      //Ð´'IP'
       show_ip_addr(1,SPACE_COL*7,(unsigned char *)&__netif.ni_ip);

       crt_outhz(3,SPACE_COL,"ÑÚÂë",4);    //Ð´'MASK'
       show_ip_addr(3,SPACE_COL*7,(unsigned char *)&__netif.ni_mask);

       crt_outhz(5,SPACE_COL,"Íø¹Ø",4);      //Ð´'GATE'
       show_ip_addr(5,SPACE_COL*7,(unsigned char *)&__netif.ni_gateway);

//       crt_outhz(7,SPACE_COL,"Ó²¼þµØÖ·",8);    //Ð´'MAC'
//       show_mac_addr();       //ÔÚ¹ØÓÚÀïÃæÏÔÊ¾

       crt_outhz(7,SPACE_COL,"·þÎñ",4);    //Ð´'GATE'
       show_ip_addr(7,SPACE_COL*7,(unsigned char *)&__netif.sys_ip0);
//       show_ip_addr(8,SPACE_COL*7,(unsigned char *)&__netif.sys_ip1);

       crt_outhz(9,SPACE_COL,"±¨¾¯",4);    //Ð´'GATE'
       show_ip_addr(9,SPACE_COL*7,(unsigned char *)&__netif.sys_ip2);
//       show_ip_addr(10,SPACE_COL*7,(unsigned char *)&__netif.sys_ip3);

       crt_outhz(12,SPACE_COL,"·µ»Ø",4);    //Ð´'GATE'

       key=get_xy(6,1,xy);
       if(key<6){
          if(key==5)
             return;
          draw_screen(35); 
          memset(buf,'0',12);     
          if(modify_data(0,buf,12)==12){  
             for(i=0;i<4;i++)
                buf[i]=atoc(buf+i*3);
             switch(key){
                case 0:
                  xp = (unsigned char *)&__netif.ni_ip;
                  break;  
                case 1:
		  xp =	(unsigned char *)&__netif.ni_mask;
                  break;  
                case 2:
                  xp = (unsigned char *)&__netif.ni_gateway;
                  break;  
                case 3:
                  xp = (unsigned char *)&__netif.sys_ip0;
                  break;  
                case 4:
                  xp = (unsigned char *)&__netif.sys_ip2;
                  break;  
             }
             memcpy(xp,buf,4);	    
	     if(key<3){	 //ÖØËãÍøÂçµØÖ·
                __netif.ni_net = __netif.ni_ip & __netif.ni_mask;
             }   
             __netif.use_flag = IP_USED_FLAG;
             eeprom_in_queue(SAVE_SYS_IP);     //±£´æIPµØÖ· 
         }
      }
   }while(key<5); 
}


void do_query(void)
{
    U_INT_8 key;

    unsigned char code xy[]={0x37,5, 0x2D,15, 0x64,15 ,0x99,15 ,0xD0,15};
    __menu_layer = SUB_MENU_1;
    do{
        draw_screen(40);
        key=get_xy(2,4,xy);
        switch(key){
        case 0:
            do_query_alm();
            break;
        case 1:
            do_query_defend();                
            break;
        case 2:
            do_query_time();
            break;
        }
    }while(key < 3); 
}

#if 0
/**************************************************************************/
/*              do_modify_meter_data ÐÞ¸ÄNDTËù½Ó±íµÄÊý¾Ý                  */
/* ²ÎÊý£º ÎÞ                                                              */ 
/* ·µ»Ø£º ÎÞ                                                              */     
/* ÈÕÆÚ£º2000/12/26                                                       */      
/*------------------------------------------------------------------------*/
/* ÐÞ¸Ä¼ÇÂ¼:		                                                  */
/*    							                  */   
/**************************************************************************/
void do_modify_meter_data(void)
{
//    unsigned char code xy[]={0x91,0x93,0x95}; 
    unsigned char code xy[]={0xB0,10, 0x2A,15, 0x68,15 ,0x9C,15};

    unsigned char key;
    unsigned char i,j;
    U_INT_32 idata Pulse_Num,Curr_Data;
    U_INT_16 idata Unitage;
    U_INT_8  data float_part;
    unsigned char xdata *xp;
    char xdata buf[10];
 
    i=j=0;       
    do{    
Next:      if(i>= _INPUT_PORT_NUM)    
              i=0;  
           if(__port[i].Usable != 0x01)
              if(++i < _INPUT_PORT_NUM){
                 if(j++ >= _INPUT_PORT_NUM) return;   //Ã»ÓÐÕÒµ½±íÀàÐÍ
                 continue;
              }               
           key = __port[i].Device_Type;       
           if(key > 0x1f || key==0){   //±íÀàÐÍÊäÈë  
              i++;
              continue;
           }   
           crt_clear();  
           j=0; 
           xp=get_config_menu(key, &key);
           if(xp){           
              crt_outhz(2,SPACE_COL, xp, key);             
           }             
	   Unitage = __port[i].type.Meter.Unitage;
	   Pulse_Num = __port[i].type.Meter.Curr_Data;
	   Curr_Data = (Pulse_Num/Unitage)*100;             //°ÑÂö³åÊý×ª»»Îª¸¡µãÊý,Ö»¹²ÏÔÊ¾,ÕûÊý²¿·Ö
	   Curr_Data += (Pulse_Num%Unitage)*100/Unitage;    //Ð¡Êý²¿·Ö
           crt_outdigital(2,SPACE_COL+84,Curr_Data,8,2);	
           crt_outdigital(2,SPACE_COL+64*3,i+1,2,0);        //ÏÔÊ¾¶Ë¿ÚºÅÅ
           crt_outhz(10, SPACE_COL, "ÏÂÒÆ", 4);             //È·¶¨
	   crt_outhz(10, SPACE_COL+64, "ÐÞ¸Ä", 4);
	   crt_outhz(10, SPACE_COL+64+64, "·µ»Ø", 4);
           do{
               key=get_xy(1,3,xy);             
 	       switch(key){
		  case 0:             //next	
		       i++;
		       goto Next;		       
		  case 1:	      //modify
		       goto MODIFY;
		  default:	      //return  	         
                       return;     
              } 
	   }while(key<1);
     }while(1);		
MODIFY:
     draw_screen(35); 
     memset(buf,'0',10);
     if(modify_data(0,buf+2,8)==8){    //ÐÞ¸Ä±íÊý¾Ý
        Curr_Data= atol(buf);
        float_part = Curr_Data % 100;                         //È¡Ð¡Êý²¿·Ö£¬Îª³õÖµ
        Pulse_Num   = (Curr_Data - float_part)/100;            //ÕûÊý²¿·Ö
        __port[i].type.Meter.Curr_Data = Pulse_Num*Unitage + (U_INT_32)float_part*Unitage/100;
        eeprom_in_queue(SAVE_PORT);              //±£´æ¶Ë¿ÚÊý¾Ý
     } 
     goto Next;
}
#endif 

void  do_help(unsigned char type) //ÇóÖú     
{
    U_INT_8 xdata buf[40];

     __alm.port = type;          //¶Ë¿ÚºÅ
     __alm.riu_addr = 0;
     buzzer_on(_BIT_ALM);
     //ALM_LIGHT = LED_ON;  
     ALM_LIGHT_ON;
     alm(1);
     buf[APP_HEAD_LEN]=0xA5;
     buf[APP_HEAD_LEN+1]=0x00;
     buf[APP_HEAD_LEN+2]=type;   //KEYBOARD±¨¾¯
     buf[APP_HEAD_LEN+3]=0x02;   //±¨¾¯
     get_timestamp((TIMESTAMP xdata *)(buf+(APP_HEAD_LEN+4)));                    
     retrans_in(buf,APP_HEAD_LEN+4+sizeof(TIMESTAMP));
}       


void timer_set(unsigned char port_no)
{
    extern  unsigned char code return_menu[];
    extern  unsigned char code week_menu[];
//    unsigned char code xy[]={0x41,0x43,0x45,0x47};
//    unsigned char code xy_timer[]={0x41,0x42,0x45,0x46,
//			           0x71,0x73,0x75,0x77,
//			           0xa1,0xa3,0xa5,0xa7,
//			           0xD1};
    unsigned char code xy[]={0x3A,10, 0x2B,15, 0x62,15 ,0x98,15 ,0xD0,15};

    unsigned char code xy_timer[]={0x39,10,      0xCC,15, 0xCC,15 ,0xCC,15, 0xCC,15,
                                   0x54,10|0x10, 0xCC,15, 0xCC,15 ,0xCC,15 ,0xCC,15,
                                   0x6B,10|0x20, 0x2A,15, 0x5F,15 ,0x86,15 ,0xCC,15,
                                   0x84,10|0x60, 0x2A,15, 0x5F,15 ,0x86,15 ,0x00,00,
                                   0x9E,10|0x90, 0x2A,15, 0x5F,15 };

    bit nr;
    U_INT_8 data key,i,tmp_week,pg;
    U_INT_8 data col;
    U_INT_8 idata hour,minute;
    U_INT_8 idata end_hour,end_minute;
    U_INT_16 idata  now;
    unsigned char xdata buf[10];
    PORT xdata *port=__port+port_no;    
    do{ 
        crt_clear();  
        if((nr=port->type->Out_Port->Normal_Status) !=port->type->Out_Port->Curr_Status){
           crt_outhz(2, 16, "*´ò¿ª",5);      //·´ÏÔ´ò¿ª        
           crt_outhz(2, 64+20,"¹Ø±Õ",4);      //¹Ø±Õ        
        }     
        else{
           crt_outhz(2, 16,"´ò¿ª",4);         //´ò¿ª        
           crt_outhz(2, 64+10, "*¹Ø±Õ",5);   //·´ÏÔ¹Ø±Õ        
        }     
        crt_outhz(2, 64+64+10+10, "¶¨Ê±",4);  //Ê±¼ä        
        crt_outhz(2, 64+64+10+10+64, return_menu,4);

        key=get_xy(1,4,xy);             
    	switch(key){
    	   case 0:
    	   case 1:
    	      output_ctrl(key-1,port_no);
              eeprom_in_queue(SAVE_PORT);    //±£´æ¶Ë¿ÚÊý¾Ý
    	      break;
    	   case 3:
	   case AUTO_RETURN_KEY:         //auto return
           case ALM_RETURN_KEY:
    	      return;   
        }
    }while(key != 2);

    minute=port->type->Out_Port->Start_Time;
    hour=port->type->Out_Port->Start_Time>>8;

    end_minute= port->type->Out_Port->End_Time;
    end_hour= port->type->Out_Port->End_Time>>8;

RE_START:
    crt_clear();  
//    port->Usable=0;    //ÉèÖÃÊ±²»¿ÉÓÃ 
    crt_outhz(2, 16, "Æô¶¯Ê±¼ä",8); //¿ªÊ¼Ê±¼äª¥
    crt_outhz(4, 16, "Í£Ö¹Ê±¼ä",8); //½áÊøÊ±¼äª¥
    
    crt_outdigital(2, 64+64, hour, 2,0);     //Hour
    crt_outdigital(2, 64+64+32, minute, 2,0);   //Minute   
    crt_outchar(2, 64+64+20, ':');    //:

    crt_outhz(2, 64+64+64+16, "ÐÞ¸Ä",4); 
    crt_outhz(4, 64+64+64+16, "ÐÞ¸Ä",4); 

    crt_outdigital(4, 64+64, end_hour, 2,0);     //Hour
    crt_outdigital(4, 64+64+32, end_minute, 2,0);   //Minute   
    crt_outchar(4, 64+64+20, ':');    //:

    tmp_week = port->type->Out_Port->Week & 0x7F;
    pg=0;
    for(i=0; i<4; i++){
	if(tmp_week & (1<<pg++)) 
           crt_outchar(6, 16-_ASCII_WIDE+64*i, '*');                           
        crt_outhz(6, 16+i*64, week_menu+i*4, 4); //ÖÜÒ»ÖÜ¶þÖÜÈýÖÜËÄ
    }  
    for(i=0; i<3; i++){
	if(tmp_week & (1<<pg++)) 
           crt_outchar(8, 16-_ASCII_WIDE+64*i, '*');                           
        crt_outhz(8, 16+i*64, week_menu+i*4+16, 4);
    } 
    crt_outhz(10, 16, week_menu+3*4+16, 4);     //È·¶¨      
    crt_outhz(10, 16+64, return_menu, 4);       //·µ»Ø   

    do{
        key=get_xy(5,4,xy_timer);                                                
        switch(key){
        case 0:   
            draw_screen(35); 
            memset(buf,'0',5);     
            if(modify_data(0,buf,4)==4){  
               hour = ascTohex(buf);
               minute = ascTohex(buf+2);               
            }        
            goto RE_START;
        case 1:   
            draw_screen(35); 
            memset(buf,'0',5);     
            if(modify_data(0,buf,4)==4){  
               end_hour = ascTohex(buf);
               end_minute = ascTohex(buf+2);
            }        
            goto RE_START;
        case 2:   //ÖÜ1
        case 3:   //ÖÜ2
        case 4:   //ÖÜ3
        case 5:   //ÖÜ4
        case 6:   //ÖÜ5
        case 7:   //ÖÜ6
        case 8:   //ÖÜ7
	    key -= 2;	
            i =1<<key;
            pg = key<4?6:8; 
            col = 16-_ASCII_WIDE+64*key;
            if(tmp_week & i){
	       tmp_week &= ~i;
               crt_outnull_num(pg, col, 1);
	    }	
            else{
               tmp_week |= i; 
               crt_outchar(pg, col, '*');        //:
	    }
            break; 
        case 9:   //ok
            now=hour;
            now<<=8;
            now += minute;  
            port->type->Out_Port->Start_Time = now;
           
            now=end_hour;
            now<<=8;
            now += end_minute;  
            port->type->Out_Port->End_Time = now;
            port->type->Out_Port->Week = tmp_week;

            if(tmp_week==0 || (tmp_week & (1<<__ymd.Week-1))==0) 
               stop_ctrl(nr, port_no, _CTRL_TYPE_DIRECT_OFF);  
            else{
//               port->Usable = 1;          
               port->type->Out_Port->Ctrl_Status = _CTRL_TYPE_NOT;
            }
            eeprom_in_queue(SAVE_PORT);    //±£´æ¶Ë¿ÚÊý¾Ý
            op_status(1);                                            
        default:
            return;
        }
    }while(key<10);
}
   
void do_defend_diy(void)   //·ÀÇøÓÃ»§×Ô¶¨Òå
{
//    unsigned char code xy[]={0x91,0x93,0x95,0x97}; 
    unsigned char code xy[]={0x85,10,      0x30,15, 0x62,15, 0x95,15, 0xC9,15,
                             0x9C,10|0x40, 0x30,15, 0x62,15, 0x95,15, 0x0,0};  

    bit modify_bit=0,find_bit=0;
    unsigned char key,open;
    unsigned char port_no;
    unsigned char xdata *xp;
    PORT xdata *pt;
    
    draw_screen(124);
    crt_outhz(2,SPACE_COL,"±àºÅ:",5);    //Ð´"±àºÅ:"
    port_no = 0;
    pt = __port;
//    for(i=0;i<INPUT_PORT_PHY_NUM;i++){
    do{   
        if(pt->Device_Type>=0x20 && pt->Device_Type<=0x5F){ //°²·ÀÌ½²âÆ÷ÊäÈë             
           crt_outnull_num(2,3*_WORD_WIDE+SPACE_COL,2);
           crt_outdigital(2,3*_WORD_WIDE+SPACE_COL,port_no+1,2,0); 
           crt_outnull(2,5*_WORD_WIDE,4);   //Çå³ý¶àÓàµÄ×Ö·û                
           xp = get_config_menu(pt->Device_Type, &key);
           if(xp){              
              crt_outhz(2,5*_WORD_WIDE, xp, key);
           }
//           crt_outnull(2,9*_WORD_WIDE,4);   //Çå³ý¶àÓàµÄ×Ö·û                
//           use = pt->Usable;
           open = pt->type->In_Port->Open;            
RE_SHOW:
            
           if(open & 0x01)   //ÔÚ¼Ò                   
              crt_outchar(8, 16-10, '*');                           
           else
              crt_outnull_num(8,16-10,1);   
           if(open & 0x02)   //ÐÝÏ¢ 
              crt_outchar(8, 16-10+64,'*');                       
           else
              crt_outnull_num(8,16-10+64,1);   

           if(open & 0x04)   //³öÃÅ 
              crt_outchar(8, 16-10+64*2, '*');                          
           else
              crt_outnull_num(8,16-10+64*2,1);   

           if(open & 0x08)   //ÆäËû 
              crt_outchar(8, 16-10+64*3, '*');                          
           else
              crt_outnull_num(8,16-10+64*3,1);   

           do{
              key=get_xy(2,4,xy);             
              switch(key){
                 case 0:
                 case 1:
                 case 2:
                 case 3:
                     if(open & 1<<key){ 
                        open &=~(1<<key);                 
                        crt_outnull_num(8,16-10+64*key,1);   //Çå³ý¶àÓàµÄ×Ö·û                
                     }    
                     else{
                        open |=(1<<key);                        
                        goto RE_SHOW;    
                     }     
                     break;                
                case 4:    
                     pt->type->In_Port->Open = open;
                     modify_bit = 1;                       //±»ÐÞ¸Ä,ÏÂÒ»¸ö                     
                case 5:
                     break;  
                case 6:
                     if(modify_bit){ 
                        eeprom_in_queue(SAVE_PORT);       //±£´æ¶Ë¿ÚÊý¾Ý
                        op_status(1);
                     }  
                     return;                                  
             }       
             find_bit=1;
          }while(key<4);
       }
       if(find_bit==0)  //Ã»ÓÐÕÒµ½
          return;    
       if(port_no++>=INPUT_PORT_PHY_NUM){
          port_no = 0;
          pt = __port;
       }
       else
          pt++;
    }while(key<7);   
}
#if 0
void bright_adjust(unsigned char flag)
{
     if(flag==3){     //ÉÏµ÷
        BRIGHT_UP = 0;
     }  
     else{         //ÏÂµ÷
        BRIGHT_DOWN = 0;
     }
     delay(30);               //Á¬Ðøµ÷ËÄ¼¶        
     BRIGHT_UP = 1;
     BRIGHT_DOWN = 1;
}

void voice_adjust(unsigned char flag)
{
     if(flag==0){     //ÉÏµ÷
        VOICE_ADJ_UP = 0;
     }  
     else{         //ÏÂµ÷
        VOICE_ADJ_DOWN = 0;
     }
     delay(30);               //Á¬Ðøµ÷ËÄ¼¶        
     VOICE_ADJ_UP = 1;
     VOICE_ADJ_DOWN = 1;
}
#endif 

//ÉèÖÃÍ¼Ïñ´æ´¢Æ÷Ê±¼ä
void set_img_times(void)
{
      unsigned char xdata buf[10];
      unsigned char len;

      draw_screen(35);      //
      len = modify_data(0,buf,10);
      if(len==10){
         iic_send_in(MSG_SET_IMG_TIME,buf);
//         wait_reply();
      }
}

void sys_adjust(void)
{
    unsigned char code xy[]={0x37,5,      0x2D,15, 0x60,15 ,0x99,15 ,0xCD,15,
                             0x58,5|0x40, 0x2D,15, 0x60,15, 0,0};  
    unsigned char data key;

    do{
        draw_screen(125);    
        key=get_xy(2,4,xy);    //sizeof(xy)-1         
	switch(key){
           case 0:
                iic_send_in_msgno(MSG_HOST_VOICE_DOWN);
                break;  
           case 1: 
                iic_send_in_msgno(MSG_HOST_VOICE_UP);
//                voice_adjust(key); 
                break;
           case 2: 
                iic_send_in_msgno(MSG_HOST_BRIGHT_DOWN);
                break;
           case 3:  
                iic_send_in_msgno(MSG_HOST_BRIGHT_UP);
//                bright_adjust(key);           
                break;
           case 4:
                set_img_times();
                break;
        }        
     }while(key<5);
}

#if 1
void open_light_set(void)
{

     unsigned char code xy[]={0x34,6,       0x46,30,
                              0x58,6|0x10,  0x46,30, 
                              0x81,6|0x20,  0x46,30,
                              0xA7,6|0x30,  0x46,30};
     unsigned char key;

     draw_screen(128);    
     if(other_item.no_defend_open_light)
        crt_outchar(2, 16, '*');     
     if(other_item.alm_open_light)
        crt_outchar(5, 16, '*');    
     if(other_item.ic_card_no_defend)
        crt_outchar(8, 16, '*');    
     do{
         key=get_xy(4,1,xy);    //sizeof(xy)-1         
         switch(key){
           case 0:   //³··À¿ªµÆÑ¡Ôñ 	
              if(other_item.no_defend_open_light){
                 other_item.no_defend_open_light=0;
                 crt_outnull_num(2, 16, 1);     
              }  
              else{
                 other_item.no_defend_open_light=0x55;
                 crt_outchar(2, 16, '*');     
              }
              break;
           case 1:  //±¨¾¯¿ªµÆÑ¡Ôñ 
              if(other_item.alm_open_light){
                 other_item.alm_open_light=0;
                 crt_outnull_num(5, 16, 1);
              }else{
                 other_item.alm_open_light=0x55;
                 crt_outchar(5, 16, '*');     
              }  
	      break;
           case 2:   //Ë¢¿¨³··ÀÑ¡Ôñ
              if(other_item.ic_card_no_defend){
                 other_item.ic_card_no_defend=0;
                 crt_outnull_num(8, 16, 1);
              }else{
                 other_item.ic_card_no_defend=0x55;
                 crt_outchar(8, 16, '*');     
              }  
	      break;   
         }         
     }while(key<3);
     eeprom_in_queue(SAVE_USED_FLAG);
}   
#endif

void show_phone(unsigned char rol, unsigned char col, unsigned char xdata *p)
{
    unsigned char i,tmp;

    for(i=0;i<PHONE_NO_LEN;i++){
        tmp = *p++;
        if(tmp){      
           switch(tmp){
             case XING+0x30:
                tmp = '*';
                break;
             case JING+0x30:
                tmp = '#';
                break;
           } 
           crt_outchar(rol,col,tmp);
           col+=_ASCII_WIDE;
        }
        else 
           return;
    }
}

void set_phone_no(void)
{

    unsigned char code  xy[]={0x28,10,      0x30,30,
                              0x4F,10|0x10, 0x30,30,
                              0x75,10|0x20, 0x30,30,
                              0x99,10|0x30, 0x30,30,
                              0xB3,10|0x40, 0x30,30};

    unsigned char xdata buf[40];
    unsigned char key;
    unsigned char xdata *xp;

    do{
       crt_clear();
       crt_outhz(1,SPACE_COL,"µç»°1",5);     
       show_phone(1,SPACE_COL*7, other_item.phone_id0);

       crt_outhz(4,SPACE_COL,"µç»°2",5);    
       show_phone(4,SPACE_COL*7, other_item.phone_id1);

       crt_outhz(7,SPACE_COL,"µç»°3",5);     
       show_phone(7,SPACE_COL*7, other_item.phone_id2);

       crt_outhz(10,SPACE_COL,"Õª»úÊ±¼ä",8);     
       crt_outdigital(10,SPACE_COL*7, other_item.hangup_times,2,0);


       crt_outhz(13,SPACE_COL,"·µ»Ø",4);   

       key=get_xy(5,1,xy);
       if(key<5){
          if(key==4)
             return;
          draw_screen(35); 
          memset(buf,0x00,PHONE_NO_LEN);   //BUFÇå¿Õ  
          if(modify_data(0,buf,PHONE_NO_LEN)<=PHONE_NO_LEN){  
             switch(key){
                case 0:
                  xp = other_item.phone_id0;
                  break;  
                case 1:
		  xp = other_item.phone_id1;
                  break;  
                case 2:
                  xp = other_item.phone_id2;
                  break;  
                case 3:
                  other_item.hangup_times = (buf[0]-0x30)*10+buf[1]-0x30;
                  goto END;
             }             
             memcpy(xp,buf,PHONE_NO_LEN);
END:	    
             eeprom_in_queue(SAVE_USED_FLAG); //±£´æ 
         }
      }
   }while(key<4); 
}
     
    
bit no_sound_bit;     //Ã»ÓÐÁåÉù  
void  do_set(void)    //ÉèÖÃ
{
//    unsigned char code xy[]={0x41,0x43,0x45,0x47};
    unsigned char code xy[]={0x34,5,      0x2D,15, 0x60,15 ,0x99,15 ,0xCD,15,
                             0x60,5|0x40, 0x2D,15, 0x60,15 ,0x99,15 ,0xCD,15,  
                             0x80,5|0x80, 0x39,15};
    unsigned char xdata buf[8];
    unsigned char data key,len;
    do{
        draw_screen(80);    
        if(no_sound_bit)
           crt_outchar(2, 16+64-10, '*');     
              
        key=get_xy(3,4,xy);    //sizeof(xy)-1         
	switch(key){
	   case 0:	
                pw_verify(0);
		break;
     	   case 1:
		no_sound_bit = no_sound_bit?0:1;
                buf[0] = no_sound_bit;
                iic_send_in(MSG_HOST_NO_SOUND,buf);
		break;
	   case 2:
                do_defend_diy();
                break; 
	   case 3:
                draw_screen(126);      //·¿ºÅ+¶Ë¿ÚºÅ
                buf[5] = 0x30;         //ÐòºÅÈ±Ê¡Îª0 
                len = modify_data(0,buf,6);
                if(len==5 || len ==6){
                   iic_send_in(MSG_HOST_TALKADDR,buf);
                   wait_reply();
                }
                break;                
	   case 4:
                draw_screen(127);      //¿ªËøÃÜÂë
                if(modify_data(0,buf,4)==4){
                   iic_send_in(MSG_HOST_TALK_LOCK_PIN,buf);
                   wait_reply();
                }
                break;                
           case 5:                    //ÁÁ¶È,ÒôÁ¿µ÷½Ú
                sys_adjust();
                break;
           case 6:
                open_light_set();
                break;
           case 7:                    //ÉèÖÃµç»°ºÅÂë
                set_phone_no();
                break;
        }    
    }while(key<8);
}   

#if 0
void show_ir_ctrl(unsigned int ptr,unsigned char num)
{
    unsigned char idata col,pg,k,key;
    unsigned int  len;
    struct IR_SUB_MENU xdata *mp;
    unsigned char xdata *xp;    
    unsigned char code xy[]={0x38,10,      0x2D,15, 0x64,15 ,0x93,15 ,0xC0,15,
                             0x60,10|0x40, 0x2D,15, 0x64,15 ,0x93,15 ,0xC0,15,
                             0x88,10|0x80, 0x2D,15, 0x64,15 ,0x93,15 ,0xC0,15
                             }; 

    do{
       crt_clear();
       col = SPACE_COL; 
       pg = 2;
       k=0;	
       xp =(unsigned char *)&ir_ctrl+ptr; 	
       mp = (struct IR_SUB_MENU *)xp;
       key = num;
       while(key--){
           crt_outhz(pg,col,mp->hz,IR_SUB_MENU_HZ_LEN);
           col += 64;         //Õ¼¼¸¸öºº×Ö¿Õ¼ä
           if(col < 64){      //ÏÔÊ¾µ½ÏÂÒ³
	      pg += 3;
	   }
	   k++;	
	   mp++; 
       }
       crt_outhz(pg, col, "·µ»Ø",4);
       key=get_xy(3,4,xy);    //sizeof(xy)-1         
       if(key<k){
          mp = (struct IR_SUB_MENU *)xp;
          mp += key;
	  len = mp->ptr;
          pg = mp->code_len; 
	  if(pg < REQUEST_BUF_LEN-APP_HEAD_LEN-3){
             resend_buf.riu_addr = mp->riu_addr;
   	     xp = (unsigned char *)&ir_ctrl+len;   //code addr
             resend_buf.dat[APP_HEAD_LEN]  = 0x16; 
             resend_buf.dat[APP_HEAD_LEN+1]= 0x00;
             resend_buf.dat[APP_HEAD_LEN+2]= 0xFF; //key id
  	     memcpy(resend_buf.dat+APP_HEAD_LEN+3,xp,pg);
             resend_buf.len = APP_HEAD_LEN+3+pg; 
             op_status(wait_reply());    //²Ù×÷×´Ì¬
          }          	   	
       }
    }while(key<k);                              	
}
#endif


static unsigned char code ctrl_xy[]={0x34,10,      0x43,30, 0x98,30, 
                                     0x50,10|0x20, 0x43,30, 0x98,30, 
                                     0x6D,10|0x40, 0x43,30, 0x98,30, 
                                     0x82,10|0x60, 0x43,30, 0x98,30
                                     }; 
#if 0
//Ö÷ÒªÊÇºìÍâÉè±¸¿ØÖÆ£¬¿ØÖÆÖ¸Áî±ä³¤
void show_other_ctrl(void)
{
     unsigned char idata main_item_num;
     unsigned char idata j,pg,col,total_item,key;
     struct IR_MAIN_MENU xdata *mp;

     do{
         crt_clear();
         mp = ir_ctrl.main_menu;
         main_item_num = ir_ctrl.main_item_num;      
         pg = 2;
         j=0;
         col = SPACE_COL; 
         total_item = 0;
         while(main_item_num){
              main_item_num--;
              crt_outhz(pg,col,mp->hz,IR_MAIN_MENU_HZ_LEN);
              col += (IR_MAIN_MENU_HZ_LEN/2)*_WORD_WIDE;         //Õ¼¼¸¸öºº×Ö¿Õ¼ä
              if(col < 64){                                      //ÏÔÊ¾µ½ÏÂÒ³
   	         pg  += 2;
	         col = SPACE_COL;
  	      }
 	      total_item++; 	
	      mp++;
              if(total_item >=12){            //´óÓÚ12Ïî£¬ºóÃæµÄÏµÍ³²»´¦Àí
                 break;
              }
         }
         crt_outhz(pg, col, "·µ»Ø",4);
#if 0
         key=get_xy(4,2,ctrl_xy);             //sizeof(xy)-1         
         if(key < total_item){
            mp = ir_ctrl.main_menu;
            mp += key;	
            show_ir_ctrl(mp->ptr,mp->num);
         }         
#endif
     }while(key < total_item);
}
#endif

void do_ctrl(void)
{

    unsigned char data key;
    unsigned char idata j;
//    unsigned char idata main_item_num=0;
    unsigned char data	i,pg,col;
    unsigned char xdata *xp;
//    struct IR_MAIN_MENU xdata *mp;
    unsigned char idata __rec[4]; 
//    unsigned char other_item_pos,
    unsigned char light_ctrl_pos; 
    bit light_ctrl_bit;  //other_ctrl_bit
    
    memset(__rec,0x00,sizeof(__rec));
    do{
	crt_clear();
        pg = 2;
        j = 0;
        col = SPACE_COL; 
	for(i=16; i<_SCU_PORT_MAX_NUM; i++){   //±¾µØÊä³öÉè±¸     
	    if(__port[i].Usable != 0x01)
               continue;
	    key=__port[i].Device_Type;
	    if(key>=0x80 && key<=0xBF){ //È¡¶Ë¿Ú,ËùÓÐÊä³öÉè±¸(°üÀ¨±¨¾¯Êä³öÉè±¸)
               xp = get_config_menu(key, &key);
               if(xp){ 			
 	          crt_outhz(pg, col, xp, key);                    
 	          col += 21*6;    //Õ¼¼¸¸öºº×Ö¿Õ¼ä
		  if(col < 64){   //ÏÔÊ¾µ½ÏÂÒ³
		     pg  += 2;
		     col = SPACE_COL;
		  }
		  __rec[j++] = i;
	       }
	   }
	}
#if 0
        other_item_pos = other_ctrl_bit=0;
        if(ir_ctrl.used==0x55){  //test 0x55  //ÏÔÊ¾ºìÍâÒ£¿Ø,ÎÞÏßÉè±¸
           crt_outhz(pg,col,"ÆäËû",4);
           col += 21*6;       //Õ¼¼¸¸öºº×Ö¿Õ¼ä
           if(col < 64){      //ÏÔÊ¾µ½ÏÂÐÐ
              pg  += 2;
	      col = SPACE_COL;
	   }   
           other_item_pos = j++;      
           other_ctrl_bit = 1;
        }  
#endif

#if 1
        light_ctrl_pos =light_ctrl_bit= 0;
        if(light_ctrl.used==0x55 ){       //GKB µÆ¿Ø
           if(j==0){
              light_ctrl_op();   //Ö±½Ó½øÈëµÆ¿Ø»­Ãæ                             
              return;
           }
           else{   
              crt_outhz(pg,col,"µÆ¿Ø",4);
              col += 21*6;       //Õ¼¼¸¸öºº×Ö¿Õ¼ä
              if(col < 64){      //ÏÔÊ¾µ½ÏÂÐÐ
                 pg  += 2;
	         col = SPACE_COL;
	      }               
              light_ctrl_pos = j++;
              light_ctrl_bit = 1;
           } 
        }          
#endif

//        num= k+j;         
        crt_outhz(pg, col, "·µ»Ø",4);
//        if(main_item_num){
//           crt_outhz(pg,SPACE_COL+(IR_MAIN_MENU_HZ_LEN/2)*_WORD_WIDE,"ÏÂÒ³",4);  //Ò»ÆÁÎ´ÏÔÊ¾Íê,ÔÚÏÂÒ»ÆÁÏÔÊ¾
//           key += 1;
//        }
        key=get_xy(4,2,ctrl_xy);    //sizeof(xy)-1         
#if 0
        if(other_ctrl_bit && key==other_item_pos){  
           show_other_ctrl();
           return;
        }  
#endif

#if 1
        if(light_ctrl_bit && key==light_ctrl_pos){
           light_ctrl_op();
           return;
        }   
#endif
        if(key<j){
           if(__rec[key]){
              timer_set(__rec[key]);
           }
	}
    }while(key < j);
}   


/**********************************************/
//          ¹â±êÒÆ¶¯´¦Àí
//   num            ×ø±êÊýÁ¿
//   col_limit      ºáÏò×ø±êÊýÁ¿
//   cp             ×ø±êÊý¾Ý
//   ·µ»Øµ±Ç°×ø±êÐòºÅ
/*********************************************/ 
#define LINE_AREAR   10
unsigned char get_xy_light(unsigned char rol_num,unsigned char col_limit,unsigned char code *cp)
{
    unsigned char idata i,x;
    unsigned char data  sn;        
    unsigned char code *pre;
    while(1){ 
       x=key_get();           
       if(x!=TOUCH_KEY)
          return x;
       pre = cp;                 
       for(i=0;i<rol_num;i++){  
           x = *pre++;              
           if(touch.y >= x)  
              x =touch.y - x;            
           else 
              x =x - touch.y;                       
           sn = *pre++;                 //ÐòºÅ
            
           if(x < LINE_AREAR){         
              goto ROW_FIND;
           }
           pre+=col_limit<<1;
       }  
#if __DEBUG
       goto END;
#endif
       continue; 
ROW_FIND:
       for(i=0;i<col_limit;i++){                  
           x = *pre++;              
           if(touch.x >= x)  
              x =touch.x - x;            
           else 
              x =x - touch.x;                       
           if(x < *pre++){   //ÕÒµ½
#if __DEBUG
       crt_outnull(13,200,5);
       crt_out_hex(13,200,touch.x);
       crt_out_hex(13,230,touch.y);
#endif
                buzzer_start(5,1);
                return (sn+i);              
           }
       }

#if __DEBUG
END:
       crt_outnull(13,200,5);
       crt_out_hex(13,200,touch.x);
       crt_out_hex(13,230,touch.y);
#endif
       //touch.flag=0;
       //EX0 = 1; 
    }
}


static unsigned char xdata light_no_buf[10];
#if 0
void refuse_status(unsigned char begin,unsigned char end)
{
     unsigned char pg,i,j,curr_light_no;

     pg = 3;
     for(i=begin;i<end;i++){
         curr_light_no = light_no_buf[i];
         j=light_ctrl_status[curr_light_no].status;
         if(j){
            crt_outchar(pg, 21*6+10,'*');
            crt_outnull_num(pg, 21*8,1);
         }   
         else{
            crt_outchar(pg, 21*8, '*');
            crt_outnull_num(pg, 21*6+10,1);
         }
         pg+=2;
     }       
}
#endif

//bit refuse_ctrl_bit;
void light_ctrl_op(void)
{
//   unsigned char code op_table[]={1,2,0x11,0x12,0x13,0x14,};
                                //ON  OFF  +   -  - 
   unsigned char code op_table[]={1,  0,   2,  3, 3};//»Û¾Ó¿ØÖÆÐ­Òé

   unsigned char code xy[]={  //0x27,0,  0x8D,5, 0xa7,5, 0xBA,5, 0xCE,5, 0xDE,5, 0xEF,5,
			      0x30,0,  0x20,10, 0x4D,10, 0x80,10, 0xB5,10, 0xE1,10,
                              0x4A,5,  0x94,15,  0xB5,15,  0xD2,15,  0xEC,15,  0xEC,15, 
                              0x63,10, 0x94,15,  0xB5,15,  0xD2,15,  0xEC,15,  0xEC,15, 
                              0x7D,15, 0x94,15,  0xB5,15,  0xD2,15,  0xEC,15,  0xEC,15, 
                              0x98,20, 0x94,15,  0xB5,15,  0xD2,15,  0xEC,15,  0xEC,15, 
                              0xAE,25, 0x94,15,  0xB5,15,  0xD2,15,  0xEC,15,  0xEC,15, 
                              0xC2,30, 0x20,10, 0x4D,10, 0x80,10, 0xB5,10  
                           }; 

    unsigned char data key;
    unsigned char data j;
    unsigned char idata curr_menu,pre_menu_page;    
    unsigned char data	i,pg;
    unsigned char xdata addr_buf[10];
//    unsigned char xdata light_no_buf[10];
//    unsigned char xdata buf[20];
    
    unsigned char begin,end;
    unsigned char curr_type;  

    struct LIGHT_TYPE  xdata *mp,*pre_mp_page;
     
//    memcpy(buf+APP_HEAD_LEN,"\x16\x02\xff\x00\x10",5);
//    make_send_package(0xFF,0,QUERY_TYPE,buf,APP_HEAD_LEN+5);   //·¢ËÍÒ»¸öË¢ÐÂÃüÁî             
    curr_type=LIGHT_CTRL_TYPE;    //µ±Ç°ÎªµÆ¹âÀàÐÍ
    do{
        curr_menu = light_ctrl.light_num;
        mp = light_ctrl.light_type;
        end = 0;        
NEXT_SRC:
        j = 0;
        pg = 3;
        crt_clear();
        begin = end;
        pre_mp_page = mp;
        pre_menu_page = curr_menu;
        while(curr_menu){   //ÏÔÊ¾Éè±¸Ãû³Æ
              curr_menu--;
              if(mp->light_type != curr_type){     //È¡µ±Ç°ÀàÐÍÉè±¸
                 mp++;
                 continue;
              } 
              crt_outhz(pg,5,mp->light_name,12);
              addr_buf[j] = mp->light_addr;
              light_no_buf[j++]= mp->light_no;

              crt_outhz(pg, 21*7, "¿ª  ¹Ø  +  -",13);
              pg  += 2;
              mp++;
              end++;
              if(pg >= 12 ){   //ÐèÒª»»Ò³ÏÔÊ¾
                 break;
              }
        }
        crt_outhz(1, 5, "µÆ¹â ¿Õµ÷  ´°Á±  µçÆ÷ ÆäËû",30);  //ÏÔÊ¾×Ü¿Ø²Ëµ¥
        crt_outhz(13, 21*2+SPACE_COL, "È«¿ª  È«¹Ø  ·µ»Ø",16);//ÏÔÊ¾×Ü¿Ø²Ëµ¥
        if(curr_menu){
           crt_outhz(13,5,"ÏÂÒ³",4);  //Ò»ÆÁÎ´ÏÔÊ¾Íê,ÔÚÏÂÒ»ÆÁÏÔÊ¾
        }
        else{
           crt_outhz(13,5,"ÉÏÒ³",4);  
        }                       
//        delay(10);
//        refuse_status(begin,end);
//WAIT_KEY:
//        refuse_ctrl_bit=1;
        key=get_xy_light(7,5,xy); //sizeof(xy)-1   
        switch(key){
            case 0x00:
                 curr_type = LIGHT_CTRL_TYPE;
                 break;
            case 0x01:
                 curr_type = AIR_CTRL_TYPE;
                 break;
            case 0x02:
                 curr_type = WINDOW_CTRL_TYPE;
                 break;
            case 0x03:
                 curr_type = ELEC_CTRL_TYPE;
                 break;
            case 0x04:
                 curr_type = OTHER_CTRL_TYPE;
                 break;
            case 30:
                 if(curr_menu){   //´¦Àí»»ÆÁÏÔÊ¾
                    goto  NEXT_SRC;           	   		
                 } 
                 continue;
            case 31:
                 resend_buf.dat[APP_HEAD_LEN+4]=0x81;    //ALL ON
                 goto SEND;
            case 32:
                 resend_buf.dat[APP_HEAD_LEN+4]=0x80;    //ALL OFF
//            case 39:   //Ë¢ÐÂ
//                 resend_buf.dat[APP_HEAD_LEN+4]=0x10;    //OP                
SEND:
                 resend_buf.riu_addr = 0xFF;    //¹ã²¥µØÖ·
     	         memcpy(resend_buf.dat+APP_HEAD_LEN,"\x16\x02\x05\xAA",4);
                 resend_buf.len = APP_HEAD_LEN+5; 
                 if(wait_reply()==0){
                    op_status(0);    //²Ù×÷Ê§°Ü
                 } 
                 key_free();
                 continue;
//            case MSG_REFUSE_LIGHT_CTRL:
//                 refuse_status(begin,end);
//                 goto WAIT_KEY;
            case 33:
                 return;
            default:
                 key -= 5;
                 if(key<30){
                    i=key/5;          //ÐÐ
                    j=key%5;          //ÁÐ                                
                    resend_buf.riu_addr = addr_buf[i];    //Íø¹ØµØÖ·
                    resend_buf.dat[APP_HEAD_LEN]  = 0x16; 
                    resend_buf.dat[APP_HEAD_LEN+1]= 0x02;
                    resend_buf.dat[APP_HEAD_LEN+2]= 0x05; //»Û¾ÓÖÇÄÜ¿ª¹ØÐ­Òé±êÊ¶
                    resend_buf.dat[APP_HEAD_LEN+3] = light_no_buf[i]; //Éè±¸ºÅ        
                    resend_buf.dat[APP_HEAD_LEN+4] = op_table[j];     //¿ØÖÆ×´Ì¬
                    resend_buf.len = APP_HEAD_LEN+5; 
                    if(wait_reply()==0){    //²Ù×÷×´Ì¬           
                        op_status(0);
                    } 
                    curr_menu = pre_menu_page;
                    mp = pre_mp_page;
                    end = begin;
                    goto NEXT_SRC;
                 }
                 return;  //¿ÕÏÐÊ±£¬·µ»Ø
        }
    }while(1);
}   

#define SCENE_CTRL_OP   0x40
//HJ³¡¾°¿ØÖÆ½çÃæ
void do_scene(void)
{
    unsigned char code xy[]={0x34,5,      0x35,15, 0x80,15 ,0xC5,15,
                             0x60,5|0x30, 0x35,15, 0x80,15 ,0xC5,15};  
    unsigned char xdata buf[20];
    unsigned char key;
    do{
        crt_clear();
        crt_outhz(2, SPACE_COL*2, "³¡¾°A   ³¡¾°B   ³¡¾°C",30);  //ÏÔÊ¾³¡¾°²Ëµ¥
        crt_outhz(5, SPACE_COL*2, "³¡¾°D   ³¡¾°E   ·µ»Ø",30);  //ÏÔÊ¾³¡¾°²Ëµ¥
              
        key=get_xy(2, 3, xy);   
        if(key<5 && hj_scene_ctrl.used==0x55){           
//           memcpy(buf+APP_HEAD_LEN,"\x16\x05\x02\x00",4);
           buf[APP_HEAD_LEN]   = 0x16;
           buf[APP_HEAD_LEN+1] = 0x05;
           memcpy(buf+APP_HEAD_LEN+2,hj_scene_ctrl.scene[key],SCENE_MAX_NUM);
           make_send_package(0xFF,0,QUERY_TYPE,buf,APP_HEAD_LEN+4+SCENE_MAX_NUM);//·¢ËÍÒ»¸ö³¡¾°ÃüÁîµ½Íø¹Ø
           op_status(1);
        }
    }while(key<5);
}

#if 0
void do_servers(void)   //·þÎñ
{
//    unsigned char code xy[]={0x91,0x93,0x95}; 

    unsigned char code xy[]={0xA0,10, 0x2c,15, 0x63,15 ,0x97,15};

    unsigned char key,Menu_Length;
    unsigned char device_seq_no;//counter=0;
    unsigned char total_item=0;
    char i;
    unsigned char xdata *xp;

    resend_buf.dat[APP_HEAD_LEN] = 0xa7;
    resend_buf.dat[APP_HEAD_LEN+1] = 0x02;
    resend_buf.len = APP_HEAD_LEN+2; 
    resend_buf.riu_addr = 0;

    if(wait_reply()==0)        //Êý¾Ý·µ
       return; 

    crt_clear();
    crt_outhz(2,SPACE_COL,"·þÎñÃû:",7);    //Éè±¸Ãû:¡¯
    crt_outhz(10, SPACE_COL, "ÏÂÒ³", 4);             //È·¶¨
    crt_outhz(10, SPACE_COL+64, "Ìá½»", 4);
    crt_outhz(10, SPACE_COL+64+64, "·µ»Ø", 4);
    for(i=0;i<REPAIR_MENU_ITEM;i++){
       if(__repair_menu.device_name[i].seq_no)
          total_item++;
    }  
    if(total_item==0)   //Ã»ÓÐÕÒµ½Ïî 
       return;
    i=0;
    do{
        device_seq_no = __repair_menu.device_name[i].seq_no;
        if(device_seq_no){
           crt_outnull_num(2,10*_WORD_WIDE,2);
           crt_outnull(2,5*_WORD_WIDE,4);   //Çå³ý¶àÓàµÄ×Ö·û                
           crt_outdigital(2,10*_WORD_WIDE,device_seq_no,2,0);   //ÏÔÊ¾Éè±¸±àºÅ
           xp = __repair_menu.Hz_bm+__repair_menu.device_name[i].Menu_Item_Pointer; //ÏÔÊ¾¶¯Ì¬²Ëµ¥£¬Éè±¸Ãû    
           Menu_Length = __repair_menu.device_name[i].Menu_Length;         
//           if(Menu_Length>8)
//              Menu_Length=8;     
           crt_outhz(2,5*_WORD_WIDE, xp, Menu_Length);
        }
//        else{
//           if(++i >= REPAIR_MENU_ITEM)
//              i = 0; 
//           if(counter++ > REPAIR_MENU_ITEM)  //Ã»ÓÐ¿ÉÓÃÏî
//              return;  
//           continue;  
//        } 
//        counter = 0;           
//        key = key_get();     
        key=get_xy(1,3,xy);             
        switch(key){      
          case 0: 
             if(++i >= total_item)//REPAIR_MENU_ITEM)  
                i = 0;
             break; 
          case 1: 
             resend_buf.dat[APP_HEAD_LEN]=0xAD;
             resend_buf.dat[APP_HEAD_LEN+1]=0x00;
             resend_buf.dat[APP_HEAD_LEN+2]=device_seq_no;
             resend_buf.len =APP_HEAD_LEN+3; 
             resend_buf.riu_addr = 0;
             op_status(wait_reply());    //²Ù×÷×´Ì¬
             return;   
/*
          case 1:
          case 2:
             if(--i < 0)
                i = total_item-1;
             break;    

          case 5:
          case 6: 
             if(++i >= total_item)//REPAIR_MENU_ITEM)  
                i = 0;
*/
         }                 
    }while(key<2 );
}       
#endif

#if 0
void do_port_config(void)   //¶Ë¿ÚÉè±¸³·Ïû£¬°²×°
{
    unsigned char code xy[]={0xA5,10, 0x30,15, 0x66,15 ,0x9A,15, 0xD1,15};


    unsigned char key,use;
    char port_no;
    unsigned char xdata *xp;
    PORT xdata *pt;
    
    draw_screen(123);
    crt_outhz(2,SPACE_COL,"±àºÅ:",5);     //Ð´¡®¶Ë¿ÚÃû:¡¯
    crt_outhz(4,SPACE_COL,"Ãû³Æ:",5);    
    crt_outhz(6,SPACE_COL,"×´Ì¬:",5);    
    port_no = 0;
    do{
        pt = __port + port_no;
        crt_outnull_num(2,4*_WORD_WIDE,2);
        crt_outdigital(2, 4*_WORD_WIDE,port_no+1,2,0); 
        if(port_no>=0 && port_no<_SCU_PORT_MAX_NUM){  //ÏÔÊ¾¶¯Ì¬²Ëµ¥£¬Éè±¸Ãû    
            crt_outnull(4,4*_WORD_WIDE,4);            //Çå³ý¶àÓàµÄ×Ö·û                
            xp = get_config_menu(pt->Device_Type, &key);
            if(xp){              
               crt_outhz(4,4*_WORD_WIDE, xp, key);
            }
        } 
        else{
            port_no=0x00;    
            continue;
        }
        crt_outnull(6,4*_WORD_WIDE,4);   //Çå³ý¶àÓàµÄ×Ö·û                
        use = pt->Usable;
        if(use == 0x01){ 
           crt_outhz(6,4*_WORD_WIDE, "¿ÉÓÃ",4);
        } 
        else{
           crt_outhz(6,4*_WORD_WIDE, "²»¿ÉÓÃ",6);
        }
        key=get_xy(1,4,xy);             
        switch(key){
            case 0:
                if(++port_no >= _SCU_PORT_MAX_NUM)  
                   port_no = 0;
                break; 
            case 1:
            case 2:
	        if(use>0 && use<0x03){          //ÓÐ´ËÉè±¸
		   pt->Usable = (key==1)?2:1;
                   eeprom_in_queue(SAVE_PORT);       //±£´æ¶Ë¿ÚÊý¾Ý
                   op_status(1);
                   return;
                }
        }       
    }while(key<3);
}
#endif       

void do_system_manager(void)
{
//    unsigned char code xy[]={0x51,0x53,0x55,0x57,
//			     0x91,0x93,0x95}; 
    unsigned char code xy[]={0x36,10,      0x29,15, 0x63,15 ,0x95,15 ,0xCA,15,
                             0x5F,10|0x40, 0x29,15, 0x63,15 ,0x95,15 ,0,0};

    unsigned char key;
    key = pw_verify(3);
//    if(key==0x05)
//       do_query_version(0);
    if(key){    //Ð£ÑéÏµÍ³ÃÜÂë
        do{
           draw_screen(85);    
           key=get_xy(2,4,xy);             
           switch(key){
            case  0:       //ÍøÂç²éÑ¯
//                do_query_net();
                break;
            case  1:
#if 0       //Ã»ÓÐÊ¹ÓÃ£¬½ÚÔ¼¿Õ¼ä
                do_modify_meter_data();//ÐÞ¸Ä±íÊý¾Ý
#endif 
                break;
            case  2:        //ÐÞ¸ÄÅäÖÃ                
#if 0
                do_port_config();
#endif
                break;
	    case  3:	
                do_query_ip_mac();
	        break;	                
   	    case  4:      
                while(1);	
                break;
            case  5:  
                memset(&__alm_log,0x00,sizeof(__alm_log));  
                memset(&__defend_log,0x00,sizeof(__defend_log));  
                __sys_passwd.Pass_Num = 0;       //Çå³ýÓÃ»§ÃÜÂë,·ÀÖ¹Êý¾Ý´íÎó²»ÄÜÏû¾¯
           	eeprom_in_queue(SAVE_DEFEND_LOG);
               	eeprom_in_queue(SAVE_ALM_LOG);
               	eeprom_in_queue(SAVE_SYS_PASSWD);
                op_status(1);
                break;
            }
        }while(key<6);
    }           
}

/**************************************************************************/
/*              get_config_menu ²éÕÒ¶¯Ì¬²Ëµ¥                                */
/* ²ÎÊý£º Device_type¡ª¡ªÉè±¸ÀàÐÍ                                            */ 
/*       pMenu_Pointer¡ª¡ª¶¯Ì¬²Ëµ¥µØÖ·                                      */            
/*       pLen¡ª¡ªÊý¾Ý³¤¶È                                                   */    
/* ·µ»Ø£º 0¡ª¡ªÃ»ÓÐÕÒµ½£¬1¡ª¡ªÕÒµ½                                            */     
/* ÈÕÆÚ£º2001/01/03                                                        */      
/*------------------------------------------------------------------------*/
/* ÐÞ¸Ä¼ÇÂ¼:		                                                  */
/*    							                  */   
/**************************************************************************/
unsigned char xdata *get_config_menu(U_INT_8 Device_Type,U_INT_8 data *len)
{
    unsigned char i;
    unsigned char xdata *xp; 

    for(i=0; i<MENU_MAX_LEN; i++){ 
        if(__menu_item.device[i].Used==0x01){
           if(Device_Type == __menu_item.device[i].Device_Type){
              xp = __menu_item.Hz_bm+__menu_item.device[i].Menu_Item_Pointer;
              *len = __menu_item.device[i].Menu_Length;                             
              if(*len > 8)    //@20061027
                 *len = 8;    
              return xp;  
           }
           continue;     
        } 
        return NULL;
    }
    return NULL;
}

/**************************************************************************/
/*             wait_reply   µÈ´ýCCCÏìÓ¦Êý¾Ý                         */    
/* ·µ»Ø£º1¡ªCCCÒÑ¾­ÏìÓ¦£¬0---NDT³¬Ê±ÍË³ö¬£¬£¬µ½                         */     
/* ÈÕÆÚ£º2001/04/13                                                        */      
/*------------------------------------------------------------------------*/
/* ÐÞ¸Ä¼ÇÂ¼:		                                                  */
/*    							                  */   
/**************************************************************************/
bit wait_reply(void)
{
    unsigned char i=0,j=1;

     draw_screen(13);      //ÍøÂçÁ¬½Ó
     __return_ok = _FALSE;
     do{
        if(i%10==0){
           make_send_package(resend_buf.riu_addr,0,QUERY_TYPE,resend_buf.dat,resend_buf.len);                
           crt_outchar(2,j*16+24*4,'>');
           j++;
        }  
        delay(15);       //100ms
        if(__return_ok) 
           return 1;
     }while(i++ <= 3*20);    //ASYN_RETRANS
     return 0;
}

ALM_INFO xdata alm_info;

extern bit  cs6_b0_talking;
extern bit  cs6_b1_switching  ;
extern bit  cs6_b2_CRTdisplay ;
extern bit  cs6_b3_unlock ;
extern unsigned char bdata cs6_out;
unsigned char xdata alm_show_pic;
void other_task(void)
{
     static bit show;
     U_INT_8 data i,page,page1,col,type; 
     unsigned char xdata *xp;
     extern unsigned char xdata lcd_display_status;
     extern unsigned char xdata lcd_sw_status;
     

      for(i=MS_MAX_SUM; i<TIMER_MAX_NUM; i++){     //1s
          if(tm[i]) tm[i]--;
      }
        
      if(!tm[KEYBOARD_TIMEOUT]){       //µÈ´ý2·ÖÖÓÎÞ°´¼ü·µ»ØMAIN menu
          tm[KEYBOARD_TIMEOUT] = KEY_TIMEOUT;   //µÈ´ý
//	  VOICE_CTRL = VOICE_OFF;
          //cs6_b1_switching = SHOW_CHAR;            //ÇÐ»»µ½×Ö·û
          SCREEN_SWITCH_CHAR;
          //cs6_b3_unlock = LOCK;                 //ÑÓÊ±×Ô¶¯¹ØÃÅ
          CLOSE_LOCK;
          //cs6_b0_talking = WAITING_RING;        //ÑÓÊ±×Ô¶¯¹Ò»ú
          WAITING_BELL;
          if(__menu_layer != MAIN_MENU)
             key_in(AUTO_RETURN_KEY);           //RETURN TO MAIN MENU

          else{
//  	     if(other_item.on_display==0){
                LCD_POWER_OFF;                  //¹Ø±ÕCRT 
//             }
//             else{
                //crt_outhz(10,10,"ÇÐ»»µ½ÊÓÆµ",10);     //³£ÁÁ£¬ÏÔÊ¾ÊÓÆµ£¬·ÀÖ¹ÆÁÄ»ÉÕÉË
                lcd_display_status=CRTDISPLAY_OFF;
                SCREEN_SWITCH_VIDEO;
//             }    
          }

      }

      if(__menu_layer==MAIN_MENU){
         if(new_img_msg || __msg.msg_newness & 0x3F){  //ÓÐÐÂµÄÍ¼ÏñÏûÏ¢ºÍÎÄ×ÖÏûÏ¢
            if(show)
               crt_outchar(6, 1*90+28+_ICON_WIDTH,'*');    //ÏÔÊ¾Ò»¸ö'*'
            else 
               crt_outnull_num(6, 1*90+28+_ICON_WIDTH,1);
            show = ~show;
         }
      }
  
      if(__defend_fail_beep){             
         buzzer_start(50,1);
         key_in(AUTO_RETURN_KEY);     
      }

      if(alm_info.ptr){
         if(alm_show_pic==0){
            if(__menu_layer!=MAIN_MENU){
               key_in(ALM_RETURN_KEY);  //·µ»Øµ½Ö÷²Ëµ¥
  	       return;
            }
            key_in(ALM_SHOW_KEY);
         }  
         page1=2;
	 for(i=0;i<alm_info.ptr;i++){
             col = alm_info.alm[i].x;
             page = alm_info.alm[i].y;
             if(show){
                crt_outnull_num(page,col,1);
             }
             else{
                crt_outchar(page,col,0x31+i); 
     
 	        type = alm_info.alm[i].device_type;  //ÏÔÊ¾±¨¾¯µãÃû³Æ
	        xp = get_config_menu(type,&type);
	        if(xp){
		   crt_outchar(page1,200,i+0x31);
                   crt_outhz(page1, 210, xp, type);
	           page1 += 1;	
	        }	
             }          
         }
//         page=2;
//         for(i=0;i<alm_info.ptr;i++){  //ÏÔÊ¾±¨¾¯µãÃû³Æ
//	       type = alm_info.alm[i].device_type;
//	       xp = get_config_menu(type,&type);
//	       if(xp){
//		  crt_outchar(page,212,i+0x31);
//                  crt_outhz(page, 222, xp, type);
//	          page += 3;	
//	       }	
//   	 }     	
         show = ~show;
      }
}
/**********************************************/
//          ¹â±êÒÆ¶¯´¦Àí
//   num            ×ø±êÊýÁ¿
//   col_limit      ºáÏò×ø±êÊýÁ¿
//   cp             ×ø±êÊý¾Ý
//   ·µ»Øµ±Ç°×ø±êÐòºÅ
/*********************************************/ 
unsigned char get_xy(unsigned char rol_num,unsigned char col_limit,unsigned char code *cp)
{
    unsigned char idata i,j,x;
    unsigned char data  sn;        
    unsigned char code *pre;
//    unsigned char code fix_xy[]={0xE0,15, 0x15,10, 0x3D,10, 0x51,10, 0x6d,10, 0x89,10,
//                                          0xA7,10, 0xC2,10, 0xE0,10, 0x00,0,  0x00,0 };

    while(1){ 
       x=key_get();           
       if(x!=TOUCH_KEY)
          return x;
       pre = cp;                 
       for(i=0;i<rol_num;i++){  
           x = *pre++;              
           if(touch.y >= x)  
              x =touch.y - x;            
           else 
              x =x - touch.y;                       
           j = *pre++;                 //ÐÐ´°¿Ú·Å´ó2±¶,¸ß4Î»×÷ÐòºÅ
           sn = j>>4;
            
           j =(j&0x0F)<<1;              
           if(x <= j){         
              goto ROW_FIND;
           }
           pre+=col_limit<<1;
       }  
#if __DEBUG
    goto END;
#endif
       continue; 
ROW_FIND:
       for(i=0;i<col_limit;i++){                  
           x = *pre++;              
           if(touch.x >= x)  
              x =touch.x - x;            
           else 
              x =x - touch.x;                       
           if(x <= *pre++){   //ÕÒµ½
#if __DEBUG
       crt_outnull(13,200,5);
       crt_out_hex(13,200,touch.x);
       crt_out_hex(13,230,touch.y);
#endif
                buzzer_start(5,1);
                return (sn+i);              
           }
       }
#if __DEBUG
END:
       crt_outnull(13,200,5);
       crt_out_hex(13,200,touch.x);
       crt_out_hex(13,230,touch.y);
#endif
    }
}

void no_alm(unsigned char type) //Í£Ö¹±¨¾¯
{
      unsigned char xdata buf[40];

//       buzzer_off(_BIT_ALM);
//       ALM_LIGHT = LED_OFF;
      alm(0);    
      buf[APP_HEAD_LEN]=0xA4;
      buf[APP_HEAD_LEN+1]=0x00;
      buf[APP_HEAD_LEN+2]=type;
      buf[APP_HEAD_LEN+3]=0x00;
      make_send_package(0,0,ALM_TYPE,buf,APP_HEAD_LEN+4);    //->CCC            
}

unsigned char code dev_type_addr[]={0x00,0x80,0x90,0xB0,0xC0}; //Éè±¸µØÖ·¿Õ¼ä
unsigned char code dev_op[]={0x80,1,0,0,0, 0,0,0,0,0x81}; //Éè±¸µØÖ·¿Õ¼ä
void phone_dev_ctrl(unsigned char dev_type,unsigned char sn,unsigned char op)
{
      resend_buf.riu_addr = 0x15;            //Íø¹ØµØÖ·
      resend_buf.dat[APP_HEAD_LEN]  = 0x16; 
      resend_buf.dat[APP_HEAD_LEN+1]= 0x02;
      resend_buf.dat[APP_HEAD_LEN+2]= 0x05;  //»Û¾ÓÖÇÄÜ¿ª¹ØÐ­Òé±êÊ¶
      resend_buf.dat[APP_HEAD_LEN+3]= dev_type_addr[dev_type]+sn; //Éè±¸ºÅ£«ÐòºÅ
      resend_buf.dat[APP_HEAD_LEN+4]= dev_op[op];    //¿ØÖÆ×´Ì¬
      resend_buf.len = APP_HEAD_LEN+5;
//      return wait_reply();                   //²Ù×÷×´Ì¬
}

//´¦Àíµç»°ÐÅºÅ
#if 0
void phone_msg(void)
{
      unsigned char key,i=0,dev_type,dev_sn,op;
      unsigned char xdata buf[10];
      bit curr_status=0;
         
      do{
          key=key_get();             
          switch(key){
             case PHONE_CALL_MSG:
                  if(curr_status==1) break;   //ÒÑ¾­Ìá»ú
#if __PHONE_DEBUG==1
                  crt_outhz(1,_WORD_WIDE, "µç»°ºôÈë..",10);     
#endif       
                  if(i++ > other_item.hangup_times){   //ÕñÁåÊ±¼ä
                     curr_status=1;
                     PHONE_CTRL_ON;           //µç»°Õª»ú
                     delay(100);              //ÑÓ³Ù1s²¥·ÅÓïÒô
                     key_free();
#if __PHONE_DEBUG==1
                     crt_outhz(3,_WORD_WIDE, "µç»°Õª»ú..",10);   
#endif
		     isd4000_cmd(0,0,ISD_SET_PLAY_CMD);
                     i=0; 
                     do{                                    
                         key = key_get(); 
                         if(key<=9){
  			    buf[i++] =key;
#if __PHONE_DEBUG==1
                            crt_outdigital(8, 4*_WORD_WIDE,key,2,0); 
#endif
                         } 
                         else 
                            goto END;
                     }while(i<PHONE_PIN_LEN);
                     if(memcmp(buf,"\x1\x2\x3\x4",PHONE_PIN_LEN)==0){
  		        isd4000_cmd(VOICE_ADDR_CTRL_0,VOICE_ADDR_CTRL_1,ISD_SET_PLAY_CMD);                     
                     }
                     else{ 
                        phone_end();
                     } 
                   }
                   break;  
              //Éè±¸ÀàÐÍÑ¡Ôñ
              case 1: //µÆ¹â¿ØÖÆ
              case 2: //¿Õµ÷¿ØÖÆ
              case 3: //´°Á±¿ØÖÆ
              case 4: //µçÊÓ¿ØÖÆ
                   dev_type = key;
                   isd4000_cmd(VOICE_ADDR_ROOM_NAME_0,VOICE_ADDR_ROOM_NAME_1,ISD_SET_PLAY_CMD);//²¥·Å"·¿¼äÃû³Æ"
                   dev_sn=key_get();  //Éè±¸ÐòºÅ
                   if(dev_sn==NUM_JING) goto END; 
                   isd4000_cmd(VOICE_ADDR_ONOFF_0,VOICE_ADDR_ONOFF_1,ISD_SET_PLAY_CMD);        //²¥·Å"¿ª¹Ø²Ù×÷"
                   op=key_get();      //cmd
                   if(op==NUM_JING) goto END; 
                   isd4000_cmd(VOICE_ADDR_WAITING_0,VOICE_ADDR_WAITING_1,ISD_SET_PLAY_CMD);    //²¥·Å"¿ª¹Ø²Ù×÷"
                   if(phone_dev_ctrl(dev_type,dev_sn,op))
                      isd4000_cmd(VOICE_ADDR_OP_OK_0,VOICE_ADDR_OP_OK_1,ISD_SET_PLAY_CMD);     //²¥·Å"¿ª¹Ø²Ù×÷"
                   else
                      isd4000_cmd(VOICE_ADDR_OP_FAIL_0,VOICE_ADDR_OP_FAIL_1,ISD_SET_PLAY_CMD); //²¥·Å"¿ª¹Ø²Ù×÷"
                   break; 
              default:
#if __PHONE_DEBUG==1
                   crt_outnull_num(6,4*_WORD_WIDE,3);
                   crt_outdigital(6, 4*_WORD_WIDE,key,2,0); 
#endif
//                   if(key==NUM_JING)
                      goto END;                             
          }   
     }while(key<AUTO_RETURN_KEY);
END:
     phone_end();
}
#endif

      
void do_menu(void)
{
//    extern  bit refuse_ctrl_bit;
    U_INT_8 data curr_xy,key;

    unsigned char code xy[]={0x40,0xA,      0x3A,35, 0x80,35 ,0xC7,35,
                             0x70,0xA|0x30, 0x3A,35, 0x80,35 ,0xC7,35,
                             0xA0,0xA|0x60, 0x3A,35, 0x80,35 ,0xC7,35};
    draw_screen(20);       
    __menu_layer = MAIN_MENU;

next:
//    refuse_ctrl_bit=0;
    curr_xy=get_xy(3,3,xy);             
    __menu_layer = SUB_MENU_1;
    switch(curr_xy){
    case 0:                       /* ²¼·À */
        do_defend();
        break;
    case 1:                       /* ²éÑ¯ */
        do_query();
        break;
    case 2:                             
        do_set();                 //ÉèÖÃ
        break;
    case 3:
        do_ctrl();
        break;
    case 4:
        do_scene();               //»Û¾Ó³¡¾°¿ØÖÆ
        break;
    case 5:                             
        do_query_msg();
        break;
    case 6:                
//        do_servers();
        break; 
    case 7:
        do_system_manager();      //ÏµÍ³¹ÜÀí      
        break;
    case 8:                       //about
        do_query_version();
        break;         
    case FAST_NO_ALM_KEY:
        if(pw_verify(1)){
	   no_alm(0x01);
           op_status(1);                       
        }
        break;
    case FAST_NO_DEFEND_KEY:
        if(pw_verify(1)){
           defend_save_and_light(1,0x80,DEFEND_MODE_KEY); 
           op_status(1);                       
        }
        break;
    case ALM_SHOW_KEY:      //ÏÔÊ¾±¨¾¯·¿¼äÌ×ÐÍ,¼°±¨¾¯µã
        alm_show_pic=1;
     	crt_clear();
        show_room_pic();
        enter_phone_alm();
        while(1){
           key=key_get();
           if(key < AUTO_RETURN_KEY){
              if(key==NUM_JING ){      //µç»°"#"ºÅÏû¾¯
                 no_alm(0x02);
              }               
	      break;
           }  
           else{
              if(key == AUTO_RETURN_KEY){
                 LCD_POWER_OFF;        //³¬Ê±¹ØÆÁÄ»,±¨¾¯¼ÌÐø
              } 
           } 
        }
        alm_info.ptr=0;
        alm_show_pic=0;
        break;          
    case NUM_JING:
        stop_phone_alm();
        no_alm(0x02);
        break; 
    default:                         //Ö÷²Éµ¥²»ÔÙË¢ÐÂ
        __menu_layer = MAIN_MENU;
        goto next;
    }
}


