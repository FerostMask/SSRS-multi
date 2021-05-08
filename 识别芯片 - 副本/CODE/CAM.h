/*------------------------------------------------------*/
/* 					   头文件声明 						*/
/*======================================================*/
#ifndef _CAM_H
#define _CAM_H
/*------------------------------------------------------*/
/*						 宏定义							*/
/*======================================================*/
#define grayscale 256
/*------------------------------------------------------*/ 
/* 						函数声明 						*/
/*======================================================*/
void border_vertical_leftsearch(void);
void border_vertical_rightsearch(void);
void binary_disp(void);
void lbor_search(void);
void rbor_search(void);
void left_fop_search(void);
void right_fop_search(void);
void img_binary(void);
void otsu(void);
#endif
