//
//  QuestionView.swift
//  AgoraHQ
//
//  Created by ZhangJi on 08/01/2018.
//  Copyright © 2018 ZhangJi. All rights reserved.
//

import UIKit

struct Answer {
    var type: Bool!
    var ratio: Double!
}

class QuestionView: UIView {

    @IBOutlet weak var questionLable: UILabel!
    @IBOutlet weak var answerTimeLabel: UILabel!
    
    var channelName: String!
    var buttons = [UIButton]()
    var answerViews = [UIView]()
    var myAnswer: Int = -1
    
    static func newQuestionView(with question: String) -> QuestionView? {
        let nibView = Bundle.main.loadNibNamed("QuestionView", owner: nil, options: nil)
        if let view = nibView?.first as? QuestionView {
            let id = UserDefaults.standard.integer(forKey: "sid") + 1
            view.questionLable.text = "\(id) . " + question
            return view
        }
        return nil
    }
    
    override init(frame: CGRect) {
        super.init(frame: frame)
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    func setAnswers(_ answers: Array<String>, answerHeights: Array<CGFloat>, enable: Bool) {
        var height: CGFloat = answerHeights[0]
        for (index, answer) in answers.enumerated() {
            let answerView = UIView(frame: CGRect(x: 30.0, y: height, width: CGFloat(self.frame.width - 60), height: answerHeights[index + 1]))
            let answerButton = UIButton(frame: CGRect(x: 20, y: 0, width: answerView.frame.width - 40, height: answerView.frame.height))
            answerView.layer.cornerRadius = 20
            answerView.layer.masksToBounds = true
            answerView.layer.borderWidth = 1
            answerView.layer.borderColor = UIColor.lightGray.cgColor
            answerButton.setTitle(answer, for: .normal)
            answerButton.contentHorizontalAlignment = .left
            answerButton.setTitleColor(UIColor.black, for: .normal)
            answerButton.titleLabel?.numberOfLines = 0
//            answerButton.titleLabel?.adjustsFontSizeToFitWidth = true
            answerButton.isEnabled = enable
            answerButton.tag = index
            answerButton.addTarget(self, action: #selector(sendAnswer(_:)), for: .touchUpInside)
            answerView.addSubview(answerButton)
            buttons.append(answerButton)
            answerViews.append(answerView)
            self.addSubview(answerView)
            height += answerHeights[index + 1] + 10
        }
    }
    
    @objc func sendAnswer(_ button: UIButton) {
        print((button.titleLabel?.text)!)
        button.superview?.backgroundColor = selectedColor
        let poster = ServerHelper()
        let paramDic = ["uid": UserDefaults.standard.string(forKey: "account")!,
                        "gid": channelName!,
                        "sid": Int(UserDefaults.standard.string(forKey: "sid")!)!,
                        "result": button.tag] as [String : Any]
        myAnswer = button.tag
        print(paramDic)
        poster.delegate = self
        poster.postAction(to: answerUrl, with: paramDic)
        
        for button in buttons {
            button.isEnabled = false
        }
    }
    
    func setBackground(color: UIColor, of view: UIView, with ratio: Double) {
        let backView = UIView(frame: CGRect(x: 0, y: 0, width: Double(view.frame.width) * ratio, height: Double(view.frame.height)))
        backView.layer.cornerRadius = 20
        backView.layer.masksToBounds = true
        backView.backgroundColor = color
        view.addSubview(backView)
        view.insertSubview(backView, at: 0)
        
        view.backgroundColor = UIColor.clear
    }
    
    func resetViewWithRightAnswers(_ result: NSDictionary) {
        let status = UserDefaults.standard.bool(forKey: "status")
        if status {
            let myResult = result["result"] as! Int == myAnswer
            let ratio = result["total"] as!Int == 0 ? 0.0 : (result["correct"] as! Double) / (result["total"] as! Double)
            self.answerTimeLabel.text = myResult ? NSLocalizedString("Bingo", comment: "right answer") : NSLocalizedString("Wrong", comment: "wrong answer")
            self.answerTimeLabel.textColor = myResult ? rightColor : wrongColor
            setBackground(color: rightColor, of: answerViews[result["result"] as! Int], with: ratio)
            answerViews[result["result"] as! Int].layer.borderColor = UIColor.lightGray.cgColor
            UserDefaults.standard.set(myResult, forKey: "status")
        } else {
            self.answerTimeLabel.text = NSLocalizedString("Can not play", comment: "title for cannot play")
            self.answerTimeLabel.textColor = wrongColor
            let ratio = result["total"] as!Int == 0 ? 0.0 : (result["correct"] as! Double) / (result["total"] as! Double)
            setBackground(color: rightColor, of: answerViews[result["result"] as! Int], with: ratio)
            answerViews[result["result"] as! Int].layer.borderColor = UIColor.lightGray.cgColor
        }
        let spreads = result["spread"] as! NSDictionary

        for i in 0..<spreads.count {
            if i != result["result"] as! Int {
                let ratio = result["total"] as!Int == 0 ? 0.0 : (spreads["\(i)"] as! Double) / (result["total"] as! Double)
                setBackground(color: wrongColor, of: answerViews[i], with: ratio)
            }
        }
        for button in buttons {
            button.isEnabled = false
        }
    }
}

extension QuestionView: ServerHelperDelagate {
    func serverHelper(_ serverHelper: ServerHelper, data: Data, error: Error?)  {
        do {
            let jsonData:NSDictionary = try JSONSerialization.jsonObject(with: data, options: .mutableContainers) as! NSDictionary
            print(jsonData)
            if let err = jsonData["err"] {
                AlertUtil.showAlert(message: err as! String)
                print("===================send answer failed with: \(err)=======================")
                return
            }
        } catch {
            AlertUtil.showAlert(message: "Connect server error!")
        }
    }
}
