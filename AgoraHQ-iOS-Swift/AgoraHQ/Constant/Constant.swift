//
//  Constant.swift
//  AgoraHQ
//
//  Created by ZhangJi on 08/01/2018.
//  Copyright © 2018 ZhangJi. All rights reserved.
//

import UIKit

/// 屏幕的宽
let ScreenWidth = UIScreen.main.bounds.size.width
/// 屏幕的高
let ScreenHeight = UIScreen.main.bounds.size.height

/// iphone4s
let isIPhone4 = ScreenHeight == 480 ? true : false
/// iPhone 5
let isIPhone5 = ScreenHeight == 568 ? true : false
/// iPhone 6
let isIPhone6 = ScreenHeight == 667 ? true : false
/// iPhone 6P
let isIPhone6P = ScreenHeight == 736 ? true : false

let isIPhoneX = ScreenHeight == 812 ? true : false

let rightColor = UIColor(hex: 0x97D24A, alpha: 1)

let wrongColor = UIColor(hex: 0xC42A29, alpha: 1)

let selectedColor = UIColor(hex: 0x56C4F8, alpha: 1)

let buttonColor = UIColor(hex: 0xE9E9EC, alpha: 1)

let server = NSLocalizedString("http://52.8.94.243:10000", comment: "")

let answerUrl = server + "/v1/answer"

let getStatusUrl = server + "/v1/canplay"

let reliveUrl = server + "/v1/relive"

let inviteResponseUrl = server + "/v1/inviteResponse"

let joinUrl = server + "/v1/join"

let winHostUid: UInt = 1

let backgroundView: UIImageView = {
    let imageView = UIImageView(frame: CGRect(x: 0, y: 0, width: ScreenWidth, height: ScreenHeight))
    imageView.image = #imageLiteral(resourceName: "background")
    let flowerView = UIImageView(frame: CGRect(x: 10, y: 55, width: ScreenWidth - 20, height: 170))
    flowerView.image = #imageLiteral(resourceName: "flower")
    imageView.addSubview(flowerView)
    return imageView
}()
