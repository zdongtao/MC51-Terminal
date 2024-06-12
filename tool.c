#include <reg51.h>
#include <absacc.h>
#include <string.h>
#include <intrins.h>
#include "inc/types.h"
#include "inc/hdres.h"
#include "inc/timer.h"
#include "inc/tool.h"
#include "inc/alarm.h"
#include "inc/ctrl.h"
#include "inc/app.h"
#include "inc/ip.h"
#include "inc/menu.h"
#include "inc/lcd.h"
#include "inc/vf040.h"

OTHER_ITEM  xdata other_item;

struct EEPROM_ITEM code __eeprom[] =
{
	{&__sys_passwd, sizeof(__sys_passwd),SYS_PASSWD_ROM_ADDR},
        {&__security_class, sizeof(__security_class),SECURITY_ROM_ADDR},
	{&__port, sizeof(__port),PORT_ROM_ADDR},       
	{&__menu_item, sizeof(__menu_item),MENU_ROM_ADDR},
	{&__netif, sizeof(__netif), NETIF_ROM_ADDR},                 
	{&__alm_log, sizeof(__alm_log), ALM_LOG_ROM_ADDR}, 
	{&__defend_log, sizeof(__defend_log),DEFEND_LOG_ROM_ADDR},                 
	{&__link_ctrl, sizeof(__link_ctrl),LINK_CTRL_ROM_ADDR},                 
        {&light_ctrl, sizeof(light_ctrl),LIGHT_CTRL_ROM_ADDR},
	{&ir_ctrl, sizeof(ir_ctrl), IR_CTRL_ROM_ADDR},
	{&house_pic, sizeof(house_pic),HOUSE_PIC_ROM_ADDR},     
        {&hj_scene_ctrl, sizeof(hj_scene_ctrl),HJ_SCENE_ADDR},     //被场景使用 20061027            
	{&other_item, sizeof(other_item),OTHER_SAVE_ROM_ADDR }, //最后一项，保存标志         
};

//保存到flash,对一个扇区进行操作.取消了队列操作,马上保存
void  eeprom_in_queue(unsigned char item) 
{
       struct EEPROM_ITEM code *cp;

       if(item < SAVE_MAX_ITEM){         //@20060621
          cp = __eeprom + item;
          write_sector(cp->sector_addr, cp->len, cp->src_add);
       }
}

void restore_from_rom(void)
{

       U_INT_8 i;
       struct EEPROM_ITEM code *cp;
       cp = __eeprom;
       for(i=0;i<SAVE_MAX_ITEM;i++){
           read_sector(cp->sector_addr,cp->len,cp->src_add);
           cp++;
       }
       if(__sys_passwd.Pass_Num > _USER_PIN_LEN){           //长度错误,清除密码
          memset(&__sys_passwd,0x00,sizeof(__sys_passwd));         
       }
}

