const progressBar = require("./lib/progressBar");
const rl = require("readline");
const interface = rl.createInterface(
    input = process.stdin,
    output = process.stdout,
);

const things = ["What is the time of the progress bar?",
                  "What is the 'interval' of the progress bar?",
                  "How long should the progress bar be?"]

function getAnswers(questions, done){
    answers = []

    const answerQuestion = (answer) => {
        answer = parseFloat(answer)
        if(isNaN(answer)){
            answer = interface.question(
                questions[answers.length],
                answerQuestion
            );
        }
        if(!isNaN(answer)){
            answers.push(answer);
        }
        if(answers.length < questions.length){
            interface.question(
                questions[answers.length],
                answerQuestion
            );
        } else{
            return done(answers);
        }
    };
    interface.question(questions[0], answerQuestion);
};

getAnswers(things, (answers) => {
    progressBar(answers[0] * 1000, answers[1] * 1000, answers[2]);
    return answers;
});