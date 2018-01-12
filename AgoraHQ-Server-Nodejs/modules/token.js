const md5 = require("md5");

var SignalingToken = {}

SignalingToken.get = function(appid, appcertificate, account, validDays){
    var expiredTime = parseInt(new Date().getTime() / 1000)+ 3600* 24 *  validDays;
    var token_items = [];

    //append SDK VERSION
    token_items.push("1");

    //append appid
    token_items.push(appid);

    //expired time
    token_items.push(expiredTime);

    //md5 account + appid + appcertificate + expiredtime
    token_items.push(md5(account + appid + appcertificate + expiredTime));

    return token_items.join(":");
}

//convenience function to get token valid within 1 day
SignalingToken.get1DayToken = function(appid, appcertificate, account){
    return SignalingToken.get(appid, appcertificate, account, 1);
}

module.exports = SignalingToken;