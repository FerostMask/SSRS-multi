/*--------------------------------------------------------------*/
/*							ͷ�ļ�����							*/
/*==============================================================*/
#include "CAM.h"
#include "pid.h"
#include "data.h"
#include "zf_tim.h"
#include "SEEKFREE_MT9V03X.h"
#include "SEEKFREE_IPS200_PARALLEL8.h"
/*--------------------------------------------------------------*/
/* 							  �궨�� 							*/
/*==============================================================*/
#define uint16 unsigned short
/*--------------------------------------------------------------*/
/* 							 �������� 							*/
/*==============================================================*/
short show_value[5];
short traf_slope[4];
short ave_slope[4];
short line_slope_diff;
unsigned char show_point;
unsigned char ac_flag[4];
unsigned char exti_leftop, exti_rigtop;
unsigned char lefhigh, righigh;
unsigned char lefwidth, rigwidth;
unsigned cut_fork, cut_fork_bottom;
/*--------------------------------------------------------------*/
/* 							 �������� 							*/
/*==============================================================*/
/*------------------------------*/
/*		   ͼ����ʾģ�� 		*/
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
//	��ʾ�߽�
	for(i = MT9V03X_H - 1; i > lcut; i--) ips200_drawpoint(lefbor[i], i, 0x00);
	for(i = MT9V03X_H - 1; i > rcut; i--) ips200_drawpoint(rigbor[i], i, 0x00);
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
	for(i = 0; i < lefbottom_cut; i++)
		ips200_drawpoint(i, bottombor[i], 0xFD10);
	for(i = 0; i < leftop_cut; i++)
		ips200_drawpoint(i,topbor[i], 0xFD10);
	for(i = 159; i > rigbottom_cut; i--)
		ips200_drawpoint(i, bottombor[i], 0xFD10);
	for(i = 159; i > rigtop_cut; i--)
		ips200_drawpoint(i, topbor[i], 0xFD10);
//	��ʾ�������
	for(i = 17; i < cut_fork; i++)
		ips200_drawpoint(i, topbor[i], RED);
//    for(i = 0; i < MT9V03X_W; i++)
//        ips200_drawpoint(i, cut_fork_bottom-4 , BLUE);
    
//	��ʾ״̬		
	ips200_showint16(160, 7, state);
	ips200_showint16(160, 8, act_flag);
//	��ʾĿ���
	ips200_drawpoint(p_target[1]+2, p_target[0], 0xED2A);
	ips200_drawpoint(p_target[1]-2, p_target[0], 0xED2A);
	ips200_drawpoint(p_target[1], p_target[0], 0xED2A);
	ips200_drawpoint(p_target[1], p_target[0]+2, 0xED2A);
	ips200_drawpoint(p_target[1], p_target[0]-2, 0xED2A);
//	��ʾ���Ŀ���
	ips200_drawpoint(cut_fork_bottom+2, topbor[cut_fork_bottom], BLUE);
	ips200_drawpoint(cut_fork_bottom-2, topbor[cut_fork_bottom], BLUE);
	ips200_drawpoint(cut_fork_bottom, topbor[cut_fork_bottom], BLUE);
	ips200_drawpoint(cut_fork_bottom, topbor[cut_fork_bottom]+2, BLUE);
	ips200_drawpoint(cut_fork_bottom, topbor[cut_fork_bottom]-2, BLUE);  
    
	ips200_showint16(170, 2, show_value[0]);
	ips200_showint16(170, 3, show_value[1]);
	ips200_showint16(170, 4, show_value[2]);
	ips200_showint16(170, 5, show_value[3]);
	ips200_showint16(170, 6, show_value[4]);
	
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
/*	    	״̬��ģ��			*/
/*==============================*/
void state_machine(void){
//	��ʼ��
	state_temp = state, state = 0;
	cut_fork = 0;
//  ���
    vertsearch_frok();
    if(state == 31 )return;

    
//	�����������
	if(!exti_lefcount)
		if(!exti_rigcount){//û�г��� | ֱ�� | ������� | ������
			if(abs(found_point[0]-rcut)<5)
				if(abs(found_point[0] - found_point[2])>45) {state = 1;return;}
//				else {state = 31;return;}
				else {vertsearch_frok();if(topbor[cut_fork_bottom]>60) state = 31;return;}
			if(abs(found_point[2]-lcut)<5)
				if(abs(found_point[0] - found_point[2])>45) {state = 2;return;}
//				else {state = 31;return;}
				else {vertsearch_frok();if(topbor[cut_fork_bottom]>60) state = 31;return;}
		}
	if(exti_lefcount)
		if(!exti_rigcount){//ֻ������г��� | ���� | �� | ʮ�ֶ���
			show_value[0] = lefwidth;
		//	û��ֱ������
			if(abs(ltraf_point_row[exti_leftop] - rcut) < 5)
				{state = 3;return;}//����
		//	��ֱ������
			if(lefhigh > 4)//9
				if(lefhigh < 23){//�����ж� | 23
					if(found_point[2] > exti_lefp[0]+10)
						slope_cal(3);
						if(abs(line_slope_diff) < 120){//�ж��ұ���ֱ�� | 120
							if(traf_slope[1] > 75)//90
								if(traf_slope[0] > 30)//0
									{state = 12, lefwidth = lefhigh;return;}
						}
				}
	}
	if(exti_rigcount)
		if(!exti_lefcount){//ֻ���ұ��г��� | ���� | �һ� | ʮ�ֶ��� | ��������
			if(abs(rtraf_point_row[exti_rigtop] - lcut) < 5)//����
				{state = 4; return;}
		}
	if(exti_lefcount)
		if(exti_rigcount){//���߶��г��� | ʮ�� | ���
		
		}
}
/*------------------------------*/
/*	    	���״̬��			*/
/*==============================*/
void state_machine_fork(void){
	switch(act_flag){
		case 31:
			if(state!=31)
				act_flag = 0, state_flag = 0, img_color = 0xAE9C;
			break;
	}
}
/*------------------------------*/
/*	    	ʮ��״̬��			*/
/*==============================*/
void state_machine_cross(void){
}
/*------------------------------*/
/*	    	����״̬��			*/
/*==============================*/
void state_machine_ring(void){
	switch(act_flag){
		case 12:
			if(state == 1)
				act_flag = 13, img_color = 0x46D0;
				return;
		case 13:
			if(state == 4 || exti_rigcount == 1)
				act_flag = 14, img_color = 0xB6DB;
			return;
		case 14:
			if(state == 1)
				if(exti_rigcount == 0)
					act_flag = 15, img_color = 0xBDB8;
		case 15:
			if(state == 0)
				act_flag = 0, state_flag = 0, img_color = 0xAE9C;
	}
}
/*-
/*------------------------------*/
/*	    	���״̬��			*/
/*==============================*/
void state_machine_bend(void){
	switch(act_flag){
		case 1:
			if(state!=1)
				act_flag = 0, state_flag = 0, img_color = 0xAE9C;
			return;
		case 2:
			if(state!=2)
				act_flag = 0, state_flag = 0, img_color = 0xAE9C;
			return;
		case 3:
			if(state!=3)
				act_flag = 0, state_flag = 0, img_color = 0xAE9C;
			return;
		case 4:
			if(state!=4)
				act_flag = 0, state_flag = 0, img_color = 0xAE9C;
			return;
	}
}
/*------------------------------*/
/*	    	״̬�����			*/
/*==============================*/
void state_machine_enter(void){
	switch(state){
	//	������������
		case 1:
			act_flag = 1, state_flag = 1, img_color = 0x6DDD;
			return;
		case 2:
			act_flag = 2, state_flag = 1, img_color = 0x6DDD;
			return;
		case 3:
			act_flag = 3, state_flag = 1, img_color = 0x7EFE;
			return;
		case 4:
			act_flag = 4, state_flag = 1, img_color = 0x7EFE;
			return;
//	//	Բ��
//		case 12:
//			act_flag = 12, state_flag = 2, img_color = 0x8CF6;
//			return;
	//	ʮ�֡�ʮ�ֶ���
	//	���
		case 31:
			act_flag = 31, state_flag = 4, img_color = 0xEFBE;
			return;
	}
}
/*------------------------------*/
/*	   	 ����б�ʷ���ģ��		*/
/*==============================*/
char slope_cal(char num){
//	��������
	register char i;
	unsigned char mp[4];
	short slope[2];
	short angle;
//	����б��
	switch(num){
//	//	��ת
//		case 1:
//			mp[0] = rcut+1;
//			mp[2] = (mp[0]+99) >> 1;
//			mp[1] = (mp[0]+mp[2]) >> 1, mp[3] = mp[2]>>1;
//			slope[0] = (float)(rigbor[mp[0]]-rigbor[mp[2]])/(float)(mp[0]-mp[2])*1000;
//			slope[1] = (float)(rigbor[mp[1]]-rigbor[mp[3]])/(float)(mp[1]-mp[3])*1000;
////			if(slope[0]+slope[1]>2300) return 1;
////			else return 0;
//			break;
//	//	��ת
//		case 2:
//			mp[0] = lcut+1;
//			mp[2] = (mp[0]+99) >> 1;
//			mp[1] = (mp[0]+mp[2]) >> 1, mp[3] = mp[2]>>1;
//			slope[0] = (float)(lefbor[mp[2]]-lefbor[mp[0]])/(float)(mp[2]-mp[0])*1000;
//			slope[1] = (float)(lefbor[mp[3]]-lefbor[mp[1]])/(float)(mp[3]-mp[1])*1000;
//			show_value[1] = (slope[0]-slope[1])>>1;
////			if(slope[0]+slope[1]<-2300) return 1;
////			else return 0;
//			break;
	//	Բ������
		case 3:
			mp[0] = ltraf_point_row[exti_leftop];
			mp[2] = mp[0]+found_point[2] >> 1;
			mp[1] = (mp[0]+mp[2]) >> 1, mp[3] = mp[2]>>1;
			slope[0] = (float)(rigbor[mp[0]]-rigbor[mp[2]])/(float)(mp[0]-mp[2])*1000;
			slope[1] = (float)(rigbor[mp[1]]-rigbor[mp[3]])/(float)(mp[1]-mp[3])*1000;
			line_slope_diff = slope[0]-slope[1];
//			show_value[0] = slope[0];
//			show_value[1] = slope[1];
			break;
	}
}
/*------------------------------*/
/*	   ��ֱ����б�ʷ���ģ��		*/
/*==============================*/
void vert_slope_cal(char num){//��̫�ٲ�Ҫִ���������
//	��������
	register char i;
	unsigned char mp[4];
	short slope[3];
	short angle[2];
//	����б��
	switch(num){
	//	����
		case 0:
			mp[0] = 0;
			mp[2] = leftop_cut>>1;
			mp[1] = mp[2]>>1, mp[3] = (mp[2]+leftop_cut)>>1;
		//	����б��
			for(i = 0; i < 3; i++)
				slope[i] = atan((float)(topbor[mp[i+1]]-topbor[mp[i]])/(float)(mp[i+1]-mp[i]))*573;
		//	����Ƕȱ仯
			for(i = 0; i < 2; i++)
				angle[i] = slope[i+1] - slope[i];
			traf_slope[0] = angle[0]+angle[1], ac_flag[0] = 1;
			ave_slope[0] = (angle[0]+angle[1])>>1;
//			show_value[0] = traf_slope[0];
//			show_value[0] = ave_slope[0];
			break;
	//	����
		case 1:
			mp[0] = 0;
			mp[2] = lefbottom_cut>>1;
			mp[1] = mp[2]>>1, mp[3] = (mp[2]+lefbottom_cut)>>1;
			for(i = 0; i < 3; i++)
				slope[i] = atan((float)(bottombor[mp[i+1]]-bottombor[mp[i]])/(float)(mp[i+1]-mp[i]))*573;
			for(i = 0; i < 2; i++)
				angle[i] = slope[i+1] - slope[i];
			traf_slope[1] = angle[0]+angle[1], ac_flag[1] = 1;
			ave_slope[1] = (angle[0]+angle[1])>>1;
//			show_value[1] = traf_slope[1];
//			show_value[1] = ave_slope[1];
			break;
	//	����
		case 2:
			mp[0] = 159;
			mp[2] = (rigtop_cut+mp[0])>>1;
			mp[1] = (mp[0]+mp[2])>>1, mp[3] = (mp[2]+rigtop_cut)>>1;
			for(i = 0; i < 3; i++)
				slope[i] = atan((float)(topbor[mp[i+1]]-topbor[mp[i]])/(float)(mp[i+1]-mp[i]))*573;
			for(i = 0; i < 2; i++)
				angle[i] = slope[i+1] - slope[i];
			traf_slope[2] = angle[0]+angle[1], ac_flag[2] = 1;
			ave_slope[2] = (angle[0]+angle[1])>>1;
//			show_value[2] = traf_slope[2];
//			show_value[2] = ave_slope[2];
			break;
	//	����
		case 3:
			mp[0] = 159;
			mp[2] = (rigbottom_cut+mp[0])>>1;
			mp[1] = (mp[0]+mp[2])>>1, mp[3] = (mp[2]+rigbottom_cut)>>1;
			for(i = 0; i < 3; i++)
				slope[i] = atan((float)(bottombor[mp[i+1]]-bottombor[mp[i]])/(float)(mp[i+1]-mp[i]))*573;
			for(i = 0; i < 2; i++)
				angle[i] = slope[i+1] - slope[i];
			traf_slope[3] = angle[0]+angle[1], ac_flag[3] = 1;
			ave_slope[3] = (angle[0]+angle[1])>>1;
//			show_value[3] = traf_slope[3];
//			show_value[3] = ave_slope[3];
			break;
	}
}
/*------------------------------*/
/*	  	����߽��Ѱ��ģ��		*/
/*==============================*/
void vertsearch_frok(void){
//	**��������**
	register unsigned char i;
	register char j, k;
	unsigned char col = (MT9V03X_W-4)>>3;//����
	unsigned char *p;
	unsigned char found_flag, view_temp;
	unsigned char bottom_point, bottom_cut;
    unsigned char cnt_level_change_points;
    unsigned char cnt_left=0, cnt_right=0;//��������б
    unsigned char flag[2];
//	**Ѱ�ұ߽����**
	cut_fork_bottom = 0, bottom_cut = 0;
	p = &binary_img[MT9V03X_H-1][9];
    
	for(i = MT9V03X_H-1; i > 0; i--, p-=col){
		if(*(unsigned int *)p == 0xffffffff) continue;
		bottom_point = i;
		break;
	}
	if(i < 25 || i > 90) return;
//  **������������������**
    cnt_level_change_points = 0;
    p = &binary_img[i-4][2];
    for(j = 2; j < 18; j++,p++){//����
    //	һ�����
        if(*(uint16*)p == 0x0000) continue;//ȫ��
        if(*(uint16*)p == 0xffff) continue;//ȫ��
 		if(*p != 0x00)
			if(*p != 0xff){//��ͻ��
				for(k = 7; k > 0; k--){
                    if( ( ((*p>>k)&0x01)==1 ) && ( ((*p>>(k-1))&0x01)==0 ) ){flag[0]++;continue;}//�׺�����
                    if((((*p>>k)&0x01)==0)&&(((*p>>(k-1))&0x01)==1)){flag[1]++;continue;}//�ڰ�����
                }
					continue;
			}
		if(*(p+1) != 0x00)
			if(*(p+1) != 0xff){//��ͻ��
				for(k = 7; k > 0; k--){
                    if((((*p>>k)&0x01)==1)&&(((*p>>(k-1))&0x01)==0)){flag[0]++;continue;}//�׺�����
                    if((((*p>>k)&0x01)==0)&&(((*p>>(k-1))&0x01)==1)){flag[1]++;continue;}//�ڰ�����
                }
					continue;
			}
    //	�������
        if(*(uint16*)p == 0x00FF){flag[0]++;continue;}//����Һ�����
		else if(*(uint16*)p == 0xFF00){flag[1]++;continue;}//����Ұ�����
    }
    cnt_level_change_points = flag[0]+flag[1];
    show_value[2] = flag[0];    
    show_value[3] = flag[1];  
    show_value[4] = cnt_level_change_points;  
//  **Ѱ�ұ߽�**

	for(j = 3; j < 17; j++){
		found_flag = 0, p = &binary_img[bottom_point][j];
		for(i = bottom_point; i > 0; i--, p-=col ){
			view_temp = *(p)^*(p+col);
			for(k = 7; k > -1; k--){
				if(!((found_flag>>k)&0x01))
					if((view_temp>>k)&0x01){
                        if( k ){
                            if(j>9)//�ұ�
                            
                                 if(abs(i - topbor[(j<<3)+7-k-1]) > 3 || (i > topbor[(j<<3)+7-k-1]) ){//����� | ������ҵ������
                                    continue;
                                 }
                                 else cnt_right++;
                            else 
                                if(abs(i - topbor[(j<<3)+7-k-1]) > 3 || (i < topbor[(j<<3)+7-k-1]) ){//����� | ������ҵ������
                                    continue;
                                 }
                                 else cnt_left++;
                        }
						topbor[(j<<3)+7-k] = i;
                        if(i > bottom_cut) bottom_cut = i, cut_fork_bottom = (j<<3)+7-k;
						if(cut_fork < (j<<3)+7-k) cut_fork =(j<<3)+7-k;
					}
			}
			found_flag |= view_temp;
			if(found_flag == 0xFF) break;
			if(*(p+col) == 0x00) break;
		}
		if(found_flag != 0xFF) break;	
	}
/********** �յ㡢����жϿ�ʼ **********/    
    if(abs(cnt_left-cnt_right) < 10 && cnt_left > 15 && cnt_right > 15){
            state = 31;
    }
}
/*------------------------------*/
/*	  ��ֱ�߽��Ѱ��ģ�飨��	*/
/*==============================*/
void vert_leftsearch(unsigned char top, unsigned char bottom){
//	��������
	register unsigned char i;
	register char j, k;
	unsigned char col = (MT9V03X_W-4)>>3;//����
	unsigned char *p;
	unsigned char found_flag, view_temp;
//	Ѱ���±߽�
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
//	Ѱ���ϱ߽�
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
/*	  ��ֱ�߽��Ѱ��ģ�飨�ң�	*/
/*==============================*/
void vert_rightsearch(unsigned char top, unsigned char bottom){
//	��������
	register unsigned char i;
	register char j, k;
	unsigned char col = (MT9V03X_W-4)>>3;//����
	unsigned char *p;
	unsigned char found_flag, view_temp;
//	Ѱ���±߽�
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
//	Ѱ���ϱ߽�
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
/*	  �߽������Ѱ��ģ�飨��	*/
/*==============================*/
void border_vertical_leftsearch(void){
//	��������
	register unsigned char i, k;
	unsigned char vetflag = 0;
	unsigned char vet_colmax, vet_rowmax;
//	��ֱ�߽�Ѱ��
	if(ltraf_count > 1)
		for(i = 1; i < ltraf_count; i++){
		//	����͹
			if(ltraf_flag[i] == 0)
				if(ltraf_flag[i-1] == 1){
					for(k = ltraf_point_row[i], vet_colmax = 0; k < ltraf_point_row[i-1]; k++) 
						if(lefbor[k] > vet_colmax) vet_colmax = lefbor[k], vet_rowmax = k; 
					lvet_trafpoint_row[lvet_trafcount] = vet_rowmax, lvet_trafpoint_col[lvet_trafcount] = vet_colmax, lvet_trafcount++;
				}
		//	����
			if(ltraf_flag[i] == 1)
				if(ltraf_flag[i-1] == 0){ 
					exti_lefp[exti_lefcount] = (ltraf_point_row[i]+ltraf_point_row[i-1])>>1, exti_lefcount++;
					if(!vetflag){
						if(i+1 < ltraf_count) {vetflag = 1, exti_leftop = i, lefhigh = ltraf_point_row[i-1] - ltraf_point_row[i];vert_leftsearch(ltraf_point_row[i+1], ltraf_point_row[i-1]+10);}
						else {vetflag = 1, exti_leftop = i, lefhigh = ltraf_point_row[i-1] - ltraf_point_row[i];vert_leftsearch(ltraf_point_row[i]-7, ltraf_point_row[i-1]+10);}
					}
				}
		}
}
/*------------------------------*/
/*	  �߽������Ѱ��ģ�飨�ң�	*/
/*==============================*/
void border_vertical_rightsearch(void){
//	��������
	register unsigned char i, k;
	unsigned char vetflag = 0;
	unsigned char vet_colmax, vet_rowmax;
//	Ѱ������
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
			if(rtraf_flag[i] == 1){
				if(rtraf_flag[i-1] == 0) exti_rigp[exti_rigcount] = (rtraf_point_row[i]+rtraf_point_row[i-1])>>1, exti_rigcount++;
				if(!vetflag){
					if(i+1 < rtraf_count){vetflag = 1, exti_rigtop = i, righigh = rtraf_point_row[i-1] - rtraf_point_row[i];vert_rightsearch(rtraf_point_row[i+1], rtraf_point_row[i-1]+10);}
					else {vetflag = 1, exti_rigtop = i, righigh = rtraf_point_row[i-1] - rtraf_point_row[i];vert_rightsearch(rtraf_point_row[i]-7, rtraf_point_row[i-1]+10);}
				}
			}
		}
}
/*------------------------------*/
/*		 ��߽��Ѱ��ģ��		*/
/*==============================*/
void lbor_search(void){
//	��������
	short traf_slope, traf_slope_temp;
	register unsigned char i = found_point[0], k;
	register char j = found_point[1];
	unsigned char col = (MT9V03X_W-4)>>3;//����
	unsigned char *p;
	unsigned char traf_flag, traf_flag_temp;
	char sloptraf, lcmax = 0, lcmin = 0, local_flag1, local_flag1_temp, local_flag2, local_flag2_temp;
//	��ʼ��
	lcut = 0, ltraf_count = 0, lefbottom_cut = 0, leftop_cut = 0;
	lvet_trafcount = 0, exti_lefcount = 0, leftop_cut = 0, lefbottom_cut = 0;
	for(k = 0; k < 4; k++) ac_flag[k] = 0;
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
	for(i--; i > 0; i--){
	//	��ʼ�����С�������
		p -= col;
		if(traf_flag!=traf_flag_temp) 
			if(i < 90)//���������µ�ת���
				if(ltraf_point_row[ltraf_count-1]-i>3 || ltraf_count == 0){//����ת���߶�
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
			if(j == 19){//�ִ�߽磬ֹͣ����
				lcut = i;
				if(traf_flag_temp)
					if(ltraf_point_row[ltraf_count-1]-i>5 || ltraf_count == 0)
						ltraf_flag[ltraf_count] = 1, ltraf_point_row[ltraf_count] = i+1, ltraf_point_col[ltraf_count] = lefbor[i+1], ltraf_count++;
				return;
			}
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
			if(j == -1){lcut = i;traf_flag = 1,j+=6,p+=6,lefbor[i] = 0;continue;}//�ִ�ȫ�ױ߽�
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
	lcut = i;//�˳�ǰ����������
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
	rcut = 0, rtraf_count = 0, rigbottom_cut = 159, rigtop_cut = 159;
	rvet_trafcount = 0, exti_rigcount = 0, rigtop_cut = 159, rigbottom_cut = 159;
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
	for(i--; i > 0; i--){
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
			if(j == -1){//�ִ�߽磬ֹͣ����
				rcut = i;
				if(traf_flag_temp)//ת�����
					if(rtraf_point_row[rtraf_count-1]-i>5 || rtraf_count == 0)//����ת���߶�
						rtraf_point_row[rtraf_count] = i+1, rtraf_point_col[rtraf_count] = rigbor[i+1], rtraf_count++;
				return;
			}
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
			if(j == 19){rcut = i;traf_flag = 1;j-=6, p-=6,rigbor[i] = 159;continue;}
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
	rcut = i;//�˳�ǰ����������
}
/*------------------------------*/
/*		��׼�㸨��ģ�飨��	*/
/*==============================*/
static void left_fop_search_sup(unsigned char* p, char j){
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
}
/*------------------------------*/
/*		��׼�㸨��ģ�飨�ң�	*/
/*==============================*/
static void right_fop_search_sup(unsigned char* p, char j){
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
}
/*------------------------------*/
/*		 ��׼��Ѱ��ģ�飨��	*/
/*==============================*/
void left_fop_search(void){
//	��������
	register unsigned char i = MT9V03X_H - 1;
	register char j;
	unsigned char* p;
//	�����Ѱ�� 
	p = &binary_img[i][8];
	for(j = 8; j > -1; j--){
		if(*(uint16*)p == 0xFFFF){p--;continue;}
		left_fop_search_sup(p, j);
		return;
	}
//	δ�ҵ����㣬����Ѱ��
	for(i = MT9V03X_H - 1; i > 0; i--){
		if(!(binary_img[i][0]^binary_img[i-1][0])) continue;
		fop_flag = 3, found_point[0] = i, found_point[1] = 0;
		return;
	}
}
/*------------------------------*/
/*		 ��׼��Ѱ��ģ�飨�ң�	*/
/*==============================*/
void right_fop_search(void){
//	��������
	register unsigned char i = MT9V03X_H - 1;
	register char j;
	unsigned char* p;
//	�һ���Ѱ�� 
	p = &binary_img[i][10];
	for(j = 10; j < 19; j++){
		if(*(uint16*)p == 0xFFFF){p++;continue;}
		right_fop_search_sup(p, j);
		return;
	}
//	δ�ҵ����㣬����Ѱ��
	for(i = MT9V03X_H - 1; i > 0; i--){
		if(!(binary_img[i][19]^binary_img[i-1][19])) continue;
		fop_flag = 3, found_point[2] = i, found_point[3] = 19;
		return;
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
}
