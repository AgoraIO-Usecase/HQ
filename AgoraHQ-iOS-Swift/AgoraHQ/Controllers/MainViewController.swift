//
//  MainViewController.swift
//  AgoraHQ
//
//  Created by ZhangJi on 08/01/2018.
//  Copyright © 2018 ZhangJi. All rights reserved.
//

import UIKit
import AgoraHQSigKit

class MainViewController: UIViewController{

    @IBOutlet weak var beginButton: UIButton!
    @IBOutlet weak var uidLabel: UILabel!
    @IBOutlet weak var loginButton: UIButton!
    @IBOutlet weak var channelNameTextField: UITextField!
    var userStatus = false
    var agoraHQSigKit : AgoraHQSigKit!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        checkUserStatus()
    }
    
    override func viewWillAppear(_ animated: Bool) {
        if agoraHQSigKit == nil {
            agoraHQSigKit = AgoraHQSigKit(appId: KeyCenter.AppId)
            
            // 测试环境需要调用下面API设置测试服务器，如需上线不需要调用，请联系商务开通生产环境
            agoraHQSigKit.dbg("lbss", b: "125.88.159.173")
        }
        agoraHQSigKit?.delegate = self
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        view.endEditing(true)
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        guard let segueId = segue.identifier else {
            return
        }
        
        switch segueId {
        case "startgame":
            let GameRoomVC = segue.destination as! GameRoomViewController
            GameRoomVC.channelName = self.channelNameTextField.text
            GameRoomVC.agoraHQSigKit = self.agoraHQSigKit
        default:
            return
        }
    }
    
    func checkUserStatus() {
        var account = UserDefaults.standard.string(forKey: "account")
        var name = UserDefaults.standard.string(forKey: "name")
        if account == nil {
            account = UIDevice.current.identifierForVendor?.uuidString
            UserDefaults.standard.set(account!, forKey: "account")
            name = String(account![account!.startIndex..<account!.index(account!.startIndex, offsetBy: 6)])
            UserDefaults.standard.set(name, forKey: "name")
        }
        if name == nil {
            name = String(account![account!.startIndex..<account!.index(account!.startIndex, offsetBy: 6)])
            UserDefaults.standard.set(name, forKey: "name")
        }
        userStatus = true
        self.uidLabel.text = name!
    }
    
    func compTokenBy(appID: String, certificate: String, account: String, expirtdTime: UInt32) -> String {
        let sign = (account + appID + certificate + String(expirtdTime)).MD5()
        return ("1:" + appID + ":" + String(expirtdTime) + ":" + sign)
    }

    @IBAction func doStartButtonPressed(_ sender: UIButton) {
        let time = NSDate().timeIntervalSince1970 + 3600
        let token = KeyCenter.AppcertificateID.isEmpty ? "_no_need_token" : compTokenBy(appID: KeyCenter.AppId, certificate: KeyCenter.AppcertificateID, account: UserDefaults.standard.string(forKey: "account")!, expirtdTime: UInt32(time))
        agoraHQSigKit.login(UserDefaults.standard.string(forKey: "account")!, token: token, channel: self.channelNameTextField.text)
    }
    
    @IBAction func showIntroduction(_ sender: UIButton) {
        AlertUtil.showAlert(message: NSLocalizedString("Please contact", comment: "title fot introduction") + "\n\n400 632 6626")
    }
    
    @IBAction func doLoginButtonPressed(_ sender: UIButton) {
        checkUserStatus()
    }
}

// MARK: - AgoraHQSigDelegate
extension MainViewController: AgoraHQSigDelegate{
    func agoraHQSigDidLoginSuccess(_ agoraHQSig: AgoraHQSigKit!) {
        print("=========================join agora sig channel success===============")
        
        self.performSegue(withIdentifier: "startgame", sender: self)
    }
    
    func agoraHQSig(_ agoraHQSi: AgoraHQSigKit!, didOccurError error: Error!) {
        AlertUtil.showAlert(message: "Agora Sig error: \(error!)")
    }
}

extension MainViewController: UITextFieldDelegate {
    func textField(_ textField: UITextField, shouldChangeCharactersIn range: NSRange, replacementString string: String) -> Bool {
        if !string.isEmpty && userStatus {
            self.beginButton.isEnabled = true
        } else {
            self.beginButton.isEnabled = false
        }
        return true
    }
}
