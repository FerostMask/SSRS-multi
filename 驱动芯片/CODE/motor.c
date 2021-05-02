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
#include "SEEKFREE_ICM20602.h"
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
//unsigned char motor_temp[4] = {0, 0, 0, 0};
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
/*------------------------------*/
/*	     编码器值获取模块		*/
/*==============================*/
void encoder_get(void){
//	获取编码器计数值后重置
	lcod = -tim_encoder_get_count(TIM_3);
	rcod = tim_encoder_get_count(TIM_4);
//	显示计数值
//	ips200_showint16(0, 0, lcod);
//	ips200_showint16(0, 1, rcod);
}
/*------------------------------*/
/*		   电机驱动模块			*/
/*==============================*/
void motor_act(void){
//	变量定义
	static unsigned char motor_count;
	motor_count = (motor_count+1)&0x01;
//	差速计算
	if(motor_count){
		gz = icm_gyro_z/(16.4*5.73);
		pos_pid(&steer, rad, gz, 80, -80);
	}
//	转速控制
	inc_pid(&lmor, spd+steer.rs, lcod, 5000);
	inc_pid(&rmor, spd-steer.rs, rcod, 5000);	
//	左电机占空比设置
	if(lmor.rs >= 0){	//正转
		pwm_duty_updata(TIM_5, MOTOR_L1, lmor.rs);     		
		pwm_duty_updata(TIM_5, MOTOR_L0, 0);                     		
	}
	else{	//反转
		pwm_duty_updata(TIM_5, MOTOR_L1, 0);                      		
		pwm_duty_updata(TIM_5, MOTOR_L0, -lmor.rs);    		
	}	
//	右电机占空比设置
	if(rmor.rs >= 0){	//正转	
		pwm_duty_updata(TIM_5, MOTOR_R1, rmor.rs);     		
		pwm_duty_updata(TIM_5, MOTOR_R0, 0);                      		
	}
	else{	//反转
		pwm_duty_updata(TIM_5, MOTOR_R1, 0);                      		
		pwm_duty_updata(TIM_5, MOTOR_R0, -rmor.rs);    		
	}
//	数据发送
//	data_conversion(lcod, rcod, spd, 0, motor_temp);
}
