const logger = require("./logger").get("hq");
const config = require("./config");
const cc_id = config.cc_id;
const socks_host = config.socks_proxy_host;
const socks_port = config.socks_proxy_port;
const QuizFactory = require("./QuizFactory");
const request = require("request");
const Agent = require('socks5-http-client/lib/Agent');
const sig_appid = config.agora_appid;
let cipher = null;
try {
    cipher = require("./Encrypt");
} catch (e) {
    cipher = require("./FakeEncrypt");
}


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

function proxy(options) {
    if (socks_host) {
        logger.info(`using proxy`);
        options.agentClass = Agent;
        options.socksHost = socks_host;
        options.socksPort = socks_port;
    }
    return options;
}

class GameMaker {
    constructor() {
        this.__games = [];
        this.sig = null;
    }

    add(game) {
        return new Promise((resolve, reject) => {
            let g = this.get(game.gid);
            if (g) {
                g.quizSet = game.quizSet;
                g.reset();
                resolve();
                return;
            }

            this.__games.push(game);
        });
    };

    get(gid) {
        let game = this.__games.filter(item => {
            return `${item.gid}` === `${gid}`;
        });
        return game.length > 0 ? game[0] : null;
    };

    init() {
        return Promise.resolve();
    };

    handle(action, options) {
        return new Promise((resolve, reject) => {
            if (action === "reset") {
                this.handleReset(options);
            }
        });
    }


    handleReset(options) {
        let gid = options.gid || "";
        if (!gid) {
            reject("gid_needed");
            return;
        }

        let game = this.get(gid);
        if (!game) {
            reject("room_not_found");
            return;
        }

        game.reset();
        resolve();
    }
}

class Game {
    constructor(gid, name, quizSet, encrypt) {
        let game = this;
        this.gid = gid || `${parseInt(Math.random() * 1000000)}`;
        this.name = name;
        this.quizSet = quizSet || [];
        this.sequence = 0;
        this.open = false;
        this.live = false;
        this.players = {};
        this.sig_session = null;
        this.timeout = 20;
        this.encrypt = encrypt || null;
        this.inviting = null;

        if (quizSet.length === 0) {
            logger.warn(`game ${gid} has an empty quiz set`);
        }
    }

    reset() {
        this.sequence = 0;
        this.open = false;
        this.live = false;
        this.players = {};
    };

    setLive(live) {
        this.live = live;
    }

    hasNext() {
        let game = this;
        return game.sequence < game.quizSet.length;
    };

    nextQuiz() {
        let game = this;
        return game.quizSet[game.sequence];
    };

    publish() {
        let game = this;
        return new Promise((resolve, reject) => {
            if (game.open) {
                reject("quiz_going_on");
                return;
            }
            if (!game.hasNext()) {
                reject("no_more_quiz");
                return;
            }
            game.publishNextQuiz().then(result => {
                resolve(result);
            });
        });
    }

    canplay(uid) {
        let game = this;
        if (game.sequence === 0) {
            return { result: true };
        }
        let player = game.players[uid];
        if (!player) {
            logger.info(`not a player ${uid}`);
            return { result: false, err: `not a player ${uid}` };
        } else {
            if (!player.alive) {
                logger.info(`${uid} is already game over`);
                return { result: false, err: `${uid} is already game over` };
            } else {
                return { result: true };
            }
        }
    };

    closeQuiz() {
        let game = this;
        logger.info(`quiz closed for ${game.gid}`);
        game.open = false;
        return game.summaryResult(game.sequence++);
    };

    publishNextQuiz() {
        let game = this;
        game.open = true;
        return new Promise((resolve, reject) => {
            let quiz = Object.assign({}, game.nextQuiz());
            let encrypted_quiz = null;
            delete quiz.answer;
            quiz.total = game.quizSet.length;
            quiz.timeout = game.timeout;
            if (cipher.supported.includes(game.encrypt)) {
                encrypted_quiz = cipher.encrypt("v1", JSON.stringify(quiz), game.gid);
            }
            let raw_quiz = { type: "quiz", data: quiz };
            encrypted_quiz = encrypted_quiz ? { type: "quiz", data: encrypted_quiz, encrypt: game.encrypt } : { type: "quiz", data: quiz, encrypt: "null" };
            raw_quiz = JSON.stringify(raw_quiz);
            encrypted_quiz = JSON.stringify(encrypted_quiz);
            var options = {
                uri: `http://hq-im.agoraio.cn:8000/signaling/v1/${sig_appid}/sendChannelMessage`,
                method: 'POST',
                json: { "m": encrypted_quiz, "channel": game.gid }
            };
            proxy(options);
            logger.info(`sending quiz ${quiz} to ${game.gid}`)
            logger.info(`sending quiz ${encrypted_quiz} to ${game.gid}`)
            request(options, function (error, response, body) {
                if (!error && response.statusCode == 200) {
                    resolve(raw_quiz);
                } else {
                    reject(error);
                }
            });
        });
    };

    joinGame(uid, force){
        let game = this;
        let player = game.players[uid];
        if(player && !force){
            logger.error(`user ${uid} already exists`);
            return player;
        }
        player = {alive: true, answers: {}};
        game.players[uid] = player;
        return player;
    };

    relive(uid) {
        let game = this;
        logger.info(`player ${uid} try to revive himself...`)
        let canplay = game.canplay(uid);
        logger.info(`can player ${uid} play? ${canplay.result}`);
        if (canplay.result) {
            logger.info(`player ${uid} revive not needed`)
        } else {
            logger.info(`god bless ${uid}....now your life has returned`);
            this.joinGame(uid, true);
        }
    };

    answerCommited(uid) {
        let game = this;
        let player = game.players[uid];
        if(!player){
            return false;
        }
        return player.answers[game.sequence] !== undefined;
    };

    commitanswer(uid, result) {
        let game = this;
        let question = game.quizSet[game.sequence];
        let resultSize = question.options.length;
        let answer = parseInt(result);
        let player = game.players[uid];

        if(!player){
            logger.info(`not a player ${uid}`);
            return;
        }

        if (answer >= resultSize || answer < 0) {
            logger.error("invalid answer");
            return;
        }

        logger.info(`anwser collected from ${uid}, ${answer}`);
        player.answers[game.sequence] = answer;
    };


    summaryResult(sequence) {
        let game = this;
        return new Promise((resolve, reject) => {
            let results = {};
            let quiz = game.quizSet[sequence];
            let options = quiz.options;
            let answer = game.quizSet[sequence].answer;
            let rightUids = [];
            let wrongUids = [];
            let resultSpread = {};
            let players = game.players;

            for (let i = 0; i < options.length; i++) {
                resultSpread[i] = 0;
            }
            //-1 for missing answers
            resultSpread[-1] = 0;


            Object.keys(players).forEach(uid => {
                let player = players[uid];
                let commited = player.answers[sequence];
                if (commited === answer) {
                    rightUids.push(uid);
                } else {
                    wrongUids.push(uid);
                    player.alive = false;
                }
                (resultSpread[commited] !== undefined) ? resultSpread[commited]++ : resultSpread[-1]++;
            });

            if (sequence === game.quizSet.length - 1) {
                logger.info("=========================FINAL ROUND==========================");
            } else {
                logger.info(`=========================QUIZ ${sequence + 1}==========================`)
            }
            logger.info(`right: ${rightUids.length} in total,  {${JSON.stringify(rightUids)}}`);
            logger.info(`wrong: ${wrongUids.length} in total, {${JSON.stringify(wrongUids)}}`);
            logger.info(`total: ${rightUids.length + wrongUids.length} in total, {${JSON.stringify(rightUids)}`);
            logger.info(`spread: ${JSON.stringify(resultSpread)}`);
            let data = JSON.stringify({
                type: "result",
                data: {
                    correct: rightUids.length,
                    total: rightUids.length + wrongUids.length,
                    sid: sequence,
                    result: answer,
                    spread: resultSpread
                }
            });
            if (sequence === game.quizSet.length - 1) {
                data = JSON.stringify({
                    type: "result",
                    data: {
                        correct: rightUids.length,
                        total: Object.keys(results).length,
                        sid: sequence,
                        result: answer,
                        spread: resultSpread,
                        winners: rightUids
                    }
                })
            }
            let request_options = {
                uri: `http://hq-im.agoraio.cn:8000/signaling/v1/${sig_appid}/sendChannelMessage`,
                method: 'POST',
                json: { "m": data, "channel": game.gid }
            };
            proxy(request_options);
            request(request_options, function (error, response, body) {
                if (!error && response.statusCode == 200) {
                    resolve(JSON.parse(data));
                } else {
                    reject(error);
                }
            });
        });
    };

    inviteRequest(invitee) {
        let game = this;
        logger.info(`try to inivite ${invitee}`);
        return new Promise((resolve, reject) => {
            game.inviteEnd().then(() => {
                let invite_msg = {
                    type: "inviteRequest",
                    data: {
                        uid: invitee
                    }
                }
                let request_options = {
                    uri: `http://hq-im.agoraio.cn:8000/signaling/v1/${sig_appid}/sendMessageTo`,
                    method: 'POST',
                    json: { "m": JSON.stringify(invite_msg), "uid": invitee }
                };
                proxy(request_options);
                request(request_options, function (error, response, body) {
                    if (!error && response.statusCode == 200) {
                        game.inviting = {
                            responded: false,
                            uid: invitee
                        };
                        resolve();
                    } else {
                        reject(error);
                    }
                });
            });
        });
    };

    inviteEnd() {
        let game = this;
        logger.info(`try to end inivite ${JSON.stringify(game.inviting)}`);
        if (!game.inviting) {
            logger.info(`no inviting exists`);
            return Promise.resolve();
        } else {
            return new Promise((resolve, reject) => {
                let invite_msg = {
                    type: "inviteEnd",
                    data: {
                        uid: game.inviting.uid
                    }
                }
                let request_options = {
                    uri: `http://hq-im.agoraio.cn:8000/signaling/v1/${sig_appid}/sendMessageTo`,
                    method: 'POST',
                    json: { "m": JSON.stringify(invite_msg), "uid": game.inviting.uid }
                };
                proxy(request_options);
                request(request_options, function (error, response, body) {
                    if (!error && response.statusCode == 200) {
                        logger.info(`invite end for ${game.inviting.uid} successfully sent`);
                        resolve();
                    } else {
                        reject(error);
                    }
                });
            });
        }
    };

    inviteResponse(invitee, accept, mediaUid) {
        let game = this;
        if (!this.inviting){
            logger.info(`no invite exist`);
        } else if (invitee !== this.inviting.uid) {
            logger.info(`sending invite response for ${invitee} while ${this.inviting.uid} is invited instead`);
        } else {
            logger.info(`invite response received from ${invitee}, send back to ${this.gid}`);
            // server.sig.messageInstantSend(this.gid, JSON.stringify({ type: "inviteResponse", data: { accept: accept, mediaUid: mediaUid, uid: invitee } }));
            game.inviting = { responded: true, accept: accept, mediaUid: mediaUid, uid: invitee };
        }
    }
}

module.exports = {
    GameMaker: GameMaker,
    Game: Game
};