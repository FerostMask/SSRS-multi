/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "menu.h"
#include "Init.h"
#include "motor.h"
#include "zf_adc.h"
#include "menu_2.h"
#include "zf_pit.h"
#include "SEEKFREE_IPS200_PARALLEL8.h"
/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/
//	二级菜单相关	
unsigned char menu2_limit = 0;
unsigned char menu2flag = 1;//次级菜单进入标志位
unsigned char menu2mode = 0;//菜单显示模式
unsigned char fixedindex = 0;
//	修改倍数相关
unsigned char magflag = 1;
unsigned char magindex = 1;
float mag[] = {10,1,0.1,0.01,0.001};
//	数值修改相关
short *shortvalue0, *shortvalue1, *shortvalue2;
float *value0, *value1, *value2, *value3, *value4;

/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
/*------------------------------*/
/*		 二级菜单初始化模块		*/
/*==============================*/
void menu2_init(void){
//	初始化数值
	menu2flag = 1;
	magflag = 0;
	menu2_index = 0;
//	初始化模块
	switch(menu_index){
		case 0:
			magflag = 0;
			menu2flag = 3;//切换为开关
//			tim_interrupt_init(TIM_8, 20, 0, 3);
			switch(menu[menu_index]){
				case 0:
					menu2mode = 0;//显示页面0
					menu2_limit = CSIMENU_FLAG-1;//索引限制
					break;
				case 1:
					menu2mode = 1;//显示页面1
					menu2_limit = WIRELESS_FLAG-1;//索引限制
					break;
			}
			break;
		case 1:
			switch(menu[menu_index]){
				case 0://电磁设置
					magflag = 0;
					menu2flag = 4;
					menu2_limit = 5;
					menu2mode = 0;
					break;
				case 1://电磁转向
					value0 = &cam_steering.Kp;
					value1 = &cam_steering.Kd;
					menu2_limit = 1;
					menu2mode = 1;
					magflag = 1;
					break;
				case 2://差比和差
					value0 = &adc_err.alpha;
					value1 = &adc_err.beta;
					value2 = &adc_err.omega;
					value3 = &adc_err.P;
					menu2_limit = 3;
					menu2mode = 2;
					magflag = 1;
//				case 1://转向
//					value0 = &angle.Kp;
//					value1 = &angle.Kd;
//					menu2_limit = 1;
//					menu2mode = 1;
//					magflag = 1;
//					break;
			}
			break;
		case 2://姿态
//			pit_close(PIT_CH0);
			switch(menu[menu_index]){
				case 0:
					shortvalue0 = &spd_adcset;
					shortvalue1 = &Kp_act;
					shortvalue2 = &spd_adcset;
					menu2_limit = 2;
					menu2mode = 0;
					magflag = 1;
					break;
			}
			break;
		case 3://监视器
		//	开启中断显示
			tim_interrupt_init(TIM_6, 40, 0, 3);
			menu2flag = 2;
			monitorflag = 1;
			break;
	}
}
/*------------------------------*/
/*		 数值显示辅助模块		*/
/*==============================*/
static void menu2value_sup(void){
	switch(menu_index){
		case 0:
			swdisplay();
			break;
		case 1:
			switch(menu2mode){
				case 0:
					ips200_showstr(140, 14, "SET ALL");
					ips200_showint16(100, 15, adc0.max);
					ips200_showint16(170, 15, adc0.min);
					ips200_showint16(100, 16, adc1.max);
					ips200_showint16(170, 16, adc1.min);
					ips200_showint16(100, 17, adc2.max);
					ips200_showint16(170, 17, adc2.min);
					ips200_showint16(100, 18, adc3.max);
					ips200_showint16(170, 18, adc3.min);
					ips200_showint16(100, 19, adc4.max);
					ips200_showint16(170, 19, adc4.min);
					break;
				case 1:
					ips200_showfloat(120, 15, *value0, 2, 3);
					ips200_showfloat(120, 16, *value1, 2, 3);
					break;
				case 2:
					ips200_showfloat(120, 15, *value0, 2, 3);
					ips200_showfloat(120, 16, *value1, 2, 3);
					ips200_showfloat(120, 17, *value2, 2, 3);
					ips200_showfloat(120, 18, *value3, 2, 3);
					break;
			}
			break;
		case 2:
			switch(menu2mode){
				case 0:
					ips200_showint16(120, 15, *shortvalue0);
					ips200_showint16(120, 16, *shortvalue1);
					ips200_showint16(120, 17, *shortvalue2);
					break;
			}
	}
}
/*------------------------------*/
/*		 菜单高亮显示模块		*/
/*==============================*/
void menu2value_hl(void){
	switch(menu_index){//行索引
		case 1:
			switch(menu2mode){//页面选择
				case 0:
					if(excollflag == 6){
						ips200_showint16(100, 15, adc0.max);
						ips200_showint16(170, 15, adc0.min);
						ips200_showint16(100, 16, adc1.max);
						ips200_showint16(170, 16, adc1.min);
						ips200_showint16(100, 17, adc2.max);
						ips200_showint16(170, 17, adc2.min);
						ips200_showint16(100, 18, adc3.max);
						ips200_showint16(170, 18, adc3.min);
						ips200_showint16(100, 19, adc4.max);
						ips200_showint16(170, 19, adc4.min);
					}
					switch(menu2_index){
						case 0:
							ips200_showstr(140, 14, "SET ALL");
							break;
						case 1:
							ips200_showint16(100, 15, adc0.max);
							ips200_showint16(170, 15, adc0.min);
							break;
						case 2:
							ips200_showint16(100, 16, adc1.max);
							ips200_showint16(170, 16, adc1.min);
							break;
						case 3:
							ips200_showint16(100, 17, adc2.max);
							ips200_showint16(170, 17, adc2.min);
							break;
						case 4:
							ips200_showint16(100, 18, adc3.max);
							ips200_showint16(170, 18, adc3.min);
							break;
						case 5:
							ips200_showint16(100, 19, adc4.max);
							ips200_showint16(170, 19, adc4.min);
							break;
						}
					break;
				case 1:
					switch(menu2_index){
						case 0:ips200_showfloat(120, 15, *value0, 2, 3);break;
						case 1:ips200_showfloat(120, 16, *value1, 2, 3);break;
					}
					break;
				case 2:
					switch(menu2_index){
						case 0:ips200_showfloat(120, 15, *value0, 2, 3);break;
						case 1:ips200_showfloat(120, 16, *value1, 2, 3);break;
						case 2:ips200_showfloat(120, 17, *value2, 2, 3);break;
						case 3:ips200_showfloat(120, 18, *value3, 2, 3);break;
					}
					break;
			}
			break;
		case 2:
			switch(menu2_index){
				case 0:ips200_showint16(120, 15, *shortvalue0);break;
				case 1:ips200_showint16(120, 16, *shortvalue1);break;
				case 2:ips200_showint16(120, 17, *shortvalue2);break;
			}
			break;
		}
}
/*------------------------------*/
/*		   数值显示模块			*/
/*==============================*/
static void menu2value(void){
//	显示级别
	if(magflag) ips200_showfloat(180, 14, mag[magindex], 2, 3);
//	显示数值
	menu2value_sup();
//	高亮数值
	if(!menu2_level){
	//	选择
		ips200_pencolor = 0xFFFF;
		ips200_bgcolor = 0xFDF8;
		menu2value_hl();
		ips200_pencolor = 0xB6DB;
		ips200_bgcolor = 0xFFFF;
	}
	else{
	//	修改
		ips200_pencolor = 0xFFFF;
		ips200_bgcolor = 0xAE9C;
		menu2value_hl();
		ips200_pencolor = 0xB6DB;
		ips200_bgcolor = 0xFFFF;
	}
}
/*------------------------------*/
/*		   信息存放模块			*/
/*==============================*/
static char info(unsigned char index, unsigned char num){
//	字模存储
	unsigned char can0[] = {0x00,0x00,0x02,0x00,0x04,0x20,0x08,0x10,0x3F,0xF8,0x01,0x08,0x7F,0xFE,0x02,0x40,0x04,0x20,0x08,0x98,0xF3,0x07,0x0C,0x20,0x00,0xCC,0x1F,0x18,0x00,0x60,0x1F,0x80};/*"参"*/
	unsigned char shu0[] = {0x00,0x00,0x00,0x00,0x25,0x20,0x25,0x20,0x26,0x3E,0x7F,0xA4,0x0E,0x64,0x35,0x64,0x44,0x24,0x08,0x24,0x7F,0x18,0x11,0x18,0x21,0x18,0x1A,0x14,0x06,0x24,0x79,0xC2};/*"数"*/
	unsigned char ming0[] = {0x00,0x00,0x00,0x00,0x02,0x00,0x07,0xF8,0x18,0x08,0x28,0x08,0x46,0x10,0x01,0x20,0x00,0xC0,0x0F,0xFC,0xF8,0x02,0x08,0x02,0x08,0x02,0x08,0x02,0x08,0x02,0x0F,0xFE};/*"名"*/
	unsigned char zhi0[] = {0x00,0x00,0x10,0x40,0x10,0x40,0x2F,0xFE,0x20,0x40,0x20,0x40,0x23,0xFC,0x64,0x04,0x63,0xFC,0xA4,0x04,0x23,0xFC,0x24,0x04,0x23,0xFC,0x24,0x04,0x24,0x04,0x2F,0xFE};/*"值"*/
	unsigned char zhuan0[] = {0x00,0x00,0x00,0x20,0x10,0x20,0x7E,0x20,0x21,0xFE,0x20,0x40,0x40,0x40,0x51,0xFE,0x50,0x40,0x7C,0x40,0x10,0x7E,0x10,0x04,0x3C,0x04,0x10,0x08,0x10,0x70,0x00,0x0E};/*"转"*/
	unsigned char su0[] = {0x00,0x00,0x00,0x00,0x20,0x40,0x27,0xFE,0x10,0x40,0x07,0xFC,0x04,0x44,0x74,0x44,0x14,0x44,0x13,0xF8,0x10,0xF0,0x11,0x58,0x12,0x4C,0x1C,0x46,0x28,0x40,0x46,0xFE};/*"速"*/
	unsigned char xiang0[] = {0x00,0x00,0x01,0x00,0x01,0x00,0x1F,0xF8,0x20,0x06,0x20,0x02,0x27,0xE2,0x28,0x12,0x28,0x12,0x28,0x12,0x28,0x12,0x28,0x12,0x28,0x12,0x27,0xE2,0x20,0x02,0x20,0x1C};/*"向"*/
	unsigned char jiao0[] = {0x00,0x00,0x04,0x00,0x0F,0xF0,0x10,0x10,0x30,0x20,0x5F,0xFC,0x20,0x84,0x20,0x84,0x3F,0xFC,0x20,0x84,0x20,0x84,0x20,0x84,0x3F,0xFC,0x20,0x84,0x20,0x84,0x40,0xBC};/*"角"*/
	unsigned char ping0[] = {0x00,0x00,0x00,0x00,0x1F,0xFC,0x21,0x00,0x21,0x04,0x11,0x04,0x09,0x08,0x09,0x10,0x05,0x20,0x01,0x00,0x7F,0xFE,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x00,0x00};/*"平"*/
	unsigned char heng0[] = {0x00,0x00,0x00,0x00,0x17,0xCE,0x28,0x40,0x48,0x80,0x4F,0xE0,0x19,0x3F,0x29,0x24,0x2F,0xE4,0x69,0x24,0x2F,0xE4,0x21,0x04,0x3F,0xF4,0x22,0x84,0x26,0x44,0x28,0x1C};/*"衡"*/
	unsigned char dian0[] = {0x00,0x00,0x00,0x00,0x02,0x00,0x01,0xFE,0x02,0x00,0x02,0x00,0x3F,0xF8,0x20,0x08,0x20,0x08,0x20,0x08,0x20,0x08,0x3F,0xF8,0x24,0x44,0x24,0x44,0x22,0x22,0x42,0x22};/*"点"*/
	unsigned char fu0[] = {0x00,0x00,0x10,0x40,0x10,0x40,0x27,0xFE,0x24,0x84,0x24,0x84,0x25,0x7E,0x65,0x04,0x65,0x44,0x27,0x24,0x27,0x24,0x29,0x14,0x29,0x14,0x29,0x04,0x29,0x04,0x29,0x3C};/*"俯"*/
	unsigned char yang0[] = {0x00,0x00,0x10,0x80,0x13,0x3C,0x24,0x42,0x24,0x42,0x24,0x42,0x24,0x42,0x64,0x42,0x64,0x42,0xA4,0x42,0x24,0x42,0x24,0x42,0x24,0xC2,0x25,0x44,0x22,0x5C,0x20,0x00};/*"仰"*/
	unsigned char hang0[] = {0x00,0x00,0x08,0x20,0x08,0x20,0x3D,0xFE,0x32,0x00,0x32,0x00,0x2A,0xF8,0x2A,0x88,0x22,0x88,0xFE,0x88,0x32,0x88,0x52,0x88,0x4A,0x8A,0x4A,0x8A,0x4B,0x0A,0x53,0x0E};/*"航"*/
	unsigned char zuo0[] = {0x00,0x00,0x01,0x00,0x01,0x00,0x02,0x00,0x7F,0xFE,0x02,0x00,0x04,0x00,0x04,0x00,0x0F,0xFC,0x10,0x40,0x20,0x40,0x40,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x3F,0xFE};/*"左"*/
	unsigned char you0[] = {0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x7F,0xFE,0x02,0x00,0x04,0x00,0x08,0x00,0x1F,0xF8,0x30,0x04,0x70,0x04,0x90,0x04,0x10,0x04,0x10,0x04,0x10,0x04,0x0F,0xF8};/*"右"*/
	unsigned char lun0[] = {0x00,0x00,0x00,0x00,0x10,0x30,0x10,0x58,0x7E,0x88,0x29,0x06,0x2A,0x01,0x48,0x80,0x7E,0x84,0x08,0x98,0x08,0xA0,0x08,0xC2,0x7F,0x82,0x08,0x82,0x08,0x82,0x08,0x7C};/*"轮"*/
	unsigned char zhuang0[] = {0x00,0x00,0x08,0x00,0x08,0x48,0x08,0x48,0x48,0x44,0x2B,0xFE,0x18,0x40,0x08,0x60,0x08,0x60,0x18,0x50,0x18,0x50,0x28,0x90,0x48,0x88,0xC9,0x04,0x0A,0x04,0x0A,0x02};/*"状"*/
	unsigned char tai0[] = {0x00,0x00,0x00,0x00,0x01,0x00,0x7F,0xFE,0x01,0x80,0x02,0x40,0x05,0x20,0x19,0x18,0xE0,0x86,0x00,0x00,0x24,0x88,0x24,0x84,0x24,0x54,0x24,0x52,0x44,0x12,0x47,0xF0};/*"态"*/
	unsigned char gong0[] = {0x00,0x00,0x00,0x20,0x00,0x20,0x7C,0x20,0x13,0xFE,0x10,0x22,0x10,0x42,0x10,0x42,0x10,0x42,0x10,0x42,0x10,0x82,0x10,0x82,0x17,0x02,0x19,0x02,0x62,0x04,0x0C,0x7C};/*"功"*/
	unsigned char neng0[] = {0x00,0x00,0x10,0x00,0x10,0x40,0x22,0x44,0x21,0x78,0x7F,0x40,0x00,0xC2,0x3F,0x42,0x41,0x3C,0x7F,0x40,0x41,0x46,0x41,0x78,0x7F,0x40,0x41,0x42,0x41,0x42,0x4F,0x7E};/*"能"*/
	unsigned char kai0[] = {0x00,0x00,0x00,0x00,0x3F,0xFC,0x08,0x10,0x08,0x10,0x08,0x10,0x08,0x10,0x08,0x10,0x7F,0xFE,0x08,0x10,0x08,0x10,0x08,0x10,0x08,0x10,0x10,0x10,0x10,0x10,0x20,0x10};/*"开"*/
	unsigned char guan0[] = {0x00,0x00,0x00,0x00,0x08,0x10,0x04,0x20,0x3F,0xFC,0x01,0x00,0x01,0x00,0x01,0x00,0x7F,0xFE,0x01,0x80,0x01,0x80,0x02,0x40,0x02,0x20,0x0C,0x10,0x10,0x08,0x60,0x06};/*"关"*/
	unsigned char cai0[] = {0x00,0x00,0x00,0x00,0x07,0x08,0x78,0x88,0x49,0x10,0x29,0x60,0x2E,0x04,0x08,0x08,0x7F,0x10,0x0C,0x20,0x1C,0xC2,0x1A,0x04,0x29,0x08,0x48,0x10,0x08,0x60,0x09,0x80};/*"彩"*/
	unsigned char se0[] = {0x00,0x00,0x00,0x00,0x04,0x00,0x0F,0xF0,0x10,0x20,0x20,0x20,0x7F,0xFC,0x20,0x84,0x20,0x84,0x20,0x84,0x20,0x84,0x3F,0xF8,0x20,0x00,0x20,0x02,0x20,0x02,0x1F,0xFE};/*"色"*/
	unsigned char hui0[] = {0x00,0x00,0x08,0x00,0x08,0x00,0x7F,0xFE,0x08,0x40,0x08,0x40,0x0A,0x48,0x0A,0x48,0x12,0x48,0x12,0x50,0x10,0x50,0x20,0xA0,0x20,0xA0,0x43,0x10,0x44,0x0C,0x18,0x02};/*"灰"*/
	unsigned char du0[] = {0x00,0x00,0x00,0x00,0x00,0x80,0x3F,0xFE,0x44,0x10,0x5F,0xFC,0x44,0x10,0x44,0x10,0x43,0xF0,0x40,0x00,0x5F,0xF8,0x44,0x08,0x42,0x30,0x41,0xC0,0x43,0x60,0xBC,0x1E};/*"度"*/
	unsigned char tu0[] = {0x00,0x00,0x00,0x00,0x3F,0xFE,0x44,0x02,0x47,0xF2,0x4C,0x22,0x53,0x62,0x40,0x82,0x43,0x62,0x5E,0x1A,0x41,0x82,0x40,0x62,0x43,0x02,0x40,0xC2,0x40,0x22,0x3F,0xFC};/*"图"*/
	unsigned char xiang1[] = {0x00,0x00,0x10,0x80,0x11,0xF0,0x1E,0x10,0x23,0xF8,0x2C,0x46,0x28,0x42,0x67,0xFA,0x60,0x84,0x23,0x46,0x2C,0xB8,0x23,0x20,0x2C,0xF0,0x23,0x28,0x2C,0x26,0x23,0x40};/*"像"*/
	unsigned char state0[] = {0x00,0x00,0x00,0x00,0x7F,0xFF,0x40,0x01,0x40,0x01,0x40,0x01,0x40,0x01,0x40,0x01,0x40,0x01,0x40,0x01,0x40,0x01,0x40,0x01,0x40,0x01,0x40,0x01,0x40,0x01,0x7F,0xFF};/*"□"*/
	unsigned char state1[] = {0x00,0x00,0x00,0x00,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF};/*"■"*/
	unsigned char xiang2[] = {0x00,0x00,0x00,0x00,0x10,0xFE,0x11,0x02,0x11,0x02,0x7F,0x02,0x19,0xFE,0x15,0x02,0x35,0x02,0x33,0x02,0x51,0xFE,0x51,0x02,0x11,0x02,0x11,0x02,0x11,0x02,0x00,0xFC};/*"相"*/
//	变量定义
	register unsigned char i;
	switch(index){
		case 0:
			switch(num){
				case 0://彩色图像
					for(i = 0; i < 32; i++) nom[i] = cai0[i];
					for(i = 0; i < 32; i++) nom[32+i] = se0[i];
					for(i = 0; i < 32; i++) nom[64+i] = tu0[i];
					for(i = 0; i < 32; i++) nom[96+i] = xiang1[i];
					return 4;
				case 1://灰度图像
					for(i = 0; i < 32; i++) nom[i] = hui0[i];
					for(i = 0; i < 32; i++) nom[32+i] = du0[i];
					for(i = 0; i < 32; i++) nom[64+i] = tu0[i];
					for(i = 0; i < 32; i++) nom[96+i] = xiang1[i];
					return 4;
				case 10://平衡相关
					for(i = 0; i < 32; i++) nom[i] = ping0[i];
					for(i = 0; i < 32; i++) nom[32+i] = heng0[i];
					for(i = 0; i < 32; i++) nom[64+i] = xiang2[i];
					for(i = 0; i < 32; i++) nom[96+i] = guan0[i];
					return 4;
				case 11://转速相关
					for(i = 0; i < 32; i++) nom[i] = zhuan0[i];
					for(i = 0; i < 32; i++) nom[32+i] = su0[i];
					for(i = 0; i < 32; i++) nom[64+i] = xiang2[i];
					for(i = 0; i < 32; i++) nom[96+i] = guan0[i];
					return 4;
			}
		case 1:
			switch(num){
				
			}
			break;
		case 2:
			switch(num){
				case 0://转速
					for(i = 0; i < 32; i++) nom[i] = zhuan0[i];
					for(i = 0; i < 32; i++) nom[32+i] = su0[i];
					return 2;
				case 1://转向角
					for(i = 0; i < 32; i++) nom[i] = zhuan0[i];
					for(i = 0; i < 32; i++) nom[32+i] = xiang0[i];
					for(i = 0; i < 32; i++) nom[64+i] = jiao0[i];
					return 3;
				case 2://平衡点
					for(i = 0; i < 32; i++) nom[i] = ping0[i];
					for(i = 0; i < 32; i++) nom[32+i] = heng0[i];
					for(i = 0; i < 32; i++) nom[64+i] = dian0[i];
					return 3;
			}
		case 3:
			switch(num){
				case 0://俯仰角
					for(i = 0; i < 32; i++) nom[i] = fu0[i];
					for(i = 0; i < 32; i++) nom[32+i] = yang0[i];
					for(i = 0; i < 32; i++) nom[64+i] = jiao0[i];
					return 3;
				case 1://航向角
					for(i = 0; i < 32; i++) nom[i] = hang0[i];
					for(i = 0; i < 32; i++) nom[32+i] = xiang0[i];
					for(i = 0; i < 32; i++) nom[64+i] = jiao0[i];
					return 3;
				case 2://左轮转速
					for(i = 0; i < 32; i++) nom[i] = zuo0[i];
					for(i = 0; i < 32; i++) nom[32+i] = lun0[i];
					for(i = 0; i < 32; i++) nom[64+i] = zhuan0[i];
					for(i = 0; i < 32; i++) nom[96+i] = su0[i];
					return 4;
				case 3://右轮转速
					for(i = 0; i < 32; i++) nom[i] = you0[i];
					for(i = 0; i < 32; i++) nom[32+i] = lun0[i];
					for(i = 0; i < 32; i++) nom[64+i] = zhuan0[i];
					for(i = 0; i < 32; i++) nom[96+i] = su0[i];
					return 4;
			}
		case 100:
			switch(menu_index){
				case 0:
					switch(num){
						case 0://开
							for(i = 0; i < 32; i++) nom[i] = kai0[i];
							return 1;
						case 1://关
							for(i = 0; i < 32; i++) nom[i] = guan0[i];
							return 1;
						case 2:
							for(i = 0; i < 32; i++) nom[i] = state1[i];
							return 1;
							break;
						case 3:
							for(i = 0; i < 32; i++) nom[i] = state0[i];
							return 1;
						case 4://功能
							for(i = 0; i < 32; i++) nom[i] = gong0[i];
							for(i = 0; i < 32; i++) nom[32+i] = neng0[i];
							return 2;
						case 5://状态
							for(i = 0; i < 32; i++) nom[i] = zhuang0[i];
							for(i = 0; i < 32; i++) nom[32+i] = tai0[i];
							return 2;
					}
			}
	}
	return 0;
}
/*------------------------------*/
/*		   基础信息存放			*/
/*==============================*/
static char info_found(unsigned char index, unsigned char num){
//	字模数组
	unsigned char can0[] = {0x00,0x00,0x02,0x00,0x04,0x20,0x08,0x10,0x3F,0xF8,0x01,0x08,0x7F,0xFE,0x02,0x40,0x04,0x20,0x08,0x98,0xF3,0x07,0x0C,0x20,0x00,0xCC,0x1F,0x18,0x00,0x60,0x1F,0x80};/*"参"*/
	unsigned char shu0[] = {0x00,0x00,0x00,0x00,0x25,0x20,0x25,0x20,0x26,0x3E,0x7F,0xA4,0x0E,0x64,0x35,0x64,0x44,0x24,0x08,0x24,0x7F,0x18,0x11,0x18,0x21,0x18,0x1A,0x14,0x06,0x24,0x79,0xC2};/*"数"*/
	unsigned char ming0[] = {0x00,0x00,0x00,0x00,0x02,0x00,0x07,0xF8,0x18,0x08,0x28,0x08,0x46,0x10,0x01,0x20,0x00,0xC0,0x0F,0xFC,0xF8,0x02,0x08,0x02,0x08,0x02,0x08,0x02,0x08,0x02,0x0F,0xFE};/*"名"*/
	unsigned char zhi0[] = {0x00,0x00,0x10,0x40,0x10,0x40,0x2F,0xFE,0x20,0x40,0x20,0x40,0x23,0xFC,0x64,0x04,0x63,0xFC,0xA4,0x04,0x23,0xFC,0x24,0x04,0x23,0xFC,0x24,0x04,0x24,0x04,0x2F,0xFE};/*"值"*/
	unsigned char tong0[] = {0x00,0x00,0x00,0x00,0x27,0xFC,0x23,0x08,0x10,0xF0,0x01,0xF8,0x06,0x46,0x74,0x42,0x17,0xFE,0x14,0x42,0x17,0xFE,0x14,0x42,0x14,0x42,0x14,0x12,0x28,0x0C,0x47,0xFE};/*"通"*/
	unsigned char dao0[] = {0x00,0x00,0x00,0x00,0x41,0x10,0x2F,0xFE,0x10,0x40,0x01,0xF8,0x06,0x04,0x74,0x04,0x17,0xFC,0x14,0x04,0x17,0xFC,0x14,0x04,0x14,0x04,0x13,0xF8,0x2C,0x00,0x43,0xFE};/*"道"*/
	unsigned char zui0[] = {0x00,0x00,0x00,0x00,0x3F,0xF8,0x3F,0xF8,0x20,0x08,0x3F,0xF8,0x00,0x00,0x7F,0xFE,0x21,0x00,0x3F,0xFC,0x21,0x44,0x3F,0x28,0x21,0x28,0x21,0x90,0x7F,0x2C,0x01,0xC2};/*"最"*/
	unsigned char da0[] = {0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x7F,0xFE,0x01,0x00,0x01,0x80,0x01,0x80,0x02,0x40,0x02,0x40,0x04,0x20,0x04,0x20,0x08,0x10,0x10,0x08,0x60,0x06};/*"大"*/
	unsigned char xiao0[] = {0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x11,0x08,0x11,0x08,0x11,0x04,0x21,0x04,0x21,0x04,0x21,0x04,0x41,0x02,0x41,0x02,0x81,0x02,0x01,0x00,0x0F,0x00};/*"小"*/
//	变量定义
	register unsigned char i;
	switch(index){
		case 1:
			switch(num){			
				case 0://参数名
					for(i = 0; i < 32; i++) nom[i] = can0[i];
					for(i = 0; i < 32; i++) nom[32+i] = shu0[i];
					for(i = 0; i < 32; i++) nom[64+i] = ming0[i];
					return 3;
				case 1://参数值
					for(i = 0; i < 32; i++) nom[i] = can0[i];
					for(i = 0; i < 32; i++) nom[32+i] = shu0[i];
					for(i = 0; i < 32; i++) nom[64+i] = zhi0[i];
					return 3;
			}
			break;
		case 2:
			switch(num){
				case 0://通道
					for(i = 0; i < 32; i++) nom[i] = tong0[i];
					for(i = 0; i < 32; i++) nom[32+i] = dao0[i];
					return 2;
				case 1://最大值
					for(i = 0; i < 32; i++) nom[i] = zui0[i];
					for(i = 0; i < 32; i++) nom[32+i] = da0[i];
					for(i = 0; i < 32; i++) nom[64+i] = zhi0[i];
					return 3;
				case 2://最小值
					for(i = 0; i < 32; i++) nom[i] = zui0[i];
					for(i = 0; i < 32; i++) nom[32+i] = xiao0[i];
					for(i = 0; i < 32; i++) nom[64+i] = zhi0[i];
					return 3;
			}
	}
}
/*------------------------------*/
/*		   名称显示模块			*/
/*==============================*/
void menu2_display(void){
//	清屏
	ips200_clear(WHITE);
//	显示名称
	switch(menu_index){
		case 0:
			switch(menu2mode){
			//	页面模板选择
				case 0:
					ips200_display_chinese(0, 208, 16, nom, info(100, 4), 0XFDF8);
					ips200_display_chinese(120, 208, 16, nom, info(100, 5), 0XFDF8);
					ips200_display_chinese(90, 224, 16, nom, info(100, 0), 0XFDF8);
					ips200_display_chinese(170, 224, 16, nom, info(100, 1), 0XFDF8);
					ips200_display_chinese(0, 240, 16, nom, info(menu_index, 0), 0xB6DB);
					ips200_display_chinese(0, 256, 16, nom, info(menu_index, 1), 0xB6DB);
					break;
				case 1:
					ips200_display_chinese(0, 208, 16, nom, info(100, 4), 0XFDF8);
					ips200_display_chinese(120, 208, 16, nom, info(100, 5), 0XFDF8);
					ips200_display_chinese(90, 224, 16, nom, info(100, 0), 0XFDF8);
					ips200_display_chinese(150, 224, 16, nom, info(100, 1), 0XFDF8);
					ips200_display_chinese(0, 240, 16, nom, info(menu_index, 10), 0xB6DB);
					ips200_display_chinese(0, 256, 16, nom, info(menu_index, 11), 0xB6DB);
					break;
			}
			break;
		case 1:
			switch(menu2mode){
				case 0:
					ips200_display_chinese(0, 208, 16, nom, info_found(2, 0), 0XFDF8);//通道
					ips200_display_chinese(100, 208, 16, nom, info_found(2, 1), 0XFDF8);//最大值
					ips200_display_chinese(170, 208, 16, nom, info_found(2, 2), 0XFDF8);//最小值
					ips200_showstr(0, 15, "ADC0");
					ips200_showstr(0, 16, "ADC1");
					ips200_showstr(0, 17, "ADC2");
					ips200_showstr(0, 18, "ADC3");
					ips200_showstr(0, 19, "ADC4");
					break;
				case 1:
					ips200_display_chinese(0, 208, 16, nom, info_found(menu_index, 0), 0XFDF8);//MM32在局部数组定义过多时会导致结构体变量被覆盖，所以单独调用基础信息模块
					ips200_display_chinese(120, 208, 16, nom, info_found(menu_index, 1), 0XFDF8);
					ips200_showstr(0, 15, "Kp");
					ips200_showstr(0, 16, "Kd");
					break;
				case 2:
					ips200_display_chinese(0, 208, 16, nom, info_found(menu_index, 0), 0XFDF8);
					ips200_display_chinese(120, 208, 16, nom, info_found(menu_index, 1), 0XFDF8);
					ips200_showstr(0, 15, "alpha");
					ips200_showstr(0, 16, "beta");
					ips200_showstr(0, 17, "omega");
					ips200_showstr(0, 18, "P");
					break;
			}
			break;
		case 2:
			switch(menu2mode){
				case 0:
					ips200_display_chinese(0, 208, 16, nom, info(1, 0), 0XFDF8);
					ips200_display_chinese(120, 208, 16, nom, info(1, 1), 0XFDF8);
					ips200_display_chinese(0, 240, 16, nom, info(menu_index, 0), 0xB6DB);
					ips200_display_chinese(0, 256, 16, nom, info(menu_index, 1), 0xB6DB);
					ips200_display_chinese(0, 272, 16, nom, info(menu_index, 2), 0xB6DB);
					break;
			}
			break;
		case 3:
			switch(menu[menu_index]){
				case 0:
					ips200_showstr(0, 15, "error");
					ips200_showstr(0, 16, "result");
					break;
				case 1:
					ips200_display_chinese(0, 208, 16, nom, info_found(2, 0), 0XFDF8);//通道
					ips200_display_chinese(120, 208, 16, nom, info_found(1, 1), 0XFDF8);//参数值
					ips200_showstr(0, 15, "ADC0");
					ips200_showstr(0, 16, "ADC1");
					ips200_showstr(0, 17, "ADC2");
					ips200_showstr(0, 18, "ADC3");
					ips200_showstr(0, 19, "ADC4");
					break;
			}
			break;
	}
//	显示数值
	menu2value();
} 
/*------------------------------*/
/*		   数值监视模块			*/
/*==============================*/
void monitor(void){
	switch(menu[menu_index]){
		case 0:
			ips200_showint16(120, 15, adc_err.rs);
			ips200_showint16(120, 16, adc_steering.rs);
			switch(act_sta){
				case 0:ips200_showstr(120, 17, "OTHERS  ");break;
				case 1:ips200_showstr(120, 17, "IN_RING ");break;
				case 2:ips200_showstr(120, 17, "OUT_RING");break;
				case 3:ips200_showstr(120, 17, "IO_RING ");break;
			}
			break;
		case 1:
			ips200_showuint8(120, 15, adc0.value);
			ips200_showuint8(120, 16, adc1.value);
			ips200_showuint8(120, 17, adc2.value);
			ips200_showuint8(120, 18, adc3.value);
			ips200_showuint8(120, 19, adc4.value);
			break;
	}
}
/*------------------------------*/
/*		   固定监视模块			*/
/*==============================*/
void fixed_monitor(void){
	switch(fixedindex){
		case 0:
		//	名称显示
			ips200_showstr(0, 0, "error");
			ips200_showstr(0, 1, "result");
		//	数值显示
			ips200_showint16(120, 0, adc_err.rs);
			ips200_showint16(120, 1, adc_steering.rs);
			break;
		case 1:
		//	名称显示
			ips200_showstr(0, 0, "ADC0");
			ips200_showstr(0, 1, "ADC1");
			ips200_showstr(0, 2, "ADC2");
			ips200_showstr(0, 3, "ADC3");
			ips200_showstr(0, 4, "ADC4");
		//	数值显示
			ips200_showuint8(120, 0, adc0.value);
			ips200_showuint8(120, 1, adc1.value);
			ips200_showuint8(120, 2, adc2.value);
			ips200_showuint8(120, 3, adc3.value);
			ips200_showuint8(120, 4, adc4.value);
			break;
	}
}
/*------------------------------*/
/*		   按钮状态模块			*/
/*==============================*/
void swdisplay(void){
//	变量定义
	register unsigned char i;
	switch(menu[menu_index]){
	//	按钮状态
		case 0:
			for(i=0; i<CSIMENU_FLAG; i++){
				if(csimenu_flag[i]){
					ips200_display_chinese(90, 240+16*i, 16, nom, info(100, 2), 0xB6DB);
					ips200_display_chinese(170, 240+16*i, 16, nom, info(100, 3), 0xB6DB);
				}
				else{
					ips200_display_chinese(90, 240+16*i, 16, nom, info(100, 3), 0xB6DB);
					ips200_display_chinese(170, 240+16*i, 16, nom, info(100, 2), 0xB6DB);
				}
			}
			//	高亮选项
			if(csimenu_flag[menu2_index]) ips200_display_chinese(90, 240+16*menu2_index, 16, nom, info(100, 2), 0XB7BD);
			else ips200_display_chinese(170, 240+16*menu2_index, 16, nom, info(100, 2), 0XF5BA);
			break;
		case 1:
			for(i=0; i<WIRELESS_FLAG; i++){
				if(wireless_flag[i]){
					ips200_display_chinese(90, 240+16*i, 16, nom, info(100, 2), 0xB6DB);
					ips200_display_chinese(170, 240+16*i, 16, nom, info(100, 3), 0xB6DB);
				}
				else{
					ips200_display_chinese(90, 240+16*i, 16, nom, info(100, 3), 0xB6DB);
					ips200_display_chinese(170, 240+16*i, 16, nom, info(100, 2), 0xB6DB);
				}
			}
			//	高亮选项
			if(wireless_flag[menu2_index]) ips200_display_chinese(90, 240+16*menu2_index, 16, nom, info(100, 2), 0XB7BD);
			else ips200_display_chinese(170, 240+16*menu2_index, 16, nom, info(100, 2), 0XF5BA);
			break;
	}
}
/*------------------------------*/
/*		   按钮切换模块			*/
/*==============================*/
static void swmode(void){
//	变量定义
	unsigned char temp, i;
//	开关操作
	switch(menu[menu_index]){
		case 0:
			fixedflag = 0;
			tim_interrupt_disabnle(TIM_6);
			temp = csimenu_flag[menu2_index];
			for(i=0; i<CSIMENU_FLAG; i++) csimenu_flag[i] = 0;
			csimenu_flag[menu2_index] = !temp;
			break;
		case 1:
			temp = wireless_flag[menu2_index];
			for(i=0; i<WIRELESS_FLAG; i++) wireless_flag[i] = 0;
			wireless_flag[menu2_index] = !temp;
//			if(!temp) tim_interrupt_init(TIM_8, 20, 0, 3);
//			else tim_interrupt_disabnle(TIM_8);
			break;
	}
}
/*------------------------------*/
/*		   数值修改模块			*/
/*==============================*/
static void modify(unsigned char index, unsigned char event, unsigned char mode){
	switch(mode){
		case 1:
			if(event){
		//	数值增加
				switch(index){
					case 0:*value0+=mag[magindex];break;
					case 1:*value1+=mag[magindex];break;
					case 2:*value2+=mag[magindex];break;
					case 3:*value3+=mag[magindex];break;
					case 4:*value4+=mag[magindex];break;
				}
			}
		//	数值减少
			else{
				switch(index){
					case 0:*value0-=mag[magindex];break;
					case 1:*value1-=mag[magindex];break;
					case 2:*value2-=mag[magindex];break;
					case 3:*value3-=mag[magindex];break;
					case 4:*value4-=mag[magindex];break;
				}
			}
			break;
		case 2:
			if(event){
				switch(index){
					case 0: *shortvalue0+=mag[magindex];break;
					case 1: *shortvalue1+=mag[magindex];break;
					case 2: *shortvalue2+=mag[magindex];break;
				}
			}
			else{
				switch(index){
					case 0: *shortvalue0-=mag[magindex];break;
					case 1: *shortvalue1-=mag[magindex];break;
					case 2: *shortvalue2-=mag[magindex];break;
				}
			}
	}
}
/*------------------------------*/
/*		 二级菜单选择模块		*/
/*==============================*/
void menu2_select(unsigned char event){
//	变量定义
	register unsigned char i;
//	返回键
	if(event == 3){
	//	重置
		menu2_level = 0;
		magindex = 1;
		monitorflag = 0;
		menu_level = 0;
		menu_display();
		excollflag = 0;
	//	操作定时器
		tim_interrupt_init_ms(TIM_2, 2, 0, 0);//开启采集通道
		if(!fixedflag) tim_interrupt_disabnle(TIM_6);//除固定显示外关闭监视通道
		return;
	}
	if(!menu2_level){
		switch(event){
			case 6://右按键
				break;
			
			case 4://左按键
				break;
			
			case 2://上按键
				if(menu2_index > 0) menu2_index--;
				break;
			
			case 5://下按键
				if(menu2_index < menu2_limit) menu2_index++;
				break;
			
			case 1://确定键
				switch(menu2flag){
					case 1://	数值修改
						menu2_level = 1;
						break;
					case 2://	监视器固定显示
						fixedflag = !fixedflag;
						fixedindex = menu[menu_index];
						if(fixedflag) for(i=0; i<CSIMENU_FLAG; i++) csimenu_flag[i] = 0;//关闭图像显示
						break;
					case 3://	按钮
						swmode();
						swdisplay();
						break;
					case 4://	电磁设置
					//	重置最值
						if(!excollflag){
							switch(menu2_index){
								case 0:
									adc0.max = 0, adc0.min = 4095;
									adc1.max = 0, adc1.min = 4095;
									adc2.max = 0, adc2.min = 4095;
									adc3.max = 0, adc3.min = 4095;
									adc4.max = 0, adc4.min = 4095;
									excollflag = 6;
									break;
								case 1:adc0.max = 0, adc0.min = 4095, excollflag = 1;break;
								case 2:adc1.max = 0, adc1.min = 4095, excollflag = 2;break;
								case 3:adc2.max = 0, adc2.min = 4095, excollflag = 3;break;
								case 4:adc3.max = 0, adc3.min = 4095, excollflag = 4;break;
								case 5:adc4.max = 0, adc4.min = 4095, excollflag = 5;break;
							}
							tim_interrupt_init(TIM_6, 40, 0, 3);
						}
						else{excollflag = 0;tim_interrupt_disabnle(TIM_6);} 
						break;
					case 0:break;
				}
				break;
		}
		menu2value();
	}
	else{
		switch(event){
			case 6://右按键
				if(magindex < 4) magindex++;
				break;
			
			case 4://左按键
				if(magindex > 0) magindex--; 
				break;
			
			case 2://上按键
				modify(menu2_index, 1, menu_index);
				break;
			
			case 5://下按键
				modify(menu2_index, 0, menu_index);
				break;
			
			case 1://确定键
				menu2_level = 0;
				break;
		}
		menu2value();
	}
}