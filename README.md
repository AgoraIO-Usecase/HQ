# Agora HQ Trivia Template

The Agora HQ Trivia template enables you to quickly get started with building a trivia game application. This tutorial will explain how to create an Agora account, download the HQ SDK, and use an Agora template branch to use as a framework for your [Agora API](https://docs.agora.io/en/) application.

- [General Overview](#general-overview)
- [Architecture Overview](#architecture-overview)
- [Hardware / Software Requirements](#hardware--software-requirements)
- [How to Build the Environment](#how-to-build-the-environment)
- [Customize the UI](#customize-the-ui)
- [Game Play Instructions](#game-play-instructions)
- [Resources](#resources)
- [License](#license)

## General Overview

Agora HQ Trivia is a live-streamed quiz show. 

- [Agora HQ Trivia Game Flow](#agora-hq-trivia-game-flow)
- [Agora SDKs and API Services](#agora-sdks-and-api-services)


### Agora HQ Trivia Game Flow

Players can participate in daily trivia games and win prize money. The game consists of a round of 10-12 questions. Each question has three answer choices.

- The host asks a the series of questions, each with increased difficulty.

- Players who correctly answer the question within the 10-second limit continue through the game. Players who answer incorrectly are eliminated from the game.

- At the end of the game, players that answer all the questions questions correctly, split the prize money.

### Agora SDKs and API Services

The HQ Trivia template is a turnkey solution for live trivia game applications and uses the Agora Special SDK.

The special SDK is pre-packaged with the broadcast SDK and signaling SDK.

[Special SDK](https://github.com/AgoraIO/HQ/tree/master/SDKs) = Native media + signaling (chat) + SEI (V2_0_2)


**Note:** The Agora Special SDK is *required* to run the HQ Trivia game demo and templates.

![contest_task.png](images/contest_task.jpeg)

The figure above, show the template solution breakdown, along with the features each service would handle. The green blocks indicate services the Agora SDK currently provides.

SDK|SDK Description|API Description
---|---|---
[Agora Signaling  SDK](https://docs.agora.io/en/2.3.1/download)  (Included in the Special SDK)|The host broadcasts questions for the audience members to answer using messaging. This SDK:<ul><li> Manages communication between the audience members and the host</li><li>Retrieves the number of current users in the channel</li><li>Pushes questions and answers back and forth</li><li>Acts as the synchronization mechanism (SEI)</li></ul>|Signaling service handles questions, answers, messaging, and comments
[Agora Interactive Broadcast SDK](https://docs.agora.io/en/2.3.1/download) (Included in the Special SDK)|The host/broadcaster pushes voice and video streams to the participants/audience/clients in real time|Media service handles audio/video streaming

## Architecture Overview

The Agora HQ Trivia template provides a base architecture for a live quiz application network. The game members consist of at least one host and multiple audience members. The host quizzes the audience, and the audience member that answers all the questions correctly receives a prize.

![image001.png](images/image001.jpg)

The demo applications use a preconfigured test server and App ID, and can support up to 30,000 simultaneous audience members. 

The Agora HQ Trivia demo consists of the following parts:

Application User|Description|System Requirements|Link
---|---|---|---
Host<br><br>(with OBS certificate)|Controls the quiz, and the host’s video stream<br><br>**Note:** Using OBS will capture and include video of the host using the virtual camera.|Windows 7+<br><br>Permission to access camera<br><br>**Note:** A working camera is required for the game players to see the host (broadcaster) on their mobile devices.<br><br>OBS Studio with OBS virtual plugin (required if the host wants to use [Open Broadcaster Software](https://obsproject.com/download) to enhance the the game broadcast)|[Download Windows](https://github.com/AgoraIO/HQ/releases/download/v1.0/HQ-win-for-obs-studio.zip)<br><br> **Note:** If you run the application and see the error “can’t start because mfc130u.dll is missing”, [download and this library](https://github.com/AgoraIO/HQ/blob/Solution-With-AgoraHQSigKit/AgoraHQ-Broadcaster-Windows/dist/vcredist_x86_vs2013.exe) to fix the issue.
Audience member|Receives / answers questions  + chat with other audience members|iOS / Android|[Download Android](https://github.com/AgoraIO/HQ/releases/download/v1.0/agora-trivia-app.apk) <br><br>Connect your mobile device with a USB cable, then install the Android client application on your mobile device.<br><br>[Download Xcode project for iOS](https://github.com/AgoraIO/HQ/tree/master/AgoraHQ-iOS-Swift) <br><br>**Note:** There is no pre-compiled .ipa file for iOS available. For iOS, build the application in Xcode and run it on your physical device. Ensure the application uses valid provisioning profile.
All users|Handles data flow between all users|NodeJS server with quiz pool|[Download source for NodeJS](https://github.com/AgoraIO/HQ/tree/master/HQ-Server-Nodejs)

For instructions on how to run the demo see the [Game Play Instructions](#game-play-instructions).


- [Template Application Types](#template-application-types)
- [Template Branches](#template-branches)
- [Communication Breakdown](#communication-breakdown)
- [Sample App Resources](#sample-app-resources)

### Template Application Types

The template consists of two main application types:

Application Type|Platform
---|---
Host application|Windows only
Audience member application|iOS and Android

**Note:** All game members (host and audience) must join with the same Agora App ID and channel name, or they will not be entered into the same game.

#### Host Features

The host controls the game configuration and settings for all audience members that join the game.

- Pre-configuration settings
- Start hosting a game
- Join / leave a channel
- Send a question
- Publish question to audience members
- Restart the game

![hq_demo.png](images/hq_demo.png)

#### Audience Member Features

- Join / leave a channel
- Receive a question
- Answer a question
- View game results

![hq_answer.png](images/hq_answer.png)

### Template Branches

There are three main game show templates available. Each template has its own separate branch in the Github repository. Learn more about the choosing the a [template branch for your project](#download-the-hq-sdk-and-choose-a-template-branch).

Template Model|Communication Flow|Description
---|---|---
Basic Live Broadcast Model|![contest_live_simple.jpeg](images/contest_live_simple.jpeg)|Allows the host to broadcast a live quiz for audience members to answer questions using messaging. Includes option to apply to add a co-host.
Co-host Model|![contest_live_cele.jpeg](images/contest_live_cele.jpeg)|Allows audience members to see the host and co-host (for example, a celebrity). The co-host can also interact with the host and answer questions.
Team Model|![contest_live_group.jpeg](images/contest_live_group.jpeg)|Allows the audience members to answer questions as a team. (This template was used in the demo provided here.)

### Communication Breakdown

The main communication for the live application network consists of the host(s), services, and audience members. The image below depicts the general communication flow the HQ Trivia template uses:

Location|Label|Description
---|---|---
Left grey box|`Room of the Host`|Represents the host(s) for the application
Middle grey box|`SD-RTN`|Represents the Agora SDK services
Right grey box|`User Group`|Represents the audience members

![contest_solution.jpeg](images/contest_solution.jpeg)

Service|Description
---|---
Live Broadcast Service|The host(s) push realtime audio and video streams to the audience members using the Agora Interactive Broadcast SDK
Signaling Service|Facilitates communication between the audience and the host using the The Agora Signaling SDK<ul><li>Retrieves the number of current users in the channel</li><li>Manages messaging between the host and the audience members</li><li>Pushes questions and answers synchronously, ensuring the video stream is synchronized with the questions</li></ul>
Customer service|Manages non-Agora SDK communication between the host and audience members
RTMP Streaming|Handles sharing to social media platforms

**Note:** Audience members who join and become co-hosts will begin using the communication protocols and services as a host, rather than an audience member.


## Hardware / Software Requirements

For mobile applications, use a physical device when publishing. Some simulators lack the functionality or the performance needed to run the Agora SDK functionality.

Platform|Software|OS / Device
---|---|---
**Server**|NodeJS|NodeJS enabled server
**Windows**|Visual Studio 2013 or higher<br><br>OBS Studio (only required if running the game with OBS Studio enabled)|Windows 7 or higher
**Android**|Android Studio 3.0 or above|Android device (e.g. Nexus 5X)
**iOS**|Xcode 8.0+ (Ensure a valid provisioning profile is applied to your project, or it will not run)|Physical iOS device (iPhone or iPad)


## How to Build the Environment

- [Download the HQ SDK and Choose a Template Branch](#download-the-hq-sdk-and-choose-a-template-branch)
- [Create an Account and Obtain an App ID](#create-an-account-and-obtain-an-app-id)
- [Obtain an App Certificate ID](#obtain-an-app-certificate-id) (Needed for Windows and iOS environments)
- [Build your Project](#build-your-project)

### Download the HQ SDK and Choose a Template Branch

Download the HQ 2nd Stage SDK [here](https://apprtcio-my.sharepoint.com/:f:/g/personal/zhangle_agora_io/EqI4y81PsbtIuxsPL_jZwL0Bl8H4TomX001-AthJVRbl3Q?e=5Z2CPb). The HQ 2nd Stage SDK is available for Android, iOS, and Windows.

Unzip the downloaded file and choose the most applicable template branch for project: 

Branch|Description
---|---
`master`|Empty branch. Do not use.
`Solution-With-AgoraHQSigKit`|(Deprecated) Based on HQ 1st stage SDK, with basic HQ functions
`Solution-for-TeamUpMode`|Based on HQ 2nd stage SDK. Includes "Team" mode and "Host-in" mode.
`Solution-for-TeamUpMode-Http`|Encompasses functionality of `Solution-for-TeamUpMode`. Adds use of http as a communication protocol between the server and clients.
`Solution-for-TeamUpMode-Http-obs-studio`|Encompasses functionality of `Solution-for-TeamUpMode-Http`. Adds OBS support.


### Create an Account and Obtain an App ID

To build and run the sample application, get an App ID: 

1. Create a developer account at [agora.io](https://dashboard.agora.io/signin/).
2. In the Dashboard that opens, click **Projects** > **Project List** in the left navigation.
3. Copy the **App ID** from the Dashboard to a text file. You will use this when you launch the app.

### Obtain an App Certificate ID

1. In the Agora dashboard, ensure your project is in **Edit** mode and press **Enable** to enable the app certificate.

	![appCert1.jpg](images/appCert1.jpg)
	
2. Click on the **Dynamic Key Server is ready, I'm going to enable app certificate** button to enable the app certificate.

	![appCert2.jpg](images/appCert2.jpg)

3. A notification box will appear indicating a confirmation message sent to your registered email address. When you receive the email, click on the **Go Confirm** button in the email.

	![appCert3.jpg](images/appCert3.jpg)
	
	**Note:** The email confirmation is only valid for 30 minutes, so ensure you confirm within that time period.

4. Click on the enable checkbox, then the **Enable** button to complete the process.
	
	![appCert4.jpg](images/appCert4.jpg)

	**Note:** The **Enable** button can only be clicked after the checkbox is selected.

5. Click on the eye icon to display the app certificate. Copy the **App Certificate** from the Dashboard to a text file. You will use this when you build the app for iOS.

	![appCert5.jpg](images/appCert5.jpg)


### Build your Project

To fully run the project, you will need to set up your environment with a server app (NodeJS), host app (Windows), and at least one audience member app (Android or iOS).

- [Build for NodeJS](#build-for-nodejs) (Server app)
- [Build for Windows](#build-for-windows) (Host app)
- [Build for Android](#build-for-android) (Audience member app)
- [Build for iOS](#build-for-ios) (Audience member app)

#### Build for NodeJS

In the `modules/config.js` file, replace `YOUR_APP_ID` with your Agora App ID.

```
module.exports = {
    agora_appid: "YOUR_APP_ID",
    cc_id: "agora_hq_cc_server_en",
    sig_server: "SIG_SERVER"
};
```

Open the Terminal app and navigate to the project directory.

Install the project using the command `npm install`

Start the server using the command `node server.js`

#### Build for Windows

In the `HQConfig.ini` file, copy the Agora App ID, append the following information:

Property|Description|Value
---|---|---
`AppId`|Agora App ID|Your App ID from the Agora Dashboard
`AppCertEnable`|Defines if an app certificate will be used|`true` or `false`
`AppCertificateId`|Agora App Certificate|App Certificate from the Agora Dashboard (required if `AppCertEnable` is `true`)


```
[LoginInfo]
AppId=
AppCertEnable=
AppCertificateId=
ChannelName=HQ_Demo
LoginUid=
VideoSolutinIndex=
CameraDeviceName=
CameraDeviceID=
```

Ensure the `Restart.bat` file is in the same location as the execution directory.

Open the `AgoraHQ.sln` file in Visual Studio C++.

Use **Build All Solution** to build and run the project.


#### Build for Android

In the `src/main/java/io/agora/agoraandroidhq/tools/Constants.java` file, replace `#YOUR_APPID` with the App ID from the Agora dashboard.

`public static final String AGORA_APP_ID = #YOUR_APPID;`

Open the sample project in Android Studio.

Connect your Android device and run the project. 

**Note:** Ensure a valid developer signing certificate is applied or your project will not run.

#### Build for iOS

Open the XCode project

In the `KeyCenter.swift` file:

	- Replace `<#Your App Id#>` with your Agora App ID.
	- Replace `<#Your App Certificate#>` with your Agora App Certificate

```
static let AppId: String = <#Your App Id#>
static let AppcertificateID:  String = <#Your App Certificate#>
```

Connect your iPhone／iPad device and run the project.

**Note:** Ensure you project has a valid provisioning profile or it will not run.


## Customize the UI

There are two versions of the audience member application. One for Android, and the other for the iOS.

The audience member application consists of three main screens. The loading screen, the game screen, and the information screen. The screen layouts are customizable and use native UI components from Android Studio and Xcode.

Screen|Android View|iOS View|Functionality
---|---|---|---
Main screen|![androidMainScreen.jpg](images/androidMainScreen.jpg)|![iosMainScreen.jpg](images/iosMainScreen.jpg)|Displays when the next game will start<br><br>Allows the user to log into a game<br><br>Displays their prize winnings
Game screen|![androidGameScreen.jpg](images/androidGameScreen.jpg)|![iosGameScreen.jpg](images/iosGameScreen.jpg)|Where the user answers questions for the game
Information screen|![androidInfoScreen.jpg](images/androidInfoScreen.jpg)|![iosInfoScreen.jpg](images/iosInfoScreen.jpg)|Where the user can send the game to their friends or view contact information for the game owner


## Game Play Instructions

- [Start the Game as a Host](#start-the-game-as-a-host)
- [Play the Game as an Audience Member](#play-the-game-as-an-audience-member)

### Start the Game as a Host

The host has the ability to start / end the game, and set game configuration parameters.

- [Start the Application](#start-the-application)
- [Enable OBS](#enable-obs)
- [Set the Configuration](#set-the-configuration)
- [Start the Game](#start-the-game)
- [Play the Game](#play-the-game)

#### Start the Application

Run the AgoraHQ.exe file. Ensure the green bars are lit in the upper left corner. The green bars indicate a successful network connection.

**Note:** If the upper left corner is red instead of green, check your network connection before proceeding or the application will not run properly.

![Page-1-Image-1.jpg](images/Page-1-Image-1.jpg)


#### Enable OBS

Enabling OBS is optional. This gives users the ability to: 

- Click the OBS image button to load a local picture
- Click the OBS camera button to launch the camera device
- Click the OBS media button to load a local media file
	
**Note:** OBS objects can be loaded one after another. OBS objects can:

- Move positions
- Change front/back relationships by right clicking the objects and selecting move up, move down, etc. 
- Apply a red, blue, or green filter using the select filter dropdown menu

Enabling OBS, requires settings in the demo application and OBS Studio:

- [Open the VirtualCam plugin](#open-the-virtualcam-plugin)
- [Enable OBS in the Demo Application](#enable-obs-in-the-demo-application)
- [Add VirtualCam Filter](#add-virtualcam-filter)


##### Open the VirtualCam plugin

If you don't have OBS Studio or the VirtualCam plugin, download the [software](https://obsproject.com/download) and the [plugin](https://obsproject.com/forum/resources/obs-virtualcam.539/)

In OBS Studio, open the VirtualCam plugin by selecting **Tools** -> **VirtualCam**.

![OBSPage-1-Image-1.jpg](images/OBSPage-1-Image-1.jpg)

Press the **Start** button and close dialog box.

![OBSPage-2-Image-2.jpg](images/OBSPage-2-Image-2.jpg)

##### Enable OBS in the Demo Application
	
1. Check **OBS** checkbox 

	![Page-2-Image-2.jpg](images/Page-2-Image-2.jpg) 

2. Click on **OBS Camera**. This will open the properties window.

	![Page-3-Image-3.jpg](images/Page-3-Image-3.jpg)

3. Choose a device from the **Camera Device** dropdown menu and press **OK**.

	**Note:** The available camera device options will vary, based on your system and hardware connections.

	![1.png](images/1.png)

##### Add VirtualCam Filter

![OBSPage.jpg](images/OBSPage.jpg)

1. Add a filter to the source you want to output to the camera .
2. Choose a sync camera target and press the **Start** button .
3. If this camera is not yet active, the **Start** button will switch to a **Stop** button 
For more information about using OBS Studio, reference the official [OBS Video Guide](https://obsproject.com/forum/resources/full-video-guide-for-obs-studio-and-twitch.377/) or reference the [Agora Github Wiki](https://github.com/AgoraIO/HQ/wiki).


#### Set the Configuration

1. Click on **HQ Config** to open the configuration window.

2. In the configuration window, set the configuration for your application:
	
	- Enter the Agora App ID into the **APPID** UI text field.

	- Select the appropriate region from the **Region** dropdown menu:
		- If the application will be used outside of mainland China select the **Overseas Edition** option.
		- If the application will be used in mainland China, select the **Chinese Edition** option.

		**Note:** After changing the region setting, the signal bar in the upper left corner of the app should turn green, indicating a successful connection. 
		
	- Enter a channel name in the **ChannelName** UI text field. Ensure the channel name is the same as your audience members.

		**Note:** If running the demo application, do not change the channel name.

	- Select a camera from the **CAMERA** dropdown menu. If OBS is enabled, select **OBS-Camera**.

	![Page-5-Image-5.jpg](images/Page-5-Image-5.jpg)

	**Note:** For security reasons, the production environment must enable a Dynamic key or Access Token (a new version of the Dynamic Key). Learn more about enabling it [here](https://docs.agora.io/en/2.3.1/product/Interactive%20Broadcast/Agora%20Platform/key_native?platform=Android).


3. Click **OK** to reboot the application. The application will restart, applying the updated configuration settings.

	![Page-6-Image-6.jpg](images/Page-6-Image-6.jpg)

#### Start the Game

The following figure explains the game flow for the host, and how to hand out questions:

![contest_sendQuestion.jpg](images/contest_sendQuestion.jpg)

The last **Send Question** indicates the host will need to repeat the **Send Question** -> **Start Question** -> **Stop Answer** process again, until all questions are sent to the audience members. The game ends when all questions have been sent and answered.

#### Join the Channel

Click **JoinChannel** to start the game. 

![2d.png](images/2d.png)

This starts the game, and the you will see your local camera view on the device.

![3d.png](images/3d.png)


#### Game Play Buttons

Command|Description
---|---
Send Question|Sends a question (and a set of multiple choice answers)
Start Question|Synchronizes the question with the video, allowing participants to see question on their mobile devices
Stop Answer|Stop audience members for answering the question. If this is pressed on the last question, the game ends. The host will receive the results of the game participants.<ul><li>Question：the sequence number of the current question</li><li>Total: the total number of the participants</li><li>Correct: the number of participants who answered correctly</li><li>Wrong: the number of participants who answered incorrectly</li><li>Answer: the number of correct answers</li><li>A/B/C/D: the number of participants selecting each answer option</li><li>rightPercent: the percentage of correct answers</li></ul>
Reset Question|Resets the game to the beginning. It restarts the answering process without changing the quiz set.


#### Play the Game

1. Press **Send Question** to send the questions to audience members
2. Press **Start Question** to display the question to audience members

**Note:** To ensure synchronization between the questions and the video, click **Start Question** after **Send Question** so all users in the channel will be able to see the questions at the simultaneously.

![3dStart.png](images/3dStart.png)

**Note:** The **Start Question**, and **Stop Answer** buttons are highlighted in blue to indicate the button can be pressed. Buttons are greyed out to indicate the button can not be pressed. Pressed buttons turn grey to prevent selection errors

Press **Stop Answer** to stop audience members from answering the question.

![4d.png](images/4d.png)

Press **Reset Question** at any time, to restart the game.

![4dReset.png](images/4dReset.png)

#### End of Game

At the end of the game, the results will display.

![5d.png](images/5d.png)


### Play the Game as an Audience Member

Start the application on your iOS or Android device. 

![hq_audience.png](images/hq_audience.png)

Enter the same **Channel Name** as [the host](#set-the-configuration). Once you have successfully logged in, you will see the broadcaster’s video.

**Note:** If the host has not started the game in the specified channel, a dialog box will appear, notifying the user that the channel is not found.

### Play the Game

When the host clicks **Start Question** in their application, the audience member will see the question on their mobile device.

Click on an answer to the question. Repeat this process until the host stops sending questions.

![Screenshot_1.png](images/Screenshot_1.png)

During the game, users can chat with other audience members.

![Screenshot_3.png](images/Screenshot_3.png)


### Game End

When the game ends, you will see a screen that shows the prize breakdown. The user can click **START** to play again.

![Screenshot_2.png](images/Screenshot_2.png)


## Resources

- API documentation about the [Interactive Broadcast SDK](https://docs.agora.io/en/2.1.3/product/Interactive%20Broadcast/Solutions/contest?platform=All%20Platforms)
- Complete API documentation is available at the [Document Center](https://docs.agora.io/en/).


## License
This software is under the MIT License (MIT). [View the license](LICENSE.md).
