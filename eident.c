/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "Init.h"
#include "math.h"
#include "stdlib.h"
#include "menu_2.h"
#include "eident.h"
#include "zf_adc.h"
#include "SEEKFREE_IPS200_PARALLEL8.h"
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
/*----------------------*/
/*	   电磁识别模块		*/
/*======================*/
void adc_jug(void){
//	变量定义
	short mid_sq;
	short mid_dif, out_dif;
	short oai_dif, oali_dif, oari_dif, lr_dif;
	static unsigned char ajug_sta_temp, count_sta, hold_jug, hold_jug_temp;
	static unsigned short jug_count;
//	差值计算
	mid_sq = adc0.value*adc0.value+adc4.value*adc4.value;
	mid_dif = adc1.value*adc1.value - adc3.value*adc3.value;
	out_dif = adc0.value*adc0.value - adc4.value*adc4.value;
	oai_dif = mid_sq - adc1.value*adc1.value - adc3.value*adc3.value;
	oali_dif = mid_sq - adc1.value*adc1.value;
	oari_dif = mid_sq - adc3.value*adc3.value;
	lr_dif = oali_dif - oari_dif;
//	元素识别
	ajug_sta_temp = ajug_sta;
	ajug_sta = 0;
//	左入环
	if(abs(lr_dif) > 4500)
		if(oari_dif > 13000)
			ajug_sta = 1;
//	状态机
	if(ajug_sta != ajug_sta_temp){//跳变检测
		if(ajug_sta == 1) count_sta = 2, jug_count = 0;//圆环
		else count_sta = 1, jug_count = 0;//其他情况
	}
	if(count_sta){
		switch(count_sta){
			case 1://其他情况
				if(ajug_sta == 0) jug_count++;
				else count_sta = 0;
				if(jug_count>20) hold_jug_temp = hold_jug, hold_jug = 0, count_sta = 0;
				break;
			case 2://圆环
					if(ajug_sta == 1) jug_count++;
					else count_sta = 0;
					if(jug_count>20) hold_jug_temp = hold_jug, hold_jug = 1, count_sta = 0;
				break;
		}
	}
//	状态更改
	if(hold_jug != hold_jug_temp){
		if(hold_jug_temp==0)
			if(hold_jug==1)//进圆环
				act_sta = 1;
		if(hold_jug_temp==0)//出圆环
			if(hold_jug==1)
				act_sta = 2;
	}
//	adc_pfc[1]();
}
/*----------------------*/
/*	   	 十字直走		*/
/*======================*/
void cross_road(void){
	uart_putchar(UART_7, 0);
	spd_bias = 5;
}
/*----------------------*/
/*	   差比和差算法		*/
/*======================*/
void adc_suminus(void){
//	变量定义
	float divd, divs;
	short mid_val;
//	差比和差算法
	mid_val = adc1.value - adc3.value;
	divd = adc_err.alpha*(float)(adc0.value - adc4.value) + adc_err.beta*(float)mid_val;
	divs = adc_err.alpha*(float)(adc0.value+adc4.value) + adc_err.omega*abs((float)mid_val);
	adc_err.rs = adc_err.P*divd/divs;
	switch(act_sta){
		case 0:
			rad_bias = 0;
			break;
		case 1:
			rad_bias = 15;
			break;
		case 2:
			rad_bias = -7;
			break;
	}
//	if(adc_err.rs < 0) adc_err.rs = adc_err.rs*1.14;//左右加权
//	PID计算、数据发送
//	adc_steering.Kp = 1.36+(adc_err.rs*adc_err.rs)/(float)Kp_act;//动态P
	pos_pid(&adc_steering, 0, -adc_err.rs, 100, -100);
//	spd_bias = -abs(adc_steering.rs)>>3;//弯道减速
//	数据监视
//	if(csimenu_flag[1]){
//		ips200_showint16(0, 0, adc0.value);
//		ips200_showint16(0, 1, adc1.value);
//		ips200_showint16(0, 2, adc2.value);
//		ips200_showint16(0, 3, adc3.value);
//		ips200_showint16(0, 4, adc4.value);
//		ips200_showint16(0, 5, adc_convert(ADC_MOD1, ADC_PIN0));
//		ips200_showint16(0, 6, adc_convert(ADC_MOD1, ADC_PIN1));
//		ips200_showint16(0, 7, adc_convert(ADC_MOD1, ADC_PIN2));
//		ips200_showint16(0, 8, adc_convert(ADC_MOD1, ADC_PIN3));
//		ips200_showint16(0, 9, adc_convert(ADC_MOD1, ADC_PIN4));
//		ips200_showstr(100, 0, "divd");
//		ips200_showfloat(160, 0, divd, 4, 4);
//		ips200_showstr(100, 1, "divs");
//		ips200_showfloat(160, 1, divs, 4, 4);
//		ips200_showstr(100, 2, "rs");
//		ips200_showfloat(160, 2, adc_err.rs, 4, 3);
//		ips200_showstr(100, 3, "pid");
//		ips200_showfloat(160, 3, adc_steering.rs, 4, 3);
//		ips200_showstr(100, 4, "a1");
//		ips200_showfloat(160, 4, adc_err.alpha*(float)(adc0.value-adc4.value), 4, 4);
//		ips200_showstr(100, 5, "b");
//		ips200_showfloat(160, 5, adc_err.beta*(float)mid_val, 4, 4);
//		ips200_showstr(100, 6, "a2");
//		ips200_showfloat(160, 6, adc_err.alpha*(float)(adc0.value+adc4.value), 4, 4);
//		ips200_showstr(100, 7, "c");
//		ips200_showfloat(160, 7, adc_err.omega*abs((float)mid_val), 4, 4);
//	}
}
/*----------------------*/
/*	    单通道滤波		*/
/*======================*/
void single_ch_filter(struct adcpara *para){
//	变量定义
	unsigned char i;
	register unsigned char j;
	unsigned short value[3];
	unsigned int sum_val = 0;
	unsigned char max_val, min_val;
//	中值均值滤波
	for(i = 0; i < SAMPFRE; i++){
		//	重置
			max_val = 0, min_val = 0;
		//	采集三次数据
			for(j = 0; j < 3; j++)
				value[j] = adc_convert(ADC_MOD1, para->pin);
		//	判断三个值是否相等
			if(value[0] == value[1])
				if(value[1] == value[2]){sum_val+= value[0];continue;}
		//	找中值
			for(j = 1; j < 3; j++){
				if(value[max_val] < value[j]) max_val = j;
				if(value[min_val] > value[j]) min_val = j;
			}
			sum_val += value[3-max_val-min_val];
		}
		para->value = (sum_val / SAMPFRE);
	//	限幅
		if(para->value > para->max) para->value = para->max;
		if(para->value < para->min) para->value = para->min;
	//	归一化
		para->value = 100*(para->value-para->min)/(para->max-para->min);
}
/*----------------------*/
/*	     电磁最值		*/
/*======================*/
void adc_extreme(struct adcpara *para){
//	变量定义
	unsigned short adc_colltemp;
//	ADC采集、最值比较
	adc_colltemp = adc_convert(ADC_MOD1, para->pin);
	if(para->max < adc_colltemp) para->max = adc_colltemp;
	if(para->min > adc_colltemp) para->min = adc_colltemp;
//	刷新高亮数值
	ips200_pencolor = 0xFFFF;
	ips200_bgcolor = 0xB6DB;
	menu2value_hl();
	ips200_pencolor = 0xB6DB;
	ips200_bgcolor = 0xFFFF;
}
