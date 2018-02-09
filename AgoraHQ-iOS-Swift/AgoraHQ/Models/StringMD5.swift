//
//  StringMD5.swift
//  AgoraSIG-Test-Demo-iOS
//
//  Created by ZhangJi on 19/09/2017.
//  Copyright © 2017 ZhangJi. All rights reserved.
//

import Foundation

extension Int {
    func hexedString() -> String {
        return NSString(format: "%02x", self) as String
    }
}

extension NSData {
    func hexedSting() -> String {
        var string = String()
        let unsafePointer = bytes.assumingMemoryBound(to: UInt8.self)
        for i in UnsafeBufferPointer<UInt8>(start: unsafePointer, count: length) {
            string += Int(i).hexedString()
        }
        return string
    }
    
    func MD5() -> NSData {
        let result = NSMutableData(length: Int(CC_MD5_DIGEST_LENGTH))!
        let unsafePointer = result.mutableBytes.assumingMemoryBound(to: UInt8.self)
        CC_MD5(bytes, CC_LONG(length), UnsafeMutablePointer<UInt8>(unsafePointer))
        return NSData(data: result as Data)
    }
}

extension String {
    func MD5() -> String {
        let data = (self as NSString).data(using: String.Encoding.utf8.rawValue)! as NSData
        return data.MD5().hexedSting()
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  token 生成方法
//
//  func compTokenBy(appID: String, certificate: String, account: String, expirtdTime: UInt32) -> String {
//      let sign = (account + appID + certificate + String(expirtdTime)).MD5()
//      return ("1:" + appID + ":" + String(expirtdTime) + ":" + sign)
//  }
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
