#ifndef _DATA_H
#define _DATA_H
#include "menu.h"
#include "menu_2.h"
#include "SEEKFREE_MT9V03X.h"
/*--------------------------------------------------------------*/
/*							  宏定义							*/
/*==============================================================*/
//	按键标志位
#define CSIMENU_FLAG 2
#define WIRELESS_FLAG 2
//	一级菜单
#define ROWS 4
#define menu_limit0 0
#define menu_limit1 2
#define menu_limit2 0
#define menu_limit3 1
//	摄像头
#define EFF_ROW 54
/*------------------------------------------------------*/
/* 					  外部变量声明 						*/
/*======================================================*/
/*----------------------*/
/*	 	摄像头模块		*/
/*======================*/
extern unsigned char cjug_sta;
//	图像ver2
extern unsigned char lefbor[MT9V03X_H], rigbor[MT9V03X_H], mid_point[MT9V03X_H];
extern unsigned char lefp, rigp, midp;
extern unsigned short imgthrsod;
extern float lefslope, rigslope, midslope;
//	ver3
extern float P[256], PK[256], MK[256];
extern unsigned char binary_img[MT9V03X_H][(MT9V03X_W-4)/8];
extern unsigned char img_thrsod;
//	基准点寻找
extern unsigned char found_point[4];
extern unsigned char fop_flag;
//	水平边界寻找
extern unsigned char ltraf_point_row[10], rtraf_point_row[10], ltraf_point_col[10], rtraf_point_col[10];
extern unsigned char ltraf_flag[10], rtraf_flag[10];
extern unsigned char lcut, rcut;
extern unsigned char ltraf_count, rtraf_count;
//	垂直边线寻找
extern unsigned char topbor[MT9V03X_W-4], bottombor[MT9V03X_W-4];//上下边线
extern unsigned char lvet_trafpoint_row[5], rvet_trafpoint_row[5], lvet_trafpoint_col[5], rvet_trafpoint_col[5];
extern unsigned char lvet_trafcount, rvet_trafcount;
extern unsigned char leftop_cut, lefbottom_cut, rigtop_cut, rigbottom_cut;//截止列
extern unsigned char exti_lefp[4], exti_rigp[4], exti_lefcount, exti_rigcount;
//	状态机
extern unsigned char act_flag, act_flag_temp, fragile_flag;
extern unsigned short img_color;
extern unsigned char state, state_temp;
extern unsigned char state_flag;
extern void(*state_pfc[])(void);
//	控制相关
extern short p_target[2];
extern short error_flit[8], ctrl_error1, ctrl_error2;
extern void(*ctrl_pfc[])(void);
//
/*----------------------*/
/*	 	 电磁模块		*/
/*======================*/
//	结构体声明
typedef struct adcpara{//电磁基本参数
	unsigned short value;
	unsigned short max;
	unsigned short min;
	unsigned short pin;
}adcpara;
typedef struct adcerrpa{//差比和差参数
	float alpha, beta, omega;
	float P;
	short rs;
}adcerrpa;
//	全局结构体声明
extern struct adcpara adc0;
extern struct adcpara adc1;
extern struct adcpara adc2;
extern struct adcpara adc3;
extern struct adcpara adc4;
extern struct adcerrpa adc_err;
//	状态标志位
extern unsigned char ajug_sta;
//	指针函数
extern void(*adc_pfc[])(void);
/*----------------------*/
/*	 	 MOTOR模块		*/
/*======================*/
//	电机
extern unsigned short Kp_act;
extern unsigned char act_sta;
extern short spd, spd_bias;
extern short spd_adcset;
extern short rad;
extern char rad_bias;
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
extern struct pidpara adc_steering;
extern struct pidpara adc_straight;
extern struct pidpara cam_steering;
/*----------------------*/
/*	 	 有来有去		*/
/*======================*/
extern unsigned char subuff_num;
extern unsigned char subuff_arr[3];
extern unsigned short subuff_ranging;
/*----------------------*/
/*	 	 菜单模块		*/
/*======================*/
//	一级菜单
extern unsigned char menu_index;
extern unsigned char menu[ROWS];
extern unsigned char nom[128];
//	标志位
extern unsigned char fixedflag;
extern unsigned char monitorflag;
extern unsigned char csimenu_flag[CSIMENU_FLAG];
extern unsigned char wireless_flag[WIRELESS_FLAG];
extern unsigned char excollflag;
extern unsigned char menu_level;
//	二级菜单
extern unsigned char menu2_index;
extern unsigned char menu2_level;
//	函数指针
extern void(*menu_pfc[])(unsigned char);
/*------------------------------------------------------*/ 
/* 						函数声明 						*/
/*======================================================*/
void Init_para(void);

#endif
