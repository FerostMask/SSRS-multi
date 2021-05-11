/*------------------------------------------------------*/ 
/* 					   头文件声明 						*/
/*======================================================*/
#ifndef _EIDENT_H
#define _EIDENT_H
//	调用头文件
#include "data.h"
/*------------------------------------------------------*/ 
/* 						 宏定义 						*/
/*======================================================*/
#define SAMPFRE 10
/*------------------------------------------------------*/ 
/* 						函数声明 						*/
/*======================================================*/
void eident_init(void);
void adc_jug(void);
void cross_road(void);
void single_ch_filter(struct adcpara *para);
void adc_extreme(struct adcpara *para);
void adc_suminus(void);
#endif
