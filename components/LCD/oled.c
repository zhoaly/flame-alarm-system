#include "LCD_init.h"
#include <stdio.h>
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "esp_log.h"


#include "stdlib.h"
#include "oledfont.h"
#include "oled.h"

uint8_t OLED_GRAM[8][128];//显存

//反显函数
void OLED_ColorTurn(esp_lcd_panel_handle_t panel)
{
	static bool invert_color_flag;
	invert_color_flag=!invert_color_flag;
	esp_lcd_panel_invert_color(panel,invert_color_flag);
}

//屏幕旋转180度
void OLED_DisplayTurn(esp_lcd_panel_handle_t panel)
{
	static bool mirror_flag;
	mirror_flag=!mirror_flag;
	esp_lcd_panel_mirror(panel, 1, mirror_flag);
}


//开启OLED显示 
void OLED_DisPlay_On(esp_lcd_panel_handle_t panel)
{	
	esp_lcd_panel_disp_on_off(panel,true);	
}

//关闭OLED显示 
void OLED_DisPlay_Off(esp_lcd_panel_handle_t panel)
{	
	esp_lcd_panel_disp_on_off(panel,false);
}

void OLED_Refresh(esp_lcd_panel_handle_t panel)//刷新显存到屏幕
{
	for (int i = 0; i < 8; i++)
	{
		esp_lcd_panel_draw_bitmap(panel, 0, i*8, 128, i*8+8, OLED_GRAM[i]);
	}
}

void OLED_Clear_GRAM(){//清空显存
    for (int n = 0; n < 8; n++)
    {
        for (int l = 0; l < 128; l++)
        {
            OLED_GRAM[n][l]=0x00;
        }
    }

}
//清屏函数//清屏不会清空显存
void OLED_Clear(esp_lcd_panel_handle_t panel)
{
	const uint8_t blank[] ={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	for (int i = 0; i < 16; i++) {
	for(int j = 0;j < 8;j++){
            esp_lcd_panel_draw_bitmap(panel, i * 8, j * 8, i * 8 + 8, j * 8 + 8, blank);
        }
    }
}


//画点 
//x:0~127
//y:0~63
void OLED_DrawPoint_To_GRAM(u8 x,u8 y)
{
	OLED_GRAM[y/8][x]|=0x01<<y%8;
}

//清点
//x:0~127
//y:0~63
void OLED_CleanPoint_To_GRAM(u8 x,u8 y)
{
	OLED_GRAM[y/8][x]&=0x00<<y%8;
}


//画线
//x1,y1:起点坐标
//x2,y2:结束坐标
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;}


	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y;

	for(t=0;t<distance+1;t++)
	{
		OLED_DrawPoint_To_GRAM(uRow,uCol);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}


//x,y:圆心坐标
//r:圆的半径
void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint_To_GRAM(x + a, y - b);
        OLED_DrawPoint_To_GRAM(x - a, y - b);
        OLED_DrawPoint_To_GRAM(x - a, y + b);
        OLED_DrawPoint_To_GRAM(x + a, y + b);
 
        OLED_DrawPoint_To_GRAM(x + b, y + a);
        OLED_DrawPoint_To_GRAM(x + b, y - a);
        OLED_DrawPoint_To_GRAM(x - b, y - a);
        OLED_DrawPoint_To_GRAM(x - b, y + a);
        
        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}



//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size1:选择字体 6x8/6x12/8x16/12x24
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1)
{
	u8 i,m,temp,size2,chr1;
	u8 x0=x,y0=y;
	if(size1==8)size2=6;
	else size2=(size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
	chr1=chr-' ';  //计算偏移后的值
	for(i=0;i<size2;i++)
	{
		if(size1==8)
		{temp=asc2_0806[chr1][i];} //调用0806字体
		else if(size1==12)
        {temp=asc2_1206[chr1][i];} //调用1206字体
		else if(size1==16)
        {temp=asc2_1608[chr1][i];} //调用1608字体
		else if(size1==24)
        {temp=asc2_2412[chr1][i];} //调用2412字体
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint_To_GRAM(x,y);
			//else OLED_CleanPoint_To_GRAM(x,y);
			temp>>=1;
			y++;
		}
		x++;
		if((size1!=8)&&((x-x0)==size1/2))
		{x=x0;y0=y0+8;}
		y=y0;
  }
}


//显示字符串
//x,y:起点坐标  
//size1:字体大小 
//*chr:字符串起始地址 
//mode:0,反色显示;1,正常显示
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1)
{
	while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
	{
		OLED_ShowChar(x,y,*chr,size1);
		if(size1==8)x+=6;
		else x+=size1/2;
		chr++;
  }
}


uint32_t Pow(uint8_t m,uint8_t n){
	uint32_t result=1;
	while(n--)result*=m;
	return result;
}

int Abs(int m) {
	int result=0;
	result=(m<0) ? (-1 * m) : m;
	return result;
}

//显示数字
//支持浮点数
void OLED_ShowNum(uint8_t x,uint8_t y,float num,uint8_t float_len,uint16_t font_size)
{
	uint16_t size_row,size_column,num_len=0;
	int num_int=Abs((int)num),temp,n=num_int;
	int num_float=Abs((num-(int)num)*Pow(10,float_len));

	switch(font_size){
		case 12 :size_row=6;size_column=12;break;//解析行列长度
		case 16 :size_row=8;size_column=16;break;
		case 24 :size_row=12;size_column=24;break;
		default: return;
	}
	//for(int n=num_int;n>=1;n/=10)num_len++;//解析整形部分长度

	if(num<0){
		OLED_ShowChar(x,y,'-',font_size);
		x+=size_row;
	}

	do{num_len++;n/=10;}while(n>=1);

	for(int i=num_len-1;i>=0;i--)
	{//打印整数部分
		temp=num_int/(Pow(10,i));
		
		num_int-=temp*(Pow(10,i));
		OLED_ShowChar(x,y,temp+'0',font_size);
		x+=size_row;
		if(x>128-size_row){x=0;y+=size_column;}//换行
	}
	if(float_len){
		OLED_ShowChar(x,y,'.',font_size);
		x+=size_row;
		if(x>128-size_row){x=0;y+=size_column;}//换行
		for(int i=float_len-1;i>=0;i--){//打印小数部分
		temp=num_float/(Pow(10,i));

		num_float-=temp*(Pow(10,i));
		OLED_ShowChar(x,y,temp+'0',font_size);
		x+=size_row;
		if(x>128-size_row){x=0;y+=size_column;}//换行
		}
		}
}



