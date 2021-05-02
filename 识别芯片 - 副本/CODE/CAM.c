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

/*--------------------------------------------------------------*/
/* 							 �������� 							*/
/*==============================================================*/
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
					fop_flag = 2, found_point[0] = MT9V03X_H - 1, found_point[1] = j+1;
					return;
				}
		//	����Ұ�
			fop_flag = 3, found_point[0] = MT9V03X_H - 1, found_point[1] = j;
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
					fop_flag = 2, found_point[2] = MT9V03X_H - 1, found_point[3] = j+1;
					return;
				}
		//	����Һ�
			fop_flag = 3, found_point[2] = MT9V03X_H - 1, found_point[3] = j+1;
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
		for(j = 10; j < 18; j++){
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
				fop_flag = 4, found_point[0] = i, found_point[1] = j;
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
				fop_flag = 4, found_point[2] = i, found_point[3] = j;
				return;
			}
			break;
	}
}
/*------------------------------*/
/*		  ��׼��Ѱ��ģ��		*/
/*==============================*/
//static void fop_search(char num){
////	��������
//	register unsigned char i = MT9V03X_H - 1;
//	register char j;
//	unsigned char bina = 0, bina_temp;
//	switch(num){
//		case 1:
//		//	��ʼ���ж�
//			if(binary_img[i][9] == 0xFF) bina = 1;//��
//			if(binary_img[i][9] == 0x00) bina = 2;//��
//			if(bina == 2){
//				fop_flag = 0;
//				if(binary_img[i][10] == 0xFF) found_point[0] = i, found_point[1] = 9, fop_flag = 1;//��ʼ��Ϊǰ������
//				return;
//			}
//			if(!bina){found_point[0] = i, found_point[1] = 9, fop_flag = 2;return;}//��ʼ��Ϊͻ���
//		//	�����Ѱ��
//			for(j = 9; j > -1; j--){
//			//	����
//				bina_temp = bina;
//				bina = 0;
//				if(binary_img[i][j] == 0xFF) bina = 1;//��
//				if(binary_img[i][j] == 0x00) bina = 2;//��
//				if(!bina){found_point[0] = i, found_point[1] = j, fop_flag = 2;return;}//�ҵ�ͻ���
//				if(bina!=bina_temp){//ǰ������
//					fop_flag = 1;
//					if(bina==1) j+=1;	
//					found_point[0] = i, found_point[1] = j;
//					return;
//				}
//			}
//		//	δ�ҵ����㣬����Ѱ��
//			j = 0;
//			for(i = MT9V03X_H - 1; i > 0; i--){
//			//	����
//				bina_temp = bina;
//				bina = 0;
//				if(binary_img[i][j] == 0xFF) bina = 1;//��
//				if(binary_img[i][j] == 0x00) bina = 2;//��
//				if(!bina){found_point[0] = i, found_point[1] = j, fop_flag = 2;return;}//�ҵ�ͻ���
//				if(bina!=bina_temp){//��������
//					fop_flag = 3, found_point[0] = i, found_point[1] = j;
//					return;
//				}
//			}
//			break;
//		case 2:
//		//	��ʼ���ж�
//			if(binary_img[i][10] == 0xFF) bina = 1;//��
//			if(binary_img[i][10] == 0x00) bina = 2;//��
//			if(bina == 2){
//				fop_flag = 0;
//				if(binary_img[i][9] == 0xFF) found_point[2] = i, found_point[3] = 10, fop_flag = 1;//��ʼ��Ϊǰ������
//				return;
//			}
//			if(!bina){found_point[2] = i, found_point[3] = 10, fop_flag = 2;return;}//��ʼ��Ϊͻ���
//		//	�һ���Ѱ��
//			for(j = 10; j < 20; j++){
//			//	����
//				bina_temp = bina;
//				bina = 0;
//				if(binary_img[i][j] == 0xFF) bina = 1;//��
//				if(binary_img[i][j] == 0x00) bina = 2;//��
//				if(!bina){found_point[2] = i, found_point[3] = j, fop_flag = 2;return;}//�ҵ�ͻ���
//				if(bina!=bina_temp){//ǰ������
//					fop_flag = 1;
//					if(bina==1) j-=1;	
//					found_point[2] = i, found_point[3] = j;
//					return;
//				}
//			}
//		//	δ�ҵ����㣬����Ѱ��
//			j = 19;
//			for(i = MT9V03X_H - 1; i > 0; i--){
//			//	����
//				bina_temp = bina;
//				bina = 0;
//				if(binary_img[i][j] == 0xFF) bina = 1;//��
//				if(binary_img[i][j] == 0x00) bina = 2;//��
//				if(!bina){found_point[2] = i, found_point[3] = j, fop_flag = 2;return;}//�ҵ�ͻ���
//				if(bina!=bina_temp){//��������
//					fop_flag = 3, found_point[2] = i, found_point[3] = j;
//					return;
//				}
//			}
//			break;
//	}
//}
/*------------------------------*/
/*		  ��ֵ����ʾģ��		*/
/*==============================*/
void binary_disp(void){
//	��������
	register unsigned char i, j, k;
	unsigned char column = MT9V03X_W>>3;
	unsigned char binary_temp;
//	ͼ����ʾ
	ips200_address_set(0,0,MT9V03X_W-1,MT9V03X_H-1);
	for(i = 0; i < MT9V03X_H; i++){
		for(j = 0; j < column; j++){
			for(k = 8; k > 0; k--){
				binary_temp = (binary_img[i][j]>>(k-1)) & 0x01;
				if(binary_temp) ips200_wr_data16(WHITE);
				else ips200_wr_data16(0xAE9C);
			}
		}
	}
//	��ʾ����
	for(k = 0; k < 8; k++)
		ips200_drawpoint(found_point[1]*8+k, found_point[0], 0x00);
	for(k = 0; k < 8; k++)
		ips200_drawpoint(found_point[3]*8+k, found_point[2], 0x00);
	ips200_showint16(0, 7, found_point[0]);
	ips200_showint16(0, 8, found_point[1]);
}
/*------------------------------*/
/*		  ͼ���ֵ��ģ��		*/
/*==============================*/
void img_binary(void){
//	��������
	register unsigned char i, j, k;
	unsigned char column = MT9V03X_W>>3;
	unsigned char binary_temp;
//	ͼ���ֵ��
	for(i = 0; i < MT9V03X_H; i++){
		for(j = 0; j < column; j++){
		//	����ͼ��
			binary_img[i][j] = 0x00;
		//	��ֵ��
			for(k = 0; k < 8; k++){
				binary_img[i][j] <<= 1;
				if(mt9v03x_image[i][j*8+k] > img_thrsod) binary_img[i][j] |= 0x01;
			}
		//	���BUG����
			if(!j)
				if(binary_img[i][0] == 127) binary_img[i][0] = 0xFF;
			if(j == 19)
				if(binary_img[i][0] == 254) binary_img[i][0] = 0xFF;
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
	img_binary();
	fop_search(1);
	fop_search(2);
	if(csimenu_flag[0]) binary_disp();
	if(csimenu_flag[1]) ips200_displayimage032(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
}
