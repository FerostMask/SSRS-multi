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
/*		  ��ֵ����ʾģ��		*/
/*==============================*/
void binary_disp(void){
//	��������
	register unsigned char i, j, k;
	unsigned char column = (MT9V03X_W-4)>>3;
	unsigned char binary_temp;
	unsigned char left_effrow = lcut-1, right_effrow = rcut-1;
//	ͼ����ʾ
	ips200_address_set(0,0,MT9V03X_W-5,MT9V03X_H-1);
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
//	ips200_showint16(0, 8, fop_flag);
//	ips200_drawpoint(lefbor[found_point[0]], found_point[0], 0XFDF8);
//	ips200_drawpoint(rigbor[found_point[2]], found_point[2], 0XFDF8);
//	��ʾ�߽�
	for(i = MT9V03X_H - 1; i > left_effrow; i--) ips200_drawpoint(lefbor[i], i, 0x00);
	for(i = MT9V03X_H - 1; i > right_effrow; i--) ips200_drawpoint(rigbor[i], i, 0x00);
}
/*------------------------------*/
/*		 ��߽��Ѱ��ģ��		*/
/*==============================*/
void lbor_search(void){
//	��������
	register unsigned char i = found_point[0], k;
	register char j = found_point[1];
	unsigned char col = (MT9V03X_W-4)>>3;//����
	unsigned direct = 2, direct_temp;//����
	unsigned char *p;
//	��ʼ��
	lcut = 0;
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
			for(k = MT9V03X_H - 1; k > i; k--) lefbor[k] = 0;
			break;
	}
//	���ϼ��
	for(; i > 0; i--){
	//	���С�������
		p -= col;
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
			if(j == -1){lcut = i;return;}
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
	unsigned direct = 1, direct_temp;//����
	unsigned char *p;
//	��ʼ��
	rcut = 0;
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
			for(k = MT9V03X_H - 1; k > i; k--) rigbor[k] = 159;
			break;
	}
//	���Ʊ߽�
	if(j == 19) j--, p--;
//	���ϼ��
	for(; i > 0; i--){
	//	���С�������
		p -= col;
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
			if(j == 19){rcut = i;return;}
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
	img_binary();
	fop_search(1);
	lbor_search();
	fop_search(2);
	rbor_search();
	if(csimenu_flag[0]) binary_disp();
	if(csimenu_flag[1]) ips200_displayimage032(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
}
