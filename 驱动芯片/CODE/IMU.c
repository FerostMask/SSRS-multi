/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "IMU.h"
#include "pid.h"
#include "math.h"
#include "motor.h"
#include "SEEKFREE_VIRSCO.h"
#include "SEEKFREE_TSL1401.h"
#include "SEEKFREE_WIRELESS.h"
#include "SEEKFREE_ICM20602.h"
#include "SEEKFREE_IPS200_PARALLEL8.h"
/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/
//unsigned char temp[4] = {0, 0, 0, 0};
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
/*----------------------*/
/*	   角度控制模块		*/
/*======================*/
void angle_ctrl(void){
//	变量定义
	register char i ;
	static unsigned char imu_count;
//	计数器循环
	imu_count = (imu_count+1)&3;
//	角速度滤波
	for(i = 2; i >= 0; i--) yfilt[i+1] = yfilt[i];
	yfilt[0] = icm_gyro_y;
	gy = (yfilt[0] + yfilt[1] + yfilt[2] + yfilt[3])/65.6;
//	串级PID
	if(imu_count == 3){
	//	偏转角速度、速度PID
		gz = icm_gyro_z/(16.4*5.73);
		pos_pid(&steer, rad, gz, 1200, -1200);
		inc_pid(&speed, spd, (lcod+rcod)>>1, 50);
	}
//	角度
	if(imu_count == 1 || imu_count == 3){
	//	计算俯仰角、车身角速度	
		for(i = 2; i >= 0; i--) pflit[i+1] = pflit[i];
	}
//	角速度、电机控制
	inc_pid(&acw, angle.rs, gy, 5000);
	motor_act();
}
