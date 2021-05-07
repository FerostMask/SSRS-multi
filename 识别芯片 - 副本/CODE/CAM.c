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
/*		  二值化显示模块		*/
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
//	显示转变点
	for(i = 0; i < ltraf_count; i++){//左边界转变
		ips200_drawpoint(ltraf_point_col[i], ltraf_point_row[i], 0x32D5);
		ips200_drawpoint(ltraf_point_col[i]+1, ltraf_point_row[i], 0x32D5);
		ips200_drawpoint(ltraf_point_col[i]+2, ltraf_point_row[i], 0x32D5);
		ips200_drawpoint(ltraf_point_col[i]+3, ltraf_point_row[i], 0x32D5);
		ips200_drawpoint(ltraf_point_col[i]+4, ltraf_point_row[i], 0x32D5);
	}
	for(i = 0; i < lvet_trafcount; i++){//左转变点
		ips200_drawpoint(lvet_trafpoint_col[i], lvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(lvet_trafpoint_col[i]+1, lvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(lvet_trafpoint_col[i]+2, lvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(lvet_trafpoint_col[i]+3, lvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(lvet_trafpoint_col[i]+4, lvet_trafpoint_row[i], 0xFD10);
	}
	for(i = 0; i < rtraf_count; i++){//右边界转变
		ips200_drawpoint(rtraf_point_col[i], rtraf_point_row[i], 0x32D5);
		ips200_drawpoint(rtraf_point_col[i]-1, rtraf_point_row[i], 0x32D5);
		ips200_drawpoint(rtraf_point_col[i]-2, rtraf_point_row[i], 0x32D5);
		ips200_drawpoint(rtraf_point_col[i]-3, rtraf_point_row[i], 0x32D5);
		ips200_drawpoint(rtraf_point_col[i]-4, rtraf_point_row[i], 0x32D5);
	}
	for(i = 0; i < rvet_trafcount; i++){//右转变点
		ips200_drawpoint(rvet_trafpoint_col[i], rvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(rvet_trafpoint_col[i]-1, rvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(rvet_trafpoint_col[i]-2, rvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(rvet_trafpoint_col[i]-3, rvet_trafpoint_row[i], 0xFD10);
		ips200_drawpoint(rvet_trafpoint_col[i]-4, rvet_trafpoint_row[i], 0xFD10);
	}
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
//	for(i = 0; i < lefup_cut; i++) ips200_drawpoint(i, upbor[i], 0xA6AE);
	for(i = 0; i < lefbottom_cut; i++)
		ips200_drawpoint(i, bottombor[i], 0x74ec);
	for(i = 0; i < leftop_cut; i++)
		ips200_drawpoint(i,topbor[i], 0x74ec);
	for(i = 159; i > rigbottom_cut; i--)
		ips200_drawpoint(i, bottombor[i], 0x74ec);
	for(i = 159; i > rigtop_cut; i--)
		ips200_drawpoint(i, topbor[i], 0x74ec);

	ips200_showint16(160, 0, act_flag);
	ips200_showint16(160, 1, turn_flag);
	ips200_showint16(170, 1, show_value[0]);
	ips200_showint16(170, 2, show_value[1]);
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

//	ips200_showint16(0, 8, turn_flag);
//	ips200_showint16(0, 9, out_vertical_flag[0]);
}
/*------------------------------*/
/*	   	 边线斜率分析模块		*/
/*==============================*/
static char slope_cal(char num){
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
static void vert_slope_cal(char num){//点太少不要执行这个函数
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
/*	    	状态机模块			*/
/*==============================*/
void state_machine(void){
//	变量定义
	static unsigned char state, state_temp;
	unsigned char line_mark;
	char row_high;
	unsigned char lefcol_width, rigcol_width, lefcross_flag, rigcross_flag;
//	状态检测
	state_temp = state; state = 0;
//	丢边检测
	if(turn_flag == 11)//左转
		if(rtraf_count < 2)
			if(slope_cal(1))
				state = 1;
	if(turn_flag == 12)//右转
		if(ltraf_count < 2)
			if(slope_cal(2))
				state = 2;
//	左环道检测
//	show_value[0] = abs(topbor[leftop_cut-1] - bottombor[lefbottom_cut-1]);
	if(exti_lefcount > 0)//环道前检测
		if(exti_rigcount == 0)
			if(rtraf_count < 2){
			//	进环模式1 | 检测到环道出环
				if(ac_flag[0])
					if(angle_change[0] < -30)
						if(ac_flag[1])
							if(angle_change[1] < 70)
								state = 11;
			//	进环模式2 | 检测到环道入环
				if(ac_flag[1])
					if(angle_change[1] > 140)
						if(ac_flag[0])
							if(angle_change[0] > 0)
								if(ltraf_point_row[exti_leftop] > rcut+1)
									if(slope_cal(3)){
										row_high = abs(topbor[leftop_cut-1] - bottombor[lefbottom_cut-1]);//检测环道上下长度
										if(row_high > 13)
											if(row_high < 26)
												state = 12;
									}
			}
//	环内检测
	if(act_flag == 13)
		if(exti_rigcount > 0)//出环
			state = 14;
	if(act_flag == 14)
		if(lvet_trafcount > 0)
			state = 15;
//	十字检测
	if(ac_flag[0])
		if(ac_flag[1]){
			lefcol_width = leftop_cut - lefbottom_cut;
			if(lefcol_width > 19)
				if(lefcol_width < 45)
					lefcross_flag = 1;
		}
	if(ac_flag[2])
		if(ac_flag[3]){
			rigcol_width =  rigbottom_cut - rigtop_cut;
			if(rigcol_width > 19)
				if(rigcol_width < 45)
					rigcross_flag = 1;
		}
	if(exti_lefcount > 0){//十字1
	//	左边为平行直线
		if(lefcross_flag)
			if(abs(ave_slope[0]-ave_slope[1]) < 40)//上下边线倾角差小于4°
				if(exti_rigcount > 0){
					if(ac_flag[2])
						if(abs(ave_slope[0]+ave_slope[2]) < 40)
							state = 21;
					if(state != 21)
						if(ac_flag[3])
							if(abs(ave_slope[1]+ave_slope[3]) < 40)
								state = 21;
					}
				else{//右丢边
					if(rtraf_flag[0])
						if(lefhigh > 20)
							if(lefhigh < 30)
								if(lefbor[lcut+1] < 130)
								if(rtraf_point_row[0] < exti_lefp[0])
									state = 24, line_mark = 1;
				}
		}
	if(state != 21)
		if(state != 24)
			if(exti_rigcount > 0){
			//	右边为平行直线
				if(rigcross_flag)
					if(exti_lefcount > 0){
						if(ac_flag[0])
							if(abs(ave_slope[0]+ave_slope[2]) < 40)
								state = 21;
						if(state != 21)
							if(ac_flag[1])
								if(abs(ave_slope[1]+ave_slope[3]) < 40)
									state = 21;	
					}
					else{//左丢边
						if(ltraf_flag[0])
							if(righigh > 20)
								if(righigh < 30)
									if(rigbor[rcut+1] > 30)
										if(ltraf_point_row[0] < exti_rigp[0])
											state = 24, line_mark = 0;
					}
			}
	if(act_flag == 21)//十字中1
		if(ltraf_count)
			if(rtraf_count)
				if(ltraf_flag[0])
					if(rtraf_flag[0])
						if(exti_rigcount == 0 || exti_lefcount == 0)
							state = 22;
	if(act_flag == 23){//十字中2
		if(line_mark){
			if(ltraf_count)
				if(rtraf_count)
					if(ltraf_flag[0])
						if(rtraf_flag[0])
							if(exti_lefcount == 0)
								if(lefbor[lcut+1] < 150)
									if(rigbor[rcut+1] > 10)
										state = 25;
		}
		else{
			if(ltraf_count)
				if(rtraf_count)
					if(ltraf_flag[0])
						if(rtraf_flag[0])
							if(exti_rigcount == 0)
								state = 25;
		}
	}
	if(act_flag == 22 || act_flag == 24)//出十字
		if(ltraf_count)
			if(rtraf_count)
				if(ltraf_flag[0])
					if(rtraf_flag[0])
						if(ltraf_point_row[0] > 80 || rtraf_point_row[0] > 80)
							state = 23;
//	状态机
	if(state_temp!=state){//检测到状态跳变
	//	弯道丢边
		if(!act_flag)
			if(state == 1)
				act_flag = 1, img_color = 0x6DDD;
		if(act_flag == 1)
			if(state != 1)
				act_flag = 0, img_color = 0xAE9C;
		if(!act_flag)
			if(state == 2)
				act_flag = 2, img_color = 0x6DDD;
		if(act_flag == 2)
			if(state != 2)
				act_flag = 0, img_color = 0xAE9C;
	//	环道
		if(!act_flag)//进环模式1 | 易碎状态
			if(state == 11){
				act_flag = 11, act_flag_temp = act_flag, img_color = 0x8CF6;
				fragile_flag = 1;
				tim_interrupt_init_ms(TIM_3, 2000, 0, 0);//状态计时
			}
		if(act_flag == 0 || act_flag == 1)//进环模式2
			if(state == 12)
				act_flag = 12, img_color = 0x46D0;
		if(act_flag == 12)
			if(state == 1)//环内
				act_flag = 13, img_color = 0xB6DB;
		if(act_flag == 13)
			if(state == 14){//出环 | 脆弱状态
				act_flag = 14, act_flag_temp = act_flag, img_color = 0xBDB8;
				tim_interrupt_init_ms(TIM_3, 2000, 0, 0);
			}
		if(act_flag == 14)
			if(state == 15)
				act_flag = 0, img_color = 0xAE9C;
	//	十字
		if((!act_flag) || fragile_flag){//十字1 | 强状态 | 可以打破易碎状态
			if(fragile_flag)
				if(act_flag != act_flag_temp){fragile_flag = 0;return;}//检查易碎状态是否可以打破
			if(state == 21)
				act_flag = 21, img_color = 0xCF7E;
		}
		if((!act_flag || fragile_flag)){//十字2 | 强状态
			if(fragile_flag)
				if(act_flag != act_flag_temp){fragile_flag = 0;return;}
			if(state == 24)
				act_flag = 23, img_color = 0xEFBE;
		}
		if(act_flag == 21)
			if(state == 22){//十字中 | 脆弱状态
				act_flag = 22, act_flag_temp = 22, img_color = 0xA75C;
				tim_interrupt_init_ms(TIM_3, 2000, 0, 0);
			}
		if(act_flag == 23)
			if(state == 25){//十字中 | 脆弱状态
				act_flag = 24, act_flag_temp = 24, img_color = 0xFFFF;
				tim_interrupt_init_ms(TIM_3, 2000, 0, 0);
			}
		if(act_flag == 22 || act_flag == 24)
			if(state == 23)//出十字
				act_flag = 0, img_color = 0xAE9C;
	}
}
/*------------------------------*/
/*	    垂直边界点寻找模块		*/
/*==============================*/
void vert_search(char num, unsigned char top, unsigned char bottom){
//	变量定义
	register unsigned char i;
	register char j, k;
	unsigned char col = (MT9V03X_W-4)>>3;//换行
	unsigned char *p;
	unsigned char found_flag, view_temp;
//	垂直边线寻找
	show_point = bottom;
	switch(num){
		case 1:
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
			break;
		case 2:
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
			break;
	}
}
/*------------------------------*/
/*	    边界跳变点寻找模块		*/
/*==============================*/
void border_vertical_search(char num){
//	变量定义
	register unsigned char i, k, k2;
	register char j;
	unsigned char col = (MT9V03X_W-4)>>3;//换行
	unsigned char *p;
	unsigned char vetflag = 0;
	unsigned char vet_colmax, vet_rowmax;
//	垂直边界寻找
	switch(num){
		case 1:
		//	变量初始化
			lvet_trafcount = 0, exti_lefcount = 0;
			leftop_cut = 0, lefbottom_cut = 0;
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
								if(i+1 < ltraf_count) {vetflag = 1, exti_leftop = i, lefhigh = ltraf_point_row[i-1] - ltraf_point_row[i];vert_search(1, ltraf_point_row[i+1], ltraf_point_row[i-1]+7);}
								else {vetflag = 1, exti_leftop = i, lefhigh = ltraf_point_row[i-1] - ltraf_point_row[i];vert_search(1, ltraf_point_row[i]-3, ltraf_point_row[i-1]+7);}
							}
						}
				}
			break;
		case 2:
		//	变量初始化
			rvet_trafcount = 0, exti_rigcount = 0;
			rigtop_cut = 159, rigbottom_cut = 159;
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
							if(i+1 < rtraf_count){vetflag = 1, exti_rigtop = i, righigh = rtraf_point_row[i-1] - rtraf_point_row[i];vert_search(2, rtraf_point_row[i+1], rtraf_point_row[i-1]+7);}
							else {vetflag = 1, exti_rigtop = i, righigh = rtraf_point_row[i-1] - rtraf_point_row[i];vert_search(2, rtraf_point_row[i]-3, rtraf_point_row[i-1]+7);}
						}
						}
				}
			break;
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
//	初始化
	lcut = 0, turn_flag = 0, ltraf_count = 0, lefbottom_cut = 0, leftop_cut = 0;
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
			turn_flag = 1;
			for(k = MT9V03X_H - 1; k > i; k--) lefbor[k] = 0;
			break;
	}
//	向上检测
	for(i--; i > 0; i--){
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
	//	检测水平边线跳变点
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
			if(j == -1){lcut = i;traf_flag = 1,j++,p++,lefbor[i] = 0;continue;}//抵达全白边界
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
			if(turn_flag == 1) turn_flag = 3;
			else turn_flag = 2;
			for(k = MT9V03X_H - 1; k > i; k--) rigbor[k] = 159;
			break;
	}
//	限制边界
	if(j == 19) j--, p--;
//	向上检测
	for(; i > 0; i--){
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
			if(j == 19){rcut = i;traf_flag = 1;j--, p--,rigbor[i] = 159;continue;}
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
}
/*------------------------------*/
/*		  基准点辅助模块		*/
/*==============================*/
static void fop_search_sup(char num, unsigned char* p, char j){
//	类型选择
	switch(num){
	//	左基准点判断
		case 1:
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
			return;
	//	右基准点判断
		case 2:
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
			return;
	//	起始点判断
		case 0:
		//	一般情况
			if(*p != 0xFF)
				if(*p != 0x00){//左起始点突变，大概率左转
					fop_flag = 11, found_point[2] = MT9V03X_H - 1, found_point[3] = 9;
					return;
				}
			if(*(p+1) != 0xFF)
				if(*(p+1) != 0x00){//右起始点突变，大概率右转
					fop_flag = 12, found_point[0] = MT9V03X_H - 1, found_point[1] = 10;
					return;
				}
		//	特殊情况
			if(*(uint16*)p == 0x00FF){//左白右黑的前后跳变，大概率左转
				fop_flag = 13, found_point[2] = MT9V03X_H - 1, found_point[3] = 10;
				return;
			}
			if(*(uint16*)p == 0xFF00){//左黑右白的前后跳变，大概率右转
				fop_flag = 14, found_point[0] = MT9V03X_H - 1, found_point[1] = 9;
				return;
			}			
			break;
	}
}
/*------------------------------*/
/*		  基准点寻找模块		*/
/*==============================*/
static void fop_search(char num){
//	变量定义
	register unsigned char i = MT9V03X_H - 1;
	register char j;
	unsigned char bina = 0, bina_temp;
	unsigned char* p;
//	起始点判断
	p = &binary_img[i][9];
	if(*(uint16*)p == 0x0000){//全黑
	//	寻找跳变
		p = &binary_img[i][8];
		for(j = 8; j > 0; j--){//向左寻找，可能为左转
			if(*(uint16*)p == 0x0000){p--;continue;}
			fop_search_sup(2, p, j);
			return;
		}
		p = &binary_img[i][10];
		for(j = 10; j < 18; j++){//向右寻找，可能为右转
			if(*(uint16*)p == 0x0000){p++;continue;}
			fop_search_sup(1, p, j);
			return;
		}
	//  未找到跳变，返回错误
		fop_flag = 0;
		return;
	}
	if(!bina) fop_search_sup(0, p, j);
//	基点寻找
	switch(num){
		case 1:
		//	左基点寻找 
			p = &binary_img[i][8];
			for(j = 8; j > -1; j--){
				if(*(uint16*)p == 0xFFFF){p--;continue;}
				fop_search_sup(1, p, j);
				return;
			}
		//	未找到基点，向上寻找
			j = 0;
			for(i = MT9V03X_H - 1; i > 0; i--){
				if(!(binary_img[i][j]^binary_img[i-1][j])) continue;
				fop_flag = 3, found_point[0] = i, found_point[1] = j;
				return;
			}
			break;
		case 2:
		//	右基点寻找 
			p = &binary_img[i][10];
			for(j = 10; j < 19; j++){
				if(*(uint16*)p == 0xFFFF){p++;continue;}
				fop_search_sup(2, p, j);
				return;
			}
		//	未找到基点，向上寻找
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
//	二值化
	img_binary();
//	寻找基点、找边线（注意寻找顺序，左右转判断基于先左后右的边线寻找
	fop_search(1);
	lbor_search();
	fop_search(2);
	rbor_search();
//	判断是否为左右转（注意寻找顺序
	if(turn_flag){
		if(abs(found_point[0]-rcut)<4) turn_flag = 11;
		if(abs(found_point[2]-lcut)<4) turn_flag = 12;
	} 
//	垂直边界检测
	if(ltraf_count) border_vertical_search(1);
	if(rtraf_count) border_vertical_search(2);
//	状态机
	state_machine();
//	图像显示
	if(csimenu_flag[0]) binary_disp();
	if(csimenu_flag[1]) ips200_displayimage032(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
}
