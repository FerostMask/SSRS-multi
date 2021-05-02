/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "CAM.h"
#include "pid.h"
#include "data.h"
#include "SEEKFREE_MT9V03X.h"
#include "SEEKFREE_IPS200_PARALLEL8.h"
/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/

/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
/*------------------------------*/
/*		  基准点寻找模块		*/
/*==============================*/
//static void fop_search(char num){
////	变量定义
//	register unsigned char i = MT9V03X_H - 1;
//	register char j;
//	unsigned char bina = 0, bina_temp;
//	switch(num){
//		case 1:
//		//	起始点判断
//			if(binary_img[i][9] == 0xFF) bina = 1;//白
//			if(binary_img[i][9] == 0x00) bina = 2;//黑
//			if(bina == 2){
//				fop_flag = 0;
//				if(binary_img[i][10] == 0xFF) found_point[0] = i, found_point[1] = 9, fop_flag = 1;//起始点为前后跳变
//				return;
//			}
//			if(!bina){found_point[0] = i, found_point[1] = 9, fop_flag = 2;return;}//起始点为突变点
//		//	左基点寻找
//			for(j = 9; j > -1; j--){
//			//	重置
//				bina_temp = bina;
//				bina = 0;
//				if(binary_img[i][j] == 0xFF) bina = 1;//白
//				if(binary_img[i][j] == 0x00) bina = 2;//黑
//				if(!bina){found_point[0] = i, found_point[1] = j, fop_flag = 2;return;}//找到突变点
//				if(bina!=bina_temp){//前后跳变
//					fop_flag = 1;
//					if(bina==1) j+=1;	
//					found_point[0] = i, found_point[1] = j;
//					return;
//				}
//			}
//		//	未找到基点，向上寻找
//			j = 0;
//			for(i = MT9V03X_H - 1; i > 0; i--){
//			//	重置
//				bina_temp = bina;
//				bina = 0;
//				if(binary_img[i][j] == 0xFF) bina = 1;//白
//				if(binary_img[i][j] == 0x00) bina = 2;//黑
//				if(!bina){found_point[0] = i, found_point[1] = j, fop_flag = 2;return;}//找到突变点
//				if(bina!=bina_temp){//上下跳变
//					fop_flag = 3, found_point[0] = i, found_point[1] = j;
//					return;
//				}
//			}
//			break;
//		case 2:
//		//	起始点判断
//			if(binary_img[i][10] == 0xFF) bina = 1;//白
//			if(binary_img[i][10] == 0x00) bina = 2;//黑
//			if(bina == 2){
//				fop_flag = 0;
//				if(binary_img[i][9] == 0xFF) found_point[2] = i, found_point[3] = 10, fop_flag = 1;//起始点为前后跳变
//				return;
//			}
//			if(!bina){found_point[2] = i, found_point[3] = 10, fop_flag = 2;return;}//起始点为突变点
//		//	右基点寻找
//			for(j = 10; j < 20; j++){
//			//	重置
//				bina_temp = bina;
//				bina = 0;
//				if(binary_img[i][j] == 0xFF) bina = 1;//白
//				if(binary_img[i][j] == 0x00) bina = 2;//黑
//				if(!bina){found_point[2] = i, found_point[3] = j, fop_flag = 2;return;}//找到突变点
//				if(bina!=bina_temp){//前后跳变
//					fop_flag = 1;
//					if(bina==1) j-=1;	
//					found_point[2] = i, found_point[3] = j;
//					return;
//				}
//			}
//		//	未找到基点，向上寻找
//			j = 19;
//			for(i = MT9V03X_H - 1; i > 0; i--){
//			//	重置
//				bina_temp = bina;
//				bina = 0;
//				if(binary_img[i][j] == 0xFF) bina = 1;//白
//				if(binary_img[i][j] == 0x00) bina = 2;//黑
//				if(!bina){found_point[2] = i, found_point[3] = j, fop_flag = 2;return;}//找到突变点
//				if(bina!=bina_temp){//上下跳变
//					fop_flag = 3, found_point[2] = i, found_point[3] = j;
//					return;
//				}
//			}
//			break;
//	}
//}
/*------------------------------*/
/*		  二值化显示模块		*/
/*==============================*/
void binary_disp(void){
//	变量定义
	register unsigned char i, j, k;
	unsigned char column = MT9V03X_W>>3;
	unsigned char binary_temp;
//	图像显示
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
//	显示基点
	for(k = 0; k < 8; k++)
		ips200_drawpoint(found_point[1]*8+k, found_point[0], 0x00);
	for(k = 0; k < 8; k++)
		ips200_drawpoint(found_point[3]*8+k, found_point[2], 0x00);
	ips200_showint16(0, 7, found_point[0]);
	ips200_showint16(0, 8, found_point[1]);
}
/*------------------------------*/
/*		  图像二值化模块		*/
/*==============================*/
void img_binary(void){
//	变量定义
	register unsigned char i, j, k;
	unsigned char column = MT9V03X_W>>3;
	unsigned char binary_temp;
//	图像二值化
	for(i = 0; i < MT9V03X_H; i++){
		for(j = 0; j < column; j++){
		//	重置图像
			binary_img[i][j] = 0x00;
		//	二值化
			for(k = 0; k < 8; k++){
				binary_img[i][j] <<= 1;
				if(mt9v03x_image[i][j*8+k] > img_thrsod) binary_img[i][j] |= 0x01;
			}
		//	左边BUG处理
			if(!j)
				if(binary_img[i][0] == 127) binary_img[i][0] = 0xFF;
			if(j == 19)
				if(binary_img[i][0] == 254) binary_img[i][0] = 0xFF;
		}
	}
}
/*------------------------------*/
/*		大津法全局阈值模块		*/
/*==============================*/
void otsu(void){
//	变量定义
	float var = 0, vartmp = 0;
	float imgsize = MT9V03X_H*MT9V03X_W, sumPK = 0, sumMK = 0;
	register short i, j;
	unsigned short hist[grayscale] = {0};	
//	获取直方图
	for(i = 0; i < MT9V03X_H; i++){
		for(j = 0; j < MT9V03X_W; j++)
			hist[mt9v03x_image[i][j]]++;
	}
//	求类间方差
	for(i = 0; i < grayscale; i++){
		P[i] = (float)hist[i]/imgsize;//计算每个灰度级出现的概率
		PK[i] = sumPK+P[i];//概率累计和
		sumPK = PK[i];
		MK[i] = sumMK+i*P[i];//灰度值累加均值
		sumMK = MK[i];
	}
	for(i = 115; i < 200; i++){
		vartmp = ((MK[grayscale-1]*PK[i] - MK[i])*(MK[grayscale-1]*PK[i] - MK[i])) / (PK[i]*(1 - PK[i]));
		if(vartmp > var){
			var = vartmp;
			img_thrsod = i;//输出阈值
		}
	}
	img_binary();
	fop_search(1);
	fop_search(2);
	if(csimenu_flag[0]) binary_disp();
	if(csimenu_flag[1]) ips200_displayimage032(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
}
