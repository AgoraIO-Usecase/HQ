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

let rightColor = UIColor(hex: 0x7ED321, alpha: 0.8)

let wrongColor = UIColor(hex: 0xD0021B, alpha: 0.8)

let selectedColor = UIColor(hex: 0x16B8FD, alpha: 0.8)

let server = NSLocalizedString("http://76.231.168.66:9000", comment: "")

let answerUrl = server + "/v1/answer"

let getStatusUrl = server + "/v1/canplay"

let reliveUrl = server + "/v1/relive"
