/*======================== MAIN.C ===============================*/
#include <reg52.h>
#include <absacc.h>
#include <string.h>
#include "inc/types.h"
#include "inc/mac.h"
#include "inc/arp.h"
#include "inc/ip.h"
#include "inc/app.h"
#include "inc/system.h"
#include "inc/timer.h"
#include "inc/tool.h"
#include "inc/ctrl.h"
#include "inc/hdres.h"
#include "inc/asyn.h"
#include "inc/menu.h"
#include "inc/key.h"
#include "inc/lcd.h"
#include "inc/alarm.h"
#include "inc/VI2C.h"
#include "inc/sst_spi.h"
#include "inc/mt8888.h"
#include "inc/isd4000.h"

extern void do_menu(void);


void main()
{    

    AUXR = 0x01;       //ALE 在访问外部有效
    BUZZER_ON;
    if(XBYTE[TEAR_ADD]==0){   //上电ISP
       XBYTE[0x7FFF]=1;
       _do_isp();                                                
    } 
    restore_from_rom(); 
    alm_init();
    timer_init();
    asyn_init();
    eth_init();
    crt_init();
    init_port();
    key_init();
//  init_spi();
    init_mt8888();
//    isd4000_init();
    WDTD = 0;                 //看门狗时间(255 -WDTD)*344064/XTAL1, 22.1184=4.0s
    WATCHDOG;                 //WATCGDIG ENABLE,SST89E58
    EA = 1;
//  load_icon();
//  draw_screen(20);
    while(1){
        do_menu();
    }
}
