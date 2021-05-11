/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "menu.h"
#include "Init.h"
#include "zf_adc.h"
#include "zf_tim.h"
#include "zf_gpio.h"
#include "zf_exti.h"
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

/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
/*------------------------------*/
/*		 核心选择模块模块		*/
/*==============================*/
char core_select(void){
	gpio_init(D0, GPI, GPIO_LOW, GPI_FLOATING_IN);
	gpio_init(D2, GPI, GPIO_LOW, GPI_FLOATING_IN);
	ips200_showstr(0, 7, "< - Press Button to Continue");
	while(1){
		if(!gpio_get(D0)){
			ips200_clear(0x00);
			return 0;
		}
		if(!gpio_get(D2)) return 1;
	}
}
/*------------------------------*/
/*		  按键初始化模块		*/
/*==============================*/
void Init_button(void){
//	按键中断初始化 | 最低优先级
	exti_interrupt_init(KEY1, EXTI_Trigger_Falling, 3, 3);
	exti_interrupt_init(KEY2, EXTI_Trigger_Falling, 3, 3);
	exti_interrupt_init(KEY3, EXTI_Trigger_Falling, 3, 3);
	exti_interrupt_init(KEY4, EXTI_Trigger_Falling, 3, 3);
	exti_interrupt_init(KEY5, EXTI_Trigger_Falling, 3, 3);
	exti_interrupt_init(KEY6, EXTI_Trigger_Falling, 3, 3);
}
/*------------------------------*/
/*		  电磁初始化模块		*/
/*==============================*/
void eident_init(void){
//	电磁引脚初始化 | 分辨率：12位
	adc_init(ADC_MOD1, ADC_PIN0, ADC_12BIT);
	adc_init(ADC_MOD1, ADC_PIN1, ADC_12BIT);
	adc_init(ADC_MOD1, ADC_PIN2, ADC_12BIT);
	adc_init(ADC_MOD1, ADC_PIN3, ADC_12BIT);
	adc_init(ADC_MOD1, ADC_PIN4, ADC_12BIT);
//	引脚赋值
	adc0.pin = ADC_PIN0;
	adc1.pin = ADC_PIN1;
	adc2.pin = ADC_PIN2;
	adc3.pin = ADC_PIN3;
	adc4.pin = ADC_PIN4;
}
