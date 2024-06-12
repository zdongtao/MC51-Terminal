#include <reg51.h>
#include <absacc.h>
#include <string.h>
#include <intrins.h>
#include "inc/system.h"
#include "inc/types.h"
#include "inc/hdres.h"
#include "inc/ctrl.h"
#include "inc/tool.h"
#include "inc/alarm.h"
#include "inc/timer.h" 
#include "inc/app.h"
#include "inc/asyn.h"
#include "inc/menu.h"
#include "inc/key.h"
#include "inc/vi2c.h"

bit delay_status_bit,continue_status_bit;
CTRL_TIME_TYPE  idata __delay_ctrl,__continue_ctrl;
U_INT_16 xdata  __count_pulse_meter[_INPUT_PORT_NUM];   //Âö³å¼ÆÊýÊý
U_INT_8  xdata  __bypass[_INPUT_PORT_NUM];              //ÅÔÂ·±êÖ¾
REPAIR_MENU xdata __repair_menu;    //Î¬ÐÞ²Ëµ¥Ïî¶¨Òå

PASSWORD xdata __sys_passwd={0,_INIT_PIN,_SYS_PIN};
struct S_RIU  xdata  __riu;


struct MENU xdata  __menu_item;

PORT xdata  __port[_SCU_PORT_MAX_NUM];

U_INT_8 xdata __invalidate_day=0;

unsigned char xdata pre_status[_INPUT_PORT_NUM];

LINK_CTRL xdata __link_ctrl;

LIGHT_CTRL_STATUS xdata light_ctrl_status[LIGHT_CTRL_NUM];
HJ_SCENE_CTRL xdata hj_scene_ctrl;


#if 0
unsigned char code ctrl_data[]={0x10,
		       0x01,0x10,0x0c,0x0b,0x03,0x00,0x10,0x00,0x00,
		       0x01,0x01,0x0c,0x10,0x01,0x00,0x10,0x00,0x00,
		       0x01,0x02,0x0d,0x10,0x01,0x00,0x20,0x00,0x00,
		       0x01,0x03,0x0e,0x10,0x01,0x00,0x30,0x00,0x00,
};
#endif
      

#if 0
unsigned char code ir_data[]={0x55,0x02,      
		       0xb4,0xb0,0xc1,0xb1,0x00,0x00,0,0, 
		       22,2,	         
		       0xb5,0xe7,0xb5,0xc6,0xb5,0xc7,0,0, 
		       46,2,			
		       0xb4,0xb0,0x31,0x00,0,0,0,0, 
		       0x00,71,5,07,     
		       0xb4,0xb0,0x32,0x00,0,0,0,0, 
		       0x00,72,5,07,     
		       0xb5,0xe7,0x31,0x00,0,0,0,0, 
		       0x00,73,5,07,     
		       0xb5,0xe7,0x32,0x00,0,0,0,0, 
		       0x00,74,5,07,
		       0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09	     
};                 
#endif

void init_port(void)
{   
    unsigned char i,curr_ad,High_level,port_no=0;

    BUZZER_OFF;
    PHONE_CTRL_OFF;      //µç»°¹Ò»ú

    for(i=0; i<INPUT_PORT_PHY_NUM; i++){   //³õÊ¼»¯¶Ë¿Ú×´Ì¬ 
        XBYTE[AD_CHANNEL] = (~i) & 0x07;      //Ñ¡ÔñÏÂÒ»¸öÍ¨µÀ     
  
        if(__port[i].Usable == 0x01)    //¿ÉÓÃ
           High_level = __port[i].High_level;
        else
           High_level = 0x04;           //´óÓÚ4.6V          
 
        curr_ad = XBYTE[READ_AD]&0x0F;  //4Î»Îª±È½ÏÆ÷Êä³ö×´Ì¬
        if(curr_ad >= High_level)
           pre_status[i] = 1;           //¸ßµçÆ½
        else 
           pre_status[i] = 0;           //µÍµçÆ½
    }

    __security_class &= 0x8F;
    XBYTE[OUT_PORT] = 0;                //³õÊ¼»¯Êä³ö¶Ë¿Ú
    for(i=16;i<_SCU_PORT_MAX_NUM;i++){   //´¦ÀíÊä³ö¶Ë¿Ú
        if(__port[i].Device_Type>=0xA0){ //ÆÕÍ¨Êä³ö,»Ö¸´µôµçÇ°µÄ×´Ì¬
           if(__port[i].type.Out_Port.Normal_Status != __port[i].type.Out_Port.Curr_Status)
    	      output_ctrl(1,i);
        }
    }  

 	
#if 0
      memcpy((unsigned char *)&__link_ctrl, ctrl_data, sizeof(ctrl_data));
//    memcpy((unsigned char *)&__link_ctrl[1],"\x01\x02\x01\x11\x00\x01\x00\x00",8);
//    memcpy((unsigned char *)&__link_ctrl[2],"\x01\x03\x02\x12\x00\x01\x00\x00",8);
//    memcpy((unsigned char *)&__link_ctrl[3],"\x01\x02\x04\x13\x00\x01\x00\x3c",8);
//    memcpy((unsigned char *)&ir_ctrl,ir_data,sizeof(ir_data));
#endif
}                

unsigned int get_delay_time(void)
{
    unsigned int delay_time=0;
    unsigned char i;    
    for(i=0;i<INPUT_PORT_PHY_NUM;i++){  
        if(__port[i].Usable == 0x01){ 
           if(__port[i].Device_Type>=0x20 && __port[i].Device_Type<=0x5F){   //°²·ÀÌ½²âÆ÷ÊäÈë
              if(delay_time < __port[i].type.In_Port.Delay)
                 delay_time = __port[i].type.In_Port.Delay;
           }      
        }
    }
    return delay_time&0xFF;    
}        

//uchar delay 1:´«¸ÐÆ÷ÊÇÑÓ³Ù¿ØÖÆ,0:´«¸ÐÆ÷ÎÞÑÓ³Ù¿ØÖÆ
bit port_check(unsigned char defend_type,unsigned char *port_no) //¼ì²âÊäÈëÌ½²âÆ÷×´Ì¬£¬ÈçÃÅ´°Î´¹Ø±Õ²»ÄÜ²¼·À
{
    U_INT_8 i,tmp8;
    
    for(i=0;i<_INPUT_PORT_NUM;i++){  
        if(__bypass[i]) continue;    //±»ÅÔÂ·                        
        if(__port[i].Usable != 0x01 ) continue;
        if(__port[i].Device_Type>=0x20 && __port[i].Device_Type<=0x5F){   //°²·ÀÌ½²âÆ÷ÊäÈë
           if(__port[i].type.In_Port.Delay)   //ÓÐÑÓ³ÙÊ±¼äµÄ¶Ë¿ÚÔÝÊ±²»¼ì²â,ÈçÃÅ´Å
              continue;       //ÔÝÊ±²»¼ì²â¶Ë¿Ú           
  	   tmp8 = __port[i].type.In_Port.Open &  defend_type;  //´¦ÔÚµ±Ç°²¼·À×´Ì¬
    	   if(tmp8 && (pre_status[i] != __port[i].type.In_Port.Normal_Status)){  //µ±Ç°×´Ì¬²»Ò»ÖÂ
              *port_no = i;
       	      return 1;       //²»¿É²¼·À           
           }     
        }                 
    }
    return 0;   //¿ÉÒÔ²¼·À
} 

void send_defend_to_ccc(unsigned char defend_class)
{
    U_INT_8 xdata buf[40];        
    extern U_INT_8  xdata __bypass[];

    buf[APP_HEAD_LEN] = 0xA3;    
    buf[APP_HEAD_LEN+1] = 0x00;    
    buf[APP_HEAD_LEN+2] = defend_class;    

    memcpy(buf+APP_HEAD_LEN+3, __bypass, _INPUT_PORT_NUM);      
    make_send_package(0,0,QUERY_TYPE,buf,_INPUT_PORT_NUM+3);                
}

U_INT_8 xdata channal_switch;
void buzzer_on(U_INT_8 channal)
{
     channal_switch |= channal;
     if(channal_switch)
        //BUZZER_CTRL_PIN = BUZZER_ON;
        BUZZER_ON;
}

void buzzer_off(U_INT_8 channal)
{
    channal_switch &= (~channal);
    if(!channal_switch)
       //BUZZER_CTRL_PIN = 
       BUZZER_OFF;
}

void make_pack(unsigned char port_no,unsigned char port_type)
{
    U_INT_8 xdata buf[40];  //ÉÏ´«Ò»¸ö¶Ë¿ÚÊý¾Ý   
    unsigned char type,type_add;
 
    switch(port_type){
      case 0:            //¶Ë¿ÚÕý³£
           type = 0xAA;    
           type_add = 0x00;    
           break;
      case SHORT_FLAG:   //¶ÌÂ·
           type = 0xA5;    
           type_add = 0x02;    
           break;
      case OPEN_FLAG:    //¿ªÂ·
           type = 0xA5;    
           type_add = 0x03;    
           break;
      default:
           return;  
     }   
     buf[APP_HEAD_LEN] = type;    
     buf[APP_HEAD_LEN+1] = type_add;    
     buf[APP_HEAD_LEN+2] = port_no+1;    

     memcpy(buf+APP_HEAD_LEN+3, &__port[port_no], sizeof(PORT));      
     make_send_package(0,0,QUERY_TYPE,buf,APP_HEAD_LEN+3+sizeof(PORT));                
}

PORT_FIFO xdata port_fifo;
void ad_process(void)
{   
    unsigned int tmp;    
    unsigned char port_no=0,curr_ad;

    do{
       XBYTE[AD_CHANNEL] = (~port_no) & 0x07;      //Ñ¡ÔñÏÂÒ»¸öÍ¨µÀ     
       tmp=(port_fifo.head+1) & PORT_BUF_LEN;
       if(tmp !=  port_fifo.tail){          
          curr_ad = (XBYTE[READ_AD]<<4)|port_no;    //¸ß4Î»Îª±È½ÏÆ÷Êä³ö×´Ì¬£¬µÍËÄÎ»Îª¶Ë¿ÚºÅ 
          port_no++;

          port_fifo.dat[port_fifo.head] = curr_ad;
          port_fifo.head = tmp;
       }
       else{   //¶ÓÁÐÂú
	  break;
       }	 	  	
    }while(port_no < INPUT_PORT_PHY_NUM);  //Ò»´Î¶ÁÈë8¸ö¶Ë¿Ú
/*
    for(i=0;i<INPUT_PORT_PHY_NUM;i++){
//        port_no++;
        XBYTE[AD_CHANNEL] = (~i) & 0x07;      //Ñ¡ÔñÏÂÒ»¸öÍ¨µÀ     

//        XBYTE[AD_CHANNEL] = i;              //Ñ¡ÔñÏÂÒ»¸öÍ¨µÀ     
        tmp=(port_fifo.head+1) & PORT_BUF_LEN;
        if(tmp !=  port_fifo.tail){ 
          curr_ad = XBYTE[READ_AD]<<4;         
          curr_ad |= i;    //¸ß4Î»Îª±È½ÏÆ÷Êä³ö×´Ì¬£¬µÍËÄÎ»Îª¶Ë¿ÚºÅ 
//          port_no++;         
//          port_no++;
//          ws5_drive &= 0xF0;               
//          ws5_drive |= ((~port_no) & 0x0F);   
//          XBYTE[AD_CHANNEL] = port_no++;              //Ñ¡ÔñÏÂÒ»¸öÍ¨µÀ     

           port_fifo.dat[port_fifo.head] = curr_ad;
           port_fifo.head = tmp;
        }
        else{   //¶ÓÁÐÂú
  	   break;
        }	 	  	
    }//while(port_no < INPUT_PORT_PHY_NUM);  //Ò»´Î¶ÁÈë16¸ö¶Ë¿Ú
*/
}                

static unsigned char xdata send_err_delay;
AD_PORT xdata ad_port[_INPUT_PORT_NUM];             
void  input_ctrl(void)
{
    U_INT_8 i,Device_Type,Normal_Status,Curr_Status; 
    U_INT_8 open;
    unsigned char op_code;    
    bit port_status;
    PORT xdata *xp;    
    AD_PORT xdata *xpAD;
    unsigned char xdata buf[20];
    unsigned char curr_ad;
    unsigned char tmp_flag,pre_flag; 
    unsigned int tail;
    
    while(1){
       tail = port_fifo.tail;
       if(port_fifo.head != tail){
          port_fifo.tail = (tail+1) & PORT_BUF_LEN;
          curr_ad = port_fifo.dat[tail];
       }      
       else
          return;          //null 	

       i = curr_ad & 0x0F;     //¶Ë¿ÚºÅ
       xp =__port+i;
       if(xp->Usable != 0x01)              //¶Ë¿Ú²»¿ÉÓÃ
 	  continue;//return;
       xpAD = ad_port+i;
       pre_flag = xpAD->curr_flag;
       open=xp->Other_Flag;
       curr_ad >>= 4;
       if(curr_ad < xp->Low_level){
          if((open & SHORT_ALM_FLAG) && curr_ad==SHORT_LEVEL){  //¶ÌÂ·±¨¾¯
             tmp_flag=(pre_flag & STOP_SEND_SHORT)?STOP_SEND_SHORT|SHORT_FLAG:SHORT_FLAG;     
          }
          else
             tmp_flag = LOW_FLAG;
       }
       else{
          if(curr_ad >= xp->High_level){
             if((open & OPEN_ALM_FLAG) && (curr_ad>xp->High_level)){//=OPEN_LEVEL){ //¿ªÂ·±¨¾¯
                 tmp_flag=(pre_flag & STOP_SEND_OPEN)?STOP_SEND_OPEN|OPEN_FLAG:OPEN_FLAG;     
             }   
             else
                 tmp_flag=HIGH_FLAG;
          }
       } 

       if(tmp_flag == pre_flag){ //Ç°Ò»×´Ì¬ºÍµ±Ç°×´Ì¬Ò»ÖÂ                
           ++xpAD->curr_times;    //Òç³öÃ»ÓÐ´¦Àí
       } 
       else
          xpAD->curr_times=1;	 //×´Ì¬·­×ª
       xpAD->curr_flag = tmp_flag;
       if(xpAD->curr_times < xp->Sample_Times) 		
	  continue;//return;    //¼ÌÐø²ÉÑù	
       
       switch(tmp_flag){
           case LOW_FLAG:
                port_status = 0;
                break;
           case HIGH_FLAG:
                port_status = 1;
                break;
           case SHORT_FLAG:
           case OPEN_FLAG:
                if(send_err_delay++ == 0xFF)  //¹Ì¶¨ÑÓ³ÙÉÏ±¨
                   make_pack(i,tmp_flag);                        
           default:
                return;//continue;
        }          

        Device_Type = xp->Device_Type;
        if(Device_Type>=0x20 && Device_Type<=0x5F){   //°²·ÀÌ½²âÆ÷ÊäÈë                   
            if(__bypass[i]) continue;    //±»ÅÔÂ·                        
            open = xp->type->In_Port->Open &  __security_class;     //±¾PORTÊÇ·ñÔÚ²¼·ÀÀàÐÍ£¬°üÀ¨ÓÀ¾Ã²¼·À       
            op_code = xp->type->In_Port->Op_Code;
            Normal_Status = xp->type->In_Port->Normal_Status & 0x01;
            Curr_Status = pre_status[i];//__port[i].type.In_Port.Curr_Status & 0x01;            
            if(port_status ^ Normal_Status){           //¿Ú×´Ì¬±ä»¯  
               if(Normal_Status == Curr_Status){ 
                     xp->type->In_Port->Curr_Status= Curr_Status= Normal_Status ^ 0x01; //¶Ë¿Ú·ÇÕý³£                                                   
                     pre_status[i] =  Curr_Status;         
                     if(xp->Other_Flag & 0x01)
                        make_pack(i,0);               //ÉÏ´«µ½CCC
                                              
                     if(open){// && !__continue_ctrl.used){ //²»Âú×ãµ±Ç°²¼·À×´Ì¬             ){    //ÒÑ¾­±¨¾¯
#if SYS_LINK_CTRL==1
                         if(op_code & 0x04){   
                            if(proce_ctrl(i,port_status,1))       //Áª¶¯¿ØÖÆ,¿¼ÂÇÓÐÁ¬¶¯Ò²ÓÐË«·ÀÇøµÄÇé¿ö
                               continue;
                         }
#endif
                         if(xp->type->In_Port->Delay == 0){
                            __delay_ctrl.ctrl_time = 0;   //Í£Ö¹ÑÓÊ±£¬Á¢¼´±¨¾¯
                         }
                         else{          //Õý³£ÉèÖÃ
                            if(!delay_status_bit)
                                __delay_ctrl.ctrl_time = __port[i].type.In_Port.Delay;
                         }
                         __delay_ctrl.port = i;
                         __continue_ctrl.ctrl_time = __port[i].type.In_Port.Respond;//È¡³ÖÐøÊ±¼ä       
                         delay_status_bit= 1;
                     }    
                }
                continue;   
            }
            else{
                if(Curr_Status != Normal_Status){ 
                   xp->type->In_Port->Curr_Status = Curr_Status = Normal_Status; //¶Ë¿ÚÕý³£
                   pre_status[i] = Curr_Status;     
                   xp->type->In_Port->Prev_Status = Normal_Status;  //¶Ë¿ÚÕý³£
                   if(xp->Other_Flag & 0x01)
                      make_pack(i,0);      
#if SYS_LINK_CTRL==1
                   if(open && (op_code & 0x04)){   //ÓÐ²¼·À×´Ì¬£¬Áª¶¯ÓÐÐ§£         
                      proce_ctrl(i,port_status,0);       //Áª¶¯¿ØÖÆ
                   }
#endif
                }  
                continue;   
            }                       
        }
        //¼ÆÁ¿±íÀàÐÍ
	if(Device_Type >0  && Device_Type <= 0x1F){   //±íÀàÐÍÊäÈë
           Curr_Status = pre_status[i];
           if(port_status ^ pre_status[i]){      
              if(port_status){   //¸ßµçÆ½¼ÆÊý
                 xp->type->Meter->Curr_Data++;    //Âö³åÊýµÝ¼Ó   5.30	
	         if(++__count_pulse_meter[i] >= xp->type->Meter->Unitage){
                    __count_pulse_meter[i] = 0 ;   // Âö³å¼ÆÊýÆ÷	                       
                    if(xp->type->Meter->Unitage==0)    //Âö³åÊý²»ÄÜÎªÁã
                       xp->type->Meter->Unitage = 1;
                    make_pack(i,0);               //ÕûÊý±ä»¯Êý¾ÝÉÏ´« 						
                    eeprom_in_queue(SAVE_PORT);
	         }		       
	      }  
	      xp->type->Meter->Curr_Status = port_status;	             
              pre_status[i] = port_status;
           }
           continue;
        }
        //ÆÕÍ¨ÊäÈëÉè±¸ÉÏ±¨¶Ë¿ÚÐÅÏ¢
	if(Device_Type >=0x60 && Device_Type <= 0x7F){   //ÆÕÍ¨ÊäÈë
           if(port_status ^ pre_status[i]){  //¶Ë¿ÚÓÐ±ä»¯
              pre_status[i] = port_status; 
              xp->type->In_Port->Curr_Status = port_status;
              make_pack(i,0);   
#if SYS_LINK_CTRL==1             
              proce_ctrl(i,port_status,0); //Áª¶¯¿ØÖÆ
#endif
           }
           continue;
        }
        //´¦ÀíÌØÊâÀàÐÍµÄÉè±¸
	switch(Device_Type){
	   case  0xC0:		     //´¦ÀíÃÅËø              	
               Normal_Status = xp->type->In_Port->Normal_Status & 0x01;
               Curr_Status = pre_status[i]; //__port[i].type.In_Port.Curr_Status & 0x01;            
               if(port_status ^ Normal_Status){  //·´Ëø
                  if(Normal_Status == Curr_Status){  //Ëø´ò¿ª×´Ì¬
                     defend_save_and_light(1,0x8F,DEFEND_MODE_DOORLOCK);   //È«²¼
                     send_defend_to_ccc(0x8F);
                     xp->type->In_Port->Curr_Status = Normal_Status^0x01;
                     pre_status[i] = Normal_Status ^ 0x01;
                  } 
               }                   
               else{   //¿ªËø
                  if(Normal_Status != Curr_Status){
                     defend_save_and_light(1,0x80,DEFEND_MODE_DOORLOCK);    //È«³·
                     send_defend_to_ccc(0x80);
                     xp->type->In_Port->Curr_Status = Normal_Status;
                     pre_status[i] = Normal_Status;
                  }
               }     
	       break;
           case 0xC1:
           case 0xC2:
           case 0xC3:
               Normal_Status = xp->type->In_Port->Normal_Status; //´¥·¢×´Ì¬
	       if(port_status ^ pre_status[i]){
                  if((Normal_Status == 0x03) || (port_status==Normal_Status)){  //ÐÅºÅÎª´¥·¢×´Ì¬
  		     if(Device_Type == 0xC1){
                        defend_save_and_light(1,0x80,DEFEND_MODE_OTHER);        //È«³·
                        send_defend_to_ccc(0x80);
		     }
		     else{ 	
       		         if(Device_Type == 0xC2){	
                             defend_save_and_light(1,0x8F,DEFEND_MODE_OTHER);   //È«²¼
                             send_defend_to_ccc(0x8F);
 	                 } 		
			 else{     			 //Ïû¾¯
			     alm(0);
	                     buf[APP_HEAD_LEN] = 0xA4;                                            
                             buf[APP_HEAD_LEN+1] = 0x04;  //Ïû¾¯ÀàÐÍ                                          
                             make_send_package(0,0,ALM_TYPE,buf,2);	
		         } 	
		     } 
	          } 
		  pre_status[i] = port_status;
                  xp->type->In_Port->Curr_Status = port_status;
	       }
	       break;
            default:    //Éè±¸Ã»ÓÐ¶¨Òå
	       xp->Usable = 0x00; 		
         }  
     }            
}

//Éè±¸¿ØÖÆ,Ö÷ÒªÓÃÓÚÓÃ»§Éè¶¨¶¨Ê±Æ÷¿ØÖÆ¼Òµç
void  device_ctrl(void)
{
    U_INT_8 i;
    U_INT_16 now,Start_Time,End_Time,Curr_Status;
    U_INT_8  Week,Normal_Status;
    bit today_bit;
    PORT xdata *xp;    
    
    for(i=16;i<_SCU_PORT_MAX_NUM;i++){
        xp = __port+i;
        if(xp->Device_Type>=0xA0 && xp->Device_Type<=0xBF){   //ÆÕÍ¨Êä³öÉè±¸
            
            if(xp->type->Out_Port->Ctrl_Status & _CTRL_TYPE_DIRECT_ON){ //Ö±½Ó¿ØÖÆ´ò¿ªºó£¬¶¨Ê±Æ÷Ò»Ö±ÎÞÐ§
                continue;
            }
            if(xp->type->Out_Port->Ctrl_Status & _CTRL_TYPE_DIRECT_OFF ){ //Ö±½Ó¿ØÖÆ¹Ø±Õºó£¬´¦ÀíÇé¿ö
               if(__invalidate_day == __ymd.Date)   //Ç°´ÎÊÇÖ±½Ó¹Ø±Õ£¬µ±Ìì¶¨Ê±Æ÷ÎÞÐ§
                  continue;
               else{
                  xp->type->Out_Port->Ctrl_Status = _CTRL_TYPE_NOT;
                  __invalidate_day = 0;   //ÖØÖÃÎÞÐ§µÄÈÕÆÚ
               }   
            }

            now = __ymd.Hour;
            now = (now<<8) & 0xFF00;        
            now += __ymd.Minite;

            Normal_Status=xp->type->Out_Port->Normal_Status & 0x01;
            Start_Time=xp->type->Out_Port->Start_Time;
            End_Time=xp->type->Out_Port->End_Time;
            Week=xp->type->Out_Port->Week;    
            today_bit=(Week & (1<<__ymd.Week-1))?1:0 ;   //µ±ÌìÊÇ·ñ¿ÉÓÃ
            
            //¿ªÊ¼Ê±¼äµÈÓÚ½áÊøÊ±¼ä
            if(Start_Time == End_Time){       //µ±ÌìÎÞÐ§Ôò¹Ø
                stop_ctrl(Normal_Status, i, _CTRL_TYPE_NOT);//ÓëÁ¬¶¯¿ØÖÆÓÐÃ¬¶Ü£¿£¿
                continue;
            }
            Curr_Status=xp->type->Out_Port->Curr_Status;
            //¿ªÊ¼Ê±¼äÐ¡ÓÚ½áÊøÊ±¼ä
            if(Start_Time < End_Time){
                if((now >= Start_Time) && (now < End_Time)){   //Ò»ÌìÄÚ
                    if(today_bit && Normal_Status == Curr_Status) //µ±ÌìÓÐÐ§Ôò¿ª£¬µ±ÌìÎÞÐ§Ôò¹Ø
                        start_ctrl(Normal_Status, i, _CTRL_TYPE_TIMER);                           
                }   
                else
                    if(Normal_Status != Curr_Status)
                        stop_ctrl(Normal_Status, i ,_CTRL_TYPE_NOT);
            } 
            else{ //¿ªÊ¼Ê±¼ä´óÓÚ½áÊøÊ±¼ä
                if((now >= End_Time) && (now < Start_Time)){
                    if(Normal_Status != Curr_Status )
                        stop_ctrl(Normal_Status, i,_CTRL_TYPE_NOT);
                }
                else
                    if(today_bit && Normal_Status == Curr_Status)  
                        start_ctrl(Normal_Status, i, _CTRL_TYPE_TIMER);       
            }               
        }
    }   
}

void  delay_ctrl(void)
{
    U_INT_8 open,i;    
    static unsigned int xdata delay_times=0;

    if(delay_status_bit){   
          i = __delay_ctrl.port;
          open = __port[i].type.In_Port.Open &  __security_class;   //±¾PORTÊÇ·ñÔÚ²¼·ÀÀàÐÍ£¬°üÀ¨ÓÀ¾Ã²¼·À  
          if(open){
             if(__delay_ctrl.ctrl_time == 0){      //ÓÐÑÓ³Ù ÇÒ ÑÓ³Ùµ½0  
        	__alm.port = i;                    //±¨¾¯¶Ë¿Ú´Ó0ºÅ¿ªÊ¼ 
                __alm.riu_addr = 0;                
	        __alm.alm_device = __port[i].Device_Type; 
                alm(1);                            //±¨¾¯
                delay_status_bit = 0x00;
                continue_status_bit = 1;           //¿ªÊ¼³ÖÐøÊ±¼äÖÃÎ»
		delay_times = 0;
              }                  
              else{                                //ÓÐÑÓ³Ù±¨¾¯Ê±ÌáÐÑÓÃ»§
                 if(delay_times++ > 8000){         //ÌáÐÑ,¸æÖªÓÃ»§¼°Ê±³··À
                    delay_times = 0;		   //¼ä¸ôÊ±¼ä,1sÒ»´Î
 	            //ALM_LIGHT = ~ALM_LIGHT;        //±¨¾¯µÆÉÁ
                    ALM_LIGHT_ON;
		    buzzer_start(20,1);
                 } 
              }     
           }   
           else
               delay_status_bit = 0;		   //ÒÑ³··À  
     }   
}

//unsigned char data ws5_drive;
void start_ctrl(U_INT_8 Normal_Status,U_INT_8 port,U_INT_8 ctrl_type)
{
    unsigned char idata i,status;

    if(Normal_Status)   //Õý³£ÊÇ¸ßµçÆ½×´Ì¬
       status  = 0;  //µÍµçÆ½ÓÐÐ§
    else                //Õý³£µÍµçÆ½×´Ì¬    
       status  = 1;  //¸ßµçÆ½ÓÐÐ§

    i = (port-16+4) & 0x03; 
    switch(i)
    {
       case 0:
            XBYTE[OUT_PORT0] = status;
            break;
       case 1:
            XBYTE[OUT_PORT1] = status;
            break;
       case 2:
            XBYTE[OUT_PORT2] = status;
            break;
       case 3:
            XBYTE[OUT_PORT3] = status;
            break;
       default:
            return;
    }
    __port[port].type.Out_Port.Curr_Status=Normal_Status ^ 0x01; //¶Ë¿Ú·ÇÕý³£
    __port[port].type.Out_Port.Ctrl_Status |= ctrl_type;          
}

void stop_ctrl(U_INT_8 Normal_Status,U_INT_8 port,U_INT_8 ctrl_type)
{
    unsigned char idata i,status;

    if(Normal_Status)   //Õý³£ÊÇ¸ßµçÆ½×´Ì¬ 
       status = 1;  //¸ßµçÆ½ÓÐÐ§
    else               
       status = 0;  //µÍµçÆ½ÓÐÐ§
    i = (port-16+4) & 0x03;     
    switch(i)
    {
       case 0:
            XBYTE[OUT_PORT0] = status;
            break;
       case 1:
            XBYTE[OUT_PORT1] = status;
            break;
       case 2:
            XBYTE[OUT_PORT2] = status;
            break;
       case 3:
            XBYTE[OUT_PORT3] = status;
            break;
       default:
            return;
    }
    __port[port].type.Out_Port.Curr_Status = Normal_Status; //¶Ë¿ÚÕý³£
    __port[port].type.Out_Port.Ctrl_Status = ctrl_type;    
}

void output_ctrl(unsigned char status,unsigned char port)   //status=0x00 ¹Ø±ÕÊä³ö  0x01 ´ò¿ªÊä³ö
{
    U_INT_8  Normal_Status,Ctrl_Status;

    if(port>=16 && port<_SCU_PORT_MAX_NUM){   
       if(__port[port].Device_Type>=0x80  &&  __port[port].Device_Type<=0xBF){   //±¨¾¯Êä³ö+ÆÕÍ¨Êä³öÉè±¸
          Normal_Status=__port[port].type.Out_Port.Normal_Status & 0x01;        
          Ctrl_Status = __port[port].type.Out_Port.Ctrl_Status;
          if(status){            
             start_ctrl(Normal_Status,port,_CTRL_TYPE_DIRECT_ON);
          }
          else{
             if(Ctrl_Status & (_CTRL_TYPE_TIMER | _CTRL_TYPE_DIRECT_ON)){  //¶¨Ê±Æ÷¹¤×÷ 
                 stop_ctrl(Normal_Status, port, _CTRL_TYPE_DIRECT_OFF);  
                 __invalidate_day = __ymd.Date;    //µ±Ìì¶¨Ê±Æ÷ÎÞÐ§      
             }
             else
                stop_ctrl(Normal_Status, port,_CTRL_TYPE_NOT);  
          }
       }
       make_pack(port,0);      //¶Ë¿Ú±ä»¯ÉÏ´«
    }    
}

unsigned char idata warn_off;
LINK_DELAY xdata link_delay[LINK_CTRL_NUM];
#if SYS_LINK_CTRL==1
bit proce_ctrl(unsigned char port_no,unsigned char curr_status,unsigned char alm_type)
{
    unsigned char i;
    bit bit_out;
    unsigned char idata return_type=0;
    CTRL_ITEM xdata *xp;

      
    if(__link_ctrl.version != CTRL_VERSION) //°æ±¾¿ÉÓÃ
       return 0; 
    xp = __link_ctrl.item;    
    for(i=0;i<LINK_CTRL_NUM;i++){
        if(xp->used == 0x01){               //¿ÉÓÃ
           if(alm_type && xp->type==TWO_ARM){  //°²·À´«¸ÐÆ÷±»´¥·¢
              if(xp->in_port1==port_no){    //¶Ë¿Ú1´¥·¢
                 link_delay[i].end_status |=0x01;
                 if(xp->out_type->alm_op & 0x01)
                    warn_off = xp->delay_times;      
		 goto next;
              }   
              else{
                 if(xp->sec_port->in_port2==port_no){ //¶Ë¿Ú2´¥·¢
                    link_delay[i].end_status |=0x02;
                    if(xp->out_type->alm_op & 0x02)
                       warn_off = xp->delay_times;      
next:               link_delay[i].delay_time = xp->delay_times;
                    link_delay[i].port = 0x80; //Æô¶¯¶¨Ê±Æ÷
                    return_type = 1;           //Ë«·ÀÇøÀàÐÍ
                 }
              }
              if(link_delay[i].end_status==0x03 && link_delay[i].delay_time){  //Á½¸ö¶¼´¥·¢
                 memset(&link_delay[i],0x00,sizeof(LINK_DELAY));
                 warn_off= 0x00;
                 __alm.riu_addr = 0;                
            	 __alm.port = port_no;                    
	         __alm.alm_device = __port[port_no].Device_Type; 
		 alm(1);
              }    
	      xp++;
              continue;
           }             
                    
           if(xp->in_port1==port_no){
              bit_out = xp->out_type->out_status;
              switch(xp->type){
              case DELAY_CTRL:           //ÑÓÊ±                                          
                 if(xp->in_status==(curr_status)){
                    output_ctrl(bit_out,xp->sec_port->out_port);                           
                    link_delay[i].port = xp->sec_port->out_port;
                    link_delay[i].end_status = ~bit_out;  //Õý³£×´Ì¬µÄ·´
                    link_delay[i].delay_time = xp->delay_times;
                 }                                                       
                 break;
              case SYN_CTRL:                                    
                 if(xp->in_status != (curr_status))
                    bit_out = ~bit_out;    
                 output_ctrl(bit_out,xp->sec_port->out_port);                           
                 break;
              case MAN_CTRL:
                 if(xp->in_status==(curr_status))
                    output_ctrl(bit_out,xp->sec_port->out_port);                           
                 break;
              }  
          }      
          xp++;
      }                  
      else
          break;      //²»¿ÉÓÃ,ÍË³ö
   }
   return return_type;                  
}
#endif

//unsigned char code voice_sec[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x03,0x06};
//×¢Òâ¶ÎµÄ±ä»»
/*
void play_voice(unsigned char seg)
{
     unsigned int i;
     unsigned char xdata buf[5];
    
     buf[0] = MSG_HOST_ONAUDIO;  //ÃüÁî¼üÅÌ¿ªAUDIO
     ISendStrExt(buf,5);
     XBYTE[VOICE_SEG] = seg; //voice_sec[seg];
     for(i=0;i<0x2000;i++);
     XBYTE[VOICE_SEG] = 0;

}    
*/
/*
void stop_voice(void)
{
     unsigned int i;

     XBYTE[VOICE_IRP] = 1;       
     XBYTE[VOICE_SEG] = 0;       
     for(i=0;i<0x500;i++);
     XBYTE[VOICE_IRP] = 0;       
}
*/
void video_sw(bit flag)
{
     unsigned char xdata buf[40];


     buf[APP_HEAD_LEN]=0x11;
     buf[APP_HEAD_LEN+1]=0x01;
     if(flag){
       buf[APP_HEAD_LEN+2]=0x10;
     }
     else{
       buf[APP_HEAD_LEN+2]=0xFF;     //Ç¿ÖÆ¹Ø±Õ
     } 
     make_send_package(VIDEO_SW_ADDR,0,SENDER_IS_NDT|SEND_TO_RIU,buf,APP_HEAD_LEN+3);    //SHN->RIUX            
}
/*
void refuse_light_status(unsigned char light_no,bit status)
{
    extern  bit refuse_ctrl_bit;    
                
    if(light_no < LIGHT_CTRL_NUM){
       light_ctrl_status[light_no].status = status;
       if(refuse_ctrl_bit)
          key_in(MSG_REFUSE_LIGHT_CTRL);
    } 
}

void set_all_light_status(bit flag)
{
    unsigned char i;    

    for(i=0;i<LIGHT_CTRL_NUM;i++){
        light_ctrl_status[i].status = flag;     
    }    
}

void proce_light_refuse(unsigned char xdata *p)
{
    unsigned char i,j,tmp;
    bit status;
 
    for(i=0;i<(LIGHT_CTRL_NUM/4);i++){            
        tmp = *p++;
        for(j=1; j<5; j++){       //´Ó[1]¿ªÊ¼,[0]Ã»ÓÐÊ¹ÓÃ
            tmp >>= 1;            //Ö»ÅÐ¶Ï¡®¿ª¡®
            status = CY; 
            refuse_light_status(i*4+j,status);
            tmp >>= 1;                         
        }
    }
}
*/
