#![allow(non_snake_case)]
mod words;
use crate::words::{WORD_LIST, SOLUTION_LIST};
use std::{ 
    fs::{File, OpenOptions},
    io::prelude::*,
};


use std::path::Path;
extern crate rand;
use rand::{rngs::ThreadRng, Rng};
const GREEN_BG: &str = "\u{001B}[42m";
const YELLOW_BG: &str = "\u{001B}[43m";
const BLACK_BG: &str = "\u{001B}[30m\u{001B}[40m";
const RESET: &str = "\u{001B}[0m";

fn getInput(prompt: &str) -> String{
    println!("{prompt}");
    let mut input: String = String::new();
    match std::io::stdin().read_line(&mut input){
        Ok(_goes_into_input_above) => {}
        Err(_no_updates_is_fine) => {}
    }
    input.trim().to_string().to_ascii_lowercase()
}
fn checkWord(currentWord: &str, solutionWord: &str) -> [String; 5]{
    let mut untakenIndecies: [usize; 5] = [0,1,2,3,4];
    let newString: &str = "";
    let mut highlightOutput: [String; 5] = [String::from(newString), String::from(newString), String::from(newString), String::from(newString), String::from(newString)];
    for (i, value) in highlightOutput.iter_mut().enumerate() {
        for j in 0..5{
            if currentWord.chars().nth(i) == solutionWord.chars().nth(j) {
                if i == j {
                    *value = String::from(GREEN_BG);
                    untakenIndecies[j] = untakenIndecies.len() + 1;
                    break;
                } else if untakenIndecies.contains(&j) && solutionWord.chars().nth(j) != currentWord.chars().nth(j) && solutionWord.chars().nth(i) != currentWord.chars().nth(i) {
                    *value = String::from(YELLOW_BG);
                    untakenIndecies[j] = 5;
                    break;
                } else{
                    *value = String::from(BLACK_BG);
                }
            }
        }
    }
    highlightOutput 
}
fn getSolution() -> String{
    let mut rng: ThreadRng = rand::thread_rng();
    (SOLUTION_LIST[rng.gen_range(0..SOLUTION_LIST.len())]).to_string()
}
fn main() {
    let mut alphabetColors: [&str; 26] = [""; 26];
    let mut checkedList: [String; 5];
    let mut board: String = String::from("-----------------------\n");
    let mut input: String;
    let mut tries: u8 = 0;
    let solution = getSolution();
    let mut solved: bool = false;
    loop{
        input = getInput("Word: ");
        if !(WORD_LIST.contains(&(input.as_str()))) && !(SOLUTION_LIST.contains(&(input.as_str()))){
            println!("Not a valid input");
            continue;
        }        
        println!("\x1B[2J\x1B[1;1H");

        checkedList = checkWord(&input, &solution);
        for (index, i) in checkedList.iter().enumerate(){
            board = format!("{}|{} {} {}|", board, if i != BLACK_BG{i.to_string()} else{"".to_string()}, input.as_bytes()[index] as char, RESET);
        }
        board = format!("{}\n----------------------\n", board);
        println!("{}", board);
        for i in 0..5{
            
            if alphabetColors[(input.as_bytes()[i]-97) as usize] != GREEN_BG{
                if checkedList[i] == GREEN_BG{
                    alphabetColors[(input.as_bytes()[i]-97) as usize] = GREEN_BG;
                } else if checkedList[i] == YELLOW_BG{
                    if alphabetColors[(input.as_bytes()[i]-97) as usize] != GREEN_BG{
                        alphabetColors[(input.as_bytes()[i]-97) as usize] = YELLOW_BG;
                    }
                } else if alphabetColors[(input.as_bytes()[i]-97) as usize] != GREEN_BG && alphabetColors[(input.as_bytes()[i]-97) as usize] != YELLOW_BG {
                    alphabetColors[(input.as_bytes()[i]-97) as usize] = BLACK_BG;
                }
            }
             
        }
        println!("\n,---,---,---,---,---,---,---,---,---,---,---,---,---,-------,");
        println!("|1/2| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | + | ' | <-    |");
        println!("|---'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-----|");
        println!("| ->| |{} Q {}|{} W {}|{} E {}|{} R {}|{} T {}|{} Y {}|{} U {}|{} I {}|{} O {}|{} P {}| ] | ^ |     |", alphabetColors[16], RESET, alphabetColors[22], RESET, alphabetColors[4], RESET, alphabetColors[17], RESET, alphabetColors[19], RESET , alphabetColors[24], RESET, alphabetColors[20], RESET, alphabetColors[8], RESET, alphabetColors[14], RESET, alphabetColors[15], RESET);
        println!("|-----',--',--',--',--',--',--',--',--',--',--',--',--'|    |");
        println!("| Caps |{} A {}|{} S {}|{} D {}|{} F {}|{} G {}|{} H {}|{} J {}|{} K {}|{} L {}| \\ | [ | * |    |", alphabetColors[0], RESET, alphabetColors[18], RESET, alphabetColors[3], RESET, alphabetColors[5], RESET, alphabetColors[6], RESET, alphabetColors[7], RESET, alphabetColors[9], RESET, alphabetColors[10], RESET, alphabetColors[11], RESET);
        println!("|----,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'---'----|");
        println!("|    | < |{} Z {}|{} X {}|{} C {}|{} V {}|{} B {}|{} N {}|{} M {}| , | . | - |          |", alphabetColors[25], RESET, alphabetColors[23], RESET, alphabetColors[2], RESET, alphabetColors[21], RESET, alphabetColors[1], RESET, alphabetColors[13], RESET, alphabetColors[12], RESET);
        println!("|----'-,-',--'--,'---'---'---'---'---'---'-,-'---',--,------|");
        println!("| ctrl |##| alt |                          |altgr |##| ctrl |");
        println!("'------'--'-----'--------------------------'------'--'------'");
        tries+=1;
        if tries > 5 || input == solution{
            break;
        }
    }
    if input == solution {
        println!("You got the word in {tries} tries!");
        solved = true;
    } else {
        println!("Close, but the word was {solution}");
    }
    // Create a path to the desired filestring to int rust
    let path = Path::new("src/stats.txt");
    writeAndDisplayStatistics(path, tries, solved)
}
fn writeZeroes(path: &Path){
    let mut file = OpenOptions::new().read(true).write(true).create(true).open(path).unwrap();
    for _ in 0..7{
        let _ = file.write("0\n".as_bytes());
    }
}
fn writeAndCreateFile(path: &Path) -> File{
    writeZeroes(path);
    File::open(path).unwrap()
}


fn writeAndDisplayStatistics(path: &Path, tries: u8, solved: bool){
    let mut file = match File::open(path) {
        Ok(file) => file,
        Err(_) => writeAndCreateFile(path),
    };
    let mut line: [i32; 8] = [0; 8];
    
    let mut buf: Vec<u8> = vec![];
    let _ = file.read_to_end(&mut buf);
    let rawFileStats = String::from_utf8(buf).unwrap();
    let currentLines = rawFileStats.trim().split('\n');
    for i in currentLines.enumerate(){
        line[i.0] = i.1.trim().parse::<i32>().unwrap();
        if i.0 == ((tries-1) as usize) && solved{
            line[i.0] += 1;
        }

    }
    if solved {
        line[6] += 1; 
    } else{
        line[7] += 1;
    }

    println!("Score Distribution: ");
    for i in 0..6{
        println!("{} - {}", i+1, "#".repeat(line[i as usize].try_into().unwrap()));
    }
    println!("Wins -- {}", line[6]);
    println!("Losses -- {}", line[7]);
    
    let mut file = OpenOptions::new().write(true).read(true).append(false).open(path).expect("File bad weeeeeee");

    for i in line{
        let _ = file.write((i.to_string()+"\n").as_bytes());
    }
}