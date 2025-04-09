#include "ESP8266.H"

// AT指令定义
uint8_t RST[]="AT+RST\r\n\0";//刷新
uint8_t mode[]="AT+CWMODE=3\r\n\0";//设置功能模式为ap+station
uint8_t ljwifi[]="AT+CWJAP=\"xiaolin\",\"1357902468\"\r\n\0";//连接wifi
uint8_t xgmc[]="AT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"\"\r\n\0";//连接阿里云平台
uint8_t multi[]="AT+MQTTUSERCFG=0,1,\"NULL\",\"esp01&a1enm3FangJ\",\"D602462311E1A696CB45B2641F9AD9F764741697\",0,0,\"\"\r\n\0";//设置MQTT登录账号密码
uint8_t jltcp[]="AT+MQTTCLIENTID=0,\"123|securemode=2\\,signmethod=hmacsha1\\,timestamp=1743428066013|\"\r\n\0";//设置MQTT的ClientID
uint8_t ljym[]="AT+MQTTCONN=0,\"iot-06z00h7yodidaaj.mqtt.iothub.aliyuncs.com\",1883,1\r\n\0";
char Switch[200];
int Switch_zhi = 0;



// USART发送字符串函数
void USART_SendString(USART_TypeDef* USARTx, uint8_t *str) {
    while(*str != '\0') {
        USART_SendData(USARTx, *str++);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    }
}



void esp8266_Init(void) {
    // 初始化USART2
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    // 配置USART2 Tx (PA2) 为推挽复用输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置USART2 Rx (PA3) 为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // USART参数配置
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    
    // 使能USART2
    USART_Cmd(USART2, ENABLE);
    
    // 发送AT指令
    USART_SendString(USART2, RST);
    delay_ms(3000);
    
    USART_SendString(USART2, mode);
    delay_ms(3000);
    
    USART_SendString(USART2, ljwifi);
    delay_ms(3000);
    
    USART_SendString(USART2, xgmc);
    delay_ms(3000);
    
    USART_SendString(USART2, multi);
    delay_ms(3000);
    
    USART_SendString(USART2, jltcp);
    delay_ms(3000);
    
    USART_SendString(USART2, ljym);
    delay_ms(3000);
    
    // 发送Switch内容
    sprintf(Switch,"AT+MQTTPUB=0,\"/sys/a1LXYGeqxiV/esp01/thing/event/property/post\",\"{params:{\\\"K\\\":%d}}\",1,0\r\n",Switch_zhi);
    USART_SendString(USART2, (uint8_t *)Switch);
}





//uint8_t RST[]="AT+RST\r\n";//刷新
//uint8_t mode[]="AT+CWMODE=3\r\n\0";//设置功能模式为ap+station

//uint8_t ljwifi[]="AT+CWJAP=\"ESP\",\"1357902468\"\r\n";//连接wifi
//uint8_t xgmc[]="AT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"\"\r\n";//连接阿里云平台
//uint8_t multi[]="AT+MQTTUSERCFG=0,1,\"NULL\",\"esp01&a1LXYGeqxiV\",\"132CBFD7FDA6422FF3BA8515C36DBA3A46A55007\",0,0,\"\"\r\n";//设置MQTT登录账号密码
//uint8_t jltcp[]="AT+MQTTCLIENTID=0,\"esp|securemode=2\\,signmethod=hmacsha1\\,timestamp=1744179832386|\"\r\n";//设置MQTT的ClientID
//uint8_t ljym[]="AT+MQTTCONN=0,\"iot-06z00h7yodidaaj.mqtt.iothub.aliyuncs.com\",1883,1\r\n";
//char Switch[200];
//int Switch_zhi=0;

//void esp8266_Init()//我直接给你开头初始化了这边没必要修改
//{
//    int j=0;
//    for(j=0; RST[j]!='\0'; j++)
//    {
//        HAL_UART_Transmit(&huart2 ,&RST[j],1,0xffff);
//    }
//    delay_ms(3000);
//    for(j=0; mode[j]!='\0'; j++)
//    {

//        HAL_UART_Transmit(&huart2,&mode[j],1,0xffff);
//    }
//    delay_ms(3000);
//    for(j=0; ljwifi[j]!='\0'; j++)
//    {

//        HAL_UART_Transmit(&huart2 ,&ljwifi[j],1,0xffff);
//    }
//	delay_ms(3000);
//    for(j=0; xgmc[j]!='\0'; j++)
//    {

//        HAL_UART_Transmit(&huart2 ,&xgmc[j],1,0xffff);
//    }
//    delay_ms(3000);
//    for(j=0; multi[j]!='\0'; j++)
//    {

//        HAL_UART_Transmit(&huart2 ,&multi[j],1,0xffff);
//    }
//    delay_ms(3000);
//    for(j=0; jltcp[j]!='\0'; j++)
//    {

//        HAL_UART_Transmit(&huart2 ,&jltcp[j],1,0xffff);
//    }
//    delay_ms(3000);
//    for(j=0; ljym[j]!='\0'; j++)
//    {
//        HAL_UART_Transmit(&huart2 ,&ljym[j],1,0xffff);
//    }
//	delay_ms(3000); 
//  //sprintf(Switch,"AT+MQTTPUB=0,\"/sys/a1LXYGeqxiV/esp01/thing/event/property/post\",\"{params:{\\\"K\\\":%d}}\",1,0\r\n",Switch_zhi);
//	HAL_UART_Transmit(&huart2,(uint8_t *)Switch,strlen(Switch),0xffff);

//}

