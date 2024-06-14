//Fist node test

//function getValue(flag){
    ////console.log(process.argv[process.argv.indexOf(flag)+1]);
    //return process.argv[process.argv.indexOf(flag)+1];
//}
////console.log(process.argv);
//const waitTime = parseInt(getValue("--wait_time"));
//const waitInterval = parseInt(getValue("--wait_interval"));
//const numOfBlocks = parseInt(getValue("--num_of_block"));
//let currentInterval = 0;
const fs = require("fs");

module.exports = (waitTime, waitInterval, numOfBlocks) => {
    let currentInterval = 0;
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
            fs.writeFileSync("./log/log"+Date.now().toString()+".txt",`Action with time ${waitTime/1000} secs done at -- ${Date.now().toString()}`);
        };
        if(fs.existsSync("./log")){
            logFile();
        } else {
            fs.mkdirSync("./log");
            logFile();
        }
        console.log("file uploaded successfully")
    }

    const progressBar = setInterval(onInterval, waitInterval);

    const timer = setTimeout(onFinished, waitTime);

}
