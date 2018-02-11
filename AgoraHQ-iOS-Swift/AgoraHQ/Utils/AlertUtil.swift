//
//  AlertUtil.swift
//  AgoraHQ
//
//  Created by ZhangJi on 09/01/2018.
//  Copyright © 2018 ZhangJi. All rights reserved.
//

import UIKit

class AlertUtil: NSObject {
    static func showAlert(message: String) {
        let alert = UIAlertController(title: nil, message: message, preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "OK", style: .cancel, handler: nil))
        
        let rootVC = UIApplication.shared.keyWindow?.rootViewController
        let topVC = rootVC?.presentedViewController != nil ? rootVC?.presentedViewController : rootVC
        
        DispatchQueue.main.async {
            topVC?.present(alert, animated: true, completion: nil)
        }
    }
}
