/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "CAM.h"
#include "pid.h"
#include "data.h"
#include "zf_tim.h"
#include "SEEKFREE_MT9V03X.h"
#include "SEEKFREE_IPS200_PARALLEL8.h"
/*--------------------------------------------------------------*/
/* 							  宏定义 							*/
/*==============================================================*/
#define uint16 unsigned short
/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/
short show_value[5];
short angle_change[4];
short ave_slope[4];
unsigned char show_point;
unsigned char ac_flag[4];
unsigned char exti_leftop, exti_rigtop;
unsigned char lefhigh, righigh;
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
/*------------------------------*/
/*		   图像显示模块 		*/
/*==============================*/
void binary_disp(void){
//	变量定义
	register unsigned char i, j, k;
	unsigned char column = (MT9V03X_W-4)>>3;
	unsigned char binary_temp;
//	图像显示
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
//	显示边界
	for(i = MT9V03X_H - 1; i > lcut; i--) ips200_drawpoint(lefbor[i], i, 0x00);
	for(i = MT9V03X_H - 1; i > rcut; i--) ips200_drawpoint(rigbor[i], i, 0x00);
//	显示出口
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
//	显示垂直边线
	for(i = 0; i < lefbottom_cut; i++)
		ips200_drawpoint(i, bottombor[i], 0x74ec);
	for(i = 0; i < leftop_cut; i++)
		ips200_drawpoint(i,topbor[i], 0x74ec);
	for(i = 159; i > rigbottom_cut; i--)
		ips200_drawpoint(i, bottombor[i], 0x74ec);
	for(i = 159; i > rigtop_cut; i--)
		ips200_drawpoint(i, topbor[i], 0x74ec);	
//	ips200_showint16(160, 0, act_flag);
//	ips200_showint16(160, 1, turn_flag);
//	ips200_showint16(170, 1, show_value[0]);
//	ips200_showint16(170, 2, show_value[1]);
//	ips200_showint16(170, 3, show_value[2]);
//	ips200_showint16(170, 4, show_value[3]);
//	ips200_showint16(170, 5, show_value[4]);
	
//	ips200_showfloat(170, 1, show_value[0], 2, 2);
//	ips200_showfloat(170, 2, show_value[1], 2, 2);
//	ips200_showfloat(170, 3, show_value[2], 2, 2);
//	ips200_showfloat(170, 4, show_value[3], 2, 2);
//	ips200_showfloat(170, 5, show_value[4], 2, 2);
	
//	ips200_drawpoint(0, show_point+1, RED);
//	ips200_drawpoint(0, show_point+2, RED);
//	ips200_drawpoint(0, show_point+1, RED);
//	ips200_drawpoint(0, show_point+2, RED);
}
/*------------------------------*/
/*	   	 边线斜率分析模块		*/
/*==============================*/
char slope_cal(char num){
//	变量定义
	register char i;
	unsigned char mp[5];
	short slope[4];
	short angle[3];
//	计算斜率
	switch(num){
	//	左转
		case 1:
			mp[0] = rcut+1, mp[4] = found_point[2];
			mp[2] = (mp[0]+mp[4])>>1;
			mp[1] = (mp[0]+mp[2])>>1, mp[3] = (mp[2]+mp[4])>>1;
		//	计算斜率
			for(i = 0; i < 4; i++)
				slope[i] = atan((float)(rigbor[mp[i+1]]-rigbor[mp[i]])/(float)(mp[i+1]-mp[i]))*573;
			for(i = 0; i < 3; i++)
				angle[i] = slope[i+1] - slope[i];
			if(angle[0]+angle[1]+angle[2] < -150) return 1;
			else return 0;
//			show_value[0] = angle[0]+angle[1]+angle[2];
			break;
	//	右转
		case 2:
			mp[0] = lcut+1, mp[4] = found_point[0];
			mp[2] = (mp[0]+mp[4])>>1;
			mp[1] = (mp[0]+mp[2])>>1, mp[3] = (mp[2]+mp[4])>>1;
		//	计算斜率
			for(i = 0; i < 4; i++)
				slope[i] = atan((float)(lefbor[mp[i+1]]-lefbor[mp[i]])/(float)(mp[i+1]-mp[i]))*573;
			for(i = 0; i < 3; i++)
				angle[i] = slope[i+1] - slope[i];
			if(angle[0]+angle[1]+angle[2] > 150) return 1;
			else return 0;
//			show_value[1] = angle[0]+angle[1]+angle[2];
			break;
	//	右直线检测 | 左环道辅助
		case 3:
			mp[0] = ltraf_point_row[exti_leftop], mp[4] = found_point[0];
			mp[2] = (mp[0]+mp[4])>>1;
			mp[1] = (mp[0]+mp[2])>>1, mp[3] = (mp[2]+mp[4])>>1;
		//	计算斜率
			for(i = 0; i < 4; i++)
				slope[i] = atan((float)(rigbor[mp[i+1]]-rigbor[mp[i]])/(float)(mp[i+1]-mp[i]))*573;
			for(i = 0; i < 3; i++)
				angle[i] = slope[i+1] - slope[i];
			show_value[0] = angle[0]+angle[1]+angle[2];
			if(abs(angle[0]+angle[1]+angle[2]) < 500) return 1;
			else return 0;
			break;
	}
}
/*------------------------------*/
/*	   垂直边线斜率分析模块		*/
/*==============================*/
void vert_slope_cal(char num){//点太少不要执行这个函数
//	变量定义
	register char i;
	unsigned char mp[4];
	short slope[3];
	short angle[2];
//	计算斜率
	switch(num){
	//	左上
		case 0:
			mp[0] = 0;
			mp[2] = leftop_cut>>1;
			mp[1] = mp[2]>>1, mp[3] = (mp[2]+leftop_cut)>>1;
		//	计算斜率
			for(i = 0; i < 3; i++)
				slope[i] = atan((float)(topbor[mp[i+1]]-topbor[mp[i]])/(float)(mp[i+1]-mp[i]))*573;
		//	计算角度变化
			for(i = 0; i < 2; i++)
				angle[i] = slope[i+1] - slope[i];
			angle_change[0] = angle[0]+angle[1], ac_flag[0] = 1;
			ave_slope[0] = (angle[0]+angle[1])>>1;
//			show_value[0] = angle_change[0];
//			show_value[0] = ave_slope[0];
			break;
	//	左下
		case 1:
			mp[0] = 0;
			mp[2] = lefbottom_cut>>1;
			mp[1] = mp[2]>>1, mp[3] = (mp[2]+lefbottom_cut)>>1;
			for(i = 0; i < 3; i++)
				slope[i] = atan((float)(bottombor[mp[i+1]]-bottombor[mp[i]])/(float)(mp[i+1]-mp[i]))*573;
			for(i = 0; i < 2; i++)
				angle[i] = slope[i+1] - slope[i];
			angle_change[1] = angle[0]+angle[1], ac_flag[1] = 1;
			ave_slope[1] = (angle[0]+angle[1])>>1;
//			show_value[1] = angle_change[1];
//			show_value[1] = ave_slope[1];
			break;
	//	右上
		case 2:
			mp[0] = 159;
			mp[2] = (rigtop_cut+mp[0])>>1;
			mp[1] = (mp[0]+mp[2])>>1, mp[3] = (mp[2]+rigtop_cut)>>1;
			for(i = 0; i < 3; i++)
				slope[i] = atan((float)(topbor[mp[i+1]]-topbor[mp[i]])/(float)(mp[i+1]-mp[i]))*573;
			for(i = 0; i < 2; i++)
				angle[i] = slope[i+1] - slope[i];
			angle_change[2] = angle[0]+angle[1], ac_flag[2] = 1;
			ave_slope[2] = (angle[0]+angle[1])>>1;
//			show_value[2] = angle_change[2];
//			show_value[2] = ave_slope[2];
			break;
	//	右下
		case 3:
			mp[0] = 159;
			mp[2] = (rigbottom_cut+mp[0])>>1;
			mp[1] = (mp[0]+mp[2])>>1, mp[3] = (mp[2]+rigbottom_cut)>>1;
			for(i = 0; i < 3; i++)
				slope[i] = atan((float)(bottombor[mp[i+1]]-bottombor[mp[i]])/(float)(mp[i+1]-mp[i]))*573;
			for(i = 0; i < 2; i++)
				angle[i] = slope[i+1] - slope[i];
			angle_change[3] = angle[0]+angle[1], ac_flag[3] = 1;
			ave_slope[3] = (angle[0]+angle[1])>>1;
//			show_value[3] = angle_change[3];
//			show_value[3] = ave_slope[3];
			break;
	}
}
/*------------------------------*/
/*	  垂直边界点寻找模块（左）	*/
/*==============================*/
void vert_leftsearch(unsigned char top, unsigned char bottom){
//	变量定义
	register unsigned char i;
	register char j, k;
	unsigned char col = (MT9V03X_W-4)>>3;//换行
	unsigned char *p;
	unsigned char found_flag, view_temp;
//	寻找下边界
	lefbottom_cut = 0;
	for(j = 0; j < 19; j++){
		found_flag = 0, p = &binary_img[exti_lefp[0]][j];
		for(i = exti_lefp[0]; i < bottom; i++, p+=col){
			view_temp = *(p)^*(p+col);
			for(k = 7; k > -1; k--){
				if(!((found_flag>>k)&0x01))
					if((view_temp>>k)&0x01){
						bottombor[(j<<3)+7-k] = i;
						if(lefbottom_cut < (j<<3)+7-k) lefbottom_cut =(j<<3)+7-k;
					}
			}
			found_flag |= view_temp;
			if(found_flag == 0xFF) break;
			if(*(p+col) == 0x00) break;
		}
		if(found_flag != 0xFF) break;
	}
	if(lefbottom_cut > 5) vert_slope_cal(1);
//	寻找上边界
	leftop_cut = 0;
	for(j = 0; j < 19; j++){
		found_flag = 0, p = &binary_img[exti_lefp[0]][j];
		for(i = exti_lefp[0]; i > top; i--, p-=col){
			view_temp = *(p)^*(p-col);
			for(k = 7; k > -1; k--){
				if(!((found_flag>>k)&0x01))
					if((view_temp>>k)&0x01){
						topbor[(j<<3)+7-k] = i;
						if(leftop_cut < (j<<3)+7-k) leftop_cut =(j<<3)+7-k;
					}
			}
			found_flag |= view_temp;
			if(found_flag == 0xFF) break;
			if(*(p+col) == 0x00) break;
		}
		if(found_flag != 0xFF) break;
	}
	if(leftop_cut > 5) vert_slope_cal(0);
}
/*------------------------------*/
/*	  垂直边界点寻找模块（右）	*/
/*==============================*/
void vert_rightsearch(unsigned char top, unsigned char bottom){
//	变量定义
	register unsigned char i;
	register char j, k;
	unsigned char col = (MT9V03X_W-4)>>3;//换行
	unsigned char *p;
	unsigned char found_flag, view_temp;
//	寻找下边界
	rigbottom_cut = 159;
	for(j = 19; j > -1; j--){
		found_flag = 0, p = &binary_img[exti_rigp[0]][j];
		for(i = exti_rigp[0]; i < bottom; i++, p+=col){
			view_temp = *(p)^*(p+col);
			for(k = 7; k > -1; k--){
				if(!((found_flag>>k)&0x01))
					if((view_temp>>k)&0x01){
						bottombor[(j<<3)+7-k] = i;
						if(rigbottom_cut > (j<<3)+7-k) rigbottom_cut =(j<<3)+7-k;
					}
			}
			found_flag |= view_temp;
			if(found_flag == 0xFF) break;
			if(*(p+col) == 0x00) break;
		}
		if(found_flag != 0xFF) break;
	}
	if(rigbottom_cut > 5) vert_slope_cal(3);
//	寻找上边界
	rigtop_cut = 159;
	for(j = 19; j > -1; j--){
		found_flag = 0, p = &binary_img[exti_rigp[0]][j];
		for(i = exti_rigp[0]; i > top; i--, p-=col){
			view_temp = *(p)^*(p-col);
			for(k = 7; k > -1; k--){
				if(!((found_flag>>k)&0x01))
					if((view_temp>>k)&0x01){
						topbor[(j<<3)+7-k] = i;
						if(rigtop_cut > (j<<3)+7-k) rigtop_cut =(j<<3)+7-k;
					}
			}
			found_flag |= view_temp;
			if(found_flag == 0xFF) break;
			if(*(p+col) == 0x00) break;
		}
		if(found_flag != 0xFF) break;
	}
	if(rigtop_cut > 5) vert_slope_cal(2);
}
/*------------------------------*/
/*	  边界跳变点寻找模块（左）	*/
/*==============================*/
void border_vertical_leftsearch(void){
//	变量定义
	register unsigned char i, k;
	unsigned char vetflag = 0;
	unsigned char vet_colmax, vet_rowmax;
//	垂直边界寻找
	if(ltraf_count > 1)
		for(i = 1; i < ltraf_count; i++){
		//	左外凸
			if(ltraf_flag[i] == 0)
				if(ltraf_flag[i-1] == 1){
					for(k = ltraf_point_row[i], vet_colmax = 0; k < ltraf_point_row[i-1]; k++) 
						if(lefbor[k] > vet_colmax) vet_colmax = lefbor[k], vet_rowmax = k; 
					lvet_trafpoint_row[lvet_trafcount] = vet_rowmax, lvet_trafpoint_col[lvet_trafcount] = vet_colmax, lvet_trafcount++;
				}
		//	出口
			if(ltraf_flag[i] == 1)
				if(ltraf_flag[i-1] == 0){ 
					exti_lefp[exti_lefcount] = (ltraf_point_row[i]+ltraf_point_row[i-1])>>1, exti_lefcount++;
					if(!vetflag){
						if(i+1 < ltraf_count) {vetflag = 1, exti_leftop = i, lefhigh = ltraf_point_row[i-1] - ltraf_point_row[i];vert_leftsearch(ltraf_point_row[i+1], ltraf_point_row[i-1]+7);}
						else {vetflag = 1, exti_leftop = i, lefhigh = ltraf_point_row[i-1] - ltraf_point_row[i];vert_leftsearch(ltraf_point_row[i]-3, ltraf_point_row[i-1]+7);}
					}
				}
		}
}
/*------------------------------*/
/*	  边界跳变点寻找模块（右）	*/
/*==============================*/
void border_vertical_rightsearch(void){
//	变量定义
	register unsigned char i, k;
	unsigned char vetflag = 0;
	unsigned char vet_colmax, vet_rowmax;
//	寻找跳变
	if(rtraf_count > 1)
		for(i = 1; i < rtraf_count; i++){
		//	右外凸
			if(rtraf_flag[i] == 0)
				if(rtraf_flag[i-1] == 1){
					for(k = rtraf_point_row[i], vet_colmax = 159; k < rtraf_point_row[i-1]; k++) 
						if(rigbor[k] < vet_colmax) vet_colmax = rigbor[k], vet_rowmax = k; 
					rvet_trafpoint_row[rvet_trafcount] = vet_rowmax, rvet_trafpoint_col[rvet_trafcount] = vet_colmax, rvet_trafcount++;
				}
		//	出口
			if(rtraf_flag[i] == 1){
				if(rtraf_flag[i-1] == 0) exti_rigp[exti_rigcount] = (rtraf_point_row[i]+rtraf_point_row[i-1])>>1, exti_rigcount++;
				if(!vetflag){
					if(i+1 < rtraf_count){vetflag = 1, exti_rigtop = i, righigh = rtraf_point_row[i-1] - rtraf_point_row[i];vert_rightsearch(rtraf_point_row[i+1], rtraf_point_row[i-1]+7);}
					else {vetflag = 1, exti_rigtop = i, righigh = rtraf_point_row[i-1] - rtraf_point_row[i];vert_rightsearch(rtraf_point_row[i]-3, rtraf_point_row[i-1]+7);}
				}
			}
		}
}
/*------------------------------*/
/*		 左边界点寻找模块		*/
/*==============================*/
void lbor_search(void){
//	变量定义
	short traf_slope, traf_slope_temp;
	register unsigned char i = found_point[0], k;
	register char j = found_point[1];
	unsigned char col = (MT9V03X_W-4)>>3;//换行
	unsigned char *p;
	unsigned char traf_flag, traf_flag_temp;
	char sloptraf, lcmax = 0, lcmin = 0, local_flag1, local_flag1_temp, local_flag2, local_flag2_temp;
//	初始化
	lcut = 0, ltraf_count = 0, lefbottom_cut = 0, leftop_cut = 0;
	lvet_trafcount = 0, exti_lefcount = 0, leftop_cut = 0, lefbottom_cut = 0;
	for(k = 0; k < 4; k++) ac_flag[k] = 0;
//	从基准点开始寻找
	p = &binary_img[i][j];
	switch(fop_flag){
		case 1://基准点在最底下
			for(k = 0; k < 7; k++)
				if(((*p>>(k+1))&0x01)^((*p>>k)&0x01)){lefbor[i] = (j<<3)+6-k;break;}
			break;
		case 2:
			lefbor[i] = (j<<3)+7;
			break;
		case 3://基准点不在底下
			for(k = 0; k < 7; k++)
				if(((*p>>(k+1))&0x01)^((*p>>k)&0x01)){lefbor[i] = (j<<3)+6-k;break;}
			for(k = MT9V03X_H - 1; k > i; k--) lefbor[k] = 0;
			break;
	}
//	向上检测
	for(i--; i > 3; i--){
	//	初始化换行、方向检测
		p -= col;
		if(traf_flag!=traf_flag_temp) 
			if(i < 90)//不检测最底下的转变点
				if(ltraf_point_row[ltraf_count-1]-i>5 || ltraf_count == 0){//限制转变点高度
					if(traf_flag == 1) ltraf_flag[ltraf_count] = 0;//从右到左转变
					else ltraf_flag[ltraf_count] = 1;//从左到右转变
					ltraf_point_row[ltraf_count] = i+1, ltraf_point_col[ltraf_count] = lefbor[i+1], ltraf_count++;
				}
		traf_flag_temp = traf_flag;
		traf_flag = 0;
		if(*(p+1) != 0x00)
			if(*(p+1) != 0xff){//右突变
				for(k = 0; k < 7; k++)
					if(((*(p+1)>>(k+1))&0x01)^((*(p+1)>>k)&0x01)){lefbor[i] = ((j+1)<<3)+6-k;break;}
					continue;
			}
		if(*p != 0x00)
			if(*p != 0xff){//左突变
				for(k = 0; k < 7; k++)
					if(((*p>>(k+1))&0x01)^((*p>>k)&0x01)){lefbor[i] = (j<<3)+6-k;break;}
					continue;
			}
		if(*(uint16*)p == 0xFF00){lefbor[i] = (j<<3)+7;continue;}//左黑右白跳变
		if(*(uint16*)p == 0x00FF){lefbor[i] = (j+1)<<3;continue;}//左白右黑跳变
	//	全黑
		if(*(uint16*)p == 0x0000){
			for(p++, j++; j < 19; p++, j++){//向右寻找跳变点
				if(*(uint16*)p == 0x0000) continue;
				else break;
			}
			if(j == 19){//抵达边界，停止搜索
				lcut = i;
				if(traf_flag_temp)
					if(ltraf_point_row[ltraf_count-1]-i>5 || ltraf_count == 0)
						ltraf_flag[ltraf_count] = 1, ltraf_point_row[ltraf_count] = i+1, ltraf_point_col[ltraf_count] = lefbor[i+1], ltraf_count++;
				return;
			}
			if(*(uint16*)p == 0xFF00){lefbor[i] = (j<<3)+7;continue;}//左黑右白跳变
			if(*(uint16*)p == 0x00FF){lefbor[i] = (j+1)<<3;continue;}//左白右黑跳变
			if(*(p+1) != 0x00)
				if(*(p+1) != 0xff){//右突变
					for(k = 0; k < 7; k++)
						if(((*(p+1)>>(k+1))&0x01)^((*(p+1)>>k)&0x01)){lefbor[i] = ((j+1)<<3)+6-k;break;}
						continue;
				}
			if(*p != 0x00)
				if(*p != 0xff){//左突变
					for(k = 0; k < 7; k++)
						if(((*p>>(k+1))&0x01)^((*p>>k)&0x01)){lefbor[i] = (j<<3)+6-k;break;}
						continue;
				}
		}
	//	全白
		if(*(uint16*)p == 0xFFFF){
			for(p--, j--; j > -1; p--, j--){//向左寻找跳变点
				if(*(uint16*)p == 0xFFFF) continue;
				else break;
			}
			if(j == -1){lcut = i;traf_flag = 1,j+=6,p+=6,lefbor[i] = 0;continue;}//抵达全白边界
			if(*(uint16*)p == 0xFF00){lefbor[i] = (j<<3)+7;continue;}//左黑右白跳变
			if(*(uint16*)p == 0x00FF){lefbor[i] = (j+1)<<3;continue;}//左白右黑跳变
			if(*p != 0x00)
				if(*p != 0xff){//左突变
					for(k = 0; k < 7; k++)
						if(((*p>>(k+1))&0x01)^((*p>>k)&0x01)){lefbor[i] = (j<<3)+6-k;break;}
						continue;
				}
			if(*(p+1) != 0x00)
				if(*(p+1) != 0xff){//右突变
					for(k = 0; k < 7; k++)
						if(((*(p+1)>>(k+1))&0x01)^((*(p+1)>>k)&0x01)){lefbor[i] = ((j+1)<<3)+6-k;break;}
						continue;
				}
		}
	}
	lcut = i;//退出前保存所在行
}
/*------------------------------*/
/*		 右边界点寻找模块		*/
/*==============================*/
void rbor_search(void){
//	变量定义
	register unsigned char i = found_point[2], k;
	register char j = found_point[3];
	unsigned char col = (MT9V03X_W-4)>>3;//换行
	unsigned char traf_flag, traf_flag_temp;
	unsigned char *p;
//	初始化
	rcut = 0, rtraf_count = 0, rigbottom_cut = 159, rigtop_cut = 159;
	rvet_trafcount = 0, exti_rigcount = 0, rigtop_cut = 159, rigbottom_cut = 159;
//	从基准点开始寻找
	p = &binary_img[i][j];
	switch(fop_flag){
		case 1://基准点在最底下
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
//	限制边界
	if(j == 19) j--, p--;
//	向上检测
	for(i--; i > 3; i--){
	//	初始化换行、方向检测
		p -= col;
		if(traf_flag!=traf_flag_temp) 
			if(i < 90)//不检测最底下的转变点
				if(rtraf_point_row[rtraf_count-1]-i>5 || rtraf_count == 0){//限制转变点高度
					if(traf_flag == 1) rtraf_flag[rtraf_count] = 0;//从左到右转变
					else rtraf_flag[rtraf_count] = 1;//从右到左转变
					rtraf_point_row[rtraf_count] = i+1, rtraf_point_col[rtraf_count] = rigbor[i+1], rtraf_count++;
				}
		traf_flag_temp = traf_flag;
		traf_flag = 0;
		if(*p != 0x00)
			if(*p != 0xff){//左突变
				for(k = 7; k > 0; k--)
					if(((*p>>k)&0x01)^((*p>>(k-1))&0x01)){rigbor[i] = (j<<3)+8-k;break;}
					continue;
			}
		if(*(p+1) != 0x00)
			if(*(p+1) != 0xff){//右突变
				for(k = 7; k > 0; k--)
					if(((*(p+1)>>k)&0x01)^((*(p+1)>>(k-1))&0x01)){rigbor[i] = ((j+1)<<3)+8-k;break;}
					continue;
			}
		if(*(uint16*)p == 0x00FF){rigbor[i] = ((j+1)<<3);continue;}//左白右黑跳变
		if(*(uint16*)p == 0xFF00){rigbor[i] = (j<<3)+7;continue;}//左黑右白跳变
	//	全黑
		if(*(uint16*)p == 0x0000){
			for(p--, j--; j >  -1; p--, j--){//向左寻找跳变点
				if(*(uint16*)p == 0x0000) continue;
				else break;
			}
			if(j == -1){//抵达边界，停止搜索
				rcut = i;
				if(traf_flag_temp)//转变点检测
					if(rtraf_point_row[rtraf_count-1]-i>5 || rtraf_count == 0)//限制转变点高度
						rtraf_point_row[rtraf_count] = i+1, rtraf_point_col[rtraf_count] = rigbor[i+1], rtraf_count++;
				return;
			}
			if(*(uint16*)p == 0x00FF){rigbor[i] = ((j+1)<<3);continue;}//左白右黑跳变
			if(*(uint16*)p == 0xFF00){rigbor[i] = (j<<3)+7;continue;}//左黑右白跳变
			if(*p != 0x00)
				if(*p != 0xff){//左突变
					for(k = 7; k > 0; k--)
						if(((*p>>k)&0x01)^((*p>>(k-1))&0x01)){rigbor[i] = (j<<3)+8-k;break;}
						continue;
				}
			if(*(p+1) != 0x00)
				if(*(p+1) != 0xff){//右突变
					for(k = 7; k > 0; k--)
						if(((*(p+1)>>k)&0x01)^((*(p+1)>>(k-1))&0x01)){rigbor[i] = ((j+1)<<3)+8-k;break;}
						continue;
				}
		}
	//	全白
		if(*(uint16*)p == 0xFFFF){
			for(p++, j++; j < 19; p++, j++){//向右寻找跳变点
				if(*(uint16*)p == 0xFFFF) continue;
				else break;
			}
			if(j == 19){rcut = i;traf_flag = 1;j-=6, p-=6,rigbor[i] = 159;continue;}
			if(*(uint16*)p == 0x00FF){rigbor[i] = ((j+1)<<3);continue;}//左白右黑跳变
			if(*(uint16*)p == 0xFF00){rigbor[i] = (j<<3)+7;continue;}//左黑右白跳变
			if(*(p+1) != 0x00)
				if(*(p+1) != 0xff){//右突变
					for(k = 7; k > 0; k--)
						if(((*(p+1)>>k)&0x01)^((*(p+1)>>(k-1))&0x01)){rigbor[i] = ((j+1)<<3)+8-k;break;}
						continue;
				}
			if(*p != 0x00)
				if(*p != 0xff){//左突变
					for(k = 7; k > 0; k--)
						if(((*p>>k)&0x01)^((*p>>(k-1))&0x01)){rigbor[i] = (j<<3)+8-k;break;}
						continue;
				}
		}
	}
	rcut = i;//退出前保存所在行
}
/*------------------------------*/
/*		基准点辅助模块（左）	*/
/*==============================*/
static void left_fop_search_sup(unsigned char* p, char j){
//	一般情况
	if(*p != 0xFF)
		if(*p != 0x00){//左视场突变
			fop_flag = 1, found_point[0] = MT9V03X_H - 1, found_point[1] = j;
			return;
		}
	if(*(p+1) != 0xFF)
		if(*(p+1) != 0x00){//右视场突变
			fop_flag = 1, found_point[0] = MT9V03X_H - 1, found_point[1] = j+1;
			return;
		}
//	左黑右白
	fop_flag = 2, found_point[0] = MT9V03X_H - 1, found_point[1] = j;
}
/*------------------------------*/
/*		基准点辅助模块（右）	*/
/*==============================*/
static void right_fop_search_sup(unsigned char* p, char j){
//	一般情况
	if(*p != 0xFF)
		if(*p != 0x00){//左视场突变
			fop_flag = 1, found_point[2] = MT9V03X_H - 1, found_point[3] = j;
			return;
		}
	if(*(p+1) != 0xFF)
		if(*(p+1) != 0x00){//右视场突变
			fop_flag = 1, found_point[2] = MT9V03X_H - 1, found_point[3] = j+1;
			return;
		}
//	左白右黑
	fop_flag = 2, found_point[2] = MT9V03X_H - 1, found_point[3] = j+1;
}
/*------------------------------*/
/*		 基准点寻找模块（左）	*/
/*==============================*/
void left_fop_search(void){
//	变量定义
	register unsigned char i = MT9V03X_H - 1;
	register char j;
	unsigned char* p;
//	左基点寻找 
	p = &binary_img[i][8];
	for(j = 8; j > -1; j--){
		if(*(uint16*)p == 0xFFFF){p--;continue;}
		left_fop_search_sup(p, j);
		return;
	}
//	未找到基点，向上寻找
	for(i = MT9V03X_H - 1; i > 0; i--){
		if(!(binary_img[i][0]^binary_img[i-1][0])) continue;
		fop_flag = 3, found_point[0] = i, found_point[1] = 0;
		return;
	}
}
/*------------------------------*/
/*		 基准点寻找模块（右）	*/
/*==============================*/
void right_fop_search(void){
//	变量定义
	register unsigned char i = MT9V03X_H - 1;
	register char j;
	unsigned char* p;
//	右基点寻找 
	p = &binary_img[i][10];
	for(j = 10; j < 19; j++){
		if(*(uint16*)p == 0xFFFF){p++;continue;}
		right_fop_search_sup(p, j);
		return;
	}
//	未找到基点，向上寻找
	for(i = MT9V03X_H - 1; i > 0; i--){
		if(!(binary_img[i][19]^binary_img[i-1][19])) continue;
		fop_flag = 3, found_point[2] = i, found_point[3] = 19;
		return;
	}
}
/*------------------------------*/
/*		  图像二值化模块		*/
/*==============================*/
void img_binary(void){
//	变量定义
	register unsigned char i, j, k;
	unsigned char column = (MT9V03X_W-4)>>3;
	unsigned char binary_temp;
//	图像二值化
	for(i = 0; i < MT9V03X_H; i++){
		for(j = 0; j < column; j++){
		//	重置图像
			binary_img[i][j] = 0x00;
		//	二值化
			for(k = 0; k < 8; k++){
				binary_img[i][j] <<= 1;
				if(mt9v03x_image[i][(j<<3)+k+2] > img_thrsod) binary_img[i][j] |= 0x01;
			}
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
}
