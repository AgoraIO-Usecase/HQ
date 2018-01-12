const fs = require("fs");
const path = require("path");
let QuizFactory = {};

QuizFactory.load = function (name) {
    return new Promise((resolve, reject) => {
        fs.readFile(path.join(__dirname, `../resources/quiz/${name}.json`), (err, data) => {
            if (err) {
                reject(err);
            } else {
                let json = JSON.parse(data.toString("utf8"));
                resolve(json);
            }
        });
    });
};


QuizFactory.loadAll = function (names) {
    let tasks = [];
    names.forEach(name => {
        tasks.push(QuizFactory.load(name));
    });
    return Promise.all(tasks);
};

module.exports = QuizFactory;