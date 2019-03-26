const logger = require("./logger").get("hq");
const config = require("./config");
const request = require("request");
const agora_appid = config.agora_appid;
const AgoraRtmSDK = require('agora-node-rtm').default
let cipher = null;
try {
    cipher = require("./Encrypt");
} catch (e) {
    cipher = require("./FakeEncrypt");
}


class GameMaker {
    constructor() {
        this.__games = [];
        this.rtm = new AgoraRtmSDK();
        this.serverName = "";
    }


    init() {
        let serverName = `server-${Math.floor(Math.random()*1000)}`
        this.serverName = serverName
        return this.rtm.login(agora_appid, serverName)
    };

    add(game) {
        let g = this.get(game.gid);
        if (g) {
            g.quizSet = game.quizSet;
            g.reset();
            return Promise.resolve();
        } else {
            return new Promise((resolve, reject) => {
                let channel = this.rtm.createChannel(game.gid)
                channel.join().then(() => {
                    game.channel = channel
                    game.rtm = this.rtm
                    this.__games.push(game);
                    resolve()
                }).catch(e => {
                    reject(e)
                })
            })
        }
    };

    get(gid) {
        let game = this.__games.filter(item => {
            return `${item.gid}` === `${gid}`;
        });
        return game.length > 0 ? game[0] : null;
    };
}

class Game {
    constructor(gid, name, quizSet, encrypt) {
        this.gid = gid || `${parseInt(Math.random() * 1000000)}`;
        this.name = name;
        this.quizSet = quizSet || [];
        this.sequence = 0;
        this.open = false;
        this.live = false;
        this.players = {};
        this.timeout = 20;
        this.encrypt = encrypt || null;
        this.inviting = null;
        this.rtm = null;
        this.channel = null;

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
        return this.sequence < this.quizSet.length;
    };

    nextQuiz() {
        return this.quizSet[this.sequence];
    };

    publish() {
        return new Promise((resolve, reject) => {
            if (this.open) {
                reject("quiz_going_on");
                return;
            }
            if (!this.hasNext()) {
                reject("no_more_quiz");
                return;
            }
            this.publishNextQuiz().then(result => {
                resolve(result);
            });
        });
    }

    canplay(uid) {
        if (this.sequence === 0) {
            return { result: true };
        }
        let player = this.players[uid];
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
        logger.info(`quiz closed for ${this.gid}`);
        this.open = false;
        return this.summaryResult(this.sequence++);
    };

    broadcast(msg) {
        this.channel && this.channel.sendMessage(msg)
    }

    publishNextQuiz() {
        this.open = true;
        return new Promise((resolve, reject) => {
            let quiz = Object.assign({}, this.nextQuiz());
            let encrypted_quiz = null;
            delete quiz.answer;
            quiz.total = this.quizSet.length;
            quiz.timeout = this.timeout;
            if (cipher.supported.includes(this.encrypt)) {
                encrypted_quiz = cipher.encrypt("v1", JSON.stringify(quiz), this.gid);
            }
            let raw_quiz = { type: "quiz", data: quiz };
            encrypted_quiz = encrypted_quiz ? { type: "quiz", data: encrypted_quiz, encrypt: this.encrypt } : { type: "quiz", data: quiz, encrypt: "null" };
            raw_quiz = JSON.stringify(raw_quiz);
            encrypted_quiz = JSON.stringify(encrypted_quiz);
            this.broadcast(encrypted_quiz)
            logger.info(`sending quiz ${encrypted_quiz} to ${this.gid}`)
            resolve(raw_quiz);
        });
    };

    joinGame(uid, force){
        let player = this.players[uid];
        if(player && !force){
            logger.error(`user ${uid} already exists`);
            return player;
        }
        player = {alive: true, answers: {}};
        this.players[uid] = player;
        return player;
    };

    relive(uid) {
        logger.info(`player ${uid} try to revive himself...`)
        let canplay = this.canplay(uid);
        logger.info(`can player ${uid} play? ${canplay.result}`);
        if (canplay.result) {
            logger.info(`player ${uid} revive not needed`)
        } else {
            logger.info(`god bless ${uid}....now your life has returned`);
            this.joinGame(uid, true);
        }
    };

    answerCommited(uid) {
        let player = this.players[uid];
        if(!player){
            return false;
        }
        return player.answers[this.sequence] !== undefined;
    };

    commitanswer(uid, result) {
        let question = this.quizSet[this.sequence];
        let resultSize = question.options.length;
        let answer = parseInt(result);
        let player = this.players[uid];

        if(!player){
            logger.info(`not a player ${uid}`);
            return;
        }

        if (answer >= resultSize || answer < 0) {
            logger.error("invalid answer");
            return;
        }

        logger.info(`anwser collected from ${uid}, ${answer}`);
        player.answers[this.sequence] = answer;
    };


    summaryResult(sequence) {
        return new Promise((resolve, reject) => {
            let results = {};
            let quiz = this.quizSet[sequence];
            let options = quiz.options;
            let answer = this.quizSet[sequence].answer;
            let rightUids = [];
            let wrongUids = [];
            let resultSpread = {};
            let players = this.players;

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

            if (sequence === this.quizSet.length - 1) {
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
            if (sequence === this.quizSet.length - 1) {
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
            this.broadcast(data)
            resolve(JSON.parse(data));
        });
    };

    inviteRequest(invitee) {
        logger.info(`try to inivite ${invitee}`);
        return new Promise((resolve, reject) => {
            this.inviteEnd().then(() => {
                let invite_msg = {
                    type: "inviteRequest",
                    data: {
                        uid: invitee
                    }
                }

                this.rtm.sendMessageToPeer(invitee, JSON.stringify(invite_msg))

                this.inviting = {
                    responded: false,
                    uid: invitee
                };
                resolve();
            });
        });
    };

    inviteEnd() {
        logger.info(`try to end inivite ${JSON.stringify(this.inviting)}`);
        if (!this.inviting) {
            logger.info(`no inviting exists`);
            return Promise.resolve();
        } else {
            return new Promise((resolve, reject) => {
                let invite_msg = {
                    type: "inviteEnd",
                    data: {
                        uid: this.inviting.uid
                    }
                }
                this.rtm.sendMessageToPeer(invitee, JSON.stringify(invite_msg))
                logger.info(`invite end for ${this.inviting.uid} successfully sent`);
                resolve();
            });
        }
    };

    inviteResponse(invitee, accept, mediaUid) {
        if (!this.inviting){
            logger.info(`no invite exist`);
        } else if (invitee !== this.inviting.uid) {
            logger.info(`sending invite response for ${invitee} while ${this.inviting.uid} is invited instead`);
        } else {
            logger.info(`invite response received from ${invitee}, send back to ${this.gid}`);
            // server.sig.messageInstantSend(this.gid, JSON.stringify({ type: "inviteResponse", data: { accept: accept, mediaUid: mediaUid, uid: invitee } }));
            this.inviting = { responded: true, accept: accept, mediaUid: mediaUid, uid: invitee };
        }
    }
}

module.exports = {
    GameMaker: GameMaker,
    Game: Game
};