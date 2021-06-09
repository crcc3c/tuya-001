# tuya-001
arduino and tuya hand to hand
This project is developed using Tuya SDK, which enables you to quickly develop branded apps connecting and controlling smart scenarios of many devices.For more information, please check Tuya Developer Website.

1、项目介绍
#
开始我的智能之旅源于【涂鸦智能+Arduino实战营】之涂鸦智能【幻彩灯带】—Arduino SDK 接入实操培训讲解，跟着池老师的讲解和QQ群里大家的讨论，完成了一版又一版的PCB设计。目前还是留有遗憾。也许事对项目的不熟悉以及硬件设计知识的缺乏，涂鸦的文档中心中也没有相关模块的硬件设计举例，小白还是需要多临摹，多参考。
建议后来的同学，一定先到涂鸦的文档中心，好好看上一边，做到心中有数

2、项目使用教程介绍
#

本项目由Tuya官方牵头，故不可缺的是免开发或少开始的MCU_SDK，涂鸦出品
其次是Arduino的开发环境，需要导入涂鸦的SDK进行细化设计
当然一个loT项目必不可缺的是可以WIFI或蓝牙连接网络的硬件，也是这次项目的主体部分，涂鸦出品的CBU或WBR3系列低功耗嵌入式模块产品。

2.1、配套的硬件设备
##

官方推荐的是CBU，我选择的是WBR3系列中的WBR3L，其实功能都差不多，就是引脚多少盒复用功能的不同。
WBR3L Wi-Fi&Bluetooth 模组是一个 RTOS 平台，集成了所有 Wi-Fi MAC 以及 TCP/IP 协议的函数库。大家可以基于这些开发满足自己需求的嵌入式 Wi-Fi 产品。

内置低功耗 KM4 MCU，可以兼作应用处理器
主频 100MHz
工作电压：3V-3.6V
外设：9×GPIOs, 1×UART, 1×Log_Tx
Wi-Fi 和蓝牙连通性
802.11 B/G/N20
通道 1-14@2.4GHz(CH1-11 for US/CA, CH1-13 for EU/CN)
支持 WEP/WPA/WPA2/WPA2 PSK（AES） 安全模式
支持Bluetooth 4.2 Low Energy
802.11b 模式下+20dBm 的输出功率
支持 SmartConfig 功能（包括 Android 和 iOS 设备）
板载 PCB Onboard 天线
通过 CE,FCC 认证
工作温度：-40℃ to 105℃

更多信息可以参考《WBR3L 模组规格书_涂鸦智能_文档中心.pdf》

2.2、使用的涂鸦SDK类型和版本号
##

2.3、如何运行项目等
##

*项目所有软件工程及硬件工程设计文档、代码及硬件BOM都已经上传到GIT，如果需要参考，可以打包下载，使用对应的软件打开即可一览无余。
电路原理图及PCB设计需要使用到KiCAD这个开源的软件，不涉及破解盒律师函，可以开心的玩耍
*MUC_SDK需要导入arduino IDE，Arduino的软件可以从官方下载，无论你使用时1.8.*版的还是2.0.*版的，都可以参考<DevelopResourcePack_WS2812B_20210528>文档进行导入。

![MCU_SDK使用指导文档链接](https://docs.tuya.com/zh/iot/device-development/access-mode-mcu/wifi-general-solution/software-reference-wifi/overview-of-migrating-tuyas-mcu-sdk?id=K9hhi0xr5vll9)

2.3.1、根据产品实际情况（重置wifi按钮和wifi状态指示灯处理方式、是否支持MCU升级等）进行配置
###

protocol.h内修改此配置
###
protocol.h 包含以下信息：MCU 要发送至 Wi-Fi 模块初始化所需的参数。SDK 裁剪所定义的宏。用户可根据需要的功能打开相应的宏定义。protocol.c中的函数声明。

protocol.c包含协议数据体的内容处理函数。您可以根据项目需求，在相应函数内添加代码，获取 Wi-Fi 模块向 MCU 发送的数据。

2.3.2、移植MCU_SDK
###

请查看protocol.c文件内的移植步骤,并正确完成移植。移植后，请完成数据下发处理、数据上报部分的代码，即可完成全部wifi功能。

2.3.3开发资源包中包括5个文件：
###

* 根据产品功能自动生成的串口通讯协议
* MCU SDK
* MCU SDK使用说明
* 涂鸦串口调试助手（内含使用说明）
* 调试文件

2.3.4使用流程
###

* 查看产品通讯协议；
* MCU SDK为根据产品自动生成的控制板MCU程序，在此基础上进行修改和调用，可以快速完成MCU程序；
* 利用涂鸦提供的串口调试助手来验证MCU程序是否调通。涂鸦串口调试助手作为为模拟涂鸦模块收发指令；
* 请务必先使用涂鸦串口调试助手调通程序，再将模块连接至控制板上进行App测试。（App下载：应用商店中搜索“涂鸦智能”）。



3、项目相关的demo照片，App截屏，视频链接
#

3.1、电路板设计
##
比如原理图的设计：
![SCH](https://github.com/crcc3c/tuya-001/blob/main/SCH.png)

3.2、电路板3D
##

电路板设计正面视图：
![正面正视图](https://github.com/crcc3c/tuya-001/blob/main/WBR3L_W_PCB_F_3D.0.png)

![正面侧视图1](https://github.com/crcc3c/tuya-001/blob/main/WBR3L_W_PCB.png)

![正面侧视图2](https://github.com/crcc3c/tuya-001/blob/main/WBR3L_W_PCB0.png)

电路板设计底面视图：
![底面正视图](https://github.com/crcc3c/tuya-001/blob/main/WBR3L_W_PCB_B_3D.00.png)

![底面侧视图1](https://github.com/crcc3c/tuya-001/blob/main/WBR3L_W_PCB_B.png)

![底面侧视图2](https://github.com/crcc3c/tuya-001/blob/main/WBR3L_W_PCB_B_3D.0.png)

![底面侧视图3](https://github.com/crcc3c/tuya-001/blob/main/WBR3L_W_PCB_B_3D.png)

3.3、电路板实物
##

目前已采购了元器件，等待都到货了，就可以开始焊接PCB了![]()
待补充……

3.4、Arduino调试
##

待补充……

3.5、APP联调
##

待补充……

3.6、视频介绍
##

待补充……

4、项目使用的开源License文件，如MIT
#

待补充……
