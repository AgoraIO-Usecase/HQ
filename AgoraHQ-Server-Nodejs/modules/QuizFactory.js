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
                let parsed = QuizFactory.parse(json);
                if(parsed.err){
                    resolve([]);
                } else {
                    resolve(parsed.data);
                }
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


QuizFactory.parse = function(data) {
    if(!Array.isArray(data)){
        return {err: "invalid_format"};
    }

    // "question": "世界上最大的宫殿是",
    // "type": "multi",
    // "options": [
    //     "故宫",
    //     "白宫",
    //     "爱丽舍宫",
    //     "克里姆林宫"
    // ],
    // "answer": 0

    let results = [];
    let error = "";
    for(let i = 0; i < data.length; i++){
        let item = data[i];
        let options = item.options;
        let answer = parseInt(item.answer);
        if(answer < 0 || answer >= options.length || isNaN(answer)){
            error = "invalid_data";
            break;
        }
        options = Array.isArray(options) ? options : [];
        results.push({
            id: i,
            question: `${item.question}` || "",
            type: `${item.type}` || "multi",
            options: options,
            answer: answer
        });
    }

    if(error){
        return {err: error};
    } else {
        return {err: null, data: results}
    }
};

module.exports = QuizFactory;