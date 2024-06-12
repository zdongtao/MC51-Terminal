/**************************************************************************/
/*                 应用层协议分析程序—APP.C                              */
/*              					                  */
/* 功能：分析应用层协议         	 		                  */
/*                                                                        */ 
/* 日期: 2005/01/01				                          */
/* 作者: 董涛                                                             */
/*                                                                        */
/* (c) Copyright reserved by HJDZ, 2004.                                  */
/*------------------------------------------------------------------------*/
/* 修改记录:		                                                  */
/*    							                  */   
/**************************************************************************/
#include <reg51.h>
#include <string.h>
#include <intrins.h>
#include <absacc.h>
#include "inc/types.h"
#include "inc/timer.h"
#include "inc/ctrl.h"
#include "inc/alarm.h"
#include "inc/tool.h"
#include "inc/app.h"
#include "inc/asyn.h"
#include "inc/system.h"
#include "inc/udp.h"
#include "inc/menu.h"
#include "inc/arp.h"
#include "inc/ip.h"
#include "inc/timer.h"
#include "inc/mac.h"
#include "inc/hdres.h"
#include "inc/lcd.h"
#include "inc/skbuff.h"
#include "inc/key.h"
#include "inc/vi2c.h"

unsigned char xdata ip_status[4]={1,1,1,1};

U_INT_8 xdata __security_class=0x80;   //必须定义在XDATA中，保存时指针类型统一
static bit __ndt_reset;
static bit update_locate_ip;
static NETIF xdata tmp_netif;
static unsigned char xdata tmp_send_buf[ASYN_SEND_MAX_BUF];
/**************************************************************************/
/*                 asyn_process_received 处理异步通讯报文                   */
/* 参数：无                                                               */   
/* 返回：无                                                               */     
/* 日期：2001/01/11                                                       */      
/*------------------------------------------------------------------------*/
/* 修改记录:		                                                  */
/*   董涛，2001/08/02对报文A5的处理作了修改                                 */      
/*   董涛，2001/09/11增加保存系统IP地址   		                    */   
/**************************************************************************/
void app_process_received(unsigned char xdata *xp)
{
        U_INT_8  data tmp8,type_add;
        APP_HEAD  xdata *pApp_head;
        unsigned int idata len;
        extern bit asyn_send_bit;        

        extern bit  __return_ok;
        extern unsigned char xdata __return_status;                

        pApp_head = (APP_HEAD *)xp;        
        if(pApp_head->version != SCU_PROTO_VERSION)      //协议版本不兼容
           return;                            
        len = pApp_head->length;
        if(rec_check_xor(len, xp))     
           return;                                 //校验错
        tmp8 = pApp_head->forward_flag;
        if(tmp8 & SEND_TO_RIU){   //to riu
	   if(asyn_send_bit==0){  //发送空闲 
              call_checksum(len, xp);
	      asyn.send_len = len+2;
              memcpy(asyn.send_buf, xp, len+2);
              asyn_send = 1;   
           }     
//	   return;
        }
        if(tmp8 & SEND_TO_SCU){   //到SCU
           tmp_send_buf[APP_HEAD_LEN+1] = 0x00;         //从类型号
           type_add = pApp_head->type_add;
           switch(pApp_head->type) {                
                case 0x00:        // 0x00 -- NDT复位
                     if(pApp_head->buf[0] & 0x02){      //SCU RESET                           
                        __ndt_reset = 1;                        
                        eeprom_in_queue(SAVE_PORT);              //保存端口数据
                        tm[NDT_RESET_WAIT] = 3;
                        tmp_send_buf[APP_HEAD_LEN] = 0x80;                        
                        make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+2);  //当0x80送到主机后NDT才复位                                       
                     }   
                     break;
                case 0x01:
                     if(type_add==0x01){  //修改系统IP地址
                        memcpy((unsigned char *)&tmp_netif, pApp_head->buf, sizeof(NETIF));  //刷新全部内容
                        update_locate_ip = 1;
                        tm[NDT_RESET_WAIT] = 3;
                        tmp_send_buf[APP_HEAD_LEN] = 0x81;                        
                        tmp_send_buf[APP_HEAD_LEN+1] = 0x01;  //从类型号
                        make_send_package(0, 0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+2);  
                        break;                        
                     }                              
                case 0x26:
                     if(type_add==0x01){
                        memcpy(&__ymd, pApp_head->buf, sizeof(YMD));
                        request_time_bit=0;   
		     }	
                     else
			__return_ok = 1;                                                        
                     break;
/*                case 0x02:                     //RIU 配置,暂时不用
                     if(type_add==0){          //从类型号不为0 
                        tmp_send_buf[APP_HEAD_LEN] = 0x82;
                        tmp_send_buf[APP_HEAD_LEN+2] = 0x0;
                        tmp8 = pApp_head->buf[0];
                     }   
		     if(tmp8 <= _RIU_NUM){	
                        tmp8 <<= 2;   //tmp*4
                        memcpy(&__riu, pApp_head->buf, tmp8+1);		              
		        make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+3);
                        eeprom_in_queue(SAVE_RIU);      //应保存在EEPROM中
                     }  
                     break;                    
*/
                case 0x03:  //NDT菜单配置
                     if(type_add==0x01){
                        len = pApp_head->length-APP_HEAD_LEN-APP_TYPE_LEN;
                        memcpy((unsigned char *)&__menu_item, pApp_head->buf, len);
                        tmp_send_buf[APP_HEAD_LEN] = 0x83;                        
                        tmp_send_buf[APP_HEAD_LEN+1] = 0x01;		              
                        tmp_send_buf[APP_HEAD_LEN+2] = 0x00;		              
                        make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+3);						 		                   
                        eeprom_in_queue(SAVE_MENU_ITEM);     //保存标志
                     }   
                     break;                                              
                case 0x04: //NDT密码修改
                     if(pApp_head->buf[0]==0){    //  用户密码    
                        memcpy(&__sys_passwd,pApp_head->buf+1,9);                  
                        tmp_send_buf[APP_HEAD_LEN+2] = 0x00;		              		           
                     }   
                     else{      // 系统密码                         
                        memcpy(__sys_passwd.Syspasswd,pApp_head->buf+2,_SYS_PASSWD_LEN);
                        tmp_send_buf[APP_HEAD_LEN+2] = 0x01;		              		           
                     } 
                     tmp_send_buf[APP_HEAD_LEN] = 0x84;                      
    	             make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+3);						 		                   
                     eeprom_in_queue(SAVE_SYS_PASSWD);
                     break;
                case 0x05:   //配置全部端口
                     xp = pApp_head->buf;
                     for(type_add=0;type_add<_SCU_PORT_MAX_NUM;type_add++){                         
                         tmp8 = *(xp+1)<=0x1F?7:0;  //表数据读数不覆盖
                         memcpy(&__port[type_add], xp, sizeof(PORT)-tmp8); 
                         xp += sizeof(PORT);
                     }
                     tmp_send_buf[APP_HEAD_LEN] = 0x85;                                            
                     tmp_send_buf[APP_HEAD_LEN+2] = 0x00;		              		           
                     make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+3);						 		                                            
                     eeprom_in_queue(SAVE_PORT);
                     break;     
                case 0x06:  //读取全部端口
                     tmp_send_buf[APP_HEAD_LEN] = 0x86;                                            
                     memcpy(tmp_send_buf+APP_HEAD_LEN+APP_TYPE_LEN, &__port, sizeof(__port));
                     make_send_package(0, 0,NORMAL_TYPE, tmp_send_buf, APP_HEAD_LEN+APP_TYPE_LEN+sizeof(__port));
                     break;     
#if 0
                case 0x07:  //配置一个端口
                     if(type_add==0){ 
                        tmp8 = pApp_head->buf[0]-1;     //端口号 
                        if(tmp8 < _SCU_PORT_MAX_NUM){	//1--20
                           memcpy(&__port[tmp8], pApp_head->buf+1, sizeof(PORT));                           
                           tmp_send_buf[APP_HEAD_LEN] = 0x87;                                            
                           tmp_send_buf[APP_HEAD_LEN+2] = tmp8+1;
                           tmp_send_buf[APP_HEAD_LEN+3] = 0x00;		              		           
                           make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+4);						 		                                                                    
                           eeprom_in_queue(SAVE_PORT);  //保存端口数据
                        }   
                     }   
                     break;
                case 0x08:  //读取一个端口
                     tmp8 = pApp_head->buf[0]-1;        //端口号                
                     if(tmp8 < _SCU_PORT_MAX_NUM){	
                        tmp_send_buf[APP_HEAD_LEN] = 0x88;                                            
                        tmp_send_buf[APP_HEAD_LEN+2] = tmp8+1;		              		           
                        memcpy(tmp_send_buf+APP_HEAD_LEN+3, &__port[tmp8], sizeof(PORT));
//                        memcpy(tmp_send_buf+APP_HEAD_LEN+3, &light_ctrl, sizeof(PORT));
                        make_send_package(0,0,NORMAL_TYPE, tmp_send_buf,APP_HEAD_LEN+ 3+sizeof(PORT));						 		                                            
                     }   
                     break;
#endif
                case 0x09:  //布防,只能全布防
                     switch(type_add){
                         case 0x00:    //从类型号不为0 
		             tmp8 = pApp_head->buf[0];
  		             defend_save_and_light(1,tmp8,DEFEND_MODE_CENTER);
//  		             defend_save_and_light(1,0x8F,DEFEND_MODE_CENTER);
                             tmp_send_buf[APP_HEAD_LEN] = 0x89;                                            
                             tmp_send_buf[APP_HEAD_LEN+2] = 0;		              		           
                             make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+3);						 		                                            
                             break;
                         case 0x01:   //遥控布防    
  		             defend_save_and_light(1,0x8F,DEFEND_MODE_REMOTE_CTRL);
		             send_defend_to_ccc(0x8F);
//                             memcpy(tmp_send_buf+APP_HEAD_LEN,"\x16\x02\xff\x00\x30",5);
//                             goto SEND_DEFEND_STATUS;
                             break;
                         case 0x02:   //遥控撤防
  		             defend_save_and_light(1,0x80,DEFEND_MODE_REMOTE_CTRL);
		             send_defend_to_ccc(0x80);
//                             memcpy(tmp_send_buf+APP_HEAD_LEN,"\x16\x02\xff\x00\x31",5);
//SEND_DEFEND_STATUS:
//                             make_send_package(0xFF,0,QUERY_TYPE,tmp_send_buf,APP_HEAD_LEN+5);   //发送一个刷新命令             
                             break;
                     }
                     break;  
                case 0x0A:  //CCC取消报警，遥控消警
                     alm(0);    
                     tmp_send_buf[APP_HEAD_LEN] = 0x8A;   //响应CCC                                         
                     tmp_send_buf[APP_HEAD_LEN+2] = 0;		              		           
                     make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+3);						 		                                            
                     break;                             
                case 0x0B:    
                     if(type_add==0x01){     //遥控报警
                        __alm.port = _REMOTE_CTROL_ALM;
                        __alm.riu_addr = pApp_head->RIU_Addr;
                        buzzer_on(_BIT_ALM);
                     }
                     else{                   //广播报警
                        __alm.port = _BOARDCASE_ALM_PORT;
                        __alm.riu_addr = 0;
                     }  
		     ALM_LIGHT_ON;
                     alm(1);
                     tmp_send_buf[APP_HEAD_LEN] = 0x8B;
                     make_send_package(__alm.riu_addr,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+2); //响应发送者
                     break;  
                case 0x0C:  //检测SCU
/*
                     tmp_send_buf[APP_HEAD_LEN] = 0x8C;
                     if(type_add==0x01){  
                        tmp8 = pApp_head->buf[0];       //组号
                        if(__netif.ni_group == tmp8){
  			   ip_status[0] = pApp_head->buf[1]; //主机查询机状态   
                           tm[MOTI_INFO_TIMER] = 0xFF;                        
                           tmp_send_buf[APP_HEAD_LEN+1] = 0x01;
                           make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+2);						 		                                            
			}
                     }
		     else{			   
*/
                        if(type_add==0x00){  
       			   ip_status[2] = pApp_head->buf[1];  //报警监控主机状态
                           tmp_send_buf[APP_HEAD_LEN+1] = 0x00;
                           tmp_send_buf[APP_HEAD_LEN+2] = 0x01;
                           tmp_send_buf[APP_HEAD_LEN+3] = SCU_SOFTWARE_VERSION;                                                 
                           make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+4);						 		                                            
                        } 
//		     }		  
                     break;  
                case 0x0D:           //消息通知
                     if(type_add==0){      //从类型号不为0 
                        len  = *(unsigned int *)(pApp_head->buf+1);  //消息号
                        tmp8=pApp_head->buf[0];   //type
                        message_in(tmp8,len);
                        if(pApp_head->buf[0] == 0){  //指定消息才应答
                           tmp_send_buf[APP_HEAD_LEN] = 0x8D;                                            
                           make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+2);						 		                                            
                        }
                     }   
                     break;
                case 0x0E:  //输出端口控制
                     tmp8 = pApp_head->buf[0];          // 端口号                      
		     if(tmp8>16 && tmp8<21){	
                        tmp_send_buf[APP_HEAD_LEN] = 0x8E;                                            
                        tmp_send_buf[APP_HEAD_LEN+2] = tmp8;                                                                 
                        make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+3);						 		                                            
                        output_ctrl(pApp_head->buf[1],tmp8-1);
		     }
                     break;
                case 0x0F:  //布防状态查询
                     if(type_add==0){      //从类型号不为0 
                        tmp_send_buf[APP_HEAD_LEN] = 0x8F;                                      
                        tmp_send_buf[APP_HEAD_LEN+2] = __security_class;      
                        make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+3);						 		                                            
                     }   
                     break;
/*空间不够
                case 0x10: //控制方法下传
                     if(type_add==0){      //从类型号不为0 
                        tmp_send_buf[APP_HEAD_LEN] = 0x90;
                        memcpy(&__link_ctrl,pApp_head->buf,sizeof(__link_ctrl));
                        make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+2);						 		                                            
                        eeprom_in_queue(SAVE_NDT_CTRL);
                     }   
                     break;   
*/
/*空间不够
                case 0x14: //读RIU状态表
                     if(type_add==0){      //从类型号不为0 
                        tmp_send_buf[APP_HEAD_LEN] = 0x94;                                      
                        memcpy(tmp_send_buf+APP_HEAD_LEN+2,riu_status,_RIU_NUM);
                        make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+2+_RIU_NUM);						 		                                            
                     }
  		     else{   
                        if(type_add==0x02){    //设置显示常亮，生产测试用
                    	   //cs6_b2_CRTdisplay = CRTDISPLAY_ON;
                           other_item.on_display=0x01;
                        }
                     }
                     break;     
*/                      
#if 0
                case 0x15:    //读系统时间
                     tmp_send_buf[APP_HEAD_LEN] = 0x95;                                      
                     memcpy(tmp_send_buf+APP_HEAD_LEN+2, &__ymd, sizeof(YMD));
                     make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+2+sizeof(YMD));						 		                                            
                     break;                           
#endif
                case 0x17:    //下传房间套型图
                     tmp_send_buf[APP_HEAD_LEN] = 0x97;                                      
    		     len-=(APP_HEAD_LEN+APP_TYPE_LEN);
		     if(type_add==00){   //house pic
			if(len<sizeof(house_pic)){
                           memcpy(&house_pic, pApp_head->buf, len);                           
                           make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+2);						 		                                            
                           eeprom_in_queue(SAVE_HOUSE_PIC);
                        }
                        break;                           
                     } 
#if 0
		     if(type_add==0x01){   //ir ctrl
			if(len<sizeof(ir_ctrl)){
                           memcpy(&ir_ctrl, pApp_head->buf, len);                           
   			   tmp_send_buf[APP_HEAD_LEN+1] = 0x01;                                      
                           make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+2);						 		                                            
                           eeprom_in_queue(SAVE_IR_CTRL);
                        }
                     } 	
#endif		            						
		     if(type_add==0x03){   //light ctrl
			if(len < sizeof(light_ctrl)){
                           memcpy(&light_ctrl, pApp_head->buf, len);                           
   			   tmp_send_buf[APP_HEAD_LEN+1] = 0x03;                                      
                           make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+2);						 		                                            
                           eeprom_in_queue(SAVE_LIGHT_CTRL);
                        }
                     } 		
#if 1
		     if(type_add==0x04){   //light ctrl，下一个1K数据
			if(len < sizeof(light_ctrl)/2){
                           xp =(unsigned char *)&light_ctrl+1024;  
                           memcpy(xp, pApp_head->buf, len);                           
   			   tmp_send_buf[APP_HEAD_LEN+1] = 0x04;                                      
                           make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+2);						 		                                            
                           eeprom_in_queue(SAVE_LIGHT_CTRL);
                        }
                     }
#endif
		     if(type_add==0x05){   //场景控制
			if(len <= sizeof(hj_scene_ctrl)){
                           memcpy(&hj_scene_ctrl, pApp_head->buf, len);                           
   			   tmp_send_buf[APP_HEAD_LEN+1] = 0x05;                                      
                           make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+2);
                           eeprom_in_queue(SAVE_HJ_SCENE);
                        }
                     } 		
                     break;
                case 0x18:   //isp请求
		     if(type_add==0){
                        XBYTE[0x7FFF] = pApp_head->buf[0];  //ISP类型,01--CPU,05--HZK
                        tmp_send_buf[APP_HEAD_LEN] = 0x98;
                        make_send_package(0,0,NORMAL_TYPE,tmp_send_buf,APP_HEAD_LEN+2);						 		                                            
                        _do_isp();                                                
                     }
                     break;        
                case 0x27:  //消息查询响应
                     len = pApp_head->length-APP_HEAD_LEN-APP_TYPE_LEN;     
                     if(type_add==0x03){     //消息响应
                         __msg.word_num=*(unsigned int *)(pApp_head->buf);      //字符数量
                         if(len > REC_MAX_BUF)
                	    len = REC_MAX_BUF;
                         memcpy(__msg.msg_data, pApp_head->buf + 2, len);                     
                     }
		     else{
                         if(len > sizeof(REPAIR_MENU))
                            len = sizeof(REPAIR_MENU);  
                         memcpy(&__repair_menu,pApp_head->buf,len);                         
                     }           
                     __return_ok = 1;                                                        
		     break;
                case 0x25:  //NDT报警响应          
                     if(type_add){   
                        tmp8 = pApp_head->buf[0]-1; // 端口号                                 
                        if(tmp8<_INPUT_PORT_NUM){
                          if(type_add==0x02)  //短路报警  
                             ad_port[tmp8].curr_flag = STOP_SEND_SHORT|SHORT_FLAG;
                          else                //开路报警
                             ad_port[tmp8].curr_flag = STOP_SEND_OPEN|OPEN_FLAG;
                        }        
                     }   
                     else
                        retrans_free(pApp_head->tcbNum);
                     break;                            
                case 0x2D:    //用户设备维修确认
                     __return_ok = 1;                                                        
                     return;
                case 0x96:    //红外控制响应
                     if(type_add==0x02){                         
//                        if((tmp8=pApp_head->buf[2])==0x27)   //112个灯的刷新返回命令
//                           proce_light_refuse(pApp_head->buf+3);
//                        else                                 //控制命令返回
//                           refuse_light_status(pApp_head->buf[1],tmp8);                     
                        __return_ok = 1;                                                        
                     }   
                     if(type_add==0)
                        __return_ok = 1;                                                        
		     break;
            } 
       }
}

/**************************************************************************/
/*                 rec_check_xor 接收报文校验                               */
/* 参数：*pBuf——当前数据缓冲区指针                                          */   
/*       len——数据长度                                                    */
/* 返回：0——成功,其他——失败                                               */     
/* 日期：2000/12/13                                                        */      
/*------------------------------------------------------------------------*/
/* 修改记录:		                                                      */
/*    							                                  */   
/**************************************************************************/
unsigned char  rec_check_xor(unsigned int len, unsigned char xdata *pBuf)
{
   unsigned char idata check_xor=0;

   while(len--) 
       check_xor ^= *pBuf++;

   return  check_xor;
}   

void call_checksum(unsigned int len,unsigned char xdata *pBuf)
{
    U_INT_16 chk_sum=0;

    while(len--){
          chk_sum += *pBuf++;
    }
    *(unsigned int *)pBuf = chk_sum;  
}

void send_to_ccc(unsigned char type,unsigned char xdata *xp,unsigned int len)
{
       struct sk_buff xdata *skb;

       skb = alloc_skb();
       udp_skb_reserve(skb);
       skb_put(skb, len);         
       memcpy(skb->dat, xp, len);   
       switch(type & 0xC0){    
           case QUERY_TYPE:    //查询
                if(ip_status[0])
                   udp_server.des_addr = __netif.sys_ip0;    //主IP
                else
                   udp_server.des_addr = __netif.sys_ip1;    //从IP
                udp_server.des_port = UDP_INFO_SERVER_PORT;
                break;    
           case ALM_TYPE: 
                if(ip_status[2])
                   udp_server.des_addr = __netif.sys_ip2;    //主IP
                else
                   udp_server.des_addr = __netif.sys_ip3;    //从IP
                udp_server.des_port = UDP_NDT_ALARM_PORT;
                break;
        }        
        udp_server.src_addr = __netif.ni_ip;
	udp_server.src_port = NDT_PORT;
        udp_send(skb);	//&udp_server
}
/**************************************************************************/
/*              make_send_package 组装一个发送报文                          */
/* 参数： RIU_Addr——目的RIU地址                                           */ 
/*       tcbNum——传输控制号                                               */
/*       *pBuf——当前数据缓冲区指针                                        */   
/*       len——数据长度                                                   */
/* 返回： 无                                                              */     
/* 日期：2000/12/13                                                       */      
/*------------------------------------------------------------------------*/
/* 修改记录:		                                                  */
/*    							                  */   
/**************************************************************************/
void  make_send_package(unsigned char riu_addr,unsigned char tcbNum, unsigned char type, unsigned char xdata *xp,unsigned int len)
{     
    APP_HEAD xdata *pApp_head;
//    struct sk_buff xdata *skb;

    if(len > APP_DATA_LEN)
       return;    
    pApp_head = (APP_HEAD *)xp;
    pApp_head->version = SCU_PROTO_VERSION;
    pApp_head->length = len;
    pApp_head->tcbNum = tcbNum;
    pApp_head->RIU_Addr = riu_addr;
    pApp_head->forward_flag = SENDER_IS_NDT;   
    pApp_head->RIU_Type = SHN_TYPE;
    pApp_head->reserved =  0;
    pApp_head->check_sum = 0;
    pApp_head->check_sum = rec_check_xor(len, xp);    

    if(riu_addr==0){           //发送到CCC
       send_to_ccc(type, xp, len);
    } 
    else{		       //发送到RIU	
       if(asyn_send_bit==0 && asyn_send==0){   //发送空闲  @20060802
          asyn_send = 1;   
          call_checksum(len, xp);
          asyn.send_len = len+2;
          memcpy(asyn.send_buf, xp, len+2);
       } 
    }    
    return; 	
}

//bit pre_open_bit;
bit pre_12V_power_bit;
U_INT_8 idata second_counter;   
void app_other_task(void)
{
    extern bit mark_isr_Mark_RST;
    unsigned char xdata buf[40];     
    extern bit nic_need_init_bit;
    extern unsigned char idata warn_off;
    extern U_INT_8 xdata channal_switch;
    extern unsigned char xdata lcd_display_status;

//    bit curr_bit;
    unsigned char i,tmp;

       if(nic_need_init_bit || mark_isr_Mark_RST){     //NIC停止
          r8019_eth_init(1);
       } 
       
       if(warn_off){
          if((--warn_off) & 0x01){
             buzzer_on(_BIT_WARN_OFF);
             //ALM_LIGHT = LED_ON;  
             ALM_LIGHT_ON;
          }     
          else{
             buzzer_off(_BIT_WARN_OFF);
             if((channal_switch&_BIT_ALM)==0)  //已经有报警,不关LED
                //ALM_LIGHT = LED_OFF;  
                ALM_LIGHT_OFF;
          }
       }

       for(i=0;i<LINK_CTRL_NUM;i++){
           if(tmp=link_delay[i].port){
              if(link_delay[i].delay_time==0){
                 link_delay[i].port = 0;
                 if(tmp < 20)
                    output_ctrl(link_delay[i].end_status,tmp);
                 else
                    link_delay[i].end_status = 0x00;
              }  
              else
                 link_delay[i].delay_time--;          
           }
       } 


       if(tm[MOTI_INFO_TIMER]==0) ip_status[0] = 0x01;

       if(__ndt_reset && tm[NDT_RESET_WAIT]==0){
          while(1) EA=0;
       } 
                     
       if(update_locate_ip && tm[NDT_RESET_WAIT]==0){ //重置IP地址
          update_locate_ip = 0;
          memcpy((unsigned char *)&__netif, (unsigned char *)&tmp_netif, sizeof(NETIF));  //刷新全部内容
          eeprom_in_queue(SAVE_SYS_IP);     //保存IP地址 
       }                 

       if(tm[NDT_STATUS_TIMER]==0){         //发送NDT状态
          tm[NDT_STATUS_TIMER] = 60+30;     //1.5分钟发送一次状态
          buf[APP_HEAD_LEN]   = 0xa1;
          buf[APP_HEAD_LEN+1] = 0x00;
          buf[APP_HEAD_LEN+2] = SCU_HARDWARE_VERSION;  //硬件版本
          buf[APP_HEAD_LEN+3] = SCU_SOFTWARE_VERSION;  //软件版本 
          buf[APP_HEAD_LEN+4] = 0x00;                  //SCU状态
          buf[APP_HEAD_LEN+5] = __security_class;      //布防状态  
          buf[APP_HEAD_LEN+6] = 0x00;  
          make_send_package(0,0,ALM_TYPE|SEND_ONES,buf,APP_HEAD_LEN+7);						 		                                                
       } 

       if(++second_counter >= 60){
          second_counter = 0;
          get_real_clock();
          if(request_time_bit){     //请求时间
             buf[APP_HEAD_LEN] = 0xA6;
             buf[APP_HEAD_LEN+1] = 0x01;
             make_send_package(0,0,QUERY_TYPE|SEND_ONES,buf,APP_HEAD_LEN+2);                
          }
          device_ctrl();   

#if LOW_VOL_TET==1
          i=read_vol_status();
          if(i<ALM_VOLTAGE_VALUES && pre_12V_power_bit==0){
   	     alm_log(0,_LOW_POWER_12V);      //记录报警 
             buf[APP_HEAD_LEN] = 0xA5;
             buf[APP_HEAD_LEN+1] = 0x00;
   	     buf[APP_HEAD_LEN+2] = _LOW_POWER_12V;   //低电报警
             buf[APP_HEAD_LEN+3] = 0x00;
             retrans_in(buf,APP_HEAD_LEN+4);
             pre_12V_power_bit=1;
             ALM_LIGHT_ON;                       //报警灯亮
          }
          else{
             if(pre_12V_power_bit && i>=ALM_VOLTAGE_VALUES){
                pre_12V_power_bit=0;
                ALM_LIGHT_OFF;                   //报警灯灭
             } 
          }  
#endif
       }

       if(delay_status_bit && __delay_ctrl.ctrl_time)
          __delay_ctrl.ctrl_time--;
       else{
          if(continue_status_bit && __continue_ctrl.ctrl_time){
             if(--__continue_ctrl.ctrl_time==0){
                continue_status_bit = 0;
                alm(0);           //当前是最后一个持续控制，取消报警
             }
          }         
       } 

//       if(!POWER_LOW)
          //ALM_LIGHT = ~ALM_LIGHT;   //报警灯闪        
//          ALM_LIGHT_ON;
/*
       if(POWER_LOW ^ pre_12V_power_bit){    //12V欠压报警
          if(!POWER_LOW){                    //低电平报警
             pre_12V_power_bit = 0;
   	     alm_log(0,_LOW_POWER_12V);      //记录报警 
             buf[APP_HEAD_LEN] = 0xA5;
             buf[APP_HEAD_LEN+1] = 0x00;
   	     buf[APP_HEAD_LEN+2] = _LOW_POWER_12V;   //低电报警
             buf[APP_HEAD_LEN+3] = 0x00;
             retrans_in(buf,APP_HEAD_LEN+4);
          }
          else
             pre_12V_power_bit = 1;
       }          
*/
/*
       if(iic_rece_out(&msg_no,&msg_data)){
          switch(msg_no){   
              case MSG_TEAR:           //外壳被打开
                 buzzer_on(_BIT_ALM);
                 __alm.port = _BACKOUT_ALM_PORT;
                 __alm.riu_addr = 0;
                 alm(1);
                 buf[APP_HEAD_LEN]=0xA5;
   	         buf[APP_HEAD_LEN+1]=0x00;
  	         buf[APP_HEAD_LEN+2]=_BACKOUT_ALM_PORT;   //打开机壳报警
	         buf[APP_HEAD_LEN+3]=0x00;
                 retrans_in(buf,APP_HEAD_LEN+4);
                 break;
              case MSG_TALK:
                 SCREEN_SWITCH_CHAR;
   	         tm[KEYBOARD_TIMEOUT] = KEY_TIMEOUT;       //定时
                 LCD_POWER_ON;   //打开LCD电源
                 break;
          }   
       } 
*/
}
void net_handle(void)
{
	struct sk_buff xdata *skb;
	struct ethhdr xdata *eth_hdr;
       
 	if(skb =skb_fifo_out()) {
		eth_hdr = (struct ethhdr *)(skb->dat);
		skb_pull(skb, ETH_HLEN);
		if(eth_hdr->h_proto == ETH_P_ARP)
		   arp_rcv_packet(skb);
		else 
                   if(eth_hdr->h_proto == ETH_P_IP)
		      ip_rcv_packet(skb);
	}
	else
	   EX1 = 1; 	           
}
/*
void do_isp(void)   //系统在线编程
{
        IE = 0;            //关闭所有中断使能  
        XICON = 0;
        SFCF = 0x00;       //地址映射,E000->0000
#pragma ASM
        LJMP 0000H         //从ISP程序开始运行
#pragma ENDASM         
        while(1);
}
*/
