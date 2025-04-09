/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.1
	*
	*	说明： 		与onenet平台的数据交互接口层
	*
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*				V1.1：提供统一接口供应用层使用，根据不同协议文件来封装协议相关的内容。
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "esp8266.h"

//协议文件
#include "onenet.h"
#include "edpkit.h"

//硬件驱动
#include "usart.h"
#include "delay.h"


//C库
#include <string.h>
#include <stdio.h>


#define DEVID	"576590390"                           //设备id

#define APIKEY	"9O8b0tnomMk3llmur6j6ZraiFSE="        //密码

 FENGSHAN_STATUS fengshan_status;                     //结构体变量定义
 
 
 extern short flag;
 
//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};				//协议包

	unsigned char *dataPtr;
	
	unsigned char status = 1;
	
	if(EDP_PacketConnect1(DEVID, APIKEY, 256, &edpPacket) == 0)		//根据devid 和 apikey封装协议包
	{
		ESP8266_SendData(edpPacket._data, edpPacket._len);			//上传平台
		
		dataPtr = ESP8266_GetIPD(250);								//等待平台响应
		if(dataPtr != NULL)
		{
			if(EDP_UnPacketRecv(dataPtr) == CONNRESP)
			{
				switch(EDP_UnPacketConnectRsp(dataPtr))
				{
					case 0:
						//UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");
					status = 0;break;
					
					case 1:
						//UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");
					break;
					case 2:
						//UsartPrintf(USART_DEBUG, "WARN:	连接失败：设备ID鉴权失败\r\n");
					break;
					case 3:
						//UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");
					break;
					case 4:
						//UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户ID鉴权失败\r\n");
					break;
					case 5:
						//UsartPrintf(USART_DEBUG, "WARN:	连接失败：未授权\r\n");
					break;
					case 6:
						//UsartPrintf(USART_DEBUG, "WARN:	连接失败：授权码无效\r\n");
					break;
					case 7:
						//UsartPrintf(USART_DEBUG, "WARN:	连接失败：激活码未分配\r\n");
					break;
					case 8:
						//UsartPrintf(USART_DEBUG, "WARN:	连接失败：该设备已被激活\r\n");
					break;
					case 9:
						//UsartPrintf(USART_DEBUG, "WARN:	连接失败：重复发送连接请求包\r\n");
					break;
					
					default:
					//	UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");
					break;
				}
			}
		}
		
		EDP_DeleteBuffer(&edpPacket);								//删包
	}
	
	return status;
	
}

unsigned char OneNet_FillBuf(char *buf)
{
	
	char text[16];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, ",;");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "fengshan,%d;", fengshan_status.fengshan);
	strcat(buf, text);
	
	return strlen(buf);

}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(void)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[128];
	
	short body_len = 0, i = 0;
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);																	//获取当前需要发送的数据流的总长度
	
	if(body_len)
	{
		if(EDP_PacketSaveData(NULL, body_len, NULL, kTypeString, &edpPacket) == 0)					//封包
		{
			for(; i < body_len; i++)
				edpPacket._data[edpPacket._len++] = buf[i];
			
			ESP8266_SendData(edpPacket._data, edpPacket._len);										//上传数据到平台
			
			EDP_DeleteBuffer(&edpPacket);															//删包
		}
	}
	
}



extern char flag_xiaodu;
//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};	//协议包
	
	char *cmdid_devid = NULL;
	unsigned short cmdid_len = 0;
	char *req = NULL;
	unsigned int req_len = 0;
	unsigned char type = 0;
	
	short result = 0;

	type = EDP_UnPacketRecv(cmd);
	switch(type)										//判断是pushdata还是命令下发
	{
		case CMDREQ:									//解命令包
			
			result = EDP_UnPacketCmd(cmd, &cmdid_devid, &cmdid_len, &req, &req_len);
			
			if(result == 0)								//解包成功，则进行命令回复的组包
			{
				EDP_PacketCmdResp(cmdid_devid, cmdid_len, req, req_len, &edpPacket);
			}
			
		break;
			
		case SAVEACK: break;
			
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();										//清空缓存
	
	if(result == -1)
		return;

	
/***********************************************根据自己的平台应用要修改的部分************************/	

	
	if((NULL != strstr((const char *)req, "fengshan1")))  //约定平台控制命令"fengshan1"为开启继电器 
	{
			//GPIO_ResetBits(GPIOC,GPIO_Pin_13);
			flag = 1;
	}
	else if((NULL != strstr((const char *)req, "fengshan0")))  //约定平台控制命令"fengshan0"为关闭继电器
	{
			//GPIO_SetBits(GPIOC, GPIO_Pin_13); 
		  flag = 2;
	}
	else if((NULL != strstr((const char *)req, "servoI1")))  //约定平台控制命令"servoI1"为打开厨余垃圾桶
	{
			//TIM_SetCompare1(TIM4,350);//打开
			flag = 3;
	}
	else if((NULL != strstr((const char *)req, "servoI0")))  //约定平台控制命令"servoI0"为关闭厨余垃圾桶
	{
			//TIM_SetCompare1(TIM4,1200);//关闭
		 flag = 4;
	}
	else if((NULL != strstr((const char *)req, "servoII1")))  //约定平台控制命令"servoII1"为打开其他垃圾桶
	{
			//TIM_SetCompare2(TIM4,600);//打开
			 flag = 5;
	}
	else if((NULL != strstr((const char *)req, "servoII0")))  //约定平台控制命令"servoII0"为关闭其他垃圾桶
	{
			//TIM_SetCompare2(TIM4,1300);//时间到关闭
		 flag = 6;
	}
	else if((NULL != strstr((const char *)req, "servoIII1")))  //约定平台控制命令"servoIII1"为打开有害垃圾桶
	{
			//TIM_SetCompare4(TIM4,350);//打开
		 flag = 7;
	}
	else if((NULL != strstr((const char *)req, "servoIII0")))  //约定平台控制命令"servoIII0"为关闭有害垃圾桶
	{
			//TIM_SetCompare4(TIM4,1200);//时间到关闭
		 flag = 8;
	}
	else if((NULL != strstr((const char *)req, "servoIV1")))  //约定平台控制命令"servoIV1"为打开可回收物垃圾桶
	{
			//TIM_SetCompare3(TIM4,500);//打开
		  flag = 9;
	}
	else if((NULL != strstr((const char *)req, "servoIV0")))  //约定平台控制命令"servoIV0"为关闭可回收物垃圾桶
	{
			//TIM_SetCompare3(TIM4,1000);//时间到关闭
		 flag = 10;
	}
		
/***************************************************************************************************/	
	
	if(type == CMDREQ && result == 0)						//如果是命令包 且 解包成功
	{
		EDP_FreeBuffer(cmdid_devid);						//释放内存
		EDP_FreeBuffer(req);
															//回复命令
		ESP8266_SendData(edpPacket._data, edpPacket._len);	//上传平台
		EDP_DeleteBuffer(&edpPacket);						//删包
	}
	
	

}
