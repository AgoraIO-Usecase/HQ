# Agora Live Quiz Android

*Read this in other languages: [English](README.md)*

这个开源示例项目演示了如何利用 Agora SDK 实现多人互动答题。本项目为观众答题端

在这个示例项目中包含了以下功能：

- 加入答题频道；
- 加入 Agora 直播频道；
- 接收题卡；
- 回答问题；
- 显示结果；

本开源项目使用 **Java** 语言

## 运行示例程序
首先在 [Agora.io](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID ，将 AppID 填写进 Constants.java 。

```
public static final String AGORA_APP_ID = #Your_APPID;

```
再去[融云](http://www.rongcloud.cn/)注册账号，并创建自己的测试项目，获取到 App Key，将 App Key 填写进 Constants.java 。

```
public static final String APP_KEY = "mgb7ka1nmwzxg";

```

最后使用 Android Studio 打开示例项目，连接 Android 设备，设置有效的开发者签名后即可运行。

## 运行环境
* Android Studio 3.0
* Android 真机设备

## 联系我们

- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题，你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题，可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持，你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的bug，欢迎提交 [issue](https://github.com/AgoraIO/HQ/issues)

## 代码许可

The MIT License (MIT).
