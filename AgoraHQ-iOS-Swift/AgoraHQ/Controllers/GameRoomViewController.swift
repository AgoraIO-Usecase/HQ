//
//  GameRoomViewController.swift
//  AgoraHQ
//
//  Created by ZhangJi on 08/01/2018.
//  Copyright © 2018 ZhangJi. All rights reserved.
//

import UIKit
import AgoraRtcEngineKit

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
    
    var hostView: UIView?
    var question: String! = "Q1.jdklfasjfdaslk"
    var answers = ["dfsfasdf","dasfadsfas","fdsafasdf","dhaskfh"]//[String]()
//    var s: NSDictionary = ["0": 20, "1": 24, "2": 30, "3": 20]
    var result: NSDictionary!
    
    // AgoraEngine
    var rtcEngine: AgoraRtcEngineKit!
    
    var questionView: QuestionView!
    var timer: Timer!
    let answerTime: Int = 10
    var pastTime: Int = 10
    
    var questionId: Int = -2
    var seiId: Int = -2
    var isAnswering: Bool = false
    
    var messageList = [NSMutableAttributedString]()
    
    let geter = ServerHelper()
    let poster = ServerHelper()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        chatTableView.rowHeight = UITableViewAutomaticDimension
        chatTableView.estimatedRowHeight = 44
        addKeyboardObserver()
        joinChannel()
        joinRCChatRoom()
        addgradientLayer()
    }
    
    // 设置消息view背景渐变
    func addgradientLayer() {
        let gradientLayer = CAGradientLayer()
        gradientLayer.frame = CGRect(x: 0, y: 0, width: ScreenWidth, height: ScreenHeight * 0.35)
        gradientLayer.startPoint = CGPoint(x: 0, y: 1)
        gradientLayer.endPoint = CGPoint(x: 0, y: 0)
        gradientLayer.colors = [UIColor(hex: 0x003366, alpha: 1).cgColor, UIColor(hex: 0x003366, alpha: 0.5).cgColor, UIColor.clear.cgColor]
        gradientLayer.locations = [0.0, 0.5, 1.0]
        
        chatContainerView.layer.addSublayer(gradientLayer)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
    
    @IBAction func doBackPressed(_ sender: UIButton) {
        leaveChannel()
        self.dismiss(animated: true, completion: nil)
    }
    
    @IBAction func doChatButtonPressed(_ sender: UIButton) {
        chatMessgaeTestField.becomeFirstResponder()
        inputContainerView.isHidden = false
    }
    
    // 终极大招：复活 ！！
    @IBAction func doReLiveButtonPressed(_ sender: UIButton) {
        if UserDefaults.standard.bool(forKey: "status") {
            AlertUtil.showAlert(message: "你还活着啊！不要乱点😫")
            return
        }
        
        poster.delegate = self
        let paramDic = ["uid": UserDefaults.standard.string(forKey: "RCUid")! ,
                        "gid": RoomName.RCIMQuestionRoom]
        poster.postAction(to: reliveUrl, with: paramDic)
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
        questionView = QuestionView.newQuestionView(with: qusetin)
        questionView.frame = CGRect(x: 20, y: 30, width: ScreenWidth - 40, height: CGFloat(160 + 50 * answers.count))
        questionView.setAnswers(answers, enable: status)
        questionView.layer.cornerRadius = 30
        questionView.layer.masksToBounds = true
        questionView.backgroundColor = UIColor.white
        
        if !status {
            questionView.answerTimeLabel.text = "不可以答题"
            questionView.answerTimeLabel.textColor = wrongColor
        }
        self.view.addSubview(questionView)

        timer = Timer.scheduledTimer(timeInterval: 1, target: self, selector: #selector(changeTime), userInfo: status, repeats: true)
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
    }
    
    // 检查是否可以答题
    func checkStatus() {
        geter.delegate = self
        let paramDic = ["uid": UserDefaults.standard.string(forKey: "RCUid")! ,
                        "gid": RoomName.RCIMQuestionRoom]
        geter.getAction(to: getStatusUrl, with: paramDic)
    }
}

// MARK: - Agora
extension GameRoomViewController {
    func setIdleTimerActive(_ active: Bool) {
        UIApplication.shared.isIdleTimerDisabled = !active
    }
    
    func joinChannel() {
        rtcEngine = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        rtcEngine.setChannelProfile(.channelProfile_LiveBroadcasting)
        rtcEngine.setClientRole(.clientRole_Audience, withKey: nil)
        rtcEngine.enableVideo()
        
        let key = KeyCenter.AppcertificateID.isEmpty ? nil : DynamicKey.generateMediaChannelKeyby(KeyCenter.AppId, certificate: KeyCenter.AppcertificateID, channelName: RoomName.AgoraChannelName, uid: 0)
        
        let code = rtcEngine.joinChannel(byKey: key, channelName: RoomName.AgoraChannelName, info: nil, uid: 0, joinSuccess: nil)
        
        if code == 0 {
            setIdleTimerActive(false)
        } else {
            AlertUtil.showAlert(message: "Join agora channel failed with error: \(code)")
            print("Join channel failed")
        }
    }
    
    func leaveChannel() {
        setIdleTimerActive(true)
        hostView?.removeFromSuperview()
        rtcEngine.leaveChannel(nil)
        RCIMClient.shared().quitChatRoom(RoomName.RCIMQuestionRoom, success: nil, error: nil)
        RCIMClient.shared().quitChatRoom(RoomName.RCIMChatRoom, success: nil, error: nil)
    }
}

// MARK: - Agora Delegate
extension GameRoomViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        print("=========================join agora channel success")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        let canvas = AgoraRtcVideoCanvas()
        hostView = UIView(frame: CGRect(x: 0, y: 0, width: remoteContainerView.frame.width, height: remoteContainerView.frame.height))
        remoteContainerView.addSubview(hostView!)
        canvas.uid = uid
        canvas.view = hostView
        canvas.renderMode = .render_Hidden
        rtcEngine.setupRemoteVideo(canvas)
        print("===================didJoinedOfUid \(uid)============================")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraRtcUserOfflineReason) {
        hostView?.removeFromSuperview()
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didReceiveSEI sei: String) {
        print("====================\(sei)================")
        let data = sei.data(using: String.Encoding.utf8)
        do {
            let jsonData: NSDictionary = try JSONSerialization.jsonObject(with: data!, options: .mutableContainers) as! NSDictionary
            print(jsonData)
            if jsonData["type"] as! String == "setSEI" {
                let data = jsonData["data"] as! NSDictionary
                self.seiId = data["questionId"] as! Int
                let time = data["timeStamp"] as! Double
                let date = NSDate().timeIntervalSince1970
                
                print("==================延迟\(date) - \(time) = \(date - time)======================")
                if self.questionId == self.seiId {
                    self.addQusetionViewWith(qusetin: question, answers: answers)
                }
            }
        } catch {
            AlertUtil.showAlert(message: error as! String)
            print(error)
        }
    }
}

// MARK: -RCIMCLient
extension GameRoomViewController {
    func joinRCChatRoom() {
        RCIMClient.shared().joinChatRoom(RoomName.RCIMQuestionRoom, messageCount: -1, success: {
            print("======================Join chat room seccess: 10001=====================")
            self.checkStatus()
        }) { (status: RCErrorCode) in
            AlertUtil.showAlert(message: "Join question room error :\(status.rawValue)")
            print("error :\(status.rawValue)")
        }
        RCIMClient.shared().joinChatRoom(RoomName.RCIMChatRoom, messageCount: -1, success: {
            print("======================Join chat room seccess: chatroom=====================")
        }) { (status: RCErrorCode) in
            AlertUtil.showAlert(message: "Join chat room error :\(status.rawValue)")
            print("error :\(status.rawValue)")
        }
        RCIMClient.shared().setReceiveMessageDelegate(self, object: nil)
    }
}

// MARK: -RCIMCLient Delegate
extension GameRoomViewController: RCIMClientReceiveMessageDelegate {
    func onReceived(_ message: RCMessage!, left nLeft: Int32, object: Any!) {
        let testMessage = message.content as! RCTextMessage
        print(testMessage.content)
        let data = testMessage.content.data(using: String.Encoding.utf8)
        do {
            let jsonData: NSDictionary = try JSONSerialization.jsonObject(with: data!, options: .mutableContainers) as! NSDictionary
            if jsonData["type"] as! String == "quiz" {
                let quiz = jsonData["data"] as! NSDictionary
                let sid = quiz["id"] as! Int
                UserDefaults.standard.set(sid, forKey: "sid")
                self.question = quiz["question"] as! String
                self.answers = quiz["options"] as! Array<String>
                self.questionId = sid
            } else if jsonData["type"] as! String == "result" {
                print(jsonData)
                self.result = jsonData["data"] as! NSDictionary
                
                DispatchQueue.main.sync(execute: {
                    showResult(result: self.result)
                })
            } else if jsonData["type"] as! String == "chat" {
                print(jsonData["data"] as! String)
                let chatMsg = (jsonData["name"] as! String) + ": " + (jsonData["data"] as! String)
                let msg = NSMutableAttributedString(string: chatMsg)
                let originalNSString = chatMsg as NSString
                let nameRange = originalNSString.range(of: jsonData["name"] as! String)
                
                msg.addAttribute(NSAttributedStringKey.foregroundColor, value: UIColor.orange, range: nameRange)
                messageList.append(msg)
                DispatchQueue.main.sync(execute: {
                    updateChatView()
                })
            }
        } catch  {
            AlertUtil.showAlert(message: "Receive message error")
            print("Error: \(error)")
        }
        
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
            let status = jsonData["result"] as! Bool
            UserDefaults.standard.set(status, forKey: "status")
            if !status {
                AlertUtil.showAlert(message: "错过了答题时间，下次早点吧")
            } else {
                AlertUtil.showAlert(message: "你可以开始答题了")
            }
        } catch  {
            AlertUtil.showAlert(message: "Connect server error!")
            return
        }
    }
}

extension GameRoomViewController: UITextFieldDelegate {
    // send chat message
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        guard let message = chatMessgaeTestField.text else { return false }
        
        if message.isEmpty {
            AlertUtil.showAlert(message: "请输入点东西吧")
        }
        let messageJson = "{\"type\":\"chat\",\"data\":\"\(message)\",\"name\":\"\(UserDefaults.standard.string(forKey: "name")!)\"}"
        let content = RCTextMessage(content: messageJson)
        RCIMClient.shared().sendMessage(.ConversationType_CHATROOM,
                                        targetId: RoomName.RCIMChatRoom,
                                        content: content,
                                        pushContent: nil,
                                        pushData: nil,
                                        success: { (msgId: Int) in
                                            let chatMsg = UserDefaults.standard.string(forKey: "name")! + ": " + message
                                            let msg = NSMutableAttributedString(string: chatMsg)
                                            let originalNSString = chatMsg as NSString
                                            let nameRange = originalNSString.range(of: UserDefaults.standard.string(forKey: "name")!)
                                            
                                            msg.addAttribute(NSAttributedStringKey.foregroundColor, value: UIColor.orange, range: nameRange)
                                            self.messageList.append(msg)
                                            DispatchQueue.main.sync(execute: {
                                                self.updateChatView()
                                                self.chatMessgaeTestField.text = ""
                                            })
                                            
                                            print("==============message send success=================")
        }) { (error: RCErrorCode, msgId: Int) in
            AlertUtil.showAlert(message: "message send error with ecode: \(error)")
            print("==============message send error with ecode: \(error)=================")
        }
        return true
    }
}

// MARK: - UITableView
extension GameRoomViewController: UITableViewDataSource {
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return messageList.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "ChatCell", for: indexPath) as! ChatCell
        cell.messageLabel.attributedText = messageList[indexPath.row]
        
        return cell
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
            let deltaY = isIPhoneX ? keyBoardBounds.size.height - 32 : keyBoardBounds.size.height
            
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
