//
//  SeverHelper.swift
//  AgoraHQ
//
//  Created by ZhangJi on 10/01/2018.
//  Copyright © 2018 ZhangJi. All rights reserved.
//

import UIKit

protocol ServerHelperDelagate: NSObjectProtocol {
    func serverHelper(_ serverHelper: ServerHelper, data: Data, error: Error?)
}

class ServerHelper: NSObject {
    weak var delegate: ServerHelperDelagate?
    
    func postAction(to url: String, with paramDic: Dictionary<String, Any>) {
        //请求URL
        let url: URL! = URL(string: url)
        var request: URLRequest = URLRequest(url: url)
        let list  = NSMutableArray()

        if paramDic.count > 0 {
            //设置为POST请求
            request.httpMethod = "POST"
            
            // if post json
            
//            request.setValue("application/json", forHTTPHeaderField: "Content-Type")
//            do {
//                let paraData = try JSONSerialization.data(withJSONObject: paramDic, options: .prettyPrinted)
//                request.httpBody = paraData
//                let jsonData: NSDictionary = try JSONSerialization.jsonObject(with: paraData, options: .mutableContainers) as! NSDictionary
//                print(jsonData)
//            } catch {
//                print(error)
//            }
            
            //拆分字典,subDic是其中一项，将key与value变成字符串
            for subDic in paramDic {
                let tmpStr = "\(subDic.0)=\(subDic.1)"
                list.add(tmpStr)
            }
            //用&拼接变成字符串的字典各项
            let paramStr = list.componentsJoined(by: "&")
            //UTF8转码，防止汉字符号引起的非法网址
            let paraData = paramStr.data(using: String.Encoding.utf8)
            //设置请求体
            request.httpBody = paraData
            print("++++++++++++++++++++++++", paramStr)
        }
        
        //默认session配置
        let config = URLSessionConfiguration.default
        let session = URLSession(configuration: config)
        
        //发起请求
        let dataTask = session.dataTask(with: request) { (data, response, error) in
            
            guard let data = data else {
                AlertUtil.showAlert(message: "\(url!) request error")
                return
            }
            if self.delegate != nil {
                self.delegate?.serverHelper(self, data: data, error: error)
            }
        }
        //请求开始
        dataTask.resume()
    }
    
    func getAction(to url: String, with paramDic: Dictionary<String, Any>) {
        var request: URLRequest!
        let list  = NSMutableArray()
        
        if paramDic.count > 0 {
            for subDic in paramDic {
                let tmpStr = "\(subDic.0)=\(subDic.1)"
                list.add(tmpStr)
            }
//            //用&拼接变成字符串的字典各项
            let paramStr = list.componentsJoined(by: "&").replacingOccurrences(of: " ", with: "")
            let url:URL! = URL(string: url + "?" + paramStr)
            request = URLRequest(url: url)
            print(url)
            request.httpMethod = "GET"
        }
        //默认session配置
        let config = URLSessionConfiguration.default
        let session = URLSession(configuration: config)
        //发起请求
        let dataTask = session.dataTask(with: request) { (data, response, error) in

            guard let data = data else {
                AlertUtil.showAlert(message: "\(url) request error")
                return
            }
            if self.delegate != nil {
                self.delegate?.serverHelper(self, data: data, error: error)
            }
        }
        //请求开始
        dataTask.resume()
    }
}
