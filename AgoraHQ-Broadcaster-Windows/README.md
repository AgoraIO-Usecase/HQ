# AgoraHQ

*Read this in other languages: [English](README.en.md)*

这个开源示例项目演示了如何快速集成AgoraHQ。
这个部分是HQ 3期部分的功能，changeLog below:
- 更新了语言包.
- fix 当下发题目数目少于4个是crash.
- 增加了操作导航.
- 增加了配置修改重启操作.

在这个示例项目中包含了以下功能：
- 角色:主持人导播
- 媒体部分:直播主讲; yy伴侣实现视频特效
- 信令部分:控制答题

本开源项目使用 **C++** 语言

AgoraHQ视频SDK支持 iOS / NodeJs / Windows  等多个平台，你可以查看对应各平台的示例项目：

- [OpenLive-Nodejs](https://github.com/AgoraIO/HQ/tree/master/AgoraHQ-Server-Nodejs)
- [OpenLive-iOS](https://github.com/AgoraIO/HQ/tree/master/AgoraHQ-iOS-Swift)

## 运行示例程序
首先在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID。修改配置文件HQConfig.ini

```
[LoginInfo]

AppId=

AppCertificateId=

LoginUid=

AppCertEnable=

VideoSolutinIndex=

CameraDeviceName=

CameraDeviceID=

```

最后使用 VC++2013 打开 AgoraHQ.sln，编译整个解决方案即可运行，

在程序主目录下有一个restart.bat脚本，需要将restart.bat copy到执行目录下.

## 运行环境
* VC++ 2013(或更高版本)
* Windows7(或更高版本)

## 联系我们

- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题，你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题，可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持，你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的bug，欢迎提交 [issue](https://github.com/AgoraIO/OpenLive-Windows/issues)

## 代码许可

The MIT License (MIT).
