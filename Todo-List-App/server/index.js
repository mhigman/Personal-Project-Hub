const express = require("express");
//const bodyParser = require("body-parser");
const cors = require("cors");
const morgan = require("morgan");
const fs = require("fs");
const PORT = process.env.PORT || 3001;
const app = express();

app.use(express.json());
app.use(cors());
app.use(morgan('dev'));

app.post("/create-progress-bar", function(req, res) {
    //console.log(req);
    let currentInterval = 0;
    let waitTime = parseFloat(req.body.timeValue) * 1000;
    let waitInterval = parseFloat(req.body.timeInterval) * 1000;
    let numOfBlocks = parseFloat(req.body.numOfBlocks);
    if(isNaN(waitTime) || isNaN(waitInterval) || isNaN(numOfBlocks)){
        res.send({status: "Failure! Please enter something in every textfeild!"});
        return;
    }
    if(waitTime === 0 || waitInterval === 0 || numOfBlocks === 0){
        res.send({status: "Failure! Please don't enter 0!"});
        return;
    }
    const onInterval = () => {
        currentInterval += waitInterval;
        //console.log((currentInterval/waitTime) * numOfBlocks);
        let blockNum = Math.floor(((currentInterval/(waitTime/numOfBlocks))));
        //console.log(blockNum);
        process.stdout.clearLine();
        process.stdout.cursorTo(0);
        process.stdout.write("#".repeat(blockNum) + "-".repeat(numOfBlocks-blockNum));
    }

    const onFinished = () => {
        process.stdout.clearLine();
        process.stdout.cursorTo(0);
        process.stdout.write("#".repeat(numOfBlocks) + " -- done\nLogging into the logs directory...");
        clearInterval(progressBar);
        const logFile = () => {
            fs.writeFileSync("./server/log/log"+Date.now().toString()+".txt",
            `Action with time ${waitTime/1000} secs, \
an iterval of ${waitInterval/1000}, \
and a progress bar length of ${numOfBlocks} \
done at -- ${Date.now().toString()}`);
        };
        if(fs.existsSync("./server/log")){
            logFile();
        } else {
            fs.mkdirSync("./server/log");
            logFile();
        }
        console.log("file uploaded successfully")    
        res.send({status: "Success!"});
    }
    
    const progressBar = setInterval(onInterval, waitInterval);

    const timer = setTimeout(onFinished, waitTime);
   
});

app.get("/get-todo-list", function(req, res) {
    if(fs.existsSync("./server/todo/todo.txt")){
        //console.log(fs.readFileSync("./server/todo/todo.txt"));
        res.json({text: fs.readFileSync("./server/todo/todo.txt").toString()});
    } else {
        res.json({text: ""})
    }
});

app.post("/write-to-todo", function(req, res){
    console.log(req.body);
    fs.appendFileSync("./server/todo/todo.txt", ('\u2022'+ req.body.text + "⌫"));
    res.send({status: "sent"});
});

app.post("/remove-todo", function(req, res){
    console.log(req.body.text);
    let data = fs.readFileSync("./server/todo/todo.txt").toString();

    let newValue = data.replaceAll(req.body.text + "⌫", '');
    console.log(newValue);
    fs.writeFileSync("./server/todo/todo.txt", newValue);
    res.send({status: "sent"});
});

app.listen(PORT, () => {
    console.log(`Server listening on ${PORT}`);
});

