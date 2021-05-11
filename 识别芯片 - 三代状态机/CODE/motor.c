/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "motor.h"
#include "pid.h"
#include "Init.h"
#include "zf_pwm.h"
#include "zf_tim.h"
#include "SEEKFREE_VIRSCO.h"
#include "SEEKFREE_WIRELESS.h"
#include "SEEKFREE_IPS200_PARALLEL8.h"
/*--------------------------------------------------------------*/
/*							  宏定义							*/
/*==============================================================*/
#define u8 unsigned char                                    //8位数据
#define u16 unsigned short                                  //16位数据
#define u32 unsigned int                                    //32位数据
/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/
//unsigned char temp[4] = {0, 0, 0, 0};
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
/*------------------------------*/
/*	     编码器值获取模块		*/
/*==============================*/
void encoder_get(void){
//	获取编码器计数值后重置
	lcod = tim_encoder_get_count(TIM_3);
	rcod = -tim_encoder_get_count(TIM_4);
}
/*------------------------------*/
/*		   电机驱动模块			*/
/*==============================*/
void motor_act(void){
	if(acw.rs >= 0){	//正转
	//	左电机
		pwm_duty_updata(TIM_5, MOTOR_L1, acw.rs+steer.rs);     		
		pwm_duty_updata(TIM_5, MOTOR_L0, 0);
	//	右电机
		pwm_duty_updata(TIM_5, MOTOR_R1, acw.rs-steer.rs);     		
		pwm_duty_updata(TIM_5, MOTOR_R0, 0);
	}
	else{	//反转
	//	左电机
		pwm_duty_updata(TIM_5, MOTOR_L1, 0);                      		
		pwm_duty_updata(TIM_5, MOTOR_L0, -acw.rs-steer.rs);
	//	右电机
		pwm_duty_updata(TIM_5, MOTOR_R1, 0);                      		
		pwm_duty_updata(TIM_5, MOTOR_R0, -acw.rs+steer.rs);
	}
}
