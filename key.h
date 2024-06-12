#ifndef _KEY_H_
#define _KEY_H_ 1

//#define _KEY_IN_PORT    P1        /* 键盘竖值扫描线端口, 也是输入端口. */
//#define _KEY_OUT_PORT   P1        /* 键盘水平扫描线端口, 也是输出端口. */
//#define _KEY_IN_MASK    0x30      /* 键盘输入端口掩码. */
//#define _KEY_OUT_MASK   0x0F      /* 键盘输出端口掩码. */

#define KEY_MASK          0x0F
#define KEY_TIMEOUT       150         //150s

#define FIX_HOT_KEY_ROL   0xE8      
#define FIX_HOT_KEY_WID   20      

//按键定义
/*
#define UP_KEY          0
#define DOWN_KEY        1
#define LEFT_KEY        2
#define RIGHT_KEY       3
#define ENTER_KEY       4

#define TALK_KEY	0x01
#define UNLOCK_KEY      0x02
#define CALL_KEY	0x04
#define LINK_INTERNET	0x08
*/
#define NUM_XING        11       //电话号码*
#define NUM_JING        12       //电话号码#

enum MSG_SYSTEM{
    MSG_TEAR = 0x81,      //防拆开关
    TOUCH_KEY,             //触摸屏事件 
    ALM_SHOW_KEY,  
    MSG_REFUSE_LIGHT_CTRL,   
    ALM_RETURN_KEY,       //报警返回到主采单显示用户套型图        
    FAST_NO_DEFEND_KEY,   //撤防快捷键 
    FAST_NO_ALM_KEY,      //消警快捷键 
    CANCL_ALM_RETURN_KEY, //消警返回键
    PHONE_CALL_MSG,

    AUTO_RETURN_KEY       //放最后面     
};

enum PHONE_STATUS{
    PHONE_MSG_IDLE,
    PHONE_MSG_HANGUP,
    PHONE_MSG_WAIT_PIN
};

#define PHONE_PIN_LEN         4   //电话密码长度
#define PHONE_TIMEOUT_MAX     30  //电话超时30s

//IIC消息

/* 键盘状态定义 */
enum KEY_STATUS {
        _KF_UP,
        _KF_DEBOUNCE,
        _KF_DOWN
};

#define WAIT_BELLING  0
#define WAIT_TAKE_UP  1
#define TAKE_UP_OK    2
/*
#define _KF_UP		0
#define _KF_DEBOUNCE	1
#define _KF_DOWN	3
*/
#define _KEY_FIFO_MASK  0x03
#define _KEY_FIFO_LEN   (_KEY_FIFO_MASK + 1)

struct KEY_CTRL
{
    U_INT_8  flag;		   /* 键盘状态 */
    U_INT_8  key_code;             /* 用来临时存储键码 */
    U_INT_8  head;
    U_INT_8  tail;
    U_INT_8  dat[_KEY_FIFO_LEN];
};
extern struct KEY_CTRL idata __key_ctrl;
/*
#define _TOUCH_FIFO_MASK  0x03
#define _TOUCH_FIFO_LEN   (_TOUCH_FIFO_MASK + 1)
struct TOUCH_CTRL
{
    U_INT_8  head;
    U_INT_8  tail;
    U_INT_16 x_y[_TOUCH_FIFO_LEN];
};
extern struct TOUCH_CTRL idata __touch_ctrl;
*/

//触摸屏定义
struct TOHUCH{
    unsigned char flag;
    unsigned char x;
    unsigned char y;
    unsigned char timeout;
};
extern struct TOHUCH idata touch;

//定义IIC主机->键盘消息
#define MSG_HOST_ONDEF    0x01  //开布防LED   
#define MSG_HOST_OFFDEF   0x02  //关布防LED   
#define MSG_HOST_ONALM    0x05  //开报警LED   
#define MSG_HOST_OFFALM   0x06  //关报警LED   
#define MSG_HOST_SWCHAR   0x09  //切换到CHAR
#define MSG_HOST_SWVIDEO  0x0A  //切换到VIDEO
#define MSG_HOST_ONAUDIO  0x0B  //开音频放大器
#define MSG_HOST_OFFAUDIO 0x0C  //关音频放大器
#define MSG_HOST_TALKADDR 0x0D  //设置室内机对讲地址
#define MSG_HOST_TALK_LOCK_PIN  0x0E  //设置室内机开锁密码

#define MSG_HOST_TALK     0x10  //主机对讲请求
#define MSG_HOST_CALL     0x11  //主机呼叫请求
#define MSG_HOST_OPENLK   0x12  //主机开锁请求

#define MSG_HOST_VOICE_UP    0x15    //音量调高
#define MSG_HOST_VOICE_DOWN  0x16    //音量调低
#define MSG_HOST_NO_SOUND    0x17    //静音

#define MSG_HOST_BRIGHT_UP   0x18    //亮度调高
#define MSG_HOST_BRIGHT_DOWN 0x19    //亮度调低


#define MSG_READ_IMG         0x20  //读图像消息
#define MSG_READ_IMG_UP      0x21  //读图像消息上一条
#define MSG_READ_IMG_DOWN    0x22  //读图像消息下一条
#define MSG_READ_IMG_END     0x23  //读图像消息结束


#define MSG_SET_IMG_TIME     0x25    //设置图像存储器时间



//键盘->主机消息
#define MSG_KEY           0x80   //键盘消息
#define MSG_TEAR          0x81   //防拆开关
#define MSG_TALK          0x82   //对讲消息
#define MSG_MULT_VIDE_SW  0x83   //多路视频切换
#define MSG_MULT_VIDE_OFF 0x84   //关闭多路视频模块
#define MSG_TALK_END      0x85   //对讲结束

#define MSG_SAVE_DATA_OK  0x86   //数据保存成功
#define MSG_TALK_CALL     0x87   //呼叫主机中

#define MSG_IC_CARD_NODEFEND   0x88   //IC卡刷卡撤防


#define MSG_READ_IMG_RETURN  0x90   //读图像返回
#define MSG_NEW_IMG_RETURN   0x91   //有新的图像通知
#define MSG_DOOR_BUSY        0x92   //门口机忙线中
#define MSG_MONITOR_RDY      0x93   //门口机监视回应


 
//与键盘IIC通讯消息缓冲区
#define IIC_SEND_FIFO_MASK  0x07
#define IIC_SEND_FIFO_LEN   (IIC_SEND_FIFO_MASK + 1)
#define IIC_MSG_LEN         12
struct IIC_MSG
{
    unsigned char  msg_no;
    unsigned char  msg_dat[IIC_MSG_LEN];   //消息数据
};

struct IIC_SEND_FIFO
{
    unsigned char  head;
    unsigned char  tail;
    struct IIC_MSG iic_msg[IIC_SEND_FIFO_LEN];
};

#define IIC_RECE_FIFO_MASK  0x07
#define IIC_RECE_FIFO_LEN   (IIC_RECE_FIFO_MASK + 1)
struct IIC_RECE_FIFO
{
    unsigned char  head;
    unsigned char  tail;
    unsigned char  msg_no[IIC_RECE_FIFO_LEN];
    unsigned long  msg_dat[IIC_RECE_FIFO_LEN];   //高字节为消息类型，低字节为消息数据
};

U_INT_8 key_get(void);
void    key_init(void);

void key_timer(void);
void proc_msg(void);
void key_in(U_INT_8 item);
bit  key_out(U_INT_8 idata *item); 
void key_press_in(void);
bit  touch_out(U_INT_8 idata *x,U_INT_8 idata *y);
bit  iic_rece_out(unsigned char *msg_no,unsigned long *dat);
void  iic_send_in(unsigned char msg_no,unsigned char *dat);
void  iic_send_in_msgno(unsigned char msg_no);
bit iic_send_out(unsigned char xdata *buf);
void key_free(void);
unsigned char  read_vol_status(void);
void ringcheck(void);

#endif
