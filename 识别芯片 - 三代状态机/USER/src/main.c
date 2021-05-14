/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				main
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.24
* @Target core		MM32F3277
* @Taobao			https://seekfree.taobao.com/
* @date				2021-02-22
********************************************************************************************************************/
/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "headfile.h"
#include "CAM.h"
#include "pid.h"
#include "Init.h"
#include "data.h"
#include "eident.h" 
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
int main(void){
/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/

/*--------------------------------------------------------------*/
/* 							初始化设置 							*/
/*==============================================================*/
/*----------------------*/
/*	 	 逐飞初始化		*/
/*======================*/
 	board_init(true);																// 初始化 debug 输出串口
	//此处编写用户代码(例如：外设初始化代码等)
//	外设初始化
	ips200_init();
	mt9v03x_init();
//	引脚状态初始化
	gpio_init(B9, GPO, GPIO_HIGH, GPO_PUSH_PULL);
//	串口初始化
	uart_init(UART_7, 115200, UART7_TX_B06, UART7_RX_B07);
	uart_init(UART_6, 115200, UART6_TX_C06, UART6_RX_C07);
//	uart_init(UART_3, 115200, UART3_TX_B10, UART3_RX_B11);
//	uart_rx_irq(UART_3, 1);
/*----------------------*/
/*	 	 用户初始化		*/
/*======================*/
	eident_init();
	if(core_select()){
		Init_button();
		menu_display();
	}
	Init_para();
	tim_interrupt_init_ms(TIM_2, 2, 0, 0);
    //  岔道方向确定
    fork_turn_direction = 1;//左0 右1
/*--------------------------------------------------------------*/
/* 							 循环执行 							*/
/*==============================================================*/	
	while(1){
	//	此处编写需要循环执行的代码
		if(mt9v03x_finish_flag){
		//	大津法二值化获取阈值、图像二值化
			otsu();
			img_binary();
		//	边界寻找
			left_fop_search();
			lbor_search();
			right_fop_search();
			rbor_search();
			if(ltraf_count) border_vertical_leftsearch();
			if(rtraf_count) border_vertical_rightsearch();
		//	状态机
			state_machine();
			if(state_temp!=state) state_pfc[state_flag]();
			spd_adcset = 50;
			p_target[0] = 62, p_target[1] = (lefbor[62]+rigbor[62])>>1;
			ctrl_pfc[state_flag]();
			pos_pid(&cam_steering, 80, p_target[1], 120, -120);
			uart_putchar(UART_7, cam_steering.rs);
			spd = spd_adcset;
			uart_putchar(UART_6, spd+spd_bias);
		//	图像显示
			if(csimenu_flag[0]) binary_disp();
			if(csimenu_flag[1]) ips200_displayimage032(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
		}
	}
}
// **************************** 代码区域 ****************************
