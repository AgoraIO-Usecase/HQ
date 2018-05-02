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
    @IBOutlet weak var profileImageView: UIImageView!
    @IBOutlet weak var backCardView: UIView!
    @IBOutlet weak var setButton: UIButton!
    var userStatus = false
    var agoraHQSigKit : AgoraHQSigKit!
    var backgroundLayer : CAGradientLayer?
    var modifyView : UIView?
    var inputTextField : UITextField?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        checkUserStatus()
    }
    
    override func viewWillAppear(_ animated: Bool) {
        if agoraHQSigKit == nil {
            agoraHQSigKit = AgoraHQSigKit(appId: KeyCenter.AppId)
            
            // 测试环境需要调用下面API设置测试服务器，如需上线请联系商务开通生产环境
            agoraHQSigKit.dbg("lbs_100", b: "1")
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
        let profilePhoto = UserDefaults.standard.data(forKey: "ProfilePhoto")
        profileImageView.image = profilePhoto == nil ? #imageLiteral(resourceName: "user_main") : UIImage(data: profilePhoto!)
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
        let token = KeyCenter.AppcertificateID.isEmpty ? "_no_need_token" : compTokenBy(appID: KeyCenter.AppId, certificate: KeyCenter.AppcertificateID, account: UserDefaults.standard.string(forKey: "name")!, expirtdTime: UInt32(time))
        agoraHQSigKit.login(UserDefaults.standard.string(forKey: "name")!, token: token, channel: self.channelNameTextField.text)
        print("==================================", UserDefaults.standard.string(forKey: "name")!)
    }
    
    @IBAction func doLoginButtonPressed(_ sender: UIButton) {
        checkUserStatus()
    }
    
    @IBAction func doSetButtonPressed(_ sender: UIButton) {
        let setSheet = UIAlertController(title: nil, message: nil, preferredStyle: .actionSheet)
        
        // support ipad
        setSheet.popoverPresentationController?.sourceView = self.view
        setSheet.popoverPresentationController?.sourceRect = CGRect(x: setButton.frame.maxX, y: backCardView.frame.minY + setButton.frame.minY, width: setButton.frame.width, height: setButton.frame.height)
        let nameAction = UIAlertAction(title: NSLocalizedString("Modify User Name", comment: "Modify User Name"), style: .default) { (_) in
            self.backgroundLayer = CAGradientLayer()
            self.backgroundLayer?.frame = self.view.bounds
            self.backgroundLayer?.backgroundColor = UIColor(hex: 0x000000, alpha: 0.4).cgColor
            
            self.view.layer.addSublayer(self.backgroundLayer!)
            
            self.modifyView = UIView(frame: CGRect(x: self.view.center.x - 120, y: self.view.center.y - 100, width: 240, height: 110))
            self.modifyView?.layer.cornerRadius = 10
            self.modifyView?.layer.masksToBounds = true
            self.modifyView?.backgroundColor = UIColor.white
            
            self.inputTextField = UITextField(frame: CGRect(x: 10, y: 10, width: 220, height: 40))
            self.inputTextField?.layer.cornerRadius = 10
            self.inputTextField?.layer.masksToBounds = true
            self.inputTextField?.layer.borderWidth = 1
            self.inputTextField?.layer.borderColor = UIColor.lightGray.cgColor
            self.inputTextField?.textAlignment = .center
            
            let cancelButton = UIButton(frame: CGRect(x: 10, y: 60, width: 100, height: 40))
            cancelButton.addTarget(self, action: #selector(self.moveModifyView(_:)), for: .touchUpInside)
            cancelButton.setTitle(NSLocalizedString("Cancel", comment: "Cancel"), for: .normal)
            cancelButton.setTitleColor(UIColor(hex: 0x3366FF, alpha: 1), for: .normal)
            cancelButton.layer.cornerRadius = 20
            cancelButton.layer.masksToBounds = true
            cancelButton.layer.borderWidth = 1
            cancelButton.layer.borderColor = UIColor.lightGray.cgColor
            
            let saveButton = UIButton(frame: CGRect(x: 130, y: 60, width: 100, height: 40))
            saveButton.setTitle(NSLocalizedString("Save", comment: "Save"), for: .normal)
            saveButton.setTitleColor(UIColor(hex: 0x3366FF, alpha: 1), for: .normal)
            saveButton.layer.cornerRadius = 20
            saveButton.layer.masksToBounds = true
            saveButton.layer.borderWidth = 1
            saveButton.layer.borderColor = UIColor.lightGray.cgColor
            
            saveButton.addTarget(self, action: #selector(self.setUserName(_:)), for: .touchUpInside)
            
            self.modifyView?.addSubview(cancelButton)
            self.modifyView?.addSubview(saveButton)
            self.modifyView?.addSubview(self.inputTextField!)
            self.view.addSubview((self.modifyView)!)
            self.inputTextField?.becomeFirstResponder()
        }
        let cameraAction = UIAlertAction(title: NSLocalizedString("Take Photo", comment: "Take Photo"), style: .default) { (_) in
            
            guard UIImagePickerController.isSourceTypeAvailable(.camera) else {
                print("相机不可以")
                return
            }
            
            let picker = UIImagePickerController()
            picker.delegate = self
            picker.allowsEditing = false
            picker.sourceType = .camera
            
            self.present(picker, animated: true, completion: nil)
        }
        let photoAction = UIAlertAction(title: NSLocalizedString("Choose from Album", comment: "Choose from Album"), style: .default) { (_) in
            guard UIImagePickerController.isSourceTypeAvailable(.photoLibrary) else {
                print("相册不可以")
                return
            }
            
            let picker = UIImagePickerController()
            picker.delegate = self
            picker.allowsEditing = false
            picker.sourceType = .photoLibrary
            
            self.present(picker, animated: true, completion: nil)
        }
        let cancelAction = UIAlertAction(title: NSLocalizedString("Cancel", comment: "Cancel"), style: .cancel, handler: nil)
        
        setSheet.addAction(cameraAction)
        setSheet.addAction(photoAction)
        setSheet.addAction(nameAction)
        setSheet.addAction(cancelAction)
        
        present(setSheet, animated: true, completion: nil)
    }
    
    @objc func moveModifyView(_ button: UIButton) {
        backgroundLayer?.removeFromSuperlayer()
        modifyView?.removeFromSuperview()
    }
    
    @objc func setUserName(_ button: UIButton) {
        guard let name = inputTextField?.text else { return }
        if name.isEmpty { return }
        self.uidLabel.text = name
        UserDefaults.standard.set(name, forKey: "name")
        backgroundLayer?.removeFromSuperlayer()
        modifyView?.removeFromSuperview()
    }
}

// MARK: - AgoraHQSigDelegate
extension MainViewController: AgoraHQSigDelegate{
    func agoraHQSigDidLoginSuccess(_ agoraHQSig: AgoraHQSigKit!) {
        print("=========================join agora sig channel success===============")
        
        self.performSegue(withIdentifier: "startgame", sender: self)
    }
    
    func agoraHQSig(_ agoraHQSi: AgoraHQSigKit!, didOccurError error: Error!) {
        AlertUtil.showAlert(message: NSLocalizedString("Login failed, please try again", comment: "login failed"))
    }
}

extension MainViewController: UITextFieldDelegate {
    
    func textField(_ textField: UITextField, shouldChangeCharactersIn range: NSRange, replacementString string: String) -> Bool {
        if textField != self.channelNameTextField { return true }
        if textField.text!.isEmpty && !string.isEmpty {
            self.beginButton.isEnabled = true
        } else if textField.text!.count == 1 && string.isEmpty {
            self.beginButton.isEnabled = false
        }
        if string == " " {
            return false
        }
        return true
    }
}

extension MainViewController: UIImagePickerControllerDelegate, UINavigationControllerDelegate {
    func imagePickerController(_ picker: UIImagePickerController, didFinishPickingMediaWithInfo info: [String : Any]) {
        let profilePhoto = info[UIImagePickerControllerOriginalImage] as? UIImage
        profileImageView.image = profilePhoto
        let imageData = UIImageJPEGRepresentation(profilePhoto!, 0.7) as Data?
        UserDefaults.standard.set(imageData, forKey: "ProfilePhoto")
        
        dismiss(animated: true, completion: nil)
    }
}
