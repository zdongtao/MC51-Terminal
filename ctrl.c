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
U_INT_16 xdata  __count_pulse_meter[_INPUT_PORT_NUM];   //���������
U_INT_8  xdata  __bypass[_INPUT_PORT_NUM];              //��·��־
REPAIR_MENU xdata __repair_menu;    //ά�޲˵����

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
    PHONE_CTRL_OFF;      //�绰�һ�

    for(i=0; i<INPUT_PORT_PHY_NUM; i++){   //��ʼ���˿�״̬ 
        XBYTE[AD_CHANNEL] = (~i) & 0x07;      //ѡ����һ��ͨ��     
  
        if(__port[i].Usable == 0x01)    //����
           High_level = __port[i].High_level;
        else
           High_level = 0x04;           //����4.6V          
 
        curr_ad = XBYTE[READ_AD]&0x0F;  //4λΪ�Ƚ������״̬
        if(curr_ad >= High_level)
           pre_status[i] = 1;           //�ߵ�ƽ
        else 
           pre_status[i] = 0;           //�͵�ƽ
    }

    __security_class &= 0x8F;
    XBYTE[OUT_PORT] = 0;                //��ʼ������˿�
    for(i=16;i<_SCU_PORT_MAX_NUM;i++){   //��������˿�
        if(__port[i].Device_Type>=0xA0){ //��ͨ���,�ָ�����ǰ��״̬
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
           if(__port[i].Device_Type>=0x20 && __port[i].Device_Type<=0x5F){   //����̽��������
              if(delay_time < __port[i].type.In_Port.Delay)
                 delay_time = __port[i].type.In_Port.Delay;
           }      
        }
    }
    return delay_time&0xFF;    
}        

//uchar delay 1:���������ӳٿ���,0:���������ӳٿ���
bit port_check(unsigned char defend_type,unsigned char *port_no) //�������̽����״̬�����Ŵ�δ�رղ��ܲ���
{
    U_INT_8 i,tmp8;
    
    for(i=0;i<_INPUT_PORT_NUM;i++){  
        if(__bypass[i]) continue;    //����·                        
        if(__port[i].Usable != 0x01 ) continue;
        if(__port[i].Device_Type>=0x20 && __port[i].Device_Type<=0x5F){   //����̽��������
           if(__port[i].type.In_Port.Delay)   //���ӳ�ʱ��Ķ˿���ʱ�����,���Ŵ�
              continue;       //��ʱ�����˿�           
  	   tmp8 = __port[i].type.In_Port.Open &  defend_type;  //���ڵ�ǰ����״̬
    	   if(tmp8 && (pre_status[i] != __port[i].type.In_Port.Normal_Status)){  //��ǰ״̬��һ��
              *port_no = i;
       	      return 1;       //���ɲ���           
           }     
        }                 
    }
    return 0;   //���Բ���
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
    U_INT_8 xdata buf[40];  //�ϴ�һ���˿�����   
    unsigned char type,type_add;
 
    switch(port_type){
      case 0:            //�˿�����
           type = 0xAA;    
           type_add = 0x00;    
           break;
      case SHORT_FLAG:   //��·
           type = 0xA5;    
           type_add = 0x02;    
           break;
      case OPEN_FLAG:    //��·
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
       XBYTE[AD_CHANNEL] = (~port_no) & 0x07;      //ѡ����һ��ͨ��     
       tmp=(port_fifo.head+1) & PORT_BUF_LEN;
       if(tmp !=  port_fifo.tail){          
          curr_ad = (XBYTE[READ_AD]<<4)|port_no;    //��4λΪ�Ƚ������״̬������λΪ�˿ں� 
          port_no++;

          port_fifo.dat[port_fifo.head] = curr_ad;
          port_fifo.head = tmp;
       }
       else{   //������
	  break;
       }	 	  	
    }while(port_no < INPUT_PORT_PHY_NUM);  //һ�ζ���8���˿�
/*
    for(i=0;i<INPUT_PORT_PHY_NUM;i++){
//        port_no++;
        XBYTE[AD_CHANNEL] = (~i) & 0x07;      //ѡ����һ��ͨ��     

//        XBYTE[AD_CHANNEL] = i;              //ѡ����һ��ͨ��     
        tmp=(port_fifo.head+1) & PORT_BUF_LEN;
        if(tmp !=  port_fifo.tail){ 
          curr_ad = XBYTE[READ_AD]<<4;         
          curr_ad |= i;    //��4λΪ�Ƚ������״̬������λΪ�˿ں� 
//          port_no++;         
//          port_no++;
//          ws5_drive &= 0xF0;               
//          ws5_drive |= ((~port_no) & 0x0F);   
//          XBYTE[AD_CHANNEL] = port_no++;              //ѡ����һ��ͨ��     

           port_fifo.dat[port_fifo.head] = curr_ad;
           port_fifo.head = tmp;
        }
        else{   //������
  	   break;
        }	 	  	
    }//while(port_no < INPUT_PORT_PHY_NUM);  //һ�ζ���16���˿�
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

       i = curr_ad & 0x0F;     //�˿ں�
       xp =__port+i;
       if(xp->Usable != 0x01)              //�˿ڲ�����
 	  continue;//return;
       xpAD = ad_port+i;
       pre_flag = xpAD->curr_flag;
       open=xp->Other_Flag;
       curr_ad >>= 4;
       if(curr_ad < xp->Low_level){
          if((open & SHORT_ALM_FLAG) && curr_ad==SHORT_LEVEL){  //��·����
             tmp_flag=(pre_flag & STOP_SEND_SHORT)?STOP_SEND_SHORT|SHORT_FLAG:SHORT_FLAG;     
          }
          else
             tmp_flag = LOW_FLAG;
       }
       else{
          if(curr_ad >= xp->High_level){
             if((open & OPEN_ALM_FLAG) && (curr_ad>xp->High_level)){//=OPEN_LEVEL){ //��·����
                 tmp_flag=(pre_flag & STOP_SEND_OPEN)?STOP_SEND_OPEN|OPEN_FLAG:OPEN_FLAG;     
             }   
             else
                 tmp_flag=HIGH_FLAG;
          }
       } 

       if(tmp_flag == pre_flag){ //ǰһ״̬�͵�ǰ״̬һ��                
           ++xpAD->curr_times;    //���û�д���
       } 
       else
          xpAD->curr_times=1;	 //״̬��ת
       xpAD->curr_flag = tmp_flag;
       if(xpAD->curr_times < xp->Sample_Times) 		
	  continue;//return;    //��������	
       
       switch(tmp_flag){
           case LOW_FLAG:
                port_status = 0;
                break;
           case HIGH_FLAG:
                port_status = 1;
                break;
           case SHORT_FLAG:
           case OPEN_FLAG:
                if(send_err_delay++ == 0xFF)  //�̶��ӳ��ϱ�
                   make_pack(i,tmp_flag);                        
           default:
                return;//continue;
        }          

        Device_Type = xp->Device_Type;
        if(Device_Type>=0x20 && Device_Type<=0x5F){   //����̽��������                   
            if(__bypass[i]) continue;    //����·                        
            open = xp->type->In_Port->Open &  __security_class;     //��PORT�Ƿ��ڲ������ͣ��������ò���       
            op_code = xp->type->In_Port->Op_Code;
            Normal_Status = xp->type->In_Port->Normal_Status & 0x01;
            Curr_Status = pre_status[i];//__port[i].type.In_Port.Curr_Status & 0x01;            
            if(port_status ^ Normal_Status){           //��״̬�仯  
               if(Normal_Status == Curr_Status){ 
                     xp->type->In_Port->Curr_Status= Curr_Status= Normal_Status ^ 0x01; //�˿ڷ�����                                                   
                     pre_status[i] =  Curr_Status;         
                     if(xp->Other_Flag & 0x01)
                        make_pack(i,0);               //�ϴ���CCC
                                              
                     if(open){// && !__continue_ctrl.used){ //�����㵱ǰ����״̬             ){    //�Ѿ�����
#if SYS_LINK_CTRL==1
                         if(op_code & 0x04){   
                            if(proce_ctrl(i,port_status,1))       //��������,����������Ҳ��˫���������
                               continue;
                         }
#endif
                         if(xp->type->In_Port->Delay == 0){
                            __delay_ctrl.ctrl_time = 0;   //ֹͣ��ʱ����������
                         }
                         else{          //��������
                            if(!delay_status_bit)
                                __delay_ctrl.ctrl_time = __port[i].type.In_Port.Delay;
                         }
                         __delay_ctrl.port = i;
                         __continue_ctrl.ctrl_time = __port[i].type.In_Port.Respond;//ȡ����ʱ��       
                         delay_status_bit= 1;
                     }    
                }
                continue;   
            }
            else{
                if(Curr_Status != Normal_Status){ 
                   xp->type->In_Port->Curr_Status = Curr_Status = Normal_Status; //�˿�����
                   pre_status[i] = Curr_Status;     
                   xp->type->In_Port->Prev_Status = Normal_Status;  //�˿�����
                   if(xp->Other_Flag & 0x01)
                      make_pack(i,0);      
#if SYS_LINK_CTRL==1
                   if(open && (op_code & 0x04)){   //�в���״̬��������Ч�         
                      proce_ctrl(i,port_status,0);       //��������
                   }
#endif
                }  
                continue;   
            }                       
        }
        //����������
	if(Device_Type >0  && Device_Type <= 0x1F){   //����������
           Curr_Status = pre_status[i];
           if(port_status ^ pre_status[i]){      
              if(port_status){   //�ߵ�ƽ����
                 xp->type->Meter->Curr_Data++;    //�������ݼ�   5.30	
	         if(++__count_pulse_meter[i] >= xp->type->Meter->Unitage){
                    __count_pulse_meter[i] = 0 ;   // ���������	                       
                    if(xp->type->Meter->Unitage==0)    //����������Ϊ��
                       xp->type->Meter->Unitage = 1;
                    make_pack(i,0);               //�����仯�����ϴ� 						
                    eeprom_in_queue(SAVE_PORT);
	         }		       
	      }  
	      xp->type->Meter->Curr_Status = port_status;	             
              pre_status[i] = port_status;
           }
           continue;
        }
        //��ͨ�����豸�ϱ��˿���Ϣ
	if(Device_Type >=0x60 && Device_Type <= 0x7F){   //��ͨ����
           if(port_status ^ pre_status[i]){  //�˿��б仯
              pre_status[i] = port_status; 
              xp->type->In_Port->Curr_Status = port_status;
              make_pack(i,0);   
#if SYS_LINK_CTRL==1             
              proce_ctrl(i,port_status,0); //��������
#endif
           }
           continue;
        }
        //�����������͵��豸
	switch(Device_Type){
	   case  0xC0:		     //��������              	
               Normal_Status = xp->type->In_Port->Normal_Status & 0x01;
               Curr_Status = pre_status[i]; //__port[i].type.In_Port.Curr_Status & 0x01;            
               if(port_status ^ Normal_Status){  //����
                  if(Normal_Status == Curr_Status){  //����״̬
                     defend_save_and_light(1,0x8F,DEFEND_MODE_DOORLOCK);   //ȫ��
                     send_defend_to_ccc(0x8F);
                     xp->type->In_Port->Curr_Status = Normal_Status^0x01;
                     pre_status[i] = Normal_Status ^ 0x01;
                  } 
               }                   
               else{   //����
                  if(Normal_Status != Curr_Status){
                     defend_save_and_light(1,0x80,DEFEND_MODE_DOORLOCK);    //ȫ��
                     send_defend_to_ccc(0x80);
                     xp->type->In_Port->Curr_Status = Normal_Status;
                     pre_status[i] = Normal_Status;
                  }
               }     
	       break;
           case 0xC1:
           case 0xC2:
           case 0xC3:
               Normal_Status = xp->type->In_Port->Normal_Status; //����״̬
	       if(port_status ^ pre_status[i]){
                  if((Normal_Status == 0x03) || (port_status==Normal_Status)){  //�ź�Ϊ����״̬
  		     if(Device_Type == 0xC1){
                        defend_save_and_light(1,0x80,DEFEND_MODE_OTHER);        //ȫ��
                        send_defend_to_ccc(0x80);
		     }
		     else{ 	
       		         if(Device_Type == 0xC2){	
                             defend_save_and_light(1,0x8F,DEFEND_MODE_OTHER);   //ȫ��
                             send_defend_to_ccc(0x8F);
 	                 } 		
			 else{     			 //����
			     alm(0);
	                     buf[APP_HEAD_LEN] = 0xA4;                                            
                             buf[APP_HEAD_LEN+1] = 0x04;  //��������                                          
                             make_send_package(0,0,ALM_TYPE,buf,2);	
		         } 	
		     } 
	          } 
		  pre_status[i] = port_status;
                  xp->type->In_Port->Curr_Status = port_status;
	       }
	       break;
            default:    //�豸û�ж���
	       xp->Usable = 0x00; 		
         }  
     }            
}

//�豸����,��Ҫ�����û��趨��ʱ�����Ƽҵ�
void  device_ctrl(void)
{
    U_INT_8 i;
    U_INT_16 now,Start_Time,End_Time,Curr_Status;
    U_INT_8  Week,Normal_Status;
    bit today_bit;
    PORT xdata *xp;    
    
    for(i=16;i<_SCU_PORT_MAX_NUM;i++){
        xp = __port+i;
        if(xp->Device_Type>=0xA0 && xp->Device_Type<=0xBF){   //��ͨ����豸
            
            if(xp->type->Out_Port->Ctrl_Status & _CTRL_TYPE_DIRECT_ON){ //ֱ�ӿ��ƴ򿪺󣬶�ʱ��һֱ��Ч
                continue;
            }
            if(xp->type->Out_Port->Ctrl_Status & _CTRL_TYPE_DIRECT_OFF ){ //ֱ�ӿ��ƹرպ󣬴������
               if(__invalidate_day == __ymd.Date)   //ǰ����ֱ�ӹرգ����춨ʱ����Ч
                  continue;
               else{
                  xp->type->Out_Port->Ctrl_Status = _CTRL_TYPE_NOT;
                  __invalidate_day = 0;   //������Ч������
               }   
            }

            now = __ymd.Hour;
            now = (now<<8) & 0xFF00;        
            now += __ymd.Minite;

            Normal_Status=xp->type->Out_Port->Normal_Status & 0x01;
            Start_Time=xp->type->Out_Port->Start_Time;
            End_Time=xp->type->Out_Port->End_Time;
            Week=xp->type->Out_Port->Week;    
            today_bit=(Week & (1<<__ymd.Week-1))?1:0 ;   //�����Ƿ����
            
            //��ʼʱ����ڽ���ʱ��
            if(Start_Time == End_Time){       //������Ч���
                stop_ctrl(Normal_Status, i, _CTRL_TYPE_NOT);//������������ì�ܣ���
                continue;
            }
            Curr_Status=xp->type->Out_Port->Curr_Status;
            //��ʼʱ��С�ڽ���ʱ��
            if(Start_Time < End_Time){
                if((now >= Start_Time) && (now < End_Time)){   //һ����
                    if(today_bit && Normal_Status == Curr_Status) //������Ч�򿪣�������Ч���
                        start_ctrl(Normal_Status, i, _CTRL_TYPE_TIMER);                           
                }   
                else
                    if(Normal_Status != Curr_Status)
                        stop_ctrl(Normal_Status, i ,_CTRL_TYPE_NOT);
            } 
            else{ //��ʼʱ����ڽ���ʱ��
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
          open = __port[i].type.In_Port.Open &  __security_class;   //��PORT�Ƿ��ڲ������ͣ��������ò���  
          if(open){
             if(__delay_ctrl.ctrl_time == 0){      //���ӳ� �� �ӳٵ�0  
        	__alm.port = i;                    //�����˿ڴ�0�ſ�ʼ 
                __alm.riu_addr = 0;                
	        __alm.alm_device = __port[i].Device_Type; 
                alm(1);                            //����
                delay_status_bit = 0x00;
                continue_status_bit = 1;           //��ʼ����ʱ����λ
		delay_times = 0;
              }                  
              else{                                //���ӳٱ���ʱ�����û�
                 if(delay_times++ > 8000){         //����,��֪�û���ʱ����
                    delay_times = 0;		   //���ʱ��,1sһ��
 	            //ALM_LIGHT = ~ALM_LIGHT;        //��������
                    ALM_LIGHT_ON;
		    buzzer_start(20,1);
                 } 
              }     
           }   
           else
               delay_status_bit = 0;		   //�ѳ���  
     }   
}

//unsigned char data ws5_drive;
void start_ctrl(U_INT_8 Normal_Status,U_INT_8 port,U_INT_8 ctrl_type)
{
    unsigned char idata i,status;

    if(Normal_Status)   //�����Ǹߵ�ƽ״̬
       status  = 0;  //�͵�ƽ��Ч
    else                //�����͵�ƽ״̬    
       status  = 1;  //�ߵ�ƽ��Ч

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
    __port[port].type.Out_Port.Curr_Status=Normal_Status ^ 0x01; //�˿ڷ�����
    __port[port].type.Out_Port.Ctrl_Status |= ctrl_type;          
}

void stop_ctrl(U_INT_8 Normal_Status,U_INT_8 port,U_INT_8 ctrl_type)
{
    unsigned char idata i,status;

    if(Normal_Status)   //�����Ǹߵ�ƽ״̬ 
       status = 1;  //�ߵ�ƽ��Ч
    else               
       status = 0;  //�͵�ƽ��Ч
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
    __port[port].type.Out_Port.Curr_Status = Normal_Status; //�˿�����
    __port[port].type.Out_Port.Ctrl_Status = ctrl_type;    
}

void output_ctrl(unsigned char status,unsigned char port)   //status=0x00 �ر����  0x01 �����
{
    U_INT_8  Normal_Status,Ctrl_Status;

    if(port>=16 && port<_SCU_PORT_MAX_NUM){   
       if(__port[port].Device_Type>=0x80  &&  __port[port].Device_Type<=0xBF){   //�������+��ͨ����豸
          Normal_Status=__port[port].type.Out_Port.Normal_Status & 0x01;        
          Ctrl_Status = __port[port].type.Out_Port.Ctrl_Status;
          if(status){            
             start_ctrl(Normal_Status,port,_CTRL_TYPE_DIRECT_ON);
          }
          else{
             if(Ctrl_Status & (_CTRL_TYPE_TIMER | _CTRL_TYPE_DIRECT_ON)){  //��ʱ������ 
                 stop_ctrl(Normal_Status, port, _CTRL_TYPE_DIRECT_OFF);  
                 __invalidate_day = __ymd.Date;    //���춨ʱ����Ч      
             }
             else
                stop_ctrl(Normal_Status, port,_CTRL_TYPE_NOT);  
          }
       }
       make_pack(port,0);      //�˿ڱ仯�ϴ�
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

      
    if(__link_ctrl.version != CTRL_VERSION) //�汾����
       return 0; 
    xp = __link_ctrl.item;    
    for(i=0;i<LINK_CTRL_NUM;i++){
        if(xp->used == 0x01){               //����
           if(alm_type && xp->type==TWO_ARM){  //����������������
              if(xp->in_port1==port_no){    //�˿�1����
                 link_delay[i].end_status |=0x01;
                 if(xp->out_type->alm_op & 0x01)
                    warn_off = xp->delay_times;      
		 goto next;
              }   
              else{
                 if(xp->sec_port->in_port2==port_no){ //�˿�2����
                    link_delay[i].end_status |=0x02;
                    if(xp->out_type->alm_op & 0x02)
                       warn_off = xp->delay_times;      
next:               link_delay[i].delay_time = xp->delay_times;
                    link_delay[i].port = 0x80; //������ʱ��
                    return_type = 1;           //˫��������
                 }
              }
              if(link_delay[i].end_status==0x03 && link_delay[i].delay_time){  //����������
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
              case DELAY_CTRL:           //��ʱ                                          
                 if(xp->in_status==(curr_status)){
                    output_ctrl(bit_out,xp->sec_port->out_port);                           
                    link_delay[i].port = xp->sec_port->out_port;
                    link_delay[i].end_status = ~bit_out;  //����״̬�ķ�
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
          break;      //������,�˳�
   }
   return return_type;                  
}
#endif

//unsigned char code voice_sec[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x03,0x06};
//ע��εı任
/*
void play_voice(unsigned char seg)
{
     unsigned int i;
     unsigned char xdata buf[5];
    
     buf[0] = MSG_HOST_ONAUDIO;  //������̿�AUDIO
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
       buf[APP_HEAD_LEN+2]=0xFF;     //ǿ�ƹر�
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
        for(j=1; j<5; j++){       //��[1]��ʼ,[0]û��ʹ��
            tmp >>= 1;            //ֻ�жϡ�����
            status = CY; 
            refuse_light_status(i*4+j,status);
            tmp >>= 1;                         
        }
    }
}
*/
