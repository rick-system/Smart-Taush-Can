# Smart-Taush-Can
[TOC]

## 功能介绍
1、离线语音识别识别垃圾种类并且垃圾桶自动翻盖
说出唤醒词“垃圾桶”后，再说一句垃圾名称，语音识别模块端识别到相应关键词，便会将结果通过串口发送到stm32端，stm32端接着会发送打开相应垃圾桶盖的指令，6s后，垃圾桶盖自动关闭。翻盖功能是通过stm32端控制舵机实现。
2、语音播报功能
stm32端若判断到有人说出唤醒词了，通过串口发送指令给语音播报模块，外放喇叭便会播报“你好”的音频，若判断到相应的垃圾种类，便会播报相应的垃圾种类音频。
3、APP/网页端远程控制垃圾桶
stm32端通过串口连接wifi模块esp8266-01s，接入onenet云平台，实现联网功能，通过云端下达指令给设备，从而控制垃圾桶实现翻盖及控制继电器通断。
4、OLED屏显示信息
OLED屏可以显示作者的相关信息、及每次语音播报也会显示相应文字信息。
