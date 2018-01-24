
const express = require("express");
const app = express();
const bodyParser = require("body-parser");
const http_server = require("http").Server(app);
const logger = require("./modules/logger").get("hq");
const HQ = require("./modules/HQ");
const cluster = require("cluster");
const numCPUs = require("os").cpus().length;


logger.info(`start in ${process.env.NODE_ENV} mode`);


function initProcess(application) {
    // parse application/json
    application.use(bodyParser.json());
    application.use(bodyParser.urlencoded({ extended: false }));

    // Add headers
    application.use(function (req, res, next) {
        // Website you wish to allow to connect
        res.setHeader("Access-Control-Allow-Origin", "*");

        // Request methods you wish to allow
        res.setHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, PATCH, DELETE");

        // Request headers you wish to allow
        res.setHeader("Access-Control-Allow-Headers", "X-Requested-With,content-type");

        // Set to true if you need the website to include cookies in the requests sent
        // to the API (e.g. in case you use sessions)
        // res.setHeader("Access-Control-Allow-Credentials", true);

        // Pass to next layer of midd leware
        next();
    });
}

if (cluster.isMaster) {
    //master node
    const api = require("./modules/Api").master;
    initProcess(app);
    let maker = new HQ.GameMaker();
    let init = maker.init();
    init.then(() => {
        api(maker, app);
        http_server.listen(9000);
    }).catch((e) => {
        logger.error(e);
    });


    // QuizFactory.load("quiz-1").then(result => {
    //     maker.add(new HQ.Game("10001", "Test Game1", result)).catch(_ => { });
    //     logger.info(`Master cluster setting up ${numCPUs}`);
    // });
} else {
    //cluster node
    // const api = require("./modules/Api").cluster;
    // initProcess(app);
    // api(app);
    // http_server.listen(8000);
}
