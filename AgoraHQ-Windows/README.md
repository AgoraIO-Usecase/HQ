# Open Live Windows

*Read this in other languages: [English](README.en.md)*

这个开源示例项目演示了如何快速集成Agora视频SDK，实现多人视频连麦直播。

在这个示例项目中包含了以下功能：

- 加入通话和离开通话；
- 主播和观众模式切换；
- 静音和解除静音；
- 切换摄像头；
- 选择分辨率、码率和帧率；

本开源项目使用 **C++** 语言

你也可以在这里查看入门版的示例项目：[Agora-Windows-Tutorial-1to1](https://github.com/AgoraIO/Agora-Windows-Tutorial-1to1)

Agora视频SDK支持 iOS / Android / Windows / macOS 等多个平台，你可以查看对应各平台的示例项目：

- [OpenLive-Android](https://github.com/AgoraIO/OpenLive-Android)
- [OpenLive-iOS](https://github.com/AgoraIO/OpenLive-iOS)
- [OpenLive-macOS](https://github.com/AgoraIO/OpenLive-macOS)

## 运行示例程序
首先在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID。将 APP_ID宏定义内容改为刚才申请的AppID

```
#define APP_ID _T("Your App ID")
```

然后在 [Agora.io SDK](https://www.agora.io/cn/blog/download/) 下载 **视频通话 + 直播 SDK**，解压后将其中的 **sdk** 复制到本项目的 “OpenLive” 文件夹下（并覆盖同名旧目录）。

最后使用 VC++2013 打开 OpenLive.sln，编译整个解决方案即可运行
* SDK发布时分为x86和x64两个版本，编译x86版本时，请在VC解决方案平台中选择Win32选项；编译x64版本时，请在VC解决方案平台选项中选择x64选项。

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
