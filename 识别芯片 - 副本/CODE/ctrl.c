/*--------------------------------------------------------------*/
/*							ͷ�ļ�����							*/
/*==============================================================*/
#include "ctrl.h"
#include "data.h"
#include "pid.h"
/*--------------------------------------------------------------*/
/* 							 �������� 							*/
/*==============================================================*/

/*--------------------------------------------------------------*/
/* 							 �������� 							*/
/*==============================================================*/
/*------------------------------*/
/*		   �������ģ�� 		*/
/*==============================*/
void cam_ctrl_fork(void){
//	����

}
/*------------------------------*/
/*		   ʮ�ֿ���ģ�� 		*/
/*==============================*/
void cam_ctrl_cross(void){
//	����

}
/*------------------------------*/
/*		   ��������ģ�� 		*/
/*==============================*/
void cam_ctrl_ring(void){
//	����

}
/*------------------------------*/
/*		   �������ģ�� 		*/
/*==============================*/
void cam_ctrl_bend(void){
//	��������
	unsigned char mp;
	float slope_temp;
//	����
	spd_adcset = 50;
//	if(act_flag == 2){//��ת
//		mp = (((99+lcut)>>1)+lcut)>>1;

//	}
//	if(act_flag == 1){//��ת
//		mp = ((((99+rcut)>>1)+rcut)>>1)+5;
//		slope_temp = (float)(rcut-found_point[2])/found_point[3];
//		p_target[0] = mp, p_target[1] = (float)(rcut-mp)/slope_temp;
//		return;
//	}
}
/*------------------------------*/
/*		   ֱ������ģ�� 		*/
/*==============================*/
void cam_ctrl_direct(void){
//	��������
	unsigned char point_temp;
//	����
	spd_adcset = 100;
//	if(ltraf_count)
//		if(rtraf_count){
//			if(ltraf_count > rtraf_count) point_temp = ltraf_point_row[ltraf_count-1]-2;
//			else point_temp = rtraf_point_row[rtraf_count-1]-2;
//			p_target[0] = point_temp, p_target[1] = (lefbor[point_temp]+rigbor[point_temp])>>1;
//			return;
//		}
//	if(!ltraf_count)
//		if(rtraf_count){//ֻ���ұ��������
//			point_temp = rtraf_point_row[rtraf_count-1]-2;
//			p_target[0] = point_temp, p_target[1] = (lefbor[point_temp]+rigbor[point_temp])>>1;
//			return;
//		}
//	if(!rtraf_count)
//		if(ltraf_count){//ֻ������������
//			point_temp = ltraf_point_row[ltraf_count-1]-2;
//			p_target[0] = point_temp, p_target[1] = (lefbor[point_temp]+rigbor[point_temp])>>1;
//			return;
//		}
//	if(lcut < rcut){
//		if(rcut < 80)
//			{p_target[0] = rcut+5, p_target[1] = (lefbor[rcut+5]+rigbor[rcut+5])>>1;return;}
//	}
//	else
//		if(lcut < 80)
//			{p_target[0] = lcut+5, p_target[1] = (lefbor[lcut+5]+rigbor[lcut+5])>>1;return;}
//	p_target[0] = 90, p_target[1] = (lefbor[90]+rigbor[90])>>1;
}
