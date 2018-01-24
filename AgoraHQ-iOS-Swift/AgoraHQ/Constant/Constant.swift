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

let rightColor = UIColor(hex: 0x66ff33, alpha: 0.8)

let wrongColor = UIColor(hex: 0xFF0000, alpha: 0.8)

let selectedColor = UIColor(hex: 0xff6600, alpha: 0.8)

let answerUrl = "http://123.155.153.87:9000/v1/answer"

let getStatusUrl = "http://123.155.153.87:9000/v1/canplay"

let reliveUrl = "http://123.155.153.87:9000/v1/relive"
