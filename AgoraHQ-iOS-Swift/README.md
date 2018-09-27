# Agora HQ iOS for Swift

*其他语言版本： [简体中文](README.zh.md)*

The Agora HQ iOS for Swift 1to1 Sample App is an open-source demo that will help you get Multi-person interactive answer integrated directly into your iOS applications using the Agora SDK.

With this sample app, you can:

- Start Game
- Join / leave channel
- Receive the question
- Answer question
- Show result

This demo is written in **Swift**

## Running the App
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID and App Certificate. Update "KeyCenter.swift" with your App ID and App Certificate. if the App Certificate is not opened, Fill in AppcertificateID with ""

```
static let AppId: String = "Your App ID"
static let AppcertificateID:  String = "Agora App Certificate"

```

Finally, Open Agora iOS Tutorial Objective-C.xcodeproj, connect your iPhone／iPad device, setup your development signing and run.


## Developer Environment Requirements
* XCode 8.0 +
* Real devices / iOS simulator

## Connect Us

- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/HQ/issues)

## License

The MIT License (MIT).
