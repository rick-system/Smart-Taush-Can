/************************************************************************************
*
* Function List:
* 1.  void I2C_Configuration(void) -- 配置CPU的软件I2C
* 2.  void I2C_WriteByte(uint8_t addr,uint8_t data) -- 向寄存器地址写一个byte的数据
* 3.  void WriteCmd(unsigned char I2C_Command) -- 写命令
* 4.  void WriteDat(unsigned char I2C_Data) -- 写数据
* 5.  void OLED_Init(void) -- OLED屏初始化
* 6.  void OLED_SetPos(unsigned char x, unsigned char y) -- 设置起始点坐标
* 7.  void OLED_Fill(unsigned char fill_Data) -- 全屏填充
* 8.  void OLED_CLS(void) -- 清屏
* 9.  void OLED_ON(void) -- 唤醒
* 10. void OLED_OFF(void) -- 睡眠
* 11. void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize) -- 显示字符串(字体大小有6*8和8*16两种)
* 12. void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N) -- 显示中文(中文需要先取模，然后放到codetab.h中)
* 13. void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]) -- BMP图片
*
*************************************************************************************/

#include "oled.h" 
#include "delay.h"
#include "codetab.h"
#include "common.h"

//IIC接口初始化GPIO
//--------------------------------------------------------
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	IIC_SCL=1;
	IIC_SDA=1;

}

//------------------------------------------------------
//启动IIC总线
//------------------------------------------------------
void IIC_Start(void)
{
  SDA_OUT();
	SDA_H;
	SCL_H;
	delay_us(2);
	SDA_L;
	delay_us(2);
	SCL_L;
	delay_us(2);
}

//------------------------------------------------------
//释放IIC总线
//------------------------------------------------------
void IIC_Stop(void)
{
  SDA_OUT();
	SCL_L;
	SDA_L;
	delay_us(2);
	SCL_H;
	SDA_H;
	delay_us(2);
}
//------------------------------------------------------
//读应答
//------------------------------------------------------
unsigned char Read_Ask(void)
{
	unsigned char ReAsk;
	SDA_IN();
	SCL_H;
	delay_us(2);
	ReAsk=(unsigned char)SDA_READ;
	SCL_L;
	delay_us(2);
	return ReAsk;
}
//-----------------------------------------------------
//写入一个字节
//-----------------------------------------------------
void IIC_Send_Byte(unsigned char Dat)
{
     unsigned char i;
			SDA_OUT();
			for(i=0;i<8;i++)
			{
				 if(Dat&0x80)
				 {
					SDA_H;
				 }
					else
					{
					 SDA_L;
					}
				Dat<<=1;
				SCL_H;
				delay_us(2);
				SCL_L;
				delay_us(2);
			}
			Read_Ask();
}
//-------------------------------------------------------------

//------------------------------------------------------
//写命令
//------------------------------------------------------
void WriteCmd(unsigned char I2C_Command)//写命令
{
	IIC_Start();
	IIC_Send_Byte(0x78);   //发送地址	 										  		   
	IIC_Send_Byte(0x00);     //发送字节							     		    	   
	IIC_Send_Byte(I2C_Command);
	IIC_Stop();
}
//------------------------------------------------------
//写数据
//------------------------------------------------------
void WriteDat(unsigned char I2C_Data)//写数据
{	
	IIC_Start();
	IIC_Send_Byte(0x78);   //发送地址	 										  		   
	IIC_Send_Byte(0x40);     //发送字节							   		    	   
	IIC_Send_Byte(I2C_Data);
	IIC_Stop();
}

//OLED显示模块初始化
void OLED_Init(void)
{
		IIC_Init();
		delay_ms(100);
	
    WriteCmd(0xae);	//关显示
 
    WriteCmd(0xd5);	//设置显示时钟分频率、振荡器频率
    WriteCmd(0x80);//A[3:0]:分频率1到16,A[7:4]频率
 
    WriteCmd(0xa8);	//duty设置
    WriteCmd(0x3f);	//duty=1/64
 
    WriteCmd(0xd3);	//显示偏移	
    WriteCmd(0x00);//不偏移
 
    WriteCmd(0x40);	//起始行 40~7F
 
    WriteCmd(0x8d);	//升压允许
    WriteCmd(0x14);
 
    WriteCmd(0x20);	//设置内存地址模式
    WriteCmd(0x02);//00水平地址模式,01垂直模式,02页地址模式,
 
    WriteCmd(0xc8); //行扫描顺序:从上到下c8	//上下颠倒c0
    WriteCmd(0xa1); //列扫描顺序:从左到右a1	//左右翻转a0
 
    WriteCmd(0xda); //sequential configuration
    WriteCmd(0x12);
 
    WriteCmd(0x81); //微调对比度,本指令的0x81不要改动,改下面的值
    WriteCmd(0xcf); //微调对比度的值,可设置范围0x00~0xff
 
    WriteCmd(0xd9); //Set Pre-Charge Period
    WriteCmd(0xf1);
 
    WriteCmd(0xdb); //Set VCOMH Deselect Level
    WriteCmd(0x40);
 
    WriteCmd(0xaf); //开显示
		
		delay_ms(500);
		OLED_Fill(0x00);//初始全屏灭
		
		/////*******************************滚动**********************************************/	
//	WriteCmd(0x2e);//关滚动
//  WriteCmd(0x2A);//29向右,2a向左
//  WriteCmd(0x00);//A:空字节
//  WriteCmd(0x00);//B:水平起始页
//  WriteCmd(0x00);//C:水品滚动速度
//  WriteCmd(0x01);//D:水平结束页
//  WriteCmd(0x00);//E:每次垂直滚动位移
////	WriteCmd(0x2f);//开滚动
//	
		
}


//void OLED_Init(void)
//{	
//	
//	IIC_Init();
//	
//	delay_ms(100); //这里的延时很重要
//	WriteCmd(0xAE); //display off
//	WriteCmd(0x20);	//Set Memory Addressing Mode	
//	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
//	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
//	WriteCmd(0xc8);	//Set COM Output Scan Direction
//	WriteCmd(0x00); //---set low column address
//	WriteCmd(0x10); //---set high column address
//	WriteCmd(0x40); //--set start line address
//	WriteCmd(0x81); //--set contrast control register
//	WriteCmd(0xff); //亮度调节 0x00~0xff
//	WriteCmd(0xa1); //--set segment re-map 0 to 127
//	WriteCmd(0xa6); //--set normal display
//	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
//	WriteCmd(0x3F); //
//	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
//	WriteCmd(0xd3); //-set display offset
//	WriteCmd(0x00); //-not offset
//	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
//	WriteCmd(0xf0); //--set divide ratio
//	WriteCmd(0xd9); //--set pre-charge period
//	WriteCmd(0x22); //
//	WriteCmd(0xda); //--set com pins hardware configuration
//	WriteCmd(0x12);
//	WriteCmd(0xdb); //--set vcomh
//	WriteCmd(0x20); //0x20,0.77xVcc
//	WriteCmd(0x8d); //--set DC-DC enable
//	WriteCmd(0x14); //
//	
//	WriteCmd(0xaf); //--turn on oled panel
//	
/////*******************************滚动**********************************************/	
//	WriteCmd(0x2e);//关滚动
//  WriteCmd(0x2A);//29向右,2a向左
//  WriteCmd(0x00);//A:空字节
//  WriteCmd(0x00);//B:水平起始页
//  WriteCmd(0x00);//C:水品滚动速度
//  WriteCmd(0x01);//D:水平结束页
//  WriteCmd(0x00);//E:每次垂直滚动位移
//	WriteCmd(0x2f);//开滚动
//	
////	
////	delay_ms(100); //这里的延时很重要
////	
////	WriteCmd(0xAE); //display off
////	WriteCmd(0x20);	//Set Memory Addressing Mode	
////	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
////	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
////	WriteCmd(0xc8);	//Set COM Output Scan Direction
////	WriteCmd(0x00); //---set low column address
////	WriteCmd(0x10); //---set high column address
////	WriteCmd(0x40); //--set start line address
////	WriteCmd(0x81); //--set contrast control register
////	WriteCmd(0xff); //亮度调节 0x00~0xff
////	WriteCmd(0xa1); //--set segment re-map 0 to 127
////	WriteCmd(0xa6); //--set normal display
////	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
////	WriteCmd(0x3F); //
////	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
////	WriteCmd(0xd3); //-set display offset
////	WriteCmd(0x00); //-not offset
////	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
////	WriteCmd(0xf0); //--set divide ratio
////	WriteCmd(0xd9); //--set pre-charge period
////	WriteCmd(0x22); //
////	WriteCmd(0xda); //--set com pins hardware configuration
////	WriteCmd(0x12);
////	WriteCmd(0xdb); //--set vcomh
////	WriteCmd(0x20); //0x20,0.77xVcc
////	WriteCmd(0x8d); //--set DC-DC enable
////	WriteCmd(0x14); //
////	WriteCmd(0xaf); //--turn on oled panel
////	
//////	delay_ms(500);
//////	OLED_Fill(0x00);//初始全屏灭
//}

void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{ 
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}

void OLED_Fill(unsigned char fill_Data)//全屏填充
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				WriteDat(fill_Data);
			}
	}
}

void OLED_CLS(void)//清屏
{
	OLED_Fill(0x00);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : 将OLED从休眠中唤醒
//--------------------------------------------------------------
void OLED_ON(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X14);  //开启电荷泵
	WriteCmd(0XAF);  //OLED唤醒
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X10);  //关闭电荷泵
	WriteCmd(0XAE);  //OLED休眠
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
//--------------------------------------------------------------
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					WriteDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
// Description    : 显示codetab.h中的汉字,16*16点阵
//--------------------------------------------------------------
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
// Calls          : 
// Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
// Description    : 显示BMP位图
//--------------------------------------------------------------
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			WriteDat(BMP[j++]);
		}
	}
}




/************************************** 自己写的显示函数 ****************************************/
void OLED_ShowMYCN(unsigned char x, unsigned char y, unsigned char arr[],unsigned char N)
{
	
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(arr[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(arr[adder]);
		adder += 1;
	}
}

void ShowNormal(void)
{
	uint8_t i;
	
	for(i=0;i<4;i++)
	{
		OLED_ShowMYCN(30+i*16,11,Normal16x16,i);//测试显示中文
	}
}

void ShowDanger(void) 
{
	uint8_t i;
	
	for(i=0;i<3;i++)
	{
		OLED_ShowMYCN(39+i*16,6,Danger16x16,i);//测试显示中文
	}
}

void ShowCurStatusSomeBody(void)
{
	uint8_t i;
	
	for(i=0;i<8;i++)
	{
		OLED_ShowMYCN(0+i*16,0,CurStatusSomeBody16x16,i);//测试显示中文
	}
}


void ShowCurStatusNoBody(void)
{
	uint8_t i;
	
	for(i=0;i<8;i++)
	{
		OLED_ShowMYCN(0+i*16,0,CurStatusNoBody16x16,i);//测试显示中文
	}
}


void ShowTemperature(void)
{
	uint8_t i;
	
	for(i=0;i<3;i++)
	{
		OLED_ShowMYCN(0+i*16,4,temperature16x16,i);//测试显示中文
	}
}

void ShowSmoke(void)
{
	uint8_t i;
	
	for(i=0;i<2;i++)
	{
		OLED_ShowMYCN(0+i*16,4,smoke16x16,i);//测试显示中文
	}
}

/******************************************************************************************************************/

void ShowSmartTrash(void)
{
	uint8_t i;
	
	for(i=0;i<5;i++)
	{	
		OLED_ShowMYCN(30+i*16,11,smartTrash16x16,i);//测试显示中文
	}
	
}

void Show_Author(void)
{
	uint8_t i;
	
	for(i=0;i<6;i++)
	{
		OLED_ShowMYCN(23+i*16,6,Chinese_author16X16,i);//测试显示中文
	}
}

void Showdu(void)
{
	uint8_t i;
	
	for(i=0;i<1;i++)
	{
		OLED_ShowMYCN(75+i*16,2,Chinese_du16X16,i);//测试显示中文
	}
}

void ShowHump(void)
{
	uint8_t i;
	
	for(i=0;i<2;i++)
	{
		OLED_ShowMYCN(0+i*16,4,Chinese_hump16X16,i);//测试显示中文
	}
}


void Show_youhai(void)
{
	uint8_t i;
	
	for(i=0;i<4;i++)
	{
		OLED_ShowMYCN(30+i*16,11,youhailaji16x16,i);//测试显示中文
	}
}

void Show_qita(void)
{
	uint8_t i;
	
	for(i=0;i<4;i++)
	{
		OLED_ShowMYCN(30+i*16,11,qitalaji16X16,i);//测试显示中文
	}
}

void Show_kehuishou(void)
{
	uint8_t i;
	
	for(i=0;i<4;i++)
	{
		OLED_ShowMYCN(30+i*16,11,kehuishouwu16X16,i);//测试显示中文
	}
}

void Show_chiyu(void)
{
	uint8_t i;
	
	for(i=0;i<4;i++)
	{
		OLED_ShowMYCN(30+i*16,11,chiyulaji16X16,i);//测试显示中文
	}
}


void ShowTrashNoPeople(void)
{
	unsigned char m,n;
	for(m=6;m<8;m++)
	{
		WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);		//high column start address
		for(n=32;n<128;n++)
			{
				WriteDat(0x00);
			}
	}
}







