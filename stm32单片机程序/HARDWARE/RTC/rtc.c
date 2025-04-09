/*���һ��LED��1����һ��*/
#include "rtc.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "oled.h" 
#include "stm32f10x.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_pwr.h"

/***************************************************/
static struct DATETABLE pDate;

unsigned char date_time[29];

static void RTC_Configuartion(void)
{
        //ʹ�ܵ�Դʱ�Ӻͱ�������ʱ��
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
        //ʹ��RTC�ͺ󱸼Ĵ�������
        PWR_BackupAccessCmd(ENABLE);
        BKP_DeInit();   //��λ��������
        RCC_LSEConfig(RCC_LSE_ON);  //�����ⲿ��������
        while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET); //�ȴ��ⲿ����ʱ�Ӿ���
//      ʹ��RTC�ڲ�40Kʱ��
//      RCC_LSICmd(ENABLE);
//    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET);
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); //ѡ��RTC�ڲ�ʱ��Ϊ������32.768
        RCC_RTCCLKCmd(ENABLE); //ʹ��RTCʱ��
        RTC_WaitForSynchro();       //�ȴ�RTC�Ĵ���ͬ��
        RTC_WaitForLastTask();  //�ȴ�дRTC�Ĵ������
        RTC_ITConfig(RTC_IT_SEC, ENABLE);   //ʹ��RTC�Ĵ������ж�
        RTC_WaitForLastTask();  //�ȴ�дRTC�Ĵ������
        RTC_EnterConfigMode();  //��������
        RTC_SetPrescaler(32767);    //����Ԥ��Ƶ���ڲ����Զ���1
        RTC_WaitForLastTask();      //�ȴ�д�Ĵ������
        RTC_ExitConfigMode();       //�˳�����ģʽ
//			 PWR_BackupAccessCmd(DISABLE);//��ֹRTC�󱸼Ĵ��� 
//       RCC_ClearFlag();  //���RCC������־
}
static void NVIC_Configuartion(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) == SET)
    {
        RTC_ClearITPendingBit(RTC_IT_SEC);
        RTC_GetDate(&pDate);
       // printf("%d-%2d-%2d\t%02d:%02d:%02d\r\n", pDate.year, pDate.month, pDate.day, pDate.hour, pDate.minute, pDate.sec);
				sprintf((char *)date_time,"%d/%d/%d %02d:%02d:%02d", pDate.year, pDate.month, pDate.day, pDate.hour, pDate.minute, pDate.sec);
				//printf("%s\n",date_time);
				OLED_ShowStr(7,5,date_time,1);
		}
}
/******************************************************/
/*
    ��������:RTC ��ʼ��������RTC��ֵ
    ��������:
    ����ֵ:    
*/
void RTC_Init(void)
{

    if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)		//0x5050
    {
        RTC_Configuartion();        //��һ�����У���ʼ������
        RTC_WaitForLastTask();  //�ȴ�дRTC�Ĵ������
//  RTC_SetCounter(0);          //����ʱ���ֵ
        RTC_EnterConfigMode();
        RTC_SetSec(2020,3,22,21,36,30);
        RTC_WaitForLastTask();  //�ȴ�д�Ĵ������
        RTC_ExitConfigMode();
        BKP_WriteBackupRegister(BKP_DR1, 0x5050);   //д�����ñ�־
    }else
    {
        RTC_WaitForSynchro();       //�ȴ�RTC�Ĵ���ͬ��
        RTC_WaitForLastTask();  //�ȴ�дRTC�Ĵ������
        RTC_ITConfig(RTC_IT_SEC, ENABLE);   //ʹ��RTC���ж�
        RTC_WaitForLastTask();  //�ȴ�дRTC�Ĵ������
    }
    RCC_ClearFlag();    //�����λ��־
    NVIC_Configuartion();

}
/***********************�ж��Ƿ�Ϊ����********/
const uint8_t monthTable[12] = {31,28,31,30,31,30,31,31,30,31,30,31}; 
/*
    ��������:�ж��Ƿ�Ϊ����
    ��������:��Ҫ�жϵ����
    ����ֵ:    ����1�ɹ�������ʧ��
*/
static uint8_t isLeapYear(uint16_t mYear)   
{
    if (((mYear%4 == 0)&&((mYear%100) != 0))||(mYear%400) == 0)
    {
        return 1;
    }else 
    {
        return 0;
    }
}
/********************************************************************************/
/*
    ��������:����ת��Ϊ�벢����RTC
    ��������:������ʱ����
    ����ֵ:    ����0�ɹ�������ʧ��
*/
uint8_t RTC_SetSec(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t sec)
{
    uint32_t secCount = 0;
  uint16_t i;

    if (year <1970 || year >2099)
    {
        return 1;
    }
    for (i=1970; i<year; i++)
    {
        if (isLeapYear(i))
        {
            secCount += 31622400UL;
        }
        else
        {
            secCount += 31536000UL;
        }
    }

    for (i=0; i<month-1; i++)
    {
        secCount += ((uint32_t)monthTable[i]*86400UL);
        if (isLeapYear(year) && i==1)   //������1��
        {
            secCount += 86400UL;
        }
    }
    secCount += (uint32_t)(day-1) * 86400UL;    
    secCount += (uint32_t)(hour)*3600;
    secCount += (uint32_t)(minute)*60;
    secCount += (uint32_t)(sec);
    //printf("%u\n",secCount);

    /*ִ�����²�������ִ�жԱ��ݼĴ�����RTC�Ĳ���: 
    1)ͨ�����üĴ��� RCC_APB1ENR �� PWREN �� BKPEN ���򿪵�Դ�ͺ󱸽ӿڵ�ʱ�� 
    2)��Դ���ƼĴ���(PWR_CR)�� DBP λ��ʹ�ܺ󱻼Ĵ����� RTC �ķ���
    */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_SetCounter(secCount);
    RTC_WaitForLastTask();
    return 0;
}
/********************************************************************************/
/*
    ��������:��RTC��ȡ��ת��Ϊ����������ʱ����
                        ���� struct thisDate ��
    ��������:struct thisDate 
    ����ֵ:    ����0�ɹ�������ʧ��
*/
uint8_t RTC_GetDate(struct DATETABLE* thisDate)
{
    uint32_t timeCount = 0;
    uint32_t Count ;
    uint16_t tmp = 0;
    timeCount = RTC_GetCounter();
    Count = timeCount/86400UL;  //���������
    thisDate->week = (4+Count)%7;
    if (Count !=0)                  //����1��
    {
        tmp = 1970;                     //��1970�꿪ʼ
        while(Count>=365)
        {
            if (isLeapYear(tmp))//������
            {
                if (Count>= 366)    //��������366
                {
                    Count -= 366;
                }else
                {
                    tmp++;                  //����С��366 ���һ��
                    break;
                }
            }else
            {
                Count -= 365;       //ƽ��
            }
                tmp++;
        }
        thisDate->year = tmp;   //��ȡ������
        tmp = 0;
        while(Count>= 28)
        {
            if (isLeapYear(thisDate->year) && tmp == 1)
            {
                if (Count >= 29) 
                {
                    Count -= 29;
                }
                else    
                {
                    break;  //���������꣬���� ĳʱ
                }
            }else
            {
                if (Count >= monthTable[tmp]) 
                {
                    Count -= monthTable[tmp];
                }else
                {
                    break;
                }
            }
                tmp ++;
        }
        thisDate->month = tmp+1;    //��ȡ����
        thisDate->day = Count+1;    //��ȡ����
    }
    Count = timeCount%86400UL;
    thisDate->hour = Count/3600;
    thisDate->minute = (Count%3600)/60;
    thisDate->sec = (Count%3600)%60;
    return 0;
}

