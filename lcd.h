#ifndef _LCD_H_
#define _LCD_H_  1

#include "inc/asyn.h"

#define VRAM_PAGE_NUM   16
#define VRAM_PAGE_SIZE  8192 
#define _WORD_WIDE    21       //���ֵ����С
#define _WORD_HIGH    21       //���ֵ���߶�

#define _NUMBER_WIDE  10        //���ֵ����С
#define _ASCII_HIGH   18       //ascii�߶�
#define _ASCII_WIDE   10        //ascii��С
//define _FUNCTION_DISTANCE  32 //ÿ�����ܼ�����
#define _CRT_MAX_COL  (_WORD_WIDE*11)      //����CRT��� 
#define _CRT_MAX_ROL  14       //����CRT��ʾ����
#define _POINT_POSITION     16   
#define _XINGHAO_POSITION   17  
#define _ICON_WIDTH         66//34//45
#define _ICON_HIGH          35//60//45
#define _ICON_SIZE          (_ICON_WIDTH*_ICON_HIGH)//2048

#define SPACE_COL           16

#define ICON_START_COL  32
 
#define _LCD_DELAY 3

#define LCD_RED       0x04
#define LCD_GREEN     0x02
#define LCD_BLUE      0x01
#define LCD_CUILU     0x03    //����
#define LCD_PINK      0x05    //�ۺ�
#define LCD_YELLOW    0x06
#define LCD_WHITE     0xFF
#define LCD_BLACK     0x00

#define LCD_SHOW_BK_COLOR  LCD_BLUE       //����ϵͳ������ɫ

#define _MSG_DELAY_COUNT    1

#define HOUSE_PIC_LEN       (256*5-7)
struct HOUSE_PIC{
     unsigned char used;       //ʹ�ñ�ǣ���0x55
     unsigned int  len;        //���ݳ���
     unsigned char wide;       //ͼ���� <=200
     unsigned char high;       //ͼ��߶� <=200
     unsigned int  check;      //�ۼӺͣ���used�����һ�����ݰ��ֽ��ۼ�
     unsigned char dat[HOUSE_PIC_LEN];   //ѹ������,N<=1270�ֽ�
};
extern struct HOUSE_PIC xdata house_pic;


#define REC_MAX_BUF       (1518-42)
#define MESSAGE_NUM       7
struct MESSAGE {
        U_INT_8   msg_newness;    //������Ϣ��־ 0111 1111
        U_INT_8   type[MESSAGE_NUM+1];
        U_INT_16  msg_no[MESSAGE_NUM+1];  //������Ϣ��
	U_INT_16  start;
        U_INT_16  end;
        U_INT_16  word_num;               //�ַ������ 
        U_INT_8   msg_data[REC_MAX_BUF];  //��Ϣ������
};
extern struct MESSAGE xdata __msg;

extern unsigned char xdata new_img_msg;        //���µ�ͼ��

void crt_init(void);
void crt_clear(void);
void crt_outnull(U_INT_8 page,U_INT_8 col,U_INT_8 num);
void crt_outdigital(U_INT_8 page, U_INT_8 col, U_INT_32 dgt, U_INT_8  length,U_INT_8 out_float);
void draw_screen(U_INT_8 num);
void loop_msg(void);
void show_msg(void);
void write_return(void);
void crt_outword_inverse(U_INT_8 page, U_INT_8 col, U_INT_8 word, U_INT_8 nr, U_INT_8 type);
void crt_out_hex(U_INT_8 page, U_INT_8 col, U_INT_8 dec_num);
void message_reindex(unsigned char key);
void message_in(unsigned char type,unsigned int msg_no);
void crt_outhz(U_INT_8 page, U_INT_8 col, U_INT_8 *pChar, U_INT_8 len);
void crt_outhz_inverse(U_INT_8 page, U_INT_8 col, U_INT_8 code *pChar, U_INT_8 len); 
void crt_outicon(U_INT_8 page,U_INT_8 col,U_INT_8 icon_no) ;
void crt_outchar(U_INT_8 page, U_INT_16 col, U_INT_8 qm) ;
void load_icon(void);
void crt_show_hand(unsigned char page,unsigned char col);
void crt_clear_hand(unsigned char page,unsigned char col);
void send_message(void);
void show_crt_number(void);
void crt_outnull_num(U_INT_8 page,U_INT_8 col,U_INT_8);
void show_room_pic(void);
void crt_out_smallicon(U_INT_8 page,U_INT_8 col,U_INT_8 icon_no);
void fast_write_icon(unsigned char xdata *src,unsigned char xdata *des,unsigned char len);

#endif
