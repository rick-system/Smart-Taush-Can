/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		��onenetƽ̨�����ݽ����ӿڲ�
	*
	*	�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
	*				V1.1���ṩͳһ�ӿڹ�Ӧ�ò�ʹ�ã����ݲ�ͬЭ���ļ�����װЭ����ص����ݡ�
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸
#include "esp8266.h"

//Э���ļ�
#include "onenet.h"
#include "edpkit.h"

//Ӳ������
#include "usart.h"
#include "delay.h"


//C��
#include <string.h>
#include <stdio.h>


#define DEVID	"576590390"                           //�豸id

#define APIKEY	"9O8b0tnomMk3llmur6j6ZraiFSE="        //����

 FENGSHAN_STATUS fengshan_status;                     //�ṹ���������
 
 
 extern short flag;
 
//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};				//Э���

	unsigned char *dataPtr;
	
	unsigned char status = 1;
	
	if(EDP_PacketConnect1(DEVID, APIKEY, 256, &edpPacket) == 0)		//����devid �� apikey��װЭ���
	{
		ESP8266_SendData(edpPacket._data, edpPacket._len);			//�ϴ�ƽ̨
		
		dataPtr = ESP8266_GetIPD(250);								//�ȴ�ƽ̨��Ӧ
		if(dataPtr != NULL)
		{
			if(EDP_UnPacketRecv(dataPtr) == CONNRESP)
			{
				switch(EDP_UnPacketConnectRsp(dataPtr))
				{
					case 0:
						//UsartPrintf(USART_DEBUG, "Tips:	���ӳɹ�\r\n");
					status = 0;break;
					
					case 1:
						//UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�Э�����\r\n");
					break;
					case 2:
						//UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��豸ID��Ȩʧ��\r\n");
					break;
					case 3:
						//UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�������ʧ��\r\n");
					break;
					case 4:
						//UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��û�ID��Ȩʧ��\r\n");
					break;
					case 5:
						//UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�δ��Ȩ\r\n");
					break;
					case 6:
						//UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ���Ȩ����Ч\r\n");
					break;
					case 7:
						//UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�������δ����\r\n");
					break;
					case 8:
						//UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ����豸�ѱ�����\r\n");
					break;
					case 9:
						//UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��ظ��������������\r\n");
					break;
					
					default:
					//	UsartPrintf(USART_DEBUG, "ERR:	����ʧ�ܣ�δ֪����\r\n");
					break;
				}
			}
		}
		
		EDP_DeleteBuffer(&edpPacket);								//ɾ��
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
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_SendData(void)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};												//Э���
	
	char buf[128];
	
	short body_len = 0, i = 0;
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);																	//��ȡ��ǰ��Ҫ���͵����������ܳ���
	
	if(body_len)
	{
		if(EDP_PacketSaveData(NULL, body_len, NULL, kTypeString, &edpPacket) == 0)					//���
		{
			for(; i < body_len; i++)
				edpPacket._data[edpPacket._len++] = buf[i];
			
			ESP8266_SendData(edpPacket._data, edpPacket._len);										//�ϴ����ݵ�ƽ̨
			
			EDP_DeleteBuffer(&edpPacket);															//ɾ��
		}
	}
	
}



extern char flag_xiaodu;
//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};	//Э���
	
	char *cmdid_devid = NULL;
	unsigned short cmdid_len = 0;
	char *req = NULL;
	unsigned int req_len = 0;
	unsigned char type = 0;
	
	short result = 0;

	type = EDP_UnPacketRecv(cmd);
	switch(type)										//�ж���pushdata���������·�
	{
		case CMDREQ:									//�������
			
			result = EDP_UnPacketCmd(cmd, &cmdid_devid, &cmdid_len, &req, &req_len);
			
			if(result == 0)								//����ɹ������������ظ������
			{
				EDP_PacketCmdResp(cmdid_devid, cmdid_len, req, req_len, &edpPacket);
			}
			
		break;
			
		case SAVEACK: break;
			
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();										//��ջ���
	
	if(result == -1)
		return;

	
/***********************************************�����Լ���ƽ̨Ӧ��Ҫ�޸ĵĲ���************************/	

	
	if((NULL != strstr((const char *)req, "fengshan1")))  //Լ��ƽ̨��������"fengshan1"Ϊ�����̵��� 
	{
			//GPIO_ResetBits(GPIOC,GPIO_Pin_13);
			flag = 1;
	}
	else if((NULL != strstr((const char *)req, "fengshan0")))  //Լ��ƽ̨��������"fengshan0"Ϊ�رռ̵���
	{
			//GPIO_SetBits(GPIOC, GPIO_Pin_13); 
		  flag = 2;
	}
	else if((NULL != strstr((const char *)req, "servoI1")))  //Լ��ƽ̨��������"servoI1"Ϊ�򿪳�������Ͱ
	{
			//TIM_SetCompare1(TIM4,350);//��
			flag = 3;
	}
	else if((NULL != strstr((const char *)req, "servoI0")))  //Լ��ƽ̨��������"servoI0"Ϊ�رճ�������Ͱ
	{
			//TIM_SetCompare1(TIM4,1200);//�ر�
		 flag = 4;
	}
	else if((NULL != strstr((const char *)req, "servoII1")))  //Լ��ƽ̨��������"servoII1"Ϊ����������Ͱ
	{
			//TIM_SetCompare2(TIM4,600);//��
			 flag = 5;
	}
	else if((NULL != strstr((const char *)req, "servoII0")))  //Լ��ƽ̨��������"servoII0"Ϊ�ر���������Ͱ
	{
			//TIM_SetCompare2(TIM4,1300);//ʱ�䵽�ر�
		 flag = 6;
	}
	else if((NULL != strstr((const char *)req, "servoIII1")))  //Լ��ƽ̨��������"servoIII1"Ϊ���к�����Ͱ
	{
			//TIM_SetCompare4(TIM4,350);//��
		 flag = 7;
	}
	else if((NULL != strstr((const char *)req, "servoIII0")))  //Լ��ƽ̨��������"servoIII0"Ϊ�ر��к�����Ͱ
	{
			//TIM_SetCompare4(TIM4,1200);//ʱ�䵽�ر�
		 flag = 8;
	}
	else if((NULL != strstr((const char *)req, "servoIV1")))  //Լ��ƽ̨��������"servoIV1"Ϊ�򿪿ɻ���������Ͱ
	{
			//TIM_SetCompare3(TIM4,500);//��
		  flag = 9;
	}
	else if((NULL != strstr((const char *)req, "servoIV0")))  //Լ��ƽ̨��������"servoIV0"Ϊ�رտɻ���������Ͱ
	{
			//TIM_SetCompare3(TIM4,1000);//ʱ�䵽�ر�
		 flag = 10;
	}
		
/***************************************************************************************************/	
	
	if(type == CMDREQ && result == 0)						//���������� �� ����ɹ�
	{
		EDP_FreeBuffer(cmdid_devid);						//�ͷ��ڴ�
		EDP_FreeBuffer(req);
															//�ظ�����
		ESP8266_SendData(edpPacket._data, edpPacket._len);	//�ϴ�ƽ̨
		EDP_DeleteBuffer(&edpPacket);						//ɾ��
	}
	
	

}
