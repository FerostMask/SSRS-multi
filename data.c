/*--------------------------------------------------------------*/
/*							ͷ�ļ�����							*/
/*==============================================================*/
#include "data.h"
#include "eident.h"
#include "CAM.h"
#include "ctrl.h"
/*--------------------------------------------------------------*/
/*							  �궨��							*/
/*==============================================================*/
#define u8 unsigned char                                    //8λ����
#define u16 unsigned short                                  //16λ����
#define u32 unsigned int                                    //32λ����

/*--------------------------------------------------------------*/
/* 							 �������� 							*/
/*==============================================================*/
/*----------------------*/
/*	 	����ͷģ��		*/
/*======================*/
//	��ֵ
unsigned short imgthrsod = 28000;
//	ʶ��״̬
unsigned char cjug_sta;
//	���ұ߽磨���У�
unsigned char lefp, rigp, midp;
//	б��
float lefslope, rigslope, midslope;
//	ver3
float P[256] = {0}, PK[256] = {0}, MK[256] = {0};
unsigned char binary_img[MT9V03X_H][(MT9V03X_W-4)/8];
unsigned char img_thrsod;
//	��׼��Ѱ��
unsigned char found_point[4];
unsigned char fop_flag;
//	ˮƽ����Ѱ��
unsigned char lefbor[MT9V03X_H], rigbor[MT9V03X_H], mid_point[MT9V03X_H];
unsigned char ltraf_point_row[10], rtraf_point_row[10], ltraf_point_col[10], rtraf_point_col[10];
unsigned char ltraf_flag[10], rtraf_flag[10];
unsigned char ltraf_count, rtraf_count;
unsigned char lcut, rcut;
//	��ֱ����Ѱ��
unsigned char topbor[MT9V03X_W-4], bottombor[MT9V03X_W-4];//���±���
unsigned char lvet_trafpoint_row[5], rvet_trafpoint_row[5], lvet_trafpoint_col[5], rvet_trafpoint_col[5];//ת���
unsigned char lvet_trafcount, rvet_trafcount;//ת������
unsigned char leftop_cut, lefbottom_cut, rigtop_cut, rigbottom_cut;//��ֹ��
unsigned char exti_lefp[4], exti_rigp[4], exti_lefcount, exti_rigcount;//����
//	״̬��
unsigned char act_flag, act_flag_temp, fragile_flag;
unsigned char state, state_temp;
unsigned char state_flag;
unsigned short img_color = 0xAE9C;
void(*state_pfc[])(void) = {state_machine_enter, state_machine_bend, state_machine_ring, state_machine_cross, state_machine_fork};
//	�������
short p_target[2];
short error_flit[8], ctrl_error1, ctrl_error2;
short spd_slow;
void(*ctrl_pfc[])(void) = {cam_ctrl_direct, cam_ctrl_bend, cam_ctrl_ring, cam_ctrl_cross, cam_ctrl_fork};
/*----------------------*/
/*	 	 ���ģ��		*/
/*======================*/
//	�ṹ�嶨��
struct adcpara adc0;
struct adcpara adc1;
struct adcpara adc2;
struct adcpara adc3;
struct adcpara adc4;
struct adcerrpa adc_err;
//	״̬��־λ
unsigned char ajug_sta, act_sta;
unsigned short Kp_act = 50;
short spd, spd_bias;
short spd_adcset = 40;
char rad_bias = 0;
//	ָ�뺯��
void(*adc_pfc[])(void) = {cross_road};
/*----------------------*/
/*	 	 ����ģ��		*/
/*======================*/
//	PID
struct pidpara adc_steering;
struct pidpara adc_straight;
struct pidpara cam_steering;
/*----------------------*/
/*	 	 �˵�ģ��		*/
/*======================*/
//	��������
unsigned char nom[128];
//	һ���˵�
unsigned char menu[ROWS] = {0, 0, 0, 0};
unsigned char menu_level;
unsigned char menu_index = 0;
//	�����˵�
unsigned char menu2_index = 0;
unsigned char menu2_level = 0;
//	��־λ
unsigned char excollflag = 0;//��ż�ֵ�ɼ���־λ
unsigned char fixedflag = 0;//�̶���ʾ
unsigned char monitorflag = 0;//������
unsigned char csimenu_flag[CSIMENU_FLAG] = {0, 0};//����ͷ
unsigned char wireless_flag[WIRELESS_FLAG] = {0, 0};//��������
//	ָ�뺯��
void(*menu_pfc[])(unsigned char) = {menu_select, menu2_select};
/*----------------------*/
/*	 	 ������ȥ		*/
/*======================*/
unsigned char subuff_num = 0;
unsigned char subuff_arr[3];
unsigned short subuff_ranging;
/*--------------------------------------------------------------*/
/* 							 �������� 							*/
/*==============================================================*/
/*----------------------*/
/*	 	������ʼ��		*/
/*======================*/
void Init_para(void){
//	���ģ��
	adc0.max = 3823, adc0.min = 102;
	adc1.max = 3815, adc1.min = 98;
	adc2.max = 4095, adc2.min = 0;
	adc3.max = 3815, adc3.min = 117;
	adc4.max = 3827, adc4.min = 38;
//	��ȺͲ�
	adc_err.alpha = 16;//���⻷ת��Ƕȱ仯��ԭ��ȺͲ���
	adc_err.beta = 32;//ת��Ƕȱ仯
	adc_err.omega = 6.5;//ת��ƽ����
	adc_err.P = 25;//�Ŵ���
//	ADCת��
	adc_steering.Kp = 2.66;
	adc_steering.Kd = 11.6;
//	ADCֱ��
	adc_straight.Kp = 1;
	adc_straight.Kd = 1;
//	CAMת��
	cam_steering.Kp = 0.9;
	cam_steering.Kd = 1.1;
////	�ٶ�
//	speed.alpha = 0.3;
//	speed.Kp = 0.1;//��Ӧ���� | ����
//	speed.Ki = 0.05;//��Ӧ����
//	speed.Kd = 0;
//	speed.I = 0;
//	speed.e1 = 0, speed.e2 = 0, speed.e3 = 0;
//	speed.rs = 0;
}
