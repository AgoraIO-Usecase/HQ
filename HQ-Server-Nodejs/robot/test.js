const Signal = require("../modules/sig_agora")
const request = require("request");

if (process.argv.length !== 5) {
    console.error("node server.js <appid> <channel> <server_account>");
    process.exit(1);
}

const appid = process.argv[2];
const channel = process.argv[3];
const server_account = process.argv[4];

function Agolet() {
    let service = this;
    this.uid = "1886000234233856";
    this.token = "653d8112ab1151112a9dda5cb49031ed0dc4208b26a3bb840ddc8ef7c1bc6368";

    this.login = (user, pass) => {
        return new Promise((resolve, reject) => {
            let options = {
                url: "http://agolet.agoralab.co/v1/login",
                method: "POST",
                json: { account: user, password: md5(pass) }
            };
            request(options, function (error, response, loginret) {
                if (response.statusCode === 200) {
                    service.uid = loginret.uid;
                    service.token = loginret.token;
                    resolve();
                } else {
                    reject(error);
                }
            })
        });
    }


    this.message = (channel, msg) => {
        if (!service.uid || !service.token) {
            return Promise.reject("uid or token not exist");
        }
        return new Promise((resolve, reject) => {
            let options = {
                url: "http://agolet.agoralab.co/v1/agobot/message",
                method: "POST",
                json: { channel: channel, body: msg, uid: service.uid, token: service.token }
            };
            request(options, function (error, response) {
                if (response.statusCode === 200) {
                    resolve();
                } else {
                    reject(error);
                }
            })
        });
    }
}


function Test(){
    this.run = () => {
        return new Promise((resolve, reject) => {
            let signal = new Signal(appid);
            let inst_msg = false;
            let chnl_msg = false;
            let timer = setTimeout(() => {
                reject({
                    inst: inst_msg, 
                    chnl: chnl_msg
                });
            }, 1000 * 30);
            signal.setup_debugging('env', 'lbs100');
            let session = signal.login(channel, "_no_need_token");
            session.onLoginSuccess = function () {
                console.log("login successful");
                session.onMessageInstantReceive = (account, uid, msg) => {
                    console.log(`msg received from ${account}: ${msg}`);
                    inst_msg = true;
                    let json = JSON.parse(msg);
                    if(json.type === "channel"){
                        let ch = session.channelJoin(channel);
                        ch.onChannelJoined = function () {
                            console.log(`channel joined`);
    
                            ch.onMessageChannelReceive = (account, uid, msg) => {
                                console.log(`channel msg received ${msg}`)
                                chnl_msg = true;
                                clearTimeout(timer);
                                resolve();
                            };
                            console.log(`start publish`);
                            session.messageInstantSend(server_account, JSON.stringify({
                                type: "publish"
                            }))
                        };
                    }
                };
                console.log(`message sent`);
                session.messageInstantSend(server_account, JSON.stringify({
                    type: "RequestChannelName",
                    QuestionLanguage: "1"
                }))
            }
        });
    }
};

let agolet = new Agolet();
function Runner(){
    new Test().run().then(() => {
        console.log("success");
    }).catch(ret => {
        let inst_msg = ret.inst;
        let chnl_msg = ret.chnl;
        console.log(`failed inst_msg: ${inst_msg} chnl_msg: ${chnl_msg}`);
        agolet.message("HQ", `failed inst_msg: ${inst_msg} chnl_msg: ${chnl_msg}`);
    });
}


Runner();
setInterval(() => {
    Runner();
}, 1000 * 60 * 30);
