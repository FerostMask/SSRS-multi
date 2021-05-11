/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "data.h"
#include "eident.h"
#include "CAM.h"
#include "ctrl.h"
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
/*	 	摄像头模块		*/
/*======================*/
//	阈值
unsigned short imgthrsod = 28000;
//	识别状态
unsigned char cjug_sta;
//	左右边界（存列）
unsigned char lefp, rigp, midp;
//	斜率
float lefslope, rigslope, midslope;
//	ver3
float P[256] = {0}, PK[256] = {0}, MK[256] = {0};
unsigned char binary_img[MT9V03X_H][(MT9V03X_W-4)/8];
unsigned char img_thrsod;
//	基准点寻找
unsigned char found_point[4];
unsigned char fop_flag;
//	水平边线寻找
unsigned char lefbor[MT9V03X_H], rigbor[MT9V03X_H], mid_point[MT9V03X_H];
unsigned char ltraf_point_row[10], rtraf_point_row[10], ltraf_point_col[10], rtraf_point_col[10];
unsigned char ltraf_flag[10], rtraf_flag[10];
unsigned char ltraf_count, rtraf_count;
unsigned char lcut, rcut;
//	垂直边线寻找
unsigned char topbor[MT9V03X_W-4], bottombor[MT9V03X_W-4];//上下边线
unsigned char lvet_trafpoint_row[5], rvet_trafpoint_row[5], lvet_trafpoint_col[5], rvet_trafpoint_col[5];//转变点
unsigned char lvet_trafcount, rvet_trafcount;//转变点计数
unsigned char leftop_cut, lefbottom_cut, rigtop_cut, rigbottom_cut;//截止列
unsigned char exti_lefp[4], exti_rigp[4], exti_lefcount, exti_rigcount;//出口
//	状态机
unsigned char act_flag, act_flag_temp, fragile_flag;
unsigned char state, state_temp;
unsigned char state_flag;
unsigned short img_color = 0xAE9C;
void(*state_pfc[])(void) = {state_machine_enter, state_machine_bend, state_machine_ring, state_machine_cross, state_machine_fork};
//	控制相关
short p_target[2];
short error_flit[8], ctrl_error1, ctrl_error2;
short spd_slow;
void(*ctrl_pfc[])(void) = {cam_ctrl_direct, cam_ctrl_bend, cam_ctrl_ring, cam_ctrl_cross, cam_ctrl_fork};
/*----------------------*/
/*	 	 电磁模块		*/
/*======================*/
//	结构体定义
struct adcpara adc0;
struct adcpara adc1;
struct adcpara adc2;
struct adcpara adc3;
struct adcpara adc4;
struct adcerrpa adc_err;
//	状态标志位
unsigned char ajug_sta, act_sta;
unsigned short Kp_act = 50;
short spd, spd_bias;
short spd_adcset = 40;
char rad_bias = 0;
//	指针函数
void(*adc_pfc[])(void) = {cross_road};
/*----------------------*/
/*	 	 控制模块		*/
/*======================*/
//	PID
struct pidpara adc_steering;
struct pidpara adc_straight;
struct pidpara cam_steering;
/*----------------------*/
/*	 	 菜单模块		*/
/*======================*/
//	汉字数组
unsigned char nom[128];
//	一级菜单
unsigned char menu[ROWS] = {0, 0, 0, 0};
unsigned char menu_level;
unsigned char menu_index = 0;
//	二级菜单
unsigned char menu2_index = 0;
unsigned char menu2_level = 0;
//	标志位
unsigned char excollflag = 0;//电磁极值采集标志位
unsigned char fixedflag = 0;//固定显示
unsigned char monitorflag = 0;//监视器
unsigned char csimenu_flag[CSIMENU_FLAG] = {0, 0};//摄像头
unsigned char wireless_flag[WIRELESS_FLAG] = {0, 0};//无线数据
//	指针函数
void(*menu_pfc[])(unsigned char) = {menu_select, menu2_select};
/*----------------------*/
/*	 	 有来有去		*/
/*======================*/
unsigned char subuff_num = 0;
unsigned char subuff_arr[3];
unsigned short subuff_ranging;
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
/*----------------------*/
/*	 	参数初始化		*/
/*======================*/
void Init_para(void){
//	电磁模块
	adc0.max = 3823, adc0.min = 102;
	adc1.max = 3815, adc1.min = 98;
	adc2.max = 4095, adc2.min = 0;
	adc3.max = 3815, adc3.min = 117;
	adc4.max = 3827, adc4.min = 38;
//	差比和差
	adc_err.alpha = 16;//内外环转向角度变化、原差比和参数
	adc_err.beta = 32;//转弯角度变化
	adc_err.omega = 6.5;//转弯平滑度
	adc_err.P = 25;//放大倍数
//	ADC转向
	adc_steering.Kp = 2.66;
	adc_steering.Kd = 11.6;
//	ADC直道
	adc_straight.Kp = 1;
	adc_straight.Kd = 1;
//	CAM转向
	cam_steering.Kp = 0.9;
	cam_steering.Kd = 1.1;
////	速度
//	speed.alpha = 0.3;
//	speed.Kp = 0.1;//反应快慢 | 超调
//	speed.Ki = 0.05;//反应力度
//	speed.Kd = 0;
//	speed.I = 0;
//	speed.e1 = 0, speed.e2 = 0, speed.e3 = 0;
//	speed.rs = 0;
}
