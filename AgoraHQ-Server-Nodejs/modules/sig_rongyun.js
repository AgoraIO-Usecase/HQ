const rongcloudSDK = require("rongcloud-sdk");
const config = require("./config");
rongcloudSDK.init(config.ry_appid, config.ry_appcert);
module.exports = rongcloudSDK;