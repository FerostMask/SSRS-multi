/*--------------------------------------------------------------*/
/*							ͷ�ļ�����							*/
/*==============================================================*/
#include "CAM.h"
#include "pid.h"
#include "data.h"
#include "SEEKFREE_MT9V03X.h"
#include "SEEKFREE_IPS200_PARALLEL8.h"
/*--------------------------------------------------------------*/
/* 							  �궨�� 							*/
/*==============================================================*/
#define uint16 unsigned short
/*--------------------------------------------------------------*/
/* 							 �������� 							*/
/*==============================================================*/
unsigned short img_color = 0xAE9C;
unsigned char lef_riseflag;
unsigned char act_flag;
/*--------------------------------------------------------------*/
/* 							 �������� 							*/
/*==============================================================*/
/*------------------------------*/
/*		  ��ֵ����ʾģ��		*/
/*==============================*/
void binary_disp(void){
//	��������
	register unsigned char i, j, k;
	unsigned char column = (MT9V03X_W-4)>>3;
	unsigned char binary_temp;
//	ͼ����ʾ
	ips200_address_set(0,0,MT9V03X_W-5,MT9V03X_H-1);
	for(i = 0; i < MT9V03X_H; i++){
		for(j = 0; j < column; j++){
			for(k = 8; k > 0; k--){
				binary_temp = (binary_img[i][j]>>(k-1)) & 0x01;
				if(binary_temp) ips200_wr_data16(WHITE);
				else ips200_wr_data16(img_color);
			}
		}
	}
//	��ʾ����
//	ips200_showint16(0, 8, fop_flag);
//	ips200_drawpoint(lefbor[found_point[0]], found_point[0], 0XFDF8);
//	ips200_drawpoint(rigbor[found_point[2]], found_point[2], 0XFDF8);
//	��ʾ�߽�
	for(i = MT9V03X_H - 1; i > lcut; i--) ips200_drawpoint(lefbor[i], i, 0x00);
	for(i = MT9V03X_H - 1; i > rcut; i--) ips200_drawpoint(rigbor[i], i, 0x00);
//	��ʾת���
	for(i = 0; i < ltraf_count; i++){//��߽�ת��
		ips200_drawpoint(ltraf_point_col[i], ltraf_point_row[i], 0x32D5);
		ips200_drawpoint(ltraf_point_col[i]+1, ltraf_point_row[i], 0x32D5);
		ips200_drawpoint(ltraf_point_col[i]+2, ltraf_point_row[i], 0x32D5);
		ips200_drawpoint(ltraf_point_col[i]+3, ltraf_point_row[i], 0x32D5);
		ips200_drawpoint(ltraf_point_col[i]+4, ltraf_point_row[i], 0x32D5);
	}
	for(i = 0; i < lvet_trafcount; i++){//��ת���
		ips200_drawpoint(lvet_trafpoint_col[i], lvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(lvet_trafpoint_col[i]+1, lvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(lvet_trafpoint_col[i]+2, lvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(lvet_trafpoint_col[i]+3, lvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(lvet_trafpoint_col[i]+4, lvet_trafpoint_row[i], 0xFD10);
	}
	for(i = 0; i < rtraf_count; i++){//�ұ߽�ת��
		ips200_drawpoint(rtraf_point_col[i], rtraf_point_row[i], 0x32D5);
		ips200_drawpoint(rtraf_point_col[i]-1, rtraf_point_row[i], 0x32D5);
		ips200_drawpoint(rtraf_point_col[i]-2, rtraf_point_row[i], 0x32D5);
		ips200_drawpoint(rtraf_point_col[i]-3, rtraf_point_row[i], 0x32D5);
		ips200_drawpoint(rtraf_point_col[i]-4, rtraf_point_row[i], 0x32D5);
	}
	for(i = 0; i < rvet_trafcount; i++){//��ת���
		ips200_drawpoint(rvet_trafpoint_col[i], rvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(rvet_trafpoint_col[i]-1, rvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(rvet_trafpoint_col[i]-2, rvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(rvet_trafpoint_col[i]-3, rvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(rvet_trafpoint_col[i]-4, rvet_trafpoint_row[i], 0xFD10);
	}
//	��ʾ����
	for(i = 0; i < exti_lefcount; i++){
		ips200_drawpoint(0, exti_lefp[i], 0xB20E);
		ips200_drawpoint(1, exti_lefp[i], 0xB20E);
		ips200_drawpoint(2, exti_lefp[i], 0xB20E);
	}
	for(i = 0; i < exti_rigcount; i++){
		ips200_drawpoint(157, exti_rigp[i], 0xB20E);
		ips200_drawpoint(158, exti_rigp[i], 0xB20E);
		ips200_drawpoint(159, exti_rigp[i], 0xB20E);
	}
//	��ʾ��ֱ����
//	for(i = 0; i < lefup_cut; i++) ips200_drawpoint(i, upbor[i], 0xA6AE);
	for(i = 0; i < lefdown_cut; i++){
		ips200_drawpoint(i, downbor[i], 0x00);
		ips200_drawpoint(i, downbor[i]+1, 0x00);
	}
	ips200_showint16(0, 10, act_flag);
//	ips200_showint16(0, 8, turn_flag);
//	ips200_showint16(0, 9, out_vertical_flag[0]);
}
/*------------------------------*/
/*	    	״̬��ģ��			*/
/*==============================*/
void state_machine(void){
//	��������
	static unsigned char state, state_temp;
//	״̬���
	state_temp = state; state = 0;
	if(turn_flag == 11) state = 1;//��ת
	if(turn_flag == 12) state = 2;//��ת
//	Բ�����
	if(exti_lefcount > 0)//�����
		if(exti_rigcount == 0)
				if(lvet_trafcount > 0){
					if(lef_riseflag) state = 11;//���뻷
					if(state!=11)if(lvet_trafcount > 1) state = 12;//�����
	}
//	��������
	if(act_flag == 3)
		if(exti_rigcount > 0)
			state = 13;
	if(act_flag == 4)
		if(lvet_trafcount > 0)
			state = 14;
	if(act_flag == 5){
//		if(turn_flag == 11|| turn_flag == 12) state = 15;
		if(turn_flag != 1)
			if(turn_flag != 3)
				if(ltraf_count == 0)
					if(rtraf_count == 0)
						state = 15;
	}
//	ʮ�ּ�⣨����
//	if(exti_lefcount > 0)
//		if(exti_rigcount >0){
//			if(abs(exti_lefp[0] - exti_rigp[0]) < 5) img_color = 0xED2A, state = 20;//ʮ��
//		}
//	״̬��
	if(state_temp!=state){//��⵽״̬����
	//	ֱ����Բ����������
		if(act_flag == 0)
			if(state == 12)
				act_flag = 1, img_color = 0xBDB8;
	//	������Բ����������
		if(act_flag == 1)
			if(state == 11)
				act_flag = 2, img_color = 0xFDEB;
	//	�����󵽳�������
		if(act_flag == 2)
			if(state == 1)
				act_flag = 3, img_color = 0xED2A;
	//	���ڵ���������
		if(act_flag == 3)
			if(state == 13)
				act_flag = 4, img_color = 0x31A7;
	//	�������뿪��������
		if(act_flag == 4)
			if(state == 14)
				act_flag = 5, img_color = 0x64D0;
	//	�ɻ���������������
		if(act_flag == 5)
			if(state == 0)
				act_flag = 0, img_color = 0xAE9C;
	}
}
/*------------------------------*/
/*	    ��ֱ�߽��Ѱ��ģ��		*/
/*==============================*/
void border_vertical_search(char num){
//	��������
	register unsigned char i, k, k2;
	register char j;
	unsigned char col = (MT9V03X_W-4)>>3;//����
	unsigned char *p;
	unsigned char vetflag;
	unsigned char vet_colmax, vet_rowmax;
	unsigned char found_flag, view_temp;
//	��ֱ�߽�Ѱ��
	switch(num){
		case 1:
		//	������ʼ��
			lvet_trafcount = 0, exti_lefcount = 0, lef_riseflag = 0;
			lefup_cut = 0, lefdown_cut = 0;
			if(ltraf_count > 1)
				for(i = 1; i < ltraf_count; i++){
				//	����͹
					if(ltraf_flag[i] == 0)
						if(ltraf_flag[i-1] == 1){
							for(k = ltraf_point_row[i], vet_colmax = 0; k < ltraf_point_row[i-1]; k++) 
								if(lefbor[k] > vet_colmax) vet_colmax = lefbor[k], vet_rowmax = k; 
							lvet_trafpoint_row[lvet_trafcount] = vet_rowmax, lvet_trafpoint_col[lvet_trafcount] = vet_colmax, lvet_trafcount++;
							if(vet_rowmax - ltraf_point_row[i] > 5) lef_riseflag = 1;
						}
				//	����
					if(ltraf_flag[i] == 1)
						if(ltraf_flag[i-1] == 0) exti_lefp[exti_lefcount] = (ltraf_point_row[i]+ltraf_point_row[i-1])>>1, exti_lefcount++;
				}	
			break;
		case 2:
		//	������ʼ��
			rvet_trafcount = 0, exti_rigcount = 0;
			if(rtraf_count > 1)
				for(i = 1; i < rtraf_count; i++){
				//	����͹
					if(rtraf_flag[i] == 0)
						if(rtraf_flag[i-1] == 1){
							for(k = rtraf_point_row[i], vet_colmax = 159; k < rtraf_point_row[i-1]; k++) 
								if(rigbor[k] < vet_colmax) vet_colmax = rigbor[k], vet_rowmax = k; 
							rvet_trafpoint_row[rvet_trafcount] = vet_rowmax, rvet_trafpoint_col[rvet_trafcount] = vet_colmax, rvet_trafcount++;
						}
				//	����
					if(rtraf_flag[i] == 1)
						if(rtraf_flag[i-1] == 0) exti_rigp[exti_rigcount] = (rtraf_point_row[i]+rtraf_point_row[i-1])>>1, exti_rigcount++;	
				}
			break;
	}
}
/*------------------------------*/
/*		 ��߽��Ѱ��ģ��		*/
/*==============================*/
void lbor_search(void){
//	��������
	register unsigned char i = found_point[0], k;
	register char j = found_point[1];
	unsigned char col = (MT9V03X_W-4)>>3;//����
	unsigned char *p;
	unsigned char traf_flag, traf_flag_temp;
//	��ʼ��
	lcut = 0, turn_flag = 0, ltraf_count = 0;
//	�ӻ�׼�㿪ʼѰ��
	p = &binary_img[i][j];
	switch(fop_flag){
		case 1://��׼���������
			for(k = 0; k < 7; k++)
				if(((*p>>(k+1))&0x01)^((*p>>k)&0x01)){lefbor[i] = (j<<3)+6-k;break;}
			break;
		case 2:
			lefbor[i] = (j<<3)+7;
			break;
		case 3://��׼�㲻�ڵ���
			for(k = 0; k < 7; k++)
				if(((*p>>(k+1))&0x01)^((*p>>k)&0x01)){lefbor[i] = (j<<3)+6-k;break;}
			turn_flag = 1;
			for(k = MT9V03X_H - 1; k > i; k--) lefbor[k] = 0;
			break;
	}
//	���ϼ��
	for(; i > 0; i--){
	//	��ʼ�����С�������
		p -= col;
		if(traf_flag!=traf_flag_temp) 
			if(i < 90)//���������µ�ת���
				if(ltraf_point_row[ltraf_count-1]-i>5 || ltraf_count == 0){//����ת���߶�
					if(traf_flag == 1) ltraf_flag[ltraf_count] = 0;//���ҵ���ת��
					else ltraf_flag[ltraf_count] = 1;//������ת��
					ltraf_point_row[ltraf_count] = i+1, ltraf_point_col[ltraf_count] = lefbor[i+1], ltraf_count++;
				}
		traf_flag_temp = traf_flag;
		traf_flag = 0;
		if(*(p+1) != 0x00)
			if(*(p+1) != 0xff){//��ͻ��
				for(k = 0; k < 7; k++)
					if(((*(p+1)>>(k+1))&0x01)^((*(p+1)>>k)&0x01)){lefbor[i] = ((j+1)<<3)+6-k;break;}
					continue;
			}
		if(*p != 0x00)
			if(*p != 0xff){//��ͻ��
				for(k = 0; k < 7; k++)
					if(((*p>>(k+1))&0x01)^((*p>>k)&0x01)){lefbor[i] = (j<<3)+6-k;break;}
					continue;
			}
		if(*(uint16*)p == 0xFF00){lefbor[i] = (j<<3)+7;continue;}//����Ұ�����
		if(*(uint16*)p == 0x00FF){lefbor[i] = (j+1)<<3;continue;}//����Һ�����
	//	ȫ��
		if(*(uint16*)p == 0x0000){
			for(p++, j++; j < 19; p++, j++){//����Ѱ�������
				if(*(uint16*)p == 0x0000) continue;
				else break;
			}
			if(j == 19){lcut = i;return;}//�ִ�߽磬ֹͣ����
			if(*(uint16*)p == 0xFF00){lefbor[i] = (j<<3)+7;continue;}//����Ұ�����
			if(*(uint16*)p == 0x00FF){lefbor[i] = (j+1)<<3;continue;}//����Һ�����
			if(*(p+1) != 0x00)
				if(*(p+1) != 0xff){//��ͻ��
					for(k = 0; k < 7; k++)
						if(((*(p+1)>>(k+1))&0x01)^((*(p+1)>>k)&0x01)){lefbor[i] = ((j+1)<<3)+6-k;break;}
						continue;
				}
			if(*p != 0x00)
				if(*p != 0xff){//��ͻ��
					for(k = 0; k < 7; k++)
						if(((*p>>(k+1))&0x01)^((*p>>k)&0x01)){lefbor[i] = (j<<3)+6-k;break;}
						continue;
				}
		}
	//	ȫ��
		if(*(uint16*)p == 0xFFFF){
			for(p--, j--; j > -1; p--, j--){//����Ѱ�������
				if(*(uint16*)p == 0xFFFF) continue;
				else break;
			}
			if(j == -1){lcut = i;traf_flag = 1,j++,p++,lefbor[i] = 0;continue;}//�ִ�ȫ�ױ߽�
			if(*(uint16*)p == 0xFF00){lefbor[i] = (j<<3)+7;continue;}//����Ұ�����
			if(*(uint16*)p == 0x00FF){lefbor[i] = (j+1)<<3;continue;}//����Һ�����
			if(*p != 0x00)
				if(*p != 0xff){//��ͻ��
					for(k = 0; k < 7; k++)
						if(((*p>>(k+1))&0x01)^((*p>>k)&0x01)){lefbor[i] = (j<<3)+6-k;break;}
						continue;
				}
			if(*(p+1) != 0x00)
				if(*(p+1) != 0xff){//��ͻ��
					for(k = 0; k < 7; k++)
						if(((*(p+1)>>(k+1))&0x01)^((*(p+1)>>k)&0x01)){lefbor[i] = ((j+1)<<3)+6-k;break;}
						continue;
				}
		}
	}
}
/*------------------------------*/
/*		 �ұ߽��Ѱ��ģ��		*/
/*==============================*/
void rbor_search(void){
//	��������
	register unsigned char i = found_point[2], k;
	register char j = found_point[3];
	unsigned char col = (MT9V03X_W-4)>>3;//����
	unsigned char traf_flag, traf_flag_temp;
	unsigned char *p;
//	��ʼ��
	rcut = 0, rtraf_count = 0;
//	�ӻ�׼�㿪ʼѰ��
	p = &binary_img[i][j];
	switch(fop_flag){
		case 1://��׼���������
			for(k = 7; k > 0; k--)
				if(((*p>>k)&0x01)^((*p>>(k-1))&0x01)){rigbor[i] = (j<<3)+8-k;break;}
			break;
		case 2:
			rigbor[i] = (j<<3);
			break;
		case 3:
			for(k = 7; k > 0; k--)
				if(((*p>>k)&0x01)^((*p>>(k-1))&0x01)){rigbor[i] = (j<<3)+8-k;break;}
			if(turn_flag == 1) turn_flag = 3;
			else turn_flag = 2;
			for(k = MT9V03X_H - 1; k > i; k--) rigbor[k] = 159;
			break;
	}
//	���Ʊ߽�
	if(j == 19) j--, p--;
//	���ϼ��
	for(; i > 0; i--){
	//	��ʼ�����С�������
		p -= col;
		if(traf_flag!=traf_flag_temp) 
			if(i < 90)//���������µ�ת���
				if(rtraf_point_row[rtraf_count-1]-i>5 || rtraf_count == 0){//����ת���߶�
					if(traf_flag == 1) rtraf_flag[rtraf_count] = 0;//������ת��
					else rtraf_flag[rtraf_count] = 1;//���ҵ���ת��
					rtraf_point_row[rtraf_count] = i+1, rtraf_point_col[rtraf_count] = rigbor[i+1], rtraf_count++;
				}
		traf_flag_temp = traf_flag;
		traf_flag = 0;
		if(*p != 0x00)
			if(*p != 0xff){//��ͻ��
				for(k = 7; k > 0; k--)
					if(((*p>>k)&0x01)^((*p>>(k-1))&0x01)){rigbor[i] = (j<<3)+8-k;break;}
					continue;
			}
		if(*(p+1) != 0x00)
			if(*(p+1) != 0xff){//��ͻ��
				for(k = 7; k > 0; k--)
					if(((*(p+1)>>k)&0x01)^((*(p+1)>>(k-1))&0x01)){rigbor[i] = ((j+1)<<3)+8-k;break;}
					continue;
			}
		if(*(uint16*)p == 0x00FF){rigbor[i] = ((j+1)<<3);continue;}//����Һ�����
		if(*(uint16*)p == 0xFF00){rigbor[i] = (j<<3)+7;continue;}//����Ұ�����
	//	ȫ��
		if(*(uint16*)p == 0x0000){
			for(p--, j--; j >  -1; p--, j--){//����Ѱ�������
				if(*(uint16*)p == 0x0000) continue;
				else break;
			}
			if(j == -1){rcut = i;return;}//�ִ�߽磬ֹͣ����
			if(*(uint16*)p == 0x00FF){rigbor[i] = ((j+1)<<3);continue;}//����Һ�����
			if(*(uint16*)p == 0xFF00){rigbor[i] = (j<<3)+7;continue;}//����Ұ�����
			if(*p != 0x00)
				if(*p != 0xff){//��ͻ��
					for(k = 7; k > 0; k--)
						if(((*p>>k)&0x01)^((*p>>(k-1))&0x01)){rigbor[i] = (j<<3)+8-k;break;}
						continue;
				}
			if(*(p+1) != 0x00)
				if(*(p+1) != 0xff){//��ͻ��
					for(k = 7; k > 0; k--)
						if(((*(p+1)>>k)&0x01)^((*(p+1)>>(k-1))&0x01)){rigbor[i] = ((j+1)<<3)+8-k;break;}
						continue;
				}
		}
	//	ȫ��
		if(*(uint16*)p == 0xFFFF){
			for(p++, j++; j < 19; p++, j++){//����Ѱ�������
				if(*(uint16*)p == 0xFFFF) continue;
				else break;
			}
			if(j == 19){rcut = i;traf_flag = 1;j--, p--,rigbor[i] = 159;continue;}
			if(*(uint16*)p == 0x00FF){rigbor[i] = ((j+1)<<3);continue;}//����Һ�����
			if(*(uint16*)p == 0xFF00){rigbor[i] = (j<<3)+7;continue;}//����Ұ�����
			if(*(p+1) != 0x00)
				if(*(p+1) != 0xff){//��ͻ��
					for(k = 7; k > 0; k--)
						if(((*(p+1)>>k)&0x01)^((*(p+1)>>(k-1))&0x01)){rigbor[i] = ((j+1)<<3)+8-k;break;}
						continue;
				}
			if(*p != 0x00)
				if(*p != 0xff){//��ͻ��
					for(k = 7; k > 0; k--)
						if(((*p>>k)&0x01)^((*p>>(k-1))&0x01)){rigbor[i] = (j<<3)+8-k;break;}
						continue;
				}
		}
	}
}
/*------------------------------*/
/*		  ��׼�㸨��ģ��		*/
/*==============================*/
static void fop_search_sup(char num, unsigned char* p, char j){
//	����ѡ��
	switch(num){
	//	���׼���ж�
		case 1:
		//	һ�����
			if(*p != 0xFF)
				if(*p != 0x00){//���ӳ�ͻ��
					fop_flag = 1, found_point[0] = MT9V03X_H - 1, found_point[1] = j;
					return;
				}
			if(*(p+1) != 0xFF)
				if(*(p+1) != 0x00){//���ӳ�ͻ��
					fop_flag = 1, found_point[0] = MT9V03X_H - 1, found_point[1] = j+1;
					return;
				}
		//	����Ұ�
			fop_flag = 2, found_point[0] = MT9V03X_H - 1, found_point[1] = j;
			return;
	//	�һ�׼���ж�
		case 2:
		//	һ�����
			if(*p != 0xFF)
				if(*p != 0x00){//���ӳ�ͻ��
					fop_flag = 1, found_point[2] = MT9V03X_H - 1, found_point[3] = j;
					return;
				}
			if(*(p+1) != 0xFF)
				if(*(p+1) != 0x00){//���ӳ�ͻ��
					fop_flag = 1, found_point[2] = MT9V03X_H - 1, found_point[3] = j+1;
					return;
				}
		//	����Һ�
			fop_flag = 2, found_point[2] = MT9V03X_H - 1, found_point[3] = j+1;
			return;
	//	��ʼ���ж�
		case 0:
		//	һ�����
			if(*p != 0xFF)
				if(*p != 0x00){//����ʼ��ͻ�䣬�������ת
					fop_flag = 11, found_point[2] = MT9V03X_H - 1, found_point[3] = 9;
					return;
				}
			if(*(p+1) != 0xFF)
				if(*(p+1) != 0x00){//����ʼ��ͻ�䣬�������ת
					fop_flag = 12, found_point[0] = MT9V03X_H - 1, found_point[1] = 10;
					return;
				}
		//	�������
			if(*(uint16*)p == 0x00FF){//����Һڵ�ǰ�����䣬�������ת
				fop_flag = 13, found_point[2] = MT9V03X_H - 1, found_point[3] = 10;
				return;
			}
			if(*(uint16*)p == 0xFF00){//����Ұ׵�ǰ�����䣬�������ת
				fop_flag = 14, found_point[0] = MT9V03X_H - 1, found_point[1] = 9;
				return;
			}			
			break;
	}
}
/*------------------------------*/
/*		  ��׼��Ѱ��ģ��		*/
/*==============================*/
static void fop_search(char num){
//	��������
	register unsigned char i = MT9V03X_H - 1;
	register char j;
	unsigned char bina = 0, bina_temp;
	unsigned char* p;
//	��ʼ���ж�
	p = &binary_img[i][9];
	if(*(uint16*)p == 0x0000){//ȫ��
	//	Ѱ������
		p = &binary_img[i][8];
		for(j = 8; j > 0; j--){//����Ѱ�ң�����Ϊ��ת
			if(*(uint16*)p == 0x0000){p--;continue;}
			fop_search_sup(2, p, j);
			return;
		}
		p = &binary_img[i][10];
		for(j = 10; j < 18; j++){//����Ѱ�ң�����Ϊ��ת
			if(*(uint16*)p == 0x0000){p++;continue;}
			fop_search_sup(1, p, j);
			return;
		}
	//  δ�ҵ����䣬���ش���
		fop_flag = 0;
		return;
	}
	if(!bina) fop_search_sup(0, p, j);
//	����Ѱ��
	switch(num){
		case 1:
		//	�����Ѱ�� 
			p = &binary_img[i][8];
			for(j = 8; j > -1; j--){
				if(*(uint16*)p == 0xFFFF){p--;continue;}
				fop_search_sup(1, p, j);
				return;
			}
		//	δ�ҵ����㣬����Ѱ��
			j = 0;
			for(i = MT9V03X_H - 1; i > 0; i--){
				if(!(binary_img[i][j]^binary_img[i-1][j])) continue;
				fop_flag = 3, found_point[0] = i, found_point[1] = j;
				return;
			}
			break;
		case 2:
		//	�һ���Ѱ�� 
			p = &binary_img[i][10];
			for(j = 10; j < 19; j++){
				if(*(uint16*)p == 0xFFFF){p++;continue;}
				fop_search_sup(2, p, j);
				return;
			}
		//	δ�ҵ����㣬����Ѱ��
			j = 19;
			for(i = MT9V03X_H - 1; i > 0; i--){
				if(!(binary_img[i][j]^binary_img[i-1][j])) continue;
				fop_flag = 3, found_point[2] = i, found_point[3] = j;
				return;
			}
			break;
	}
}
/*------------------------------*/
/*		  ͼ���ֵ��ģ��		*/
/*==============================*/
void img_binary(void){
//	��������
	register unsigned char i, j, k;
	unsigned char column = (MT9V03X_W-4)>>3;
	unsigned char binary_temp;
//	ͼ���ֵ��
	for(i = 0; i < MT9V03X_H; i++){
		for(j = 0; j < column; j++){
		//	����ͼ��
			binary_img[i][j] = 0x00;
		//	��ֵ��
			for(k = 0; k < 8; k++){
				binary_img[i][j] <<= 1;
				if(mt9v03x_image[i][(j<<3)+k+2] > img_thrsod) binary_img[i][j] |= 0x01;
			}
		}
	}
}
/*------------------------------*/
/*		���ȫ����ֵģ��		*/
/*==============================*/
void otsu(void){
//	��������
	float var = 0, vartmp = 0;
	float imgsize = MT9V03X_H*MT9V03X_W, sumPK = 0, sumMK = 0;
	register short i, j;
	unsigned short hist[grayscale] = {0};	
//	��ȡֱ��ͼ
	for(i = 0; i < MT9V03X_H; i++){
		for(j = 0; j < MT9V03X_W; j++)
			hist[mt9v03x_image[i][j]]++;
	}
//	����䷽��
	for(i = 0; i < grayscale; i++){
		P[i] = (float)hist[i]/imgsize;//����ÿ���Ҷȼ����ֵĸ���
		PK[i] = sumPK+P[i];//�����ۼƺ�
		sumPK = PK[i];
		MK[i] = sumMK+i*P[i];//�Ҷ�ֵ�ۼӾ�ֵ
		sumMK = MK[i];
	}
	for(i = 115; i < 200; i++){
		vartmp = ((MK[grayscale-1]*PK[i] - MK[i])*(MK[grayscale-1]*PK[i] - MK[i])) / (PK[i]*(1 - PK[i]));
		if(vartmp > var){
			var = vartmp;
			img_thrsod = i;//�����ֵ
		}
	}
//	��ֵ��
	img_binary();
//	Ѱ�һ��㡢�ұ��ߣ�ע��Ѱ��˳������ת�жϻ���������ҵı���Ѱ��
	fop_search(1);
	lbor_search();
	fop_search(2);
	rbor_search();
//	�ж��Ƿ�Ϊ����ת��ע��Ѱ��˳��
	if(turn_flag){
		if(abs(found_point[0]-rcut)<5) turn_flag = 11;
		if(abs(found_point[2]-lcut)<5) turn_flag = 12;
	} 
//	��ֱ�߽���
	if(ltraf_count) border_vertical_search(1);
	if(rtraf_count) border_vertical_search(2);
//	״̬��
	state_machine();
//	ͼ����ʾ
	if(csimenu_flag[0]) binary_disp();
	if(csimenu_flag[1]) ips200_displayimage032(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
}
