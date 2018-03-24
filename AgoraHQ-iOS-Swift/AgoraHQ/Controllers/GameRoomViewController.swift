//
//  GameRoomViewController.swift
//  AgoraHQ
//
//  Created by ZhangJi on 08/01/2018.
//  Copyright © 2018 ZhangJi. All rights reserved.
//

import UIKit
import AgoraRtcEngineKit
import AgoraHQSigKit

struct Message {
    let name: String!
    let content: NSMutableAttributedString!
}

class GameRoomViewController: UIViewController {

    @IBOutlet weak var chatTableView: UITableView!
    @IBOutlet weak var backgroundImageView: UIImageView!
    @IBOutlet weak var chatMessgaeTestField: UITextField!
    @IBOutlet weak var inputContainerView: UIView!
    @IBOutlet weak var inputContainerConstraint: NSLayoutConstraint!
    @IBOutlet weak var chatTableViewConstraint: NSLayoutConstraint!
    @IBOutlet weak var chatContainerConstraint: NSLayoutConstraint!
    @IBOutlet weak var remoteContainerView: UIView!
    @IBOutlet weak var chatContainerView: UIView!
    @IBOutlet weak var cheatChannelTableView: UITableView!
    @IBOutlet weak var cheatButton: UIButton!
    @IBOutlet weak var disconnectButton: UIButton!
    @IBOutlet weak var chatButton: UIButton!
    
    var channelName: String!
    var hostView: UIView?
    var hostUid: UInt?
    var invitedView: UIView?
    var invitedUid: UInt?
    var myUid: UInt?
    var question: String!
    var answers = [String]()
    var result: NSDictionary!
    var totalNum: UInt = 12
    
    var backgroundLayer : CAGradientLayer?
    
    var cheatContainView : UIView?
    var cheatChannelTextField : UITextField?
    var cheatChannelUsers = [UInt]() {
        didSet {
            cheatChannelTableView.reloadData()
        }
    }
    
    var isInCheatChannel = false {
        didSet {
            cheatChannelTableView.isHidden = !isInCheatChannel
            cheatButton.setTitle(isInCheatChannel ? NSLocalizedString("Leave", comment: "Leave Cheat") : NSLocalizedString("Team mode", comment: "In Cheat"), for: .normal)
        }
    }
    
    var isInConnection = false {
        didSet {
            if isInConnection {
                cheatButton.isEnabled = false
                disconnectButton.isHidden = false
            } else {
                cheatButton.isEnabled = true
                disconnectButton.isHidden = true
            }
        }
    }
    
    // AgoraEngine
    var rtcEngine: AgoraRtcEngineKit!
    var rtcCheatEngine: AgoraRtcEngineKit?
    var agoraHQSigKit: AgoraHQSigKit!
    
    var questionView: QuestionView!
    var timer: Timer!
    var answerTime: Int = 10
    var pastTime: Int = 10
    
    var questionId: Int = -2
    var seiId: Int = -2
    var isAnswering: Bool = false
    
    var messageList = [Message]()
    
    let geter = ServerHelper()
    let poster = ServerHelper()
    var inviteResponsePoster: ServerHelper?
    
    var msgId = 0
    
    // audio set var
    var audioSetTimer: Timer?
    var audioSetTime: Int = 0
    var audioSetted = false
    
    // decryt
    var decryptKey: String!
    var decryptIv: String = "00000000000agora"
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        if agoraHQSigKit == nil {
            agoraHQSigKit = AgoraHQSigKit(appId: KeyCenter.AppId)
        }
        if isIPhoneX {
            backgroundImageView.image = #imageLiteral(resourceName: "background_x")
        }
        agoraHQSigKit.delegate = self
        
        decryptKey = channelName
        
        cheatButton.titleLabel?.adjustsFontSizeToFitWidth = true
        disconnectButton.titleLabel?.adjustsFontSizeToFitWidth = true
        
        chatTableView.rowHeight = UITableViewAutomaticDimension
        chatTableView.estimatedRowHeight = 44
        addTouchEventToTableView(chatTableView)
        addKeyboardObserver()
        joinMediaChannel()
        checkStatus()
    }
    
    // 设置消息view背景渐变
    func addgradientLayer() {
        let gradientLayer = CAGradientLayer()
        gradientLayer.frame = CGRect(x: 0, y: isIPhoneX ? 32 : 0, width: ScreenWidth, height: ScreenHeight * 0.35)
        gradientLayer.startPoint = CGPoint(x: 0, y: 1)
        gradientLayer.endPoint = CGPoint(x: 0, y: 0)
        gradientLayer.colors = [UIColor(hex: 0x003366, alpha: 1).cgColor, UIColor(hex: 0x003366, alpha: 0.5).cgColor, UIColor.clear.cgColor]
        gradientLayer.locations = [0.0, 0.5, 1.0]
        
        chatContainerView.layer.addSublayer(gradientLayer)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
    
    deinit {
        print("===============deinit===================")
    }
    
    @IBAction func doBackPressed(_ sender: UIButton) {
        leaveChannel()
        self.dismiss(animated: true, completion: nil)
    }
    
    @IBAction func doChatButtonPressed(_ sender: UIButton) {
        view.bringSubview(toFront: inputContainerView)
        chatMessgaeTestField.becomeFirstResponder()
        inputContainerView.isHidden = false
    }
    
    // 终极大招：复活 ！！
    @IBAction func doReLiveButtonPressed(_ sender: UIButton) {
        if UserDefaults.standard.bool(forKey: "status") {
            AlertUtil.showAlert(message: NSLocalizedString("You are still alive", comment: "title for relive"))
            return
        }
        
        poster.delegate = self
        let paramDic = ["uid": UserDefaults.standard.string(forKey: "name")! ,
                        "gid": channelName!]
        poster.postAction(to: reliveUrl, with: paramDic)
    }
    
    @IBAction func doCheatButtonPressed(_ sender: UIButton) {
        if isInCheatChannel {
            rtcCheatEngine?.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":false, \"bitrate\":0}}")
            rtcCheatEngine?.leaveChannel(nil)
            return
        }
        isAnswering = true
        self.backgroundLayer = CAGradientLayer()
        self.backgroundLayer?.frame = self.view.bounds
        self.backgroundLayer?.backgroundColor = UIColor(hex: 0x000000, alpha: 0.4).cgColor
        
        self.view.layer.addSublayer(self.backgroundLayer!)
        self.cheatContainView = UIView(frame: CGRect(x: self.view.center.x - 130, y: self.view.center.y - 120, width: 260, height: 130))
        self.cheatContainView?.backgroundColor = UIColor.clear
        
        let cheatView = UIView(frame: CGRect(x: 0, y: 20, width: 260, height: 110))
        cheatView.layer.cornerRadius = 10
        cheatView.layer.masksToBounds = true
        cheatView.backgroundColor = UIColor.white
        
        self.cheatChannelTextField = UITextField(frame: CGRect(x: 10, y: 10, width: 240, height: 40))
        self.cheatChannelTextField?.layer.cornerRadius = 10
        self.cheatChannelTextField?.layer.masksToBounds = true
        self.cheatChannelTextField?.layer.borderWidth = 1
        self.cheatChannelTextField?.layer.borderColor = UIColor.lightGray.cgColor
        self.cheatChannelTextField?.textAlignment = .center
        
        let cancelButton = UIButton(frame: CGRect(x: 240, y: 0, width: 20, height: 20))
        cancelButton.tag = 1001
        cancelButton.setImage(#imageLiteral(resourceName: "cancel"), for: .normal)
        cancelButton.addTarget(self, action: #selector(joinOrCreateRoom(_:)), for: .touchUpInside)
        
        let createButton = UIButton(frame: CGRect(x: 10, y: 60, width: 110, height: 40))
        createButton.setTitle(NSLocalizedString("Create Room", comment: "Create Room"), for: .normal)
        createButton.titleLabel?.font = UIFont.systemFont(ofSize: 14)
        createButton.setTitleColor(UIColor(hex: 0x3366FF, alpha: 1), for: .normal)
        createButton.layer.cornerRadius = 20
        createButton.layer.masksToBounds = true
        createButton.layer.borderWidth = 1
        createButton.layer.borderColor = UIColor.lightGray.cgColor
        createButton.tag = 1002
        createButton.addTarget(self, action: #selector(joinOrCreateRoom(_:)), for: .touchUpInside)
        
        let joinButton = UIButton(frame: CGRect(x: 140, y: 60, width: 110, height: 40))
        joinButton.setTitle(NSLocalizedString("Join Room", comment: "Join Room"), for: .normal)
        joinButton.titleLabel?.font = UIFont.systemFont(ofSize: 14)
        joinButton.setTitleColor(UIColor(hex: 0x3366FF, alpha: 1), for: .normal)
        joinButton.layer.cornerRadius = 20
        joinButton.layer.masksToBounds = true
        joinButton.layer.borderWidth = 1
        joinButton.layer.borderColor = UIColor.lightGray.cgColor
        joinButton.tag = 1003
        joinButton.addTarget(self, action: #selector(joinOrCreateRoom(_:)), for: .touchUpInside)
        
        cheatView.addSubview(createButton)
        cheatView.addSubview(joinButton)
        cheatView.addSubview(self.cheatChannelTextField!)
        self.cheatContainView?.addSubview(cheatView)
        self.cheatContainView?.addSubview(cancelButton)
        self.view.addSubview((self.cheatContainView)!)
        self.cheatChannelTextField?.becomeFirstResponder()
    }
    
    @objc func joinOrCreateRoom(_ button: UIButton) {
        guard let cheatRoomName = self.cheatChannelTextField?.text else { return }
        if cheatRoomName.isEmpty && button.tag != 1001 {
            return
        }
        switch button.tag {
        case 1002:
            rtcCheatEngine = AgoraRtcEngineKit.createRtcEngine(withAppId: KeyCenter.AppId, delegate: self)
            rtcCheatEngine?.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":true, \"bitrate\":50}}")
            rtcCheatEngine?.joinChannel(byKey: nil, channelName: self.channelName + "_" + cheatRoomName, info: nil, uid: myUid!, joinSuccess: nil)
        case 1003:
            rtcCheatEngine = AgoraRtcEngineKit.createRtcEngine(withAppId: KeyCenter.AppId, delegate: self)
            rtcCheatEngine?.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":true, \"bitrate\":50}}")
            rtcCheatEngine?.joinChannel(byKey: nil, channelName: self.channelName + "_" + cheatRoomName, info: nil, uid: myUid!, joinSuccess: nil)
        default:
            break
        }
        self.cheatContainView?.removeFromSuperview()
        self.backgroundLayer?.removeFromSuperlayer()
        isAnswering = false
    }
    
    @IBAction func doDisconnectButtonPressed(_ sender: UIButton) {
        isInConnection = false
        invitedUid = nil
        rtcEngine.muteLocalVideoStream(true)
        rtcEngine.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":false, \"bitrate\":0}}")
        invitedView?.removeFromSuperview()
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        view.endEditing(true)
        inputContainerView.isHidden = true
    }
    
    // 设置提卡
    func addQusetionViewWith(qusetin: String, answers: Array<String>) {
        pastTime = answerTime
        if isAnswering {
            questionView.removeFromSuperview()
            timer?.invalidate()
        }
        isAnswering = true
        let status = UserDefaults.standard.bool(forKey: "status")
        var answerHeights = [CGFloat]()
        var allHeight: CGFloat = 0
        let height = getLabHeigh(labelStr: question, width: ScreenWidth - 140) + CGFloat(100)
        answerHeights.append(height)
        allHeight += height
        for answer in answers {
            let height = getLabHeigh(labelStr: answer, width: ScreenWidth - 140) + CGFloat(20)
            answerHeights.append(height)
            allHeight += height
        }
        questionView = QuestionView.newQuestionView(with: qusetin)
        questionView.channelName = self.channelName
        questionView.frame = CGRect(x: 20, y: 30, width: ScreenWidth - 40, height: CGFloat(10 * answers.count + 20) + allHeight)
        questionView.setAnswers(answers, answerHeights: answerHeights, enable: status)
        questionView.layer.cornerRadius = 15
        questionView.layer.masksToBounds = true
        questionView.backgroundColor = UIColor.white
        
        if !status {
            questionView.answerTimeLabel.text = NSLocalizedString("Can not play", comment: "title for cannot play")
            questionView.answerTimeLabel.textColor = wrongColor
        }
        self.view.addSubview(questionView)

        timer = Timer.scheduledTimer(timeInterval: 1, target: self, selector: #selector(changeTime), userInfo: status, repeats: true)
    }
    
    func getLabHeigh(labelStr:String, width:CGFloat) -> CGFloat {
        
        let size = labelStr.boundingRect(with: CGSize(width: width, height: 8000), options: .usesLineFragmentOrigin, attributes: [NSAttributedStringKey.font: UIFont.systemFont(ofSize: 17)], context: nil)

        return size.height
    }
    
    @objc func changeTime() {
        let status = timer.userInfo as! Bool
        if pastTime > 0 {
            if status {
               questionView.answerTimeLabel.text = String(pastTime)
            }
            pastTime -= 1
        } else {
            questionView.answerTimeLabel.text = String(pastTime)
            pastTime = answerTime
            questionView.isHidden = true
            isAnswering = false
            timer?.invalidate()
        }
    }
    
    // 显示答题结果
    func showResult(result: NSDictionary) {
        timer?.invalidate()
        if questionView == nil {
            return
        }
        questionView.resetViewWithRightAnswers(result)
        isAnswering = true
        questionView.isHidden = false
        timer = Timer.scheduledTimer(timeInterval: 5, target: self, selector: #selector(remove), userInfo: nil, repeats: false)
    }
    
    @objc func remove() {
        questionView.removeFromSuperview()
        isAnswering = false
        timer.invalidate()
        
        let id = self.seiId + 1
        if id == totalNum && UserDefaults.standard.bool(forKey: "status") {
            showCongralationView()
        }
    }
    
    func showCongralationView() {
        let profilePhoto = UserDefaults.standard.data(forKey: "ProfilePhoto")
        let name = UserDefaults.standard.string(forKey: "name")
        let image = profilePhoto == nil ? #imageLiteral(resourceName: "user_main") : UIImage(data: profilePhoto!)
        let cview = CongratulationView.newCongratulationView(with: image!, name: name!) as? CongratulationView
        cview?.frame = self.view.frame
        //        cview?.frame = CGRect(x: 50, y: 100, width: ScreenWidth - 100, height: ScreenWidth - 100)
        
        view.addSubview(cview!)
        cview?.containView.transform = CGAffineTransform(scaleX: 0, y: 0)
        UIView.animate(withDuration: 1, delay: 0, usingSpringWithDamping: 0.5, initialSpringVelocity: 0.3, options: [], animations: {
            cview?.containView.transform = CGAffineTransform.identity
        }, completion: nil)
    }
    
    // 检查是否可以答题
    func checkStatus() {
        geter.delegate = self
        let paramDic = ["uid": UserDefaults.standard.string(forKey: "name")! ,
                        "gid": channelName!]
        print(paramDic)
        geter.getAction(to: getStatusUrl, with: paramDic)
    }
}

// MARK: - Agora
extension GameRoomViewController {
    func setIdleTimerActive(_ active: Bool) {
        UIApplication.shared.isIdleTimerDisabled = !active
    }
    
    func joinMediaChannel() {
        rtcEngine = AgoraRtcEngineKit.createRtcEngine(withAppId: KeyCenter.AppId, delegate: self)
        rtcEngine.setChannelProfile(.channelProfile_LiveBroadcasting)
        rtcEngine.setClientRole(.clientRole_Broadcaster, withKey: nil)
        rtcEngine.setVideoProfile(._VideoProfile_360P, swapWidthAndHeight: true)
        rtcEngine.enableVideo()
        rtcEngine.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":false, \"bitrate\":0}}")
        
        let key = KeyCenter.AppcertificateID.isEmpty ? nil : DynamicKey.generateMediaChannelKeyby(KeyCenter.AppId, certificate: KeyCenter.AppcertificateID, channelName: channelName, uid: 0)
        
        let code = rtcEngine.joinChannel(byKey: key, channelName: channelName, info: nil, uid: 0, joinSuccess: nil)
        rtcEngine.enableAudioVolumeIndication(1000, smooth: 3)
        
        if code == 0 {
            setIdleTimerActive(false)
        } else {
            AlertUtil.showAlert(message: "Join agora channel failed with error: \(code)")
            print("Join channel failed")
        }
    }
    
    func joinSigChannel() {
        agoraHQSigKit = AgoraHQSigKit(appId: KeyCenter.AppId)
        agoraHQSigKit?.delegate = self
        
    }
    
    func leaveChannel() {
        setIdleTimerActive(true)
        hostView?.removeFromSuperview()
        rtcEngine.leaveChannel(nil)
        if rtcCheatEngine != nil {
            rtcCheatEngine?.leaveChannel(nil)
        }
        agoraHQSigKit.logout()
    }
}

// MARK: - Agora Delegate
extension GameRoomViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        rtcCheatEngine?.setEnableSpeakerphone(true)
        myUid = uid
        if engine == rtcCheatEngine {
            cheatChannelUsers.append(uid)
            isInCheatChannel = true
        }
        print("=========================join agora media channel: \(channel) success===============")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        if engine == rtcCheatEngine {
            cheatChannelUsers.append(uid)
        }

        print("===================did Joined Of Uid \(uid)============================")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, firstRemoteVideoDecodedOfUid uid: UInt, size: CGSize, elapsed: Int) {
        if uid == winHostUid {
            let canvas = AgoraRtcVideoCanvas()
            hostView = UIView(frame: CGRect(x: 0, y: 0, width: remoteContainerView.frame.width, height: isIPhoneX ? remoteContainerView.frame.height + 34 : remoteContainerView.frame.height))
            remoteContainerView.addSubview(hostView!)
            if invitedView != nil {
                remoteContainerView.bringSubview(toFront: invitedView!)
            }
            hostUid = uid
            canvas.uid = uid
            canvas.view = hostView
            canvas.renderMode = .render_Hidden
            rtcEngine.setupRemoteVideo(canvas)
        } else {
            if invitedUid != nil {
                invitedView?.removeFromSuperview()
                invitedUid = nil
            }
            let canvas = AgoraRtcVideoCanvas()
            invitedView = UIView()
            invitedView?.translatesAutoresizingMaskIntoConstraints = false
            invitedView?.layer.cornerRadius = 10
            invitedView?.layer.masksToBounds = true
            remoteContainerView.addSubview(invitedView!)
            invitedUid = uid
            canvas.uid = uid
            canvas.view = invitedView
            canvas.renderMode = .render_Hidden
            rtcEngine.setupRemoteVideo(canvas)
            
            self.view.addConstraints([
                                      NSLayoutConstraint.init(item: self.invitedView!, attribute: .right, relatedBy: .equal, toItem: self.remoteContainerView, attribute: .right, multiplier: 1, constant: -20),
                                      NSLayoutConstraint.init(item: self.invitedView!, attribute: .bottom, relatedBy: .equal, toItem: self.remoteContainerView, attribute: .bottom, multiplier: 1, constant: -70),
                                      NSLayoutConstraint(item: self.invitedView!, attribute: .width, relatedBy: .equal, toItem: nil, attribute: .width, multiplier: 1, constant: ScreenWidth * 0.35),
                                      NSLayoutConstraint(item: self.invitedView!, attribute: .height, relatedBy: .equal, toItem: nil, attribute: .height, multiplier: 1, constant: ScreenWidth * 0.35 * 4.0 / 3.0)
                                     ])
        }
        
        print("===================First Remote Video Decoded Of Uid \(uid)============================")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didLeaveChannelWith stats: AgoraRtcStats) {
        if engine == rtcCheatEngine {
            isInCheatChannel = false
            cheatChannelUsers.removeAll()
        }
        
        print("===================Did Leave Channel============================")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didReceiveSEI sei: String) {
        print("====================SEI Received \(sei)================")
        let data = sei.data(using: String.Encoding.utf8)
        do {
            let jsonData: NSDictionary = try JSONSerialization.jsonObject(with: data!, options: .mutableContainers) as! NSDictionary
            print(jsonData)
            if jsonData["type"] as! String == "setSEI" {
                let data = jsonData["data"] as! NSDictionary
                self.seiId = data["questionId"] as! Int
                let time = data["timeStamp"] as! Double
                let date = NSDate().timeIntervalSince1970
                
                print("==================delay \(date) - \(time) = \(date - time)======================")
                if self.questionId == self.seiId {
                    self.addQusetionViewWith(qusetin: question, answers: answers)
                }
            }
        } catch {
            AlertUtil.showAlert(message: error as! String)
            print(error)
        }
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportAudioVolumeIndicationOfSpeakers speakers: [AgoraRtcAudioVolumeInfo], totalVolume: Int) {
        if engine == rtcCheatEngine {
           return
        }
        var count = 0
        for speaker in speakers {
            if speaker.uid == hostUid || speaker.uid == invitedUid || speaker.uid == 0 {
                count += 1
            }
        }
        if speakers.count - count > 0 && !audioSetted {
            audioSetted = true
            if audioSetTimer != nil {
                audioSetTimer?.invalidate()
                audioSetTime = 0
            }
            if self.hostUid != nil {
                self.rtcEngine.setParameters("{\"che.audio.playout.uid.volume\":{\"uid\":\(self.hostUid!),\"volume\":30}}")
            }
            if self.invitedUid != nil {
                self.rtcEngine.setParameters("{\"che.audio.playout.uid.volume\":{\"uid\":\(self.invitedUid!),\"volume\":30}}")
            }
        } else if speakers.count - count == 0 && audioSetted {
            if #available(iOS 10.0, *) {
                audioSetTimer = Timer.scheduledTimer(withTimeInterval: 0.1, repeats: true, block: { (_) in
                    self.audioSetTime += 1
                    self.audioSetted = false
                    if self.hostUid != nil {
                        self.rtcEngine.setParameters("{\"che.audio.playout.uid.volume\":{\"uid\":\(self.hostUid!),\"volume\":\(30 + 10 * self.audioSetTime)}}")
                    }
                    if self.invitedUid != nil {
                        self.rtcEngine.setParameters("{\"che.audio.playout.uid.volume\":{\"uid\":\(self.invitedUid!),\"volume\":\(30 + 10 * self.audioSetTime)}}")
                    }
                    if self.audioSetTime >= 7 {
                        self.audioSetTime = 0
                        self.audioSetTimer?.invalidate()
                    }
                })
            } else {
                audioSetTimer = Timer.scheduledTimer(timeInterval: 0.1, target: true, selector: #selector(updateVolume), userInfo: nil, repeats: true)
            }
        }
    }
    
    @objc func updateVolume() {
        self.audioSetTime += 1
        self.audioSetted = false
        if self.hostUid != nil {
            self.rtcEngine.setParameters("{\"che.audio.playout.uid.volume\":{\"uid\":\(self.hostUid!),\"volume\":\(30 + 10 * self.audioSetTime)}}")
        }
        if self.invitedUid != nil {
            self.rtcEngine.setParameters("{\"che.audio.playout.uid.volume\":{\"uid\":\(self.invitedUid!),\"volume\":\(30 + 10 * self.audioSetTime)}}")
        }
        if self.audioSetTime >= 7 {
            self.audioSetTime = 0
            self.audioSetTimer?.invalidate()
        }
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraRtcUserOfflineReason) {
        if engine == rtcCheatEngine {
            for (index, id) in cheatChannelUsers.enumerated() {
                if id == uid {
                    cheatChannelUsers.remove(at: index)
                    return
                }
            }
        }
        if uid == hostUid {
            hostView?.removeFromSuperview()
            hostUid = nil
        }
        if uid == invitedUid {
            invitedView?.removeFromSuperview()
            invitedUid = nil
        }
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didVideoMuted muted: Bool, byUid uid: UInt) {
        if uid == invitedUid && muted{
            invitedView?.removeFromSuperview()
            invitedUid = nil
        } else if !muted {
            if invitedUid != nil {
                invitedView?.removeFromSuperview()
                invitedUid = nil
            }
            let canvas = AgoraRtcVideoCanvas()
            invitedView = UIView()
            invitedView?.translatesAutoresizingMaskIntoConstraints = false
            invitedView?.layer.cornerRadius = 10
            invitedView?.layer.masksToBounds = true
            remoteContainerView.addSubview(invitedView!)
            invitedUid = uid
            canvas.uid = uid
            canvas.view = invitedView
            canvas.renderMode = .render_Hidden
            rtcEngine.setupRemoteVideo(canvas)
            
            self.view.addConstraints([
                NSLayoutConstraint.init(item: self.invitedView!, attribute: .right, relatedBy: .equal, toItem: self.remoteContainerView, attribute: .right, multiplier: 1, constant: -20),
                NSLayoutConstraint.init(item: self.invitedView!, attribute: .bottom, relatedBy: .equal, toItem: self.remoteContainerView, attribute: .bottom, multiplier: 1, constant: -70),
                NSLayoutConstraint(item: self.invitedView!, attribute: .width, relatedBy: .equal, toItem: nil, attribute: .width, multiplier: 1, constant: ScreenWidth * 0.35),
                NSLayoutConstraint(item: self.invitedView!, attribute: .height, relatedBy: .equal, toItem: nil, attribute: .height, multiplier: 1, constant: ScreenWidth * 0.35 * 4.0 / 3.0)
                ])
        }
    }
}

// MARK: - AgoraHQSigDelegate
extension GameRoomViewController: AgoraHQSigDelegate{
    func agoraHQSigDidLoginSuccess(_ agoraHQSig: AgoraHQSigKit!) {
        print("=========================join agora sig channel success===============")
    }
    
    func agoraHQSig(_ agoraHQSi: AgoraHQSigKit!, didOccurError error: Error!) {
        AlertUtil.showAlert(message: "Agora Sig error: \(error!)")
    }
    
    func agoraHQSig(_ agoraHQSig: AgoraHQSigKit!, didReceivedChannelMessage channel: String!, message: String!, messageId: Int64) {
        print("=============",message!,"===============")
        let data = message.data(using: String.Encoding.utf8)
        do {
            let jsonData: NSDictionary = try JSONSerialization.jsonObject(with: data!, options: .mutableContainers) as! NSDictionary
            if jsonData["type"] as! String == "quiz" {
                var quiz = NSDictionary()
                let encryptType = jsonData["encrypt"] as? String
                if let type = encryptType {
                    switch type {
                    case "v1":
                        guard let data = Data.init(base64Encoded: jsonData["data"] as! String, options: Data.Base64DecodingOptions(rawValue: 0)) else {
                            print("================= Base64 Decoding failed ==============")
                            return
                        }
                        let cypher = SymmetricCryptor(algorithm: .aes_128, options: kCCOptionPKCS7Padding, iv: decryptIv)
                        if decryptKey.count < 16 {
                            for _ in 0 ..< 16 - decryptKey.count {
                                decryptKey.insert("0", at: decryptKey.startIndex)
                            }
                        } else if decryptKey.count > 16 {
                            decryptKey.replaceSubrange(decryptKey.startIndex..<decryptKey.index(decryptKey.startIndex, offsetBy: decryptKey.count - 16), with: "")
                        }
                        let quizData = try cypher.decrypt(data, key: decryptKey)
                        let quizeString = String(data: quizData, encoding: String.Encoding.utf8)
                        print(quizeString ?? "error")
                        quiz = try JSONSerialization.jsonObject(with: quizData, options: .mutableContainers) as! NSDictionary
                    default:
                        quiz = jsonData["data"] as! NSDictionary
                    }
                } else {
                    quiz = jsonData["data"] as! NSDictionary
                }
                let sid = quiz["id"] as! Int
                UserDefaults.standard.set(sid, forKey: "sid")
                self.question = quiz["question"] as! String
                self.answers = quiz["options"] as! Array<String>
                self.questionId = sid
                self.answerTime = quiz["timeout"] as! Int
                self.totalNum = quiz["total"] as! UInt
            } else if jsonData["type"] as! String == "result" {
                print(jsonData)
                self.result = jsonData["data"] as! NSDictionary
                showResult(result: self.result)
            } else if jsonData["type"] as! String == "chat" {
                print(jsonData["data"] as! String)
                let chatMsg = (jsonData["name"] as! String) + ": " + (jsonData["data"] as! String)
                let msgContent = NSMutableAttributedString(string: chatMsg)
                let originalNSString = chatMsg as NSString
                let nameRange = originalNSString.range(of: jsonData["name"] as! String)
                
                msgContent.addAttribute(NSAttributedStringKey.foregroundColor, value: UIColor.orange, range: nameRange)
                let msg = Message(name: jsonData["name"] as! String, content: msgContent)
                messageList.append(msg)
                updateChatView()
            }
        } catch  {
            AlertUtil.showAlert(message: "Receive message error: \(error)")
            print("Error: \(error)")
        }
    }
    
    func agoraHQSig(_ agoraHQSig: AgoraHQSigKit!, didReceivedMessageFromAccount account: String!, message: String!, messageId: Int64) {
        print("=============",message!,"===============")
        let data = message.data(using: String.Encoding.utf8)
        do {
            let jsonData: NSDictionary = try JSONSerialization.jsonObject(with: data!, options: .mutableContainers) as! NSDictionary
            if jsonData["type"] as! String == "inviteRequest" {
                if isInCheatChannel {
                    if self.inviteResponsePoster == nil {
                        self.inviteResponsePoster = ServerHelper()
                    }
                    let paramDic = ["accept": false,
                                    "account": UserDefaults.standard.string(forKey: "name")!,
                                    "mediaUid": myUid!,
                                    "gid": channelName] as [String : Any]
                    self.inviteResponsePoster?.postAction(to: inviteResponseUrl, with: paramDic)
                    
                    AlertUtil.showAlert(message: NSLocalizedString("Host invite you to connect, But your are still in team mode!", comment: "can not connect"))
                    return
                }
                let requestController = UIAlertController(title: nil, message: NSLocalizedString("Host invite you to connect", comment: "invite request"), preferredStyle: .alert)
                let acceptAction = UIAlertAction(title: NSLocalizedString("Accept", comment: "Accept"), style: .default, handler: { (_) in
                    // send message
                    if self.inviteResponsePoster == nil {
                        self.inviteResponsePoster = ServerHelper()
                    }
                    if self.invitedView != nil {
                        self.invitedView?.removeFromSuperview()
                    }
                    let paramDic = ["accept": true,
                                    "account": UserDefaults.standard.string(forKey: "name")!,
                                    "mediaUid": self.myUid!,
                                    "gid": self.channelName] as [String : Any]
                    self.inviteResponsePoster?.postAction(to: inviteResponseUrl, with: paramDic)
                    
                    self.rtcEngine.setParameters("{\"rtc.hq_mode\": {\"hq\": true, \"broadcaster\":true, \"bitrate\":400}}")
                    let canvas = AgoraRtcVideoCanvas()
                    self.invitedUid = self.myUid
                    self.invitedView = UIView()
                    self.invitedView?.translatesAutoresizingMaskIntoConstraints = false
                    self.invitedView?.layer.cornerRadius = 10
                    self.invitedView?.layer.masksToBounds = true
                    canvas.uid = self.myUid!
                    canvas.view = self.invitedView
                    canvas.renderMode = .render_Hidden
                    self.rtcEngine.setupLocalVideo(canvas)
                    self.isInConnection = true
                    self.rtcEngine.muteLocalVideoStream(false)
                    self.remoteContainerView.addSubview(self.invitedView!)
                    self.view.addConstraints([
                        NSLayoutConstraint.init(item: self.invitedView!, attribute: .right, relatedBy: .equal, toItem: self.remoteContainerView, attribute: .right, multiplier: 1, constant: -20),
                        NSLayoutConstraint.init(item: self.invitedView!, attribute: .bottom, relatedBy: .equal, toItem: self.remoteContainerView, attribute: .bottom, multiplier: 1, constant: -70),
                        NSLayoutConstraint(item: self.invitedView!, attribute: .width, relatedBy: .equal, toItem: nil, attribute: .width, multiplier: 1, constant: ScreenWidth * 0.35),
                        NSLayoutConstraint(item: self.invitedView!, attribute: .height, relatedBy: .equal, toItem: nil, attribute: .height, multiplier: 1, constant: ScreenWidth * 0.35 * 4.0 / 3.0)
                        ])
                })
                let rejectAction = UIAlertAction(title: NSLocalizedString("Reject", comment: "Reject"), style: .cancel, handler: { (_) in
                    if self.inviteResponsePoster == nil {
                        self.inviteResponsePoster = ServerHelper()
                    }
                    let paramDic = ["accept": false,
                                    "account": UserDefaults.standard.string(forKey: "name")!,
                                    "mediaUid": self.myUid!,
                                    "gid": self.channelName] as [String : Any]
                    self.inviteResponsePoster?.postAction(to: inviteResponseUrl, with: paramDic)
                })
                
                requestController.addAction(acceptAction)
                requestController.addAction(rejectAction)
                
                present(requestController, animated: true, completion: nil)
                
            } else if jsonData["type"] as! String == "inviteEnd" {
                
                self.doDisconnectButtonPressed(self.disconnectButton)
                
            }
        } catch  {
            AlertUtil.showAlert(message: "Receive message error")
            print("Error: \(error)")
        }
    }
}

extension GameRoomViewController: ServerHelperDelagate {
    func serverHelper(_ serverHelper: ServerHelper, data: Data, error: Error?) {
        if serverHelper == poster {
            self.checkStatus()
            return
        }
        do {
            let jsonData: NSDictionary = try JSONSerialization.jsonObject(with: data, options: .mutableContainers) as! NSDictionary
            print(jsonData)
            if let err = jsonData["err"] {
                AlertUtil.showAlert(message: err as! String)
                UserDefaults.standard.set(false, forKey: "status")
                print("===================Check status failed with: \(err)=======================")
                return
            }
            let status = jsonData["result"] as! Bool
            UserDefaults.standard.set(status, forKey: "status")
            if !status {
                AlertUtil.showAlert(message: NSLocalizedString("The game is going, cannot play", comment: "title fot game going"))
            } else {
                AlertUtil.showAlert(message: NSLocalizedString("You can play now", comment: "title for relive"))
            }
        } catch  {
            AlertUtil.showAlert(message: "Connect server error!")
            return
        }
    }
}

// MARK: - UITextFieldDelegate
extension GameRoomViewController: UITextFieldDelegate {
    // send chat message
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        guard let message = chatMessgaeTestField.text else { return false }

        let messageJson = "{\"type\":\"chat\",\"data\":\"\(message)\",\"name\":\"\(UserDefaults.standard.string(forKey: "name")!)\"}"
        agoraHQSigKit.sendChannelMessage(messageJson, messageId: Int64(self.msgId))
        msgId += 1
        
        let chatMsg = UserDefaults.standard.string(forKey: "name")! + ": " + message
        let msgContent = NSMutableAttributedString(string: chatMsg)
        let originalNSString = chatMsg as NSString
        let nameRange = originalNSString.range(of: UserDefaults.standard.string(forKey: "name")!)
        
        msgContent.addAttribute(NSAttributedStringKey.foregroundColor, value: UIColor.orange, range: nameRange)
        let msg = Message(name: UserDefaults.standard.string(forKey: "name")!, content: msgContent)
        self.messageList.append(msg)
        self.updateChatView()
        self.chatMessgaeTestField.text = ""
        
        print("==============message send success \(messageJson)=================")

        return true
    }
}

// MARK: - UITableView
extension GameRoomViewController: UITableViewDataSource {
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return tableView == chatTableView ? messageList.count : cheatChannelUsers.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        if tableView == cheatChannelTableView {
            let cell = tableView.dequeueReusableCell(withIdentifier: "UidCell", for: indexPath) as! UidCell
            cell.uidLabel.text = String(cheatChannelUsers[indexPath.row])
            cell.uidLabel.adjustsFontSizeToFitWidth = true
            if cheatChannelUsers[indexPath.row] == myUid {
                cell.uidImageView.image = #imageLiteral(resourceName: "user_cheat_host")
            } else {
                cell.uidImageView.image = #imageLiteral(resourceName: "user_cheat")
            }
            return cell
        }
        let cell = tableView.dequeueReusableCell(withIdentifier: "ChatCell", for: indexPath) as! ChatCell
        cell.messageLabel.attributedText = messageList[indexPath.row].content
        if messageList[indexPath.row].name == UserDefaults.standard.string(forKey: "name")!
        {
            let profilePhoto = UserDefaults.standard.data(forKey: "ProfilePhoto")
            cell.userImageView.image = profilePhoto == nil ? #imageLiteral(resourceName: "user_chat") : UIImage(data: profilePhoto!)
        } else {
            cell.userImageView.image = #imageLiteral(resourceName: "user_chat")
        }
        
        return cell
    }
    
    func updateChatView() {
        guard let tableView = chatTableView else {
            return
        }
        
        tableView.beginUpdates()
        if messageList.count > 100 {
            messageList.removeFirst()
            tableView.deleteRows(at: [IndexPath(row: 0, section: 0)], with: .none)
        }
        let insertIndexPath = IndexPath(row: messageList.count - 1, section: 0)
        tableView.insertRows(at: [insertIndexPath], with: .none)
        tableView.endUpdates()
        
        tableView.scrollToRow(at: insertIndexPath, at: .bottom, animated: false)
    }
    
    func addTouchEventToTableView(_ tableView: UITableView) {
        let tableViewGesture = UITapGestureRecognizer(target: self, action: #selector(tableViewTouchInSide))
        tableViewGesture.numberOfTapsRequired = 1
        tableViewGesture.cancelsTouchesInView = false
        tableView.addGestureRecognizer(tableViewGesture)
    }
    
    @objc func tableViewTouchInSide() {
        self.chatMessgaeTestField.resignFirstResponder()
        inputContainerView.isHidden = true
    }
}

private extension GameRoomViewController {
    func addKeyboardObserver() {
        NotificationCenter.default.addObserver(forName: NSNotification.Name.UIKeyboardWillShow, object: nil, queue: nil) { [weak self] notify in
            guard let strongSelf = self, let userInfo = (notify as NSNotification).userInfo,
                let keyBoardBoundsValue = userInfo[UIKeyboardFrameEndUserInfoKey] as? NSValue,
                let durationValue = userInfo[UIKeyboardAnimationDurationUserInfoKey] as? NSNumber else {
                    return
            }
            
            let keyBoardBounds = keyBoardBoundsValue.cgRectValue
            let duration = durationValue.doubleValue
            let deltaY = isIPhoneX ? keyBoardBounds.size.height - 34 : keyBoardBounds.size.height
            
            if duration > 0 {
                var optionsInt: UInt = 0
                if let optionsValue = userInfo[UIKeyboardAnimationCurveUserInfoKey] as? NSNumber {
                    optionsInt = optionsValue.uintValue
                }
                let options = UIViewAnimationOptions(rawValue: optionsInt)
                
                UIView.animate(withDuration: duration, delay: 0, options: options, animations: {
                    strongSelf.inputContainerConstraint.constant = deltaY
                    strongSelf.chatTableViewConstraint.constant = (self?.isAnswering)! ? 0 : deltaY + 40
                    strongSelf.chatContainerConstraint.constant = (self?.isAnswering)! ? 0 : deltaY + 40
                    strongSelf.view?.layoutIfNeeded()
                }, completion: nil)
                
            } else {
                strongSelf.inputContainerConstraint.constant = deltaY
                strongSelf.chatTableViewConstraint.constant = (self?.isAnswering)! ? 0 : deltaY + 40
                strongSelf.chatContainerConstraint.constant = (self?.isAnswering)! ? 0 : deltaY + 40
            }
        }
        
        NotificationCenter.default.addObserver(forName: NSNotification.Name.UIKeyboardWillHide, object: nil, queue: nil) { [weak self] notify in
            guard let strongSelf = self else {
                return
            }
            
            let duration: Double
            if let userInfo = (notify as NSNotification).userInfo, let durationValue = userInfo[UIKeyboardAnimationDurationUserInfoKey] as? NSNumber {
                duration = durationValue.doubleValue
            } else {
                duration = 0
            }
            
            if duration > 0 {
                var optionsInt: UInt = 0
                if let userInfo = (notify as NSNotification).userInfo, let optionsValue = userInfo[UIKeyboardAnimationCurveUserInfoKey] as? NSNumber {
                    optionsInt = optionsValue.uintValue
                }
                let options = UIViewAnimationOptions(rawValue: optionsInt)
                
                UIView.animate(withDuration: duration, delay: 0, options: options, animations: {
                    strongSelf.inputContainerConstraint.constant = 0
                    strongSelf.chatTableViewConstraint.constant = 0
                    strongSelf.chatContainerConstraint.constant = 0
                    strongSelf.view?.layoutIfNeeded()
                }, completion: nil)
                
            } else {
                strongSelf.inputContainerConstraint.constant = 0
                strongSelf.chatTableViewConstraint.constant = 0
                strongSelf.chatContainerConstraint.constant = 0
            }
        }
    }
}
