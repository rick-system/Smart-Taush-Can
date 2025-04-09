/*添加一个LED灯1秒闪一次*/
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
        //使能电源时钟和备份区域时钟
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
        //使能RTC和后备寄存器访问
        PWR_BackupAccessCmd(ENABLE);
        BKP_DeInit();   //复位备份区域
        RCC_LSEConfig(RCC_LSE_ON);  //开启外部低速振荡器
        while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET); //等待外部低速时钟就绪
//      使能RTC内部40K时钟
//      RCC_LSICmd(ENABLE);
//    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET);
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); //选择RTC内部时钟为低速振荡32.768
        RCC_RTCCLKCmd(ENABLE); //使能RTC时钟
        RTC_WaitForSynchro();       //等待RTC寄存器同步
        RTC_WaitForLastTask();  //等待写RTC寄存器完成
        RTC_ITConfig(RTC_IT_SEC, ENABLE);   //使能RTC寄存器秒中断
        RTC_WaitForLastTask();  //等待写RTC寄存器完成
        RTC_EnterConfigMode();  //允许配置
        RTC_SetPrescaler(32767);    //设置预分频，内部会自动加1
        RTC_WaitForLastTask();      //等待写寄存器完成
        RTC_ExitConfigMode();       //退出配置模式
//			 PWR_BackupAccessCmd(DISABLE);//禁止RTC后备寄存器 
//       RCC_ClearFlag();  //清除RCC重启标志
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
    函数功能:RTC 初始化并设置RTC的值
    函数参数:
    返回值:    
*/
void RTC_Init(void)
{

    if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)		//0x5050
    {
        RTC_Configuartion();        //第一次运行，初始化设置
        RTC_WaitForLastTask();  //等待写RTC寄存器完成
//  RTC_SetCounter(0);          //设置时间初值
        RTC_EnterConfigMode();
        RTC_SetSec(2020,3,22,21,36,30);
        RTC_WaitForLastTask();  //等待写寄存器完成
        RTC_ExitConfigMode();
        BKP_WriteBackupRegister(BKP_DR1, 0x5050);   //写入配置标志
    }else
    {
        RTC_WaitForSynchro();       //等待RTC寄存器同步
        RTC_WaitForLastTask();  //等待写RTC寄存器完成
        RTC_ITConfig(RTC_IT_SEC, ENABLE);   //使能RTC秒中断
        RTC_WaitForLastTask();  //等待写RTC寄存器完成
    }
    RCC_ClearFlag();    //清除复位标志
    NVIC_Configuartion();

}
/***********************判断是否为闰年********/
const uint8_t monthTable[12] = {31,28,31,30,31,30,31,31,30,31,30,31}; 
/*
    函数功能:判断是否为闰年
    函数参数:需要判断的年份
    返回值:    返回1成功，其他失败
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
    函数功能:日期转换为秒并配置RTC
    函数参数:年月日时分秒
    返回值:    返回0成功，其他失败
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
        if (isLeapYear(year) && i==1)   //闰年多加1天
        {
            secCount += 86400UL;
        }
    }
    secCount += (uint32_t)(day-1) * 86400UL;    
    secCount += (uint32_t)(hour)*3600;
    secCount += (uint32_t)(minute)*60;
    secCount += (uint32_t)(sec);
    //printf("%u\n",secCount);

    /*执行以下操作可以执行对备份寄存器和RTC的操作: 
    1)通过设置寄存器 RCC_APB1ENR 的 PWREN 和 BKPEN 来打开电源和后备接口的时钟 
    2)电源控制寄存器(PWR_CR)的 DBP 位来使能后被寄存器和 RTC 的访问
    */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_SetCounter(secCount);
    RTC_WaitForLastTask();
    return 0;
}
/********************************************************************************/
/*
    函数功能:从RTC获取秒转换为年月日星期时分秒
                        存入 struct thisDate 中
    函数参数:struct thisDate 
    返回值:    返回0成功，其他失败
*/
uint8_t RTC_GetDate(struct DATETABLE* thisDate)
{
    uint32_t timeCount = 0;
    uint32_t Count ;
    uint16_t tmp = 0;
    timeCount = RTC_GetCounter();
    Count = timeCount/86400UL;  //计算出天数
    thisDate->week = (4+Count)%7;
    if (Count !=0)                  //大于1天
    {
        tmp = 1970;                     //从1970年开始
        while(Count>=365)
        {
            if (isLeapYear(tmp))//是闰年
            {
                if (Count>= 366)    //天数大于366
                {
                    Count -= 366;
                }else
                {
                    tmp++;                  //天数小于366 最后一年
                    break;
                }
            }else
            {
                Count -= 365;       //平年
            }
                tmp++;
        }
        thisDate->year = tmp;   //获取到了年
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
                    break;  //当年是闰年，二月 某时
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
        thisDate->month = tmp+1;    //获取到月
        thisDate->day = Count+1;    //获取到日
    }
    Count = timeCount%86400UL;
    thisDate->hour = Count/3600;
    thisDate->minute = (Count%3600)/60;
    thisDate->sec = (Count%3600)%60;
    return 0;
}

