#ifndef _DATA_H
#define _DATA_H
#include "menu.h"
#include "menu_2.h"
/*--------------------------------------------------------------*/
/*							  宏定义							*/
/*==============================================================*/
//	按键标志位
#define CSIMENU_FLAG 2
#define WIRELESS_FLAG 2
//	一级菜单
#define ROWS 4
#define menu_limit0 1
#define menu_limit1 3
#define menu_limit2 0
#define menu_limit3 0
/*------------------------------------------------------*/
/* 					  外部变量声明 						*/
/*======================================================*/
/*----------------------*/
/*	    线性CCD模块		*/
/*======================*/
extern unsigned cjug_sta;
/*----------------------*/
/*	 	 MOTOR模块		*/
/*======================*/
//	编码器
extern short lcod, rcod;			//编码器读数
extern short ltcod, rtcod;          //编码器目标值
//	电机
extern short spd;
extern short rad;
/*----------------------*/
/*	 	  PID模块		*/
/*======================*/
//	结构体声明
typedef struct pidpara{
	float alpha;
	float Kp;
	float Ki;
	float Kd;
	float I;
	int e1;
	int e2;
	int e3;
	short rs;
}pidpara;

//	电机
extern struct pidpara speed;
extern struct pidpara steer;
extern struct pidpara lmor;
extern struct pidpara rmor;
//	角度调定参数
extern struct pidpara angle;
//	角速度
extern struct pidpara acw;

/*----------------------*/
/*	   角度控制模块		*/
/*======================*/
extern short blcp;
extern short pita;
extern short gy, gz;
extern short yfilt[4];
extern short pflit[4];

/*----------------------*/
/*	 	 菜单模块		*/
/*======================*/
//	一级菜单
extern unsigned char menu_index;
extern unsigned char menu[ROWS];
extern unsigned char nom[128];
extern unsigned char fixedflag;
extern unsigned char monitorflag;
extern unsigned char csimenu_flag[CSIMENU_FLAG];
extern unsigned char wireless_flag[WIRELESS_FLAG];
extern unsigned char menu_level;
//	二级菜单
extern unsigned char menu2_index;
extern unsigned char menu2_level;
//	函数指针
extern void(*menu_pfc[])(unsigned char);
/*----------------------*/
/*	 	 通信模块		*/
/*======================*/
extern unsigned char buff_get6, buff_get7;
/*------------------------------------------------------*/ 
/* 						函数声明 						*/
/*======================================================*/
void Init_para(void);

#endif
