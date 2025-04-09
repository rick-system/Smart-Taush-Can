/************************************************************************************
*
* Function List:
* 1.  void I2C_Configuration(void) -- ����CPU�����I2C
* 2.  void I2C_WriteByte(uint8_t addr,uint8_t data) -- ��Ĵ�����ַдһ��byte������
* 3.  void WriteCmd(unsigned char I2C_Command) -- д����
* 4.  void WriteDat(unsigned char I2C_Data) -- д����
* 5.  void OLED_Init(void) -- OLED����ʼ��
* 6.  void OLED_SetPos(unsigned char x, unsigned char y) -- ������ʼ������
* 7.  void OLED_Fill(unsigned char fill_Data) -- ȫ�����
* 8.  void OLED_CLS(void) -- ����
* 9.  void OLED_ON(void) -- ����
* 10. void OLED_OFF(void) -- ˯��
* 11. void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize) -- ��ʾ�ַ���(�����С��6*8��8*16����)
* 12. void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N) -- ��ʾ����(������Ҫ��ȡģ��Ȼ��ŵ�codetab.h��)
* 13. void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]) -- BMPͼƬ
*
*************************************************************************************/

#include "oled.h" 
#include "delay.h"
#include "codetab.h"
#include "common.h"

//IIC�ӿڳ�ʼ��GPIO
//--------------------------------------------------------
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	IIC_SCL=1;
	IIC_SDA=1;

}

//------------------------------------------------------
//����IIC����
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
//�ͷ�IIC����
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
//��Ӧ��
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
//д��һ���ֽ�
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
//д����
//------------------------------------------------------
void WriteCmd(unsigned char I2C_Command)//д����
{
	IIC_Start();
	IIC_Send_Byte(0x78);   //���͵�ַ	 										  		   
	IIC_Send_Byte(0x00);     //�����ֽ�							     		    	   
	IIC_Send_Byte(I2C_Command);
	IIC_Stop();
}
//------------------------------------------------------
//д����
//------------------------------------------------------
void WriteDat(unsigned char I2C_Data)//д����
{	
	IIC_Start();
	IIC_Send_Byte(0x78);   //���͵�ַ	 										  		   
	IIC_Send_Byte(0x40);     //�����ֽ�							   		    	   
	IIC_Send_Byte(I2C_Data);
	IIC_Stop();
}

//OLED��ʾģ���ʼ��
void OLED_Init(void)
{
		IIC_Init();
		delay_ms(100);
	
    WriteCmd(0xae);	//����ʾ
 
    WriteCmd(0xd5);	//������ʾʱ�ӷ�Ƶ�ʡ�����Ƶ��
    WriteCmd(0x80);//A[3:0]:��Ƶ��1��16,A[7:4]Ƶ��
 
    WriteCmd(0xa8);	//duty����
    WriteCmd(0x3f);	//duty=1/64
 
    WriteCmd(0xd3);	//��ʾƫ��	
    WriteCmd(0x00);//��ƫ��
 
    WriteCmd(0x40);	//��ʼ�� 40~7F
 
    WriteCmd(0x8d);	//��ѹ����
    WriteCmd(0x14);
 
    WriteCmd(0x20);	//�����ڴ��ַģʽ
    WriteCmd(0x02);//00ˮƽ��ַģʽ,01��ֱģʽ,02ҳ��ַģʽ,
 
    WriteCmd(0xc8); //��ɨ��˳��:���ϵ���c8	//���µߵ�c0
    WriteCmd(0xa1); //��ɨ��˳��:������a1	//���ҷ�תa0
 
    WriteCmd(0xda); //sequential configuration
    WriteCmd(0x12);
 
    WriteCmd(0x81); //΢���Աȶ�,��ָ���0x81��Ҫ�Ķ�,�������ֵ
    WriteCmd(0xcf); //΢���Աȶȵ�ֵ,�����÷�Χ0x00~0xff
 
    WriteCmd(0xd9); //Set Pre-Charge Period
    WriteCmd(0xf1);
 
    WriteCmd(0xdb); //Set VCOMH Deselect Level
    WriteCmd(0x40);
 
    WriteCmd(0xaf); //����ʾ
		
		delay_ms(500);
		OLED_Fill(0x00);//��ʼȫ����
		
		/////*******************************����**********************************************/	
//	WriteCmd(0x2e);//�ع���
//  WriteCmd(0x2A);//29����,2a����
//  WriteCmd(0x00);//A:���ֽ�
//  WriteCmd(0x00);//B:ˮƽ��ʼҳ
//  WriteCmd(0x00);//C:ˮƷ�����ٶ�
//  WriteCmd(0x01);//D:ˮƽ����ҳ
//  WriteCmd(0x00);//E:ÿ�δ�ֱ����λ��
////	WriteCmd(0x2f);//������
//	
		
}


//void OLED_Init(void)
//{	
//	
//	IIC_Init();
//	
//	delay_ms(100); //�������ʱ����Ҫ
//	WriteCmd(0xAE); //display off
//	WriteCmd(0x20);	//Set Memory Addressing Mode	
//	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
//	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
//	WriteCmd(0xc8);	//Set COM Output Scan Direction
//	WriteCmd(0x00); //---set low column address
//	WriteCmd(0x10); //---set high column address
//	WriteCmd(0x40); //--set start line address
//	WriteCmd(0x81); //--set contrast control register
//	WriteCmd(0xff); //���ȵ��� 0x00~0xff
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
/////*******************************����**********************************************/	
//	WriteCmd(0x2e);//�ع���
//  WriteCmd(0x2A);//29����,2a����
//  WriteCmd(0x00);//A:���ֽ�
//  WriteCmd(0x00);//B:ˮƽ��ʼҳ
//  WriteCmd(0x00);//C:ˮƷ�����ٶ�
//  WriteCmd(0x01);//D:ˮƽ����ҳ
//  WriteCmd(0x00);//E:ÿ�δ�ֱ����λ��
//	WriteCmd(0x2f);//������
//	
////	
////	delay_ms(100); //�������ʱ����Ҫ
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
////	WriteCmd(0xff); //���ȵ��� 0x00~0xff
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
//////	OLED_Fill(0x00);//��ʼȫ����
//}

void OLED_SetPos(unsigned char x, unsigned char y) //������ʼ������
{ 
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}

void OLED_Fill(unsigned char fill_Data)//ȫ�����
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

void OLED_CLS(void)//����
{
	OLED_Fill(0x00);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : ��OLED�������л���
//--------------------------------------------------------------
void OLED_ON(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X14);  //������ɱ�
	WriteCmd(0XAF);  //OLED����
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : ��OLED���� -- ����ģʽ��,OLED���Ĳ���10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X10);  //�رյ�ɱ�
	WriteCmd(0XAE);  //OLED����
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          : 
// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); ch[] -- Ҫ��ʾ���ַ���; TextSize -- �ַ���С(1:6*8 ; 2:8*16)
// Description    : ��ʾcodetab.h�е�ASCII�ַ�,��6*8��8*16��ѡ��
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
// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); N:������codetab.h�е�����
// Description    : ��ʾcodetab.h�еĺ���,16*16����
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
// Parameters     : x0,y0 -- ��ʼ������(x0:0~127, y0:0~7); x1,y1 -- ���Խ���(������)������(x1:1~128,y1:1~8)
// Description    : ��ʾBMPλͼ
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




/************************************** �Լ�д����ʾ���� ****************************************/
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
		OLED_ShowMYCN(30+i*16,11,Normal16x16,i);//������ʾ����
	}
}

void ShowDanger(void) 
{
	uint8_t i;
	
	for(i=0;i<3;i++)
	{
		OLED_ShowMYCN(39+i*16,6,Danger16x16,i);//������ʾ����
	}
}

void ShowCurStatusSomeBody(void)
{
	uint8_t i;
	
	for(i=0;i<8;i++)
	{
		OLED_ShowMYCN(0+i*16,0,CurStatusSomeBody16x16,i);//������ʾ����
	}
}


void ShowCurStatusNoBody(void)
{
	uint8_t i;
	
	for(i=0;i<8;i++)
	{
		OLED_ShowMYCN(0+i*16,0,CurStatusNoBody16x16,i);//������ʾ����
	}
}


void ShowTemperature(void)
{
	uint8_t i;
	
	for(i=0;i<3;i++)
	{
		OLED_ShowMYCN(0+i*16,4,temperature16x16,i);//������ʾ����
	}
}

void ShowSmoke(void)
{
	uint8_t i;
	
	for(i=0;i<2;i++)
	{
		OLED_ShowMYCN(0+i*16,4,smoke16x16,i);//������ʾ����
	}
}

/******************************************************************************************************************/

void ShowSmartTrash(void)
{
	uint8_t i;
	
	for(i=0;i<5;i++)
	{	
		OLED_ShowMYCN(30+i*16,11,smartTrash16x16,i);//������ʾ����
	}
	
}

void Show_Author(void)
{
	uint8_t i;
	
	for(i=0;i<6;i++)
	{
		OLED_ShowMYCN(23+i*16,6,Chinese_author16X16,i);//������ʾ����
	}
}

void Showdu(void)
{
	uint8_t i;
	
	for(i=0;i<1;i++)
	{
		OLED_ShowMYCN(75+i*16,2,Chinese_du16X16,i);//������ʾ����
	}
}

void ShowHump(void)
{
	uint8_t i;
	
	for(i=0;i<2;i++)
	{
		OLED_ShowMYCN(0+i*16,4,Chinese_hump16X16,i);//������ʾ����
	}
}


void Show_youhai(void)
{
	uint8_t i;
	
	for(i=0;i<4;i++)
	{
		OLED_ShowMYCN(30+i*16,11,youhailaji16x16,i);//������ʾ����
	}
}

void Show_qita(void)
{
	uint8_t i;
	
	for(i=0;i<4;i++)
	{
		OLED_ShowMYCN(30+i*16,11,qitalaji16X16,i);//������ʾ����
	}
}

void Show_kehuishou(void)
{
	uint8_t i;
	
	for(i=0;i<4;i++)
	{
		OLED_ShowMYCN(30+i*16,11,kehuishouwu16X16,i);//������ʾ����
	}
}

void Show_chiyu(void)
{
	uint8_t i;
	
	for(i=0;i<4;i++)
	{
		OLED_ShowMYCN(30+i*16,11,chiyulaji16X16,i);//������ʾ����
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







