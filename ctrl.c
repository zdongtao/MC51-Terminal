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
U_INT_16 xdata  __count_pulse_meter[_INPUT_PORT_NUM];   //脉冲计数数
U_INT_8  xdata  __bypass[_INPUT_PORT_NUM];              //旁路标志
REPAIR_MENU xdata __repair_menu;    //维修菜单项定义

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
    PHONE_CTRL_OFF;      //电话挂机

    for(i=0; i<INPUT_PORT_PHY_NUM; i++){   //初始化端口状态 
        XBYTE[AD_CHANNEL] = (~i) & 0x07;      //选择下一个通道     
  
        if(__port[i].Usable == 0x01)    //可用
           High_level = __port[i].High_level;
        else
           High_level = 0x04;           //大于4.6V          
 
        curr_ad = XBYTE[READ_AD]&0x0F;  //4位为比较器输出状态
        if(curr_ad >= High_level)
           pre_status[i] = 1;           //高电平
        else 
           pre_status[i] = 0;           //低电平
    }

    __security_class &= 0x8F;
    XBYTE[OUT_PORT] = 0;                //初始化输出端口
    for(i=16;i<_SCU_PORT_MAX_NUM;i++){   //处理输出端口
        if(__port[i].Device_Type>=0xA0){ //普通输出,恢复掉电前的状态
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
           if(__port[i].Device_Type>=0x20 && __port[i].Device_Type<=0x5F){   //安防探测器输入
              if(delay_time < __port[i].type.In_Port.Delay)
                 delay_time = __port[i].type.In_Port.Delay;
           }      
        }
    }
    return delay_time&0xFF;    
}        

//uchar delay 1:传感器是延迟控制,0:传感器无延迟控制
bit port_check(unsigned char defend_type,unsigned char *port_no) //检测输入探测器状态，如门窗未关闭不能布防
{
    U_INT_8 i,tmp8;
    
    for(i=0;i<_INPUT_PORT_NUM;i++){  
        if(__bypass[i]) continue;    //被旁路                        
        if(__port[i].Usable != 0x01 ) continue;
        if(__port[i].Device_Type>=0x20 && __port[i].Device_Type<=0x5F){   //安防探测器输入
           if(__port[i].type.In_Port.Delay)   //有延迟时间的端口暂时不检测,如门磁
              continue;       //暂时不检测端口           
  	   tmp8 = __port[i].type.In_Port.Open &  defend_type;  //处在当前布防状态
    	   if(tmp8 && (pre_status[i] != __port[i].type.In_Port.Normal_Status)){  //当前状态不一致
              *port_no = i;
       	      return 1;       //不可布防           
           }     
        }                 
    }
    return 0;   //可以布防
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
    U_INT_8 xdata buf[40];  //上传一个端口数据   
    unsigned char type,type_add;
 
    switch(port_type){
      case 0:            //端口正常
           type = 0xAA;    
           type_add = 0x00;    
           break;
      case SHORT_FLAG:   //短路
           type = 0xA5;    
           type_add = 0x02;    
           break;
      case OPEN_FLAG:    //开路
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
       XBYTE[AD_CHANNEL] = (~port_no) & 0x07;      //选择下一个通道     
       tmp=(port_fifo.head+1) & PORT_BUF_LEN;
       if(tmp !=  port_fifo.tail){          
          curr_ad = (XBYTE[READ_AD]<<4)|port_no;    //高4位为比较器输出状态，低四位为端口号 
          port_no++;

          port_fifo.dat[port_fifo.head] = curr_ad;
          port_fifo.head = tmp;
       }
       else{   //队列满
	  break;
       }	 	  	
    }while(port_no < INPUT_PORT_PHY_NUM);  //一次读入8个端口
/*
    for(i=0;i<INPUT_PORT_PHY_NUM;i++){
//        port_no++;
        XBYTE[AD_CHANNEL] = (~i) & 0x07;      //选择下一个通道     

//        XBYTE[AD_CHANNEL] = i;              //选择下一个通道     
        tmp=(port_fifo.head+1) & PORT_BUF_LEN;
        if(tmp !=  port_fifo.tail){ 
          curr_ad = XBYTE[READ_AD]<<4;         
          curr_ad |= i;    //高4位为比较器输出状态，低四位为端口号 
//          port_no++;         
//          port_no++;
//          ws5_drive &= 0xF0;               
//          ws5_drive |= ((~port_no) & 0x0F);   
//          XBYTE[AD_CHANNEL] = port_no++;              //选择下一个通道     

           port_fifo.dat[port_fifo.head] = curr_ad;
           port_fifo.head = tmp;
        }
        else{   //队列满
  	   break;
        }	 	  	
    }//while(port_no < INPUT_PORT_PHY_NUM);  //一次读入16个端口
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

       i = curr_ad & 0x0F;     //端口号
       xp =__port+i;
       if(xp->Usable != 0x01)              //端口不可用
 	  continue;//return;
       xpAD = ad_port+i;
       pre_flag = xpAD->curr_flag;
       open=xp->Other_Flag;
       curr_ad >>= 4;
       if(curr_ad < xp->Low_level){
          if((open & SHORT_ALM_FLAG) && curr_ad==SHORT_LEVEL){  //短路报警
             tmp_flag=(pre_flag & STOP_SEND_SHORT)?STOP_SEND_SHORT|SHORT_FLAG:SHORT_FLAG;     
          }
          else
             tmp_flag = LOW_FLAG;
       }
       else{
          if(curr_ad >= xp->High_level){
             if((open & OPEN_ALM_FLAG) && (curr_ad>xp->High_level)){//=OPEN_LEVEL){ //开路报警
                 tmp_flag=(pre_flag & STOP_SEND_OPEN)?STOP_SEND_OPEN|OPEN_FLAG:OPEN_FLAG;     
             }   
             else
                 tmp_flag=HIGH_FLAG;
          }
       } 

       if(tmp_flag == pre_flag){ //前一状态和当前状态一致                
           ++xpAD->curr_times;    //溢出没有处理
       } 
       else
          xpAD->curr_times=1;	 //状态翻转
       xpAD->curr_flag = tmp_flag;
       if(xpAD->curr_times < xp->Sample_Times) 		
	  continue;//return;    //继续采样	
       
       switch(tmp_flag){
           case LOW_FLAG:
                port_status = 0;
                break;
           case HIGH_FLAG:
                port_status = 1;
                break;
           case SHORT_FLAG:
           case OPEN_FLAG:
                if(send_err_delay++ == 0xFF)  //固定延迟上报
                   make_pack(i,tmp_flag);                        
           default:
                return;//continue;
        }          

        Device_Type = xp->Device_Type;
        if(Device_Type>=0x20 && Device_Type<=0x5F){   //安防探测器输入                   
            if(__bypass[i]) continue;    //被旁路                        
            open = xp->type->In_Port->Open &  __security_class;     //本PORT是否在布防类型，包括永久布防       
            op_code = xp->type->In_Port->Op_Code;
            Normal_Status = xp->type->In_Port->Normal_Status & 0x01;
            Curr_Status = pre_status[i];//__port[i].type.In_Port.Curr_Status & 0x01;            
            if(port_status ^ Normal_Status){           //口状态变化  
               if(Normal_Status == Curr_Status){ 
                     xp->type->In_Port->Curr_Status= Curr_Status= Normal_Status ^ 0x01; //端口非正常                                                   
                     pre_status[i] =  Curr_Status;         
                     if(xp->Other_Flag & 0x01)
                        make_pack(i,0);               //上传到CCC
                                              
                     if(open){// && !__continue_ctrl.used){ //不满足当前布防状态             ){    //已经报警
#if SYS_LINK_CTRL==1
                         if(op_code & 0x04){   
                            if(proce_ctrl(i,port_status,1))       //联动控制,考虑有连动也有双防区的情况
                               continue;
                         }
#endif
                         if(xp->type->In_Port->Delay == 0){
                            __delay_ctrl.ctrl_time = 0;   //停止延时，立即报警
                         }
                         else{          //正常设置
                            if(!delay_status_bit)
                                __delay_ctrl.ctrl_time = __port[i].type.In_Port.Delay;
                         }
                         __delay_ctrl.port = i;
                         __continue_ctrl.ctrl_time = __port[i].type.In_Port.Respond;//取持续时间       
                         delay_status_bit= 1;
                     }    
                }
                continue;   
            }
            else{
                if(Curr_Status != Normal_Status){ 
                   xp->type->In_Port->Curr_Status = Curr_Status = Normal_Status; //端口正常
                   pre_status[i] = Curr_Status;     
                   xp->type->In_Port->Prev_Status = Normal_Status;  //端口正常
                   if(xp->Other_Flag & 0x01)
                      make_pack(i,0);      
#if SYS_LINK_CTRL==1
                   if(open && (op_code & 0x04)){   //有布防状态，联动有效�         
                      proce_ctrl(i,port_status,0);       //联动控制
                   }
#endif
                }  
                continue;   
            }                       
        }
        //计量表类型
	if(Device_Type >0  && Device_Type <= 0x1F){   //表类型输入
           Curr_Status = pre_status[i];
           if(port_status ^ pre_status[i]){      
              if(port_status){   //高电平计数
                 xp->type->Meter->Curr_Data++;    //脉冲数递加   5.30	
	         if(++__count_pulse_meter[i] >= xp->type->Meter->Unitage){
                    __count_pulse_meter[i] = 0 ;   // 脉冲计数器	                       
                    if(xp->type->Meter->Unitage==0)    //脉冲数不能为零
                       xp->type->Meter->Unitage = 1;
                    make_pack(i,0);               //整数变化数据上传 						
                    eeprom_in_queue(SAVE_PORT);
	         }		       
	      }  
	      xp->type->Meter->Curr_Status = port_status;	             
              pre_status[i] = port_status;
           }
           continue;
        }
        //普通输入设备上报端口信息
	if(Device_Type >=0x60 && Device_Type <= 0x7F){   //普通输入
           if(port_status ^ pre_status[i]){  //端口有变化
              pre_status[i] = port_status; 
              xp->type->In_Port->Curr_Status = port_status;
              make_pack(i,0);   
#if SYS_LINK_CTRL==1             
              proce_ctrl(i,port_status,0); //联动控制
#endif
           }
           continue;
        }
        //处理特殊类型的设备
	switch(Device_Type){
	   case  0xC0:		     //处理门锁              	
               Normal_Status = xp->type->In_Port->Normal_Status & 0x01;
               Curr_Status = pre_status[i]; //__port[i].type.In_Port.Curr_Status & 0x01;            
               if(port_status ^ Normal_Status){  //反锁
                  if(Normal_Status == Curr_Status){  //锁打开状态
                     defend_save_and_light(1,0x8F,DEFEND_MODE_DOORLOCK);   //全布
                     send_defend_to_ccc(0x8F);
                     xp->type->In_Port->Curr_Status = Normal_Status^0x01;
                     pre_status[i] = Normal_Status ^ 0x01;
                  } 
               }                   
               else{   //开锁
                  if(Normal_Status != Curr_Status){
                     defend_save_and_light(1,0x80,DEFEND_MODE_DOORLOCK);    //全撤
                     send_defend_to_ccc(0x80);
                     xp->type->In_Port->Curr_Status = Normal_Status;
                     pre_status[i] = Normal_Status;
                  }
               }     
	       break;
           case 0xC1:
           case 0xC2:
           case 0xC3:
               Normal_Status = xp->type->In_Port->Normal_Status; //触发状态
	       if(port_status ^ pre_status[i]){
                  if((Normal_Status == 0x03) || (port_status==Normal_Status)){  //信号为触发状态
  		     if(Device_Type == 0xC1){
                        defend_save_and_light(1,0x80,DEFEND_MODE_OTHER);        //全撤
                        send_defend_to_ccc(0x80);
		     }
		     else{ 	
       		         if(Device_Type == 0xC2){	
                             defend_save_and_light(1,0x8F,DEFEND_MODE_OTHER);   //全布
                             send_defend_to_ccc(0x8F);
 	                 } 		
			 else{     			 //消警
			     alm(0);
	                     buf[APP_HEAD_LEN] = 0xA4;                                            
                             buf[APP_HEAD_LEN+1] = 0x04;  //消警类型                                          
                             make_send_package(0,0,ALM_TYPE,buf,2);	
		         } 	
		     } 
	          } 
		  pre_status[i] = port_status;
                  xp->type->In_Port->Curr_Status = port_status;
	       }
	       break;
            default:    //设备没有定义
	       xp->Usable = 0x00; 		
         }  
     }            
}

//设备控制,主要用于用户设定定时器控制家电
void  device_ctrl(void)
{
    U_INT_8 i;
    U_INT_16 now,Start_Time,End_Time,Curr_Status;
    U_INT_8  Week,Normal_Status;
    bit today_bit;
    PORT xdata *xp;    
    
    for(i=16;i<_SCU_PORT_MAX_NUM;i++){
        xp = __port+i;
        if(xp->Device_Type>=0xA0 && xp->Device_Type<=0xBF){   //普通输出设备
            
            if(xp->type->Out_Port->Ctrl_Status & _CTRL_TYPE_DIRECT_ON){ //直接控制打开后，定时器一直无效
                continue;
            }
            if(xp->type->Out_Port->Ctrl_Status & _CTRL_TYPE_DIRECT_OFF ){ //直接控制关闭后，处理情况
               if(__invalidate_day == __ymd.Date)   //前次是直接关闭，当天定时器无效
                  continue;
               else{
                  xp->type->Out_Port->Ctrl_Status = _CTRL_TYPE_NOT;
                  __invalidate_day = 0;   //重置无效的日期
               }   
            }

            now = __ymd.Hour;
            now = (now<<8) & 0xFF00;        
            now += __ymd.Minite;

            Normal_Status=xp->type->Out_Port->Normal_Status & 0x01;
            Start_Time=xp->type->Out_Port->Start_Time;
            End_Time=xp->type->Out_Port->End_Time;
            Week=xp->type->Out_Port->Week;    
            today_bit=(Week & (1<<__ymd.Week-1))?1:0 ;   //当天是否可用
            
            //开始时间等于结束时间
            if(Start_Time == End_Time){       //当天无效则关
                stop_ctrl(Normal_Status, i, _CTRL_TYPE_NOT);//与连动控制有矛盾？？
                continue;
            }
            Curr_Status=xp->type->Out_Port->Curr_Status;
            //开始时间小于结束时间
            if(Start_Time < End_Time){
                if((now >= Start_Time) && (now < End_Time)){   //一天内
                    if(today_bit && Normal_Status == Curr_Status) //当天有效则开，当天无效则关
                        start_ctrl(Normal_Status, i, _CTRL_TYPE_TIMER);                           
                }   
                else
                    if(Normal_Status != Curr_Status)
                        stop_ctrl(Normal_Status, i ,_CTRL_TYPE_NOT);
            } 
            else{ //开始时间大于结束时间
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
          open = __port[i].type.In_Port.Open &  __security_class;   //本PORT是否在布防类型，包括永久布防  
          if(open){
             if(__delay_ctrl.ctrl_time == 0){      //有延迟 且 延迟到0  
        	__alm.port = i;                    //报警端口从0号开始 
                __alm.riu_addr = 0;                
	        __alm.alm_device = __port[i].Device_Type; 
                alm(1);                            //报警
                delay_status_bit = 0x00;
                continue_status_bit = 1;           //开始持续时间置位
		delay_times = 0;
              }                  
              else{                                //有延迟报警时提醒用户
                 if(delay_times++ > 8000){         //提醒,告知用户及时撤防
                    delay_times = 0;		   //间隔时间,1s一次
 	            //ALM_LIGHT = ~ALM_LIGHT;        //报警灯闪
                    ALM_LIGHT_ON;
		    buzzer_start(20,1);
                 } 
              }     
           }   
           else
               delay_status_bit = 0;		   //已撤防  
     }   
}

//unsigned char data ws5_drive;
void start_ctrl(U_INT_8 Normal_Status,U_INT_8 port,U_INT_8 ctrl_type)
{
    unsigned char idata i,status;

    if(Normal_Status)   //正常是高电平状态
       status  = 0;  //低电平有效
    else                //正常低电平状态    
       status  = 1;  //高电平有效

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
    __port[port].type.Out_Port.Curr_Status=Normal_Status ^ 0x01; //端口非正常
    __port[port].type.Out_Port.Ctrl_Status |= ctrl_type;          
}

void stop_ctrl(U_INT_8 Normal_Status,U_INT_8 port,U_INT_8 ctrl_type)
{
    unsigned char idata i,status;

    if(Normal_Status)   //正常是高电平状态 
       status = 1;  //高电平有效
    else               
       status = 0;  //低电平有效
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
    __port[port].type.Out_Port.Curr_Status = Normal_Status; //端口正常
    __port[port].type.Out_Port.Ctrl_Status = ctrl_type;    
}

void output_ctrl(unsigned char status,unsigned char port)   //status=0x00 关闭输出  0x01 打开输出
{
    U_INT_8  Normal_Status,Ctrl_Status;

    if(port>=16 && port<_SCU_PORT_MAX_NUM){   
       if(__port[port].Device_Type>=0x80  &&  __port[port].Device_Type<=0xBF){   //报警输出+普通输出设备
          Normal_Status=__port[port].type.Out_Port.Normal_Status & 0x01;        
          Ctrl_Status = __port[port].type.Out_Port.Ctrl_Status;
          if(status){            
             start_ctrl(Normal_Status,port,_CTRL_TYPE_DIRECT_ON);
          }
          else{
             if(Ctrl_Status & (_CTRL_TYPE_TIMER | _CTRL_TYPE_DIRECT_ON)){  //定时器工作 
                 stop_ctrl(Normal_Status, port, _CTRL_TYPE_DIRECT_OFF);  
                 __invalidate_day = __ymd.Date;    //当天定时器无效      
             }
             else
                stop_ctrl(Normal_Status, port,_CTRL_TYPE_NOT);  
          }
       }
       make_pack(port,0);      //端口变化上传
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

      
    if(__link_ctrl.version != CTRL_VERSION) //版本可用
       return 0; 
    xp = __link_ctrl.item;    
    for(i=0;i<LINK_CTRL_NUM;i++){
        if(xp->used == 0x01){               //可用
           if(alm_type && xp->type==TWO_ARM){  //安防传感器被触发
              if(xp->in_port1==port_no){    //端口1触发
                 link_delay[i].end_status |=0x01;
                 if(xp->out_type->alm_op & 0x01)
                    warn_off = xp->delay_times;      
		 goto next;
              }   
              else{
                 if(xp->sec_port->in_port2==port_no){ //端口2触发
                    link_delay[i].end_status |=0x02;
                    if(xp->out_type->alm_op & 0x02)
                       warn_off = xp->delay_times;      
next:               link_delay[i].delay_time = xp->delay_times;
                    link_delay[i].port = 0x80; //启动定时器
                    return_type = 1;           //双防区类型
                 }
              }
              if(link_delay[i].end_status==0x03 && link_delay[i].delay_time){  //两个都触发
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
              case DELAY_CTRL:           //延时                                          
                 if(xp->in_status==(curr_status)){
                    output_ctrl(bit_out,xp->sec_port->out_port);                           
                    link_delay[i].port = xp->sec_port->out_port;
                    link_delay[i].end_status = ~bit_out;  //正常状态的反
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
          break;      //不可用,退出
   }
   return return_type;                  
}
#endif

//unsigned char code voice_sec[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x03,0x06};
//注意段的变换
/*
void play_voice(unsigned char seg)
{
     unsigned int i;
     unsigned char xdata buf[5];
    
     buf[0] = MSG_HOST_ONAUDIO;  //命令键盘开AUDIO
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
       buf[APP_HEAD_LEN+2]=0xFF;     //强制关闭
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
        for(j=1; j<5; j++){       //从[1]开始,[0]没有使用
            tmp >>= 1;            //只判断‘开‘
            status = CY; 
            refuse_light_status(i*4+j,status);
            tmp >>= 1;                         
        }
    }
}
*/
