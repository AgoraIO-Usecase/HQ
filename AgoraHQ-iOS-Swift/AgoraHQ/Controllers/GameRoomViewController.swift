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
    
    var channelName: String!
    var hostView: UIView?
    var question: String!
    var answers = [String]()
    var result: NSDictionary!
    
    // AgoraEngine
    var rtcEngine: AgoraRtcEngineKit!
    var agoraHQSigKit: AgoraHQSigKit!
    
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
    
    var msgId = 0
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        if agoraHQSigKit == nil {
            agoraHQSigKit = AgoraHQSigKit(appId: KeyCenter.AppId)
        }
        agoraHQSigKit.delegate = self
        
        chatTableView.rowHeight = UITableViewAutomaticDimension
        chatTableView.estimatedRowHeight = 44
        addTouchEventToTableView(chatTableView)
        addKeyboardObserver()
        joinMediaChannel()
        addgradientLayer()
        checkStatus()
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
        let paramDic = ["uid": UserDefaults.standard.string(forKey: "account")! ,
                        "gid": channelName!]
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
        var answerHeights = [CGFloat]()
        var allHeight: CGFloat = 0
        let height = getLabHeigh(labelStr: question, width: ScreenWidth - 140) + CGFloat(80)
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
        questionView.layer.cornerRadius = 30
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
    }
    
    // 检查是否可以答题
    func checkStatus() {
        geter.delegate = self
        let paramDic = ["uid": UserDefaults.standard.string(forKey: "account")! ,
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
        rtcEngine = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        rtcEngine.setChannelProfile(.channelProfile_LiveBroadcasting)
        rtcEngine.setClientRole(.clientRole_Audience, withKey: nil)
        rtcEngine.enableVideo()
        
        let key = KeyCenter.AppcertificateID.isEmpty ? nil : DynamicKey.generateMediaChannelKeyby(KeyCenter.AppId, certificate: KeyCenter.AppcertificateID, channelName: channelName, uid: 0)
        
        let code = rtcEngine.joinChannel(byKey: key, channelName: channelName, info: nil, uid: 0, joinSuccess: nil)
        
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
        agoraHQSigKit.logout()
    }
}

// MARK: - Agora Delegate
extension GameRoomViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        print("=========================join agora media channel success===============")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        let canvas = AgoraRtcVideoCanvas()
        hostView = UIView(frame: CGRect(x: 0, y: 0, width: remoteContainerView.frame.width, height: remoteContainerView.frame.height))
        remoteContainerView.addSubview(hostView!)
        canvas.uid = uid
        canvas.view = hostView
        canvas.renderMode = .render_Hidden
        rtcEngine.setupRemoteVideo(canvas)
        print("===================did Joined Of Uid \(uid)============================")
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
                let quiz = jsonData["data"] as! NSDictionary
                let sid = quiz["id"] as! Int
                UserDefaults.standard.set(sid, forKey: "sid")
                self.question = quiz["question"] as! String
                self.answers = quiz["options"] as! Array<String>
                self.questionId = sid
            } else if jsonData["type"] as! String == "result" {
                print(jsonData)
                self.result = jsonData["data"] as! NSDictionary
                showResult(result: self.result)
            } else if jsonData["type"] as! String == "chat" {
                print(jsonData["data"] as! String)
                let chatMsg = (jsonData["name"] as! String) + ": " + (jsonData["data"] as! String)
                let msg = NSMutableAttributedString(string: chatMsg)
                let originalNSString = chatMsg as NSString
                let nameRange = originalNSString.range(of: jsonData["name"] as! String)
                
                msg.addAttribute(NSAttributedStringKey.foregroundColor, value: UIColor.orange, range: nameRange)
                messageList.append(msg)
                updateChatView()
            }
        } catch  {
            AlertUtil.showAlert(message: "Receive message error")
            print("Error: \(error)")
        }
    }
    
    func agoraHQSig(_ agoraHQSig: AgoraHQSigKit!, didReceivedMessageFromAccount account: String!, message: String!, messageId: Int64) {
        
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
        let msg = NSMutableAttributedString(string: chatMsg)
        let originalNSString = chatMsg as NSString
        let nameRange = originalNSString.range(of: UserDefaults.standard.string(forKey: "name")!)
        
        msg.addAttribute(NSAttributedStringKey.foregroundColor, value: UIColor.orange, range: nameRange)
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
        return messageList.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "ChatCell", for: indexPath) as! ChatCell
        cell.messageLabel.attributedText = messageList[indexPath.row]
        
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
