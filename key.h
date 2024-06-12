#ifndef _KEY_H_
#define _KEY_H_ 1

//#define _KEY_IN_PORT    P1        /* ������ֵɨ���߶˿�, Ҳ������˿�. */
//#define _KEY_OUT_PORT   P1        /* ����ˮƽɨ���߶˿�, Ҳ������˿�. */
//#define _KEY_IN_MASK    0x30      /* ��������˿�����. */
//#define _KEY_OUT_MASK   0x0F      /* ��������˿�����. */

#define KEY_MASK          0x0F
#define KEY_TIMEOUT       150         //150s

#define FIX_HOT_KEY_ROL   0xE8      
#define FIX_HOT_KEY_WID   20      

//��������
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
#define NUM_XING        11       //�绰����*
#define NUM_JING        12       //�绰����#

enum MSG_SYSTEM{
    MSG_TEAR = 0x81,      //���𿪹�
    TOUCH_KEY,             //�������¼� 
    ALM_SHOW_KEY,  
    MSG_REFUSE_LIGHT_CTRL,   
    ALM_RETURN_KEY,       //�������ص����ɵ���ʾ�û�����ͼ        
    FAST_NO_DEFEND_KEY,   //������ݼ� 
    FAST_NO_ALM_KEY,      //������ݼ� 
    CANCL_ALM_RETURN_KEY, //�������ؼ�
    PHONE_CALL_MSG,

    AUTO_RETURN_KEY       //�������     
};

enum PHONE_STATUS{
    PHONE_MSG_IDLE,
    PHONE_MSG_HANGUP,
    PHONE_MSG_WAIT_PIN
};

#define PHONE_PIN_LEN         4   //�绰���볤��
#define PHONE_TIMEOUT_MAX     30  //�绰��ʱ30s

//IIC��Ϣ

/* ����״̬���� */
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
    U_INT_8  flag;		   /* ����״̬ */
    U_INT_8  key_code;             /* ������ʱ�洢���� */
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

//����������
struct TOHUCH{
    unsigned char flag;
    unsigned char x;
    unsigned char y;
    unsigned char timeout;
};
extern struct TOHUCH idata touch;

//����IIC����->������Ϣ
#define MSG_HOST_ONDEF    0x01  //������LED   
#define MSG_HOST_OFFDEF   0x02  //�ز���LED   
#define MSG_HOST_ONALM    0x05  //������LED   
#define MSG_HOST_OFFALM   0x06  //�ر���LED   
#define MSG_HOST_SWCHAR   0x09  //�л���CHAR
#define MSG_HOST_SWVIDEO  0x0A  //�л���VIDEO
#define MSG_HOST_ONAUDIO  0x0B  //����Ƶ�Ŵ���
#define MSG_HOST_OFFAUDIO 0x0C  //����Ƶ�Ŵ���
#define MSG_HOST_TALKADDR 0x0D  //�������ڻ��Խ���ַ
#define MSG_HOST_TALK_LOCK_PIN  0x0E  //�������ڻ���������

#define MSG_HOST_TALK     0x10  //�����Խ�����
#define MSG_HOST_CALL     0x11  //������������
#define MSG_HOST_OPENLK   0x12  //������������

#define MSG_HOST_VOICE_UP    0x15    //��������
#define MSG_HOST_VOICE_DOWN  0x16    //��������
#define MSG_HOST_NO_SOUND    0x17    //����

#define MSG_HOST_BRIGHT_UP   0x18    //���ȵ���
#define MSG_HOST_BRIGHT_DOWN 0x19    //���ȵ���


#define MSG_READ_IMG         0x20  //��ͼ����Ϣ
#define MSG_READ_IMG_UP      0x21  //��ͼ����Ϣ��һ��
#define MSG_READ_IMG_DOWN    0x22  //��ͼ����Ϣ��һ��
#define MSG_READ_IMG_END     0x23  //��ͼ����Ϣ����


#define MSG_SET_IMG_TIME     0x25    //����ͼ��洢��ʱ��



//����->������Ϣ
#define MSG_KEY           0x80   //������Ϣ
#define MSG_TEAR          0x81   //���𿪹�
#define MSG_TALK          0x82   //�Խ���Ϣ
#define MSG_MULT_VIDE_SW  0x83   //��·��Ƶ�л�
#define MSG_MULT_VIDE_OFF 0x84   //�رն�·��Ƶģ��
#define MSG_TALK_END      0x85   //�Խ�����

#define MSG_SAVE_DATA_OK  0x86   //���ݱ���ɹ�
#define MSG_TALK_CALL     0x87   //����������

#define MSG_IC_CARD_NODEFEND   0x88   //IC��ˢ������


#define MSG_READ_IMG_RETURN  0x90   //��ͼ�񷵻�
#define MSG_NEW_IMG_RETURN   0x91   //���µ�ͼ��֪ͨ
#define MSG_DOOR_BUSY        0x92   //�ſڻ�æ����
#define MSG_MONITOR_RDY      0x93   //�ſڻ����ӻ�Ӧ


 
//�����IICͨѶ��Ϣ������
#define IIC_SEND_FIFO_MASK  0x07
#define IIC_SEND_FIFO_LEN   (IIC_SEND_FIFO_MASK + 1)
#define IIC_MSG_LEN         12
struct IIC_MSG
{
    unsigned char  msg_no;
    unsigned char  msg_dat[IIC_MSG_LEN];   //��Ϣ����
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
    unsigned long  msg_dat[IIC_RECE_FIFO_LEN];   //���ֽ�Ϊ��Ϣ���ͣ����ֽ�Ϊ��Ϣ����
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
