/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "data.h"
/*--------------------------------------------------------------*/
/*							  宏定义							*/
/*==============================================================*/
#define u8 unsigned char                                    //8位数据
#define u16 unsigned short                                  //16位数据
#define u32 unsigned int                                    //32位数据

/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/
/*----------------------*/
/*	    线性CCD模块		*/
/*======================*/
unsigned cjug_sta;
/*----------------------*/
/*	 	 MOTOR模块		*/
/*======================*/
//	编码器
short lcod = 0,rcod = 0;
short spd = 90;
short rad = 0;
//	PID
struct pidpara speed;
struct pidpara steer;
struct pidpara angle;
struct pidpara acw;
struct pidpara lmor;
struct pidpara rmor;
/*----------------------*/
/*	   角度控制模块		*/
/*======================*/
short blcp = 23;
short pita;
short yawa[2];
short gy, gz;
short yfilt[4] = {0, 0, 0, 0};
short pflit[4] = {0, 0, 0, 0};
/*----------------------*/
/*	 	 菜单模块		*/
/*======================*/
//	汉字数组
unsigned char nom[128];
//	一级菜单
unsigned char menu[ROWS];
unsigned char menu_level;
unsigned char menu_index = 0;
//	二级菜单
unsigned char menu2_index = 0;
unsigned char menu2_level = 0;
//	标志位
unsigned char fixedflag = 0;//固定显示
unsigned char monitorflag = 0;//监视器
unsigned char csimenu_flag[CSIMENU_FLAG] = {0, 0};//摄像头
unsigned char wireless_flag[WIRELESS_FLAG] = {0, 0};//无线数据
//	函数指针
void(*menu_pfc[])(unsigned char) = {menu_select, menu2_select};
/*----------------------*/
/*	 	 通信模块		*/
/*======================*/
unsigned char buff_get6, buff_get7;
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
/*----------------------*/
/*	 	参数初始化		*/
/*======================*/
//	PID
void Init_para(void){
//	左轮转向
	lmor.alpha = 0.15;
	lmor.Kp = 30;
	lmor.Ki = 0.45;
	lmor.Kd = 0;
	lmor.I = 0;
	lmor.e1 = 0, lmor.e2 = 0, lmor.e3 = 0;
	lmor.rs = 0;
//	右轮转向
	rmor.alpha = 0.15;
	rmor.Kp = 30;
	rmor.Ki = 0.45;
	rmor.Kd = 0;
	rmor.I = 0;
	rmor.e1 = 0, rmor.e2 = 0, rmor.e3 = 0;
	rmor.rs = 0;	
//	转向
	steer.alpha = 0.1;
	steer.Kp = 24;//反应快慢 | 超调
	steer.Ki = 0;//反应力度
	steer.Kd = 6.4;
	steer.I = 0;
	steer.e1 = 0, steer.e2 = 0, steer.e3 = 0;
	steer.rs = 0;
}
