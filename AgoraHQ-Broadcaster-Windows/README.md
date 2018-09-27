#  AgoraHQ

*其他语言版本： [简体中文](README.en.md)*

The AgoraHQ Windows Sample App is an open-source demo that will help you get live video chat integrated directly into your windows applications using the Agora Video SDK. This part is the function of HQ Three
- Update the Language pack
- fix the crash when the number of choose less than four
- add operation navigation
- add update config restart

With this sample app, you can:
- Role: host guide broadcast
- Media section: Live Speaker; yy partner to achieve video effects
- Signaling part: control the answer

This demo is written in **C++**

Agora Video SDK supports iOS / NodeJs / Windows etc. You can find demos of these platform here:

- [OpenLive-Nodejs](https://github.com/AgoraIO/HQ/tree/master/AgoraHQ-Server-Nodejs)
- [OpenLive-iOS](https://github.com/AgoraIO/HQ/tree/master/AgoraHQ-iOS-Swift)

## Running the App
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. 

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

Finally, Open AgoraHQ.sln with your VC++ 2013(or higher) and build all solution and run.

There is a Restart.bat script in the home directory. You need to copy the script to the execution directory during the actual running.

## Developer Environment Requirements
* VC++ 2013(or higher)
* win7(or higher)

## Connect Us

- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can fire bugs about this demo at [issue](https://github.com/AgoraIO/OpenLive-Windows/issues)

## License

The MIT License (MIT).
