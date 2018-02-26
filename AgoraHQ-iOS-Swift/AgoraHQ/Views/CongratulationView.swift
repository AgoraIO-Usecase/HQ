//
//  CongratulationView.swift
//  AgoraHQ
//
//  Created by ZhangJi on 2018/2/24.
//  Copyright © 2018 ZhangJi. All rights reserved.
//

import UIKit

class CongratulationView: UIView {

    @IBOutlet weak var championImageView: UIImageView!
    @IBOutlet weak var profileImageView: UIImageView!
    @IBOutlet weak var nameLabel: UILabel!
    @IBOutlet weak var containView: UIView!
    
    override init(frame: CGRect) {
        super.init(frame: frame)
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    static func newCongratulationView(with profileImage: UIImage, name: String) -> UIView? {
        let nibView = Bundle.main.loadNibNamed("CongratulationView", owner: nil, options: nil)
        if let view = nibView?.first as? CongratulationView {
            view.profileImageView.image = profileImage
            view.nameLabel.text = name
            return view
        }
        return nil
    }
    
    @IBAction func doCancelButtonPressed(_ sender: UIButton) {
        self.removeFromSuperview()
    }
}
