const rongcloudSDK = require("./sig_rongyun");
const Signal = require("./sig_agora");
const logger = require("./logger").get("hq");
const config = require("./config");
const sig_appid = config.agora_appid;
const mc_id = config.mc_id;

let HQ = {};

function parseResult(err, resultText) {
    if (err) {
        logger.error(`request failed: ${err}`);
    } else {
        let result = JSON.parse(resultText);
        logger.info(`request: ${resultText}`);
        if (result.code === 200) {
            return result;
        }
    }
    return null;
}

HQ.GameMaker = function () {
    let server = this;
    this.__games = [];

    /*------------------------------------------------
    |   class : Game
    \*----------------------------------------------*/
    HQ.Game = function (gid, name, quizSet) {
        let game = this;
        this.gid = gid || `${parseInt(Math.random() * 1000000)}`;
        this.name = name;
        this.quizSet = quizSet || [];
        this.sequence = 0;
        this.open = false;
        this.live = false;
        this.answers = {};
        this.gameovers = {};
        this.players = {};
        this.sig_session = null;

        if (quizSet.length === 0) {
            logger.warn(`game ${gid} has an empty quiz set`);
        }

        this.reset = _ => {
            game.sequence = 0;
            game.open = false;
            game.live = false;
            game.answers = {};
            game.gameovers = {};
        };

        this.setLive = live => {
            game.live = live;
            game.sig_session.onMessageInstantReceive = (account, uid, msg) => {
                logger.info(`received msg: ${msg}`);
                let json = JSON.parse(msg);
                switch (json.type) {
                    case "publish":
                        game.publish(result => {
                            logger.info(JSON.stringify(result));
                            game.sig_session.messageInstantSend(mc_id, JSON.stringify({type: "info", data: result}));
                        });
                        break;
                    case "stopAnswer":
                        if(game.open){
                            game.closeQuiz();
                            game.sig_session.messageInstantSend(mc_id, JSON.stringify({type: "info", data: {err:"game closed, waiting for summary info..."}}));
                        } else {
                            logger.info("try to stop a quiz which is already closed");
                            game.sig_session.messageInstantSend(mc_id, JSON.stringify({type: "info", data: {err:"try to stop a quiz which is already closed"}}));
                        }
                        break;
                    case "reset":
                        game.reset();
                        game.sig_session.messageInstantSend(mc_id, JSON.stringify({type: "info", data: {}}));
                        break;
                }
            };
        }

        this.hasNext = function () {
            return game.sequence < game.quizSet.length;
        };

        this.nextQuiz = function () {
            return game.quizSet[game.sequence];
        };

        this.publish = function(cb) {
            if(game.open){
                cb({err: "quiz_going_on"});
                return;
            }
            if(!game.hasNext()){
                cb({err: "no_more_quiz"});
                return;
            }
            game.publishNextQuiz().then(_ => {
                cb({});
            });
        }

        this.canplay = function(uid) {
            if(game.sequence === 0){
                return {result: true};
            }
            if(!game.players[uid]){
                logger.info(`not a player ${uid}`);
                return {result: false, err: `not a player ${uid}`};
            } else {
                if(game.gameovers[uid]){
                    logger.info(`${uid} is already game over`);
                    return {result: false, err: `${uid} is already game over`};
                } else {
                    return {result: true};
                }
            }
        };

        this.closeQuiz = function() {
            logger.info(`quiz closed for ${game.gid}`);
            game.open = false;
            game.summaryResult(game.sequence++);
        };

        this.publishNextQuiz = function () {
            game.open = true;
            return new Promise((resolve, reject) => {
                let quiz = Object.assign({}, game.nextQuiz());
                delete quiz.answer;
                quiz = JSON.stringify({ type: "quiz", data: quiz });
                game.sig_session.messageInstantSend(mc_id, quiz);
                rongcloudSDK.message.chatroom.publish(game.gid, game.gid, "RC:TxtMsg", JSON.stringify({ content: quiz, extra: "" }), (err, resultText) => {
                    let result = parseResult(err, resultText);
                    if (result) {
                        logger.info(`quiz published: ${quiz}`);
                        game.answers[game.sequence] = {};
                        resolve(result);
                    } else {
                        logger.error(`quiz publish failed: ${quiz}`);
                        reject(err);
                    }
                });
            });
        };

        this.relive = function(uid){
            logger.info(`god bless ${uid}....now your life has returned`);
            game.gameovers[uid] = undefined;
            game.players[uid] = true;
        };

        this.answerCommited = function (uid) {
            return game.answers[game.sequence][uid] !== undefined;
        };

        this.commitanswer = function (uid, result) {
            let question = game.quizSet[game.sequence];
            let resultSize = question.options.length;
            let answer = parseInt(result);

            if (answer >= resultSize || answer < 0) {
                logger.error("invalid answer");
                return;
            }

            let correct_answer = question.answer;
            if (answer !== correct_answer) {
                game.gameovers[uid] = true;
            }

            logger.info(`anwser collected from ${uid}, ${answer}`);
            game.answers[game.sequence][uid] = answer;
            if(game.sequence === 0){
                game.players[uid] = true;
            }
        };

        this.summaryResult = function (sequence) {
            let results = game.answers[sequence];
            let quiz = game.quizSet[sequence];
            let options = quiz.options;
            let answer = game.quizSet[sequence].answer;
            let rightUids = [];
            let wrongUids = [];
            let resultSpread = {};

            for(let i = 0; i < options.length; i++){
                resultSpread[i] = 0;
            }

            Object.keys(results).forEach(uid => {
                let commited = results[uid];
                if (commited === answer) {
                    rightUids.push(uid);
                } else {
                    wrongUids.push(uid);
                }
                (resultSpread[commited] !== undefined) && resultSpread[commited]++;
            });
            logger.info(`right: ${rightUids.length}, wrong: ${wrongUids.length}, total: ${Object.keys(results).length}`);
            let data = JSON.stringify({
                type: "result",
                data: {
                    correct: rightUids.length,
                    total: Object.keys(results).length,
                    sid: sequence,
                    result: answer,
                    spread: resultSpread
                }
            });
            game.sig_session.messageInstantSend(mc_id, data);
            rongcloudSDK.message.chatroom.publish(game.gid, game.gid, "RC:TxtMsg", JSON.stringify({ content: data, extra: "" }), (err, resultText) => {
                let result = parseResult(err, resultText);
                if (result) {
                    logger.info(`result published for ${data}`);
                } else {
                    logger.error(`result publish failed for ${data}`);
                }
            });
        };
    };

    /*------------------------------------------------
    |   function : GameMaker
    \*----------------------------------------------*/
    this.add = function (game) {
        return new Promise((resolve, reject) => {
            let rc_promise = new Promise((rc_resolve, rc_reject) => {
                rongcloudSDK.chatroom.query(game.gid, (err, resultText) => {
                    let res = parseResult(err, resultText);
                    if (res) {
                        if (res.chatRooms.length === 0) {
                            logger.info(`room ${game.gid} not exist, try to create one`);
                            rongcloudSDK.chatroom.create({
                                id: game.gid,
                                name: game.name
                            }, (err, retText) => {
                                let ret = parseResult(err, retText);
                                if (ret) {
                                    logger.info("rongyun room " + game.gid + " added");
                                    server.__games.push(game);
                                    rc_resolve();
                                } else {
                                    rc_reject(err);
                                }
                            });
                        } else {
                            logger.info("rongyun room " + game.gid + " added");
                            server.__games.push(game);
                            rc_resolve();
                        }
                    }
                });
            });
            let agora_promise = new Promise((agora_resovle, agora_reject) => {
                let signal = new Signal(sig_appid);
                // game.sig_session = signal.login(game.gid, sig_token.get(sig_appid, sig_appcert, game.gid, 1));
                // game.sig_session = signal.login(game.gid, sig_calc_token);
                game.sig_session = signal.login(game.gid, "_no_need_token");
                game.sig_session.onLoginSuccess = function () {
                    logger.info(`agora signal login successful ${game.gid}`);
                    agora_resovle();
                };
                game.sig_session.onLoginFailed = function () {
                    logger.error(`agora signal login failed ${game.gid}`);
                    agora_reject("failed");
                };
            });


            Promise.all([rc_promise, agora_promise]).then(_ => {
                game.setLive(true);
                logger.info(`game ${game.gid} is now live`);
                resolve();
            }).catch(e => { });
        });
    };

    this.get = function (gid) {
        let game = server.__games.filter(item => {
            return `${item.gid}` === `${gid}`;
        });
        return game.length > 0 ? game[0] : null;
    };
};

module.exports = HQ;