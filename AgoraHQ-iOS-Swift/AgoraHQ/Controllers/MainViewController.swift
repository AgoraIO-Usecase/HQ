//
//  MainViewController.swift
//  AgoraHQ
//
//  Created by ZhangJi on 08/01/2018.
//  Copyright © 2018 ZhangJi. All rights reserved.
//

import UIKit

class MainViewController: UIViewController{

    @IBOutlet weak var beginButton: UIButton!
    @IBOutlet weak var uidLabel: UILabel!
    @IBOutlet weak var loginButton: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        RCIMClient.shared().setRCConnectionStatusChangeDelegate(self)
        loadRCIMClient()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
    
    func loadRCIMClient() {
        var uid = UserDefaults.standard.string(forKey: "RCUid")
        let token = UserDefaults.standard.string(forKey: "token")
        var name = UserDefaults.standard.string(forKey: "name")
        if uid == nil {
            uid = UIDevice.current.identifierForVendor?.uuidString
            UserDefaults.standard.set(uid!, forKey: "RCUid")
            name = String(uid![uid!.startIndex..<uid!.index(uid!.startIndex, offsetBy: 6)])
            UserDefaults.standard.set(name, forKey: "name")
        }
        if name == nil {
            name = String(uid![uid!.startIndex..<uid!.index(uid!.startIndex, offsetBy: 6)])
            UserDefaults.standard.set(name, forKey: "name")
        }
        
        if token != nil {
            connectRCIM(withToken: token!)
        } else {
            requestToken()
        }
    }
    
    @IBAction func showIntroduction(_ sender: UIButton) {
        AlertUtil.showAlert(message: NSLocalizedString("Please contact", comment: "title fot introduction") + "\n\n400 632 6626")
    }
    
    @IBAction func doLoginButtonPressed(_ sender: UIButton) {
        loadRCIMClient()
    }
    
    func connectRCIM(withToken token: String) {
        RCIMClient.shared().connect(withToken: token, success: { (uid: String?) in
            print("================join seccess \(uid!)=========")
        } , error: { (status: RCConnectErrorCode) in
            AlertUtil.showAlert(message: "RCloud Login Failed ecode: \(status.rawValue)")
            print("RCloud Login Failed ecode: \(status.rawValue)")
        }) {
            AlertUtil.showAlert(message: "Token Wrong!")
            print("token Wrong")
            self.requestToken()
        }
    }
    
    func requestToken() {
        let poster = ServerHelper()
        poster.delegate = self
        let paramDic = ["uid": UserDefaults.standard.string(forKey: "RCUid")!]
        poster.postAction(to: tokenUrl, with: paramDic)
    }
}

extension MainViewController: RCConnectionStatusChangeDelegate {
    func onConnectionStatusChanged(_ status: RCConnectionStatus) {
        if status.rawValue == 0 {
            self.beginButton.isEnabled = true
            self.uidLabel.text = UserDefaults.standard.string(forKey: "name")!
            self.loginButton.isEnabled = false
        } else {
            self.beginButton.isEnabled = false
            self.uidLabel.text = "Login"
//            self.loginButton.isEnabled = true
        }
    }
}

extension MainViewController: ServerHelperDelagate {
    func serverHelper(_ serverHelper: ServerHelper, data: Data, error: Error?) {
        do {
            let jsonData: NSDictionary = try JSONSerialization.jsonObject(with: data, options: .mutableContainers) as! NSDictionary
            if let err = jsonData["err"] {
                AlertUtil.showAlert(message: err as! String)
                self.loginButton.isEnabled = true
                print("===================Request token failed with: \(err)=======================")
                return
            }
            let token: String = jsonData["token"] as! String
            UserDefaults.standard.set(token, forKey: "token")
            connectRCIM(withToken: token)
        } catch  {
            AlertUtil.showAlert(message: "Request token error!")
        }
    }
}
