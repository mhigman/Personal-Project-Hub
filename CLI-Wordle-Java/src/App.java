public class App {
    public static void main(String[] args) throws Exception {
       WordleGame wordleHandler = new WordleGame();
       wordleHandler.clearConsole();
       wordleHandler.printIntro();
       wordleHandler.run();
       
    }
}