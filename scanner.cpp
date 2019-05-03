/*
Programmer: Briton A. Powe          Program: P1
Date: 3/14/19                       Class: Compilers
File: scanner.cpp
------------------------------------------------------------------------
Program Description:
This is source file for scanner.cpp. It contains the main driver function
and a series of helper functions to generate tokens from input. 
*/

#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include "token.h"
#include "scanner.h"

using namespace std;

#define INITIAL 0

//List of token descriptions for token IDs.
static string descriptors[] = {
    "Identifier", "Integer", "Right Bracket",
    "Left Bracket", "Left Brace", "Right Brace",
    "Forward Slash", "Colon", "Semi-Colon",
    "Comma", "Period", "Left Parenthesis",
    "Right Parenthesis", "Plus Operator",
    "Minus Operator", "Equal Operator",
    "Percent Operator", "Greater Than Operator",
    "Less Than Operator", "Asterisk", "End of File",
    "Keyword"
};

//Keywords to check against ID tokens.
static string keywords[]={
    "Begin", "End", "Loop", "Void", "INT", "Return", "Read", 
    "Output", "Program", "IFF",  "Then", "Let"
};


static int lineCount = 1;

//FA table to figure out what state a token generation is in.
static int table[21][24] = {
    1, -3, 2, 14, 15, 7, 19, 18, 11, 10, 5, 6, 4, 3, 12, 13, 16, 20, 8, 9, 17, 0, -2, 70,
    1, 1, 1, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, -2, 50,
    51, -3, 2, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, -2, 51,
    52, -3, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, -2, 52,
    53, -3, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, -2, 53,
    54, -3, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, -2, 54,
    55, -3, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, -2, 55,
    56, -3, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, -2, 56,
    57, -3, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, -2, 57,
    58, -3, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, -2, 58,
    59, -3, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, -2, 59,
    60, -3, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, -2, 60,
    61, -3, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, -2, 61,
    62, -3, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, -2, 62,
    63, -3, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, -2, 63,
    64, -3, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, -2, 64,
    65, -3, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, -2, 65,
    66, -3, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, -2, 66,
    67, -3, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, -2, 67,
    68, -3, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, -2, 68,
    69, -3, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, -2, 69
};

//Filter 1 for counting lines and skipping comments
static char getChar(){
    char nextLetter;
    cin.get(nextLetter);

    if (nextLetter == '\n'){
        lineCount++;
        return nextLetter;
    }

    while (nextLetter == '\\'){
        while (nextLetter != '\n'){
            cin.get(nextLetter);
        }
        
        if (nextLetter == '\n'){
            lineCount++;
        }
    }
    return nextLetter;
}

//Function to set-up error tokens.
static void scannerErrorHandler(s_token *errorToken, state *errorState, char errorChar){
    
    //If a token is started with a lowercase letter, causing an error.
    if (table[errorState->row][errorState->column] == -3){
        errorToken->id = "Scanner Error: ";
        errorToken->tokenInstance = "Identifier started with lowercase letter ";
        errorToken->tokenInstance += errorChar;
        errorToken->lineNumber = lineCount;
    }
    //If a character that is not part of the language is entered, causing an error.
    else if (table[errorState->row][errorState->column] == -2){
        errorToken->id = "Scanner Error: ";
        errorToken->tokenInstance = "Illegal character entered ";
        errorToken->tokenInstance += errorChar;
        errorToken->lineNumber = lineCount;
    }
    
    cout << errorToken->id << errorToken->tokenInstance <<  ", line "
         << errorToken->lineNumber << endl;

   exit(-1);
}

//Filter 2 for finding ID tokens that are keyword tokens and setting them.
static void foundKeyword(s_token *testingToken){
    int x;
    for (x = 0; x < 12; x++){
        if (testingToken->tokenInstance == keywords[x]){
            testingToken->id =descriptors[21];
            break;
        }
    }
}

//Function to translate characters from input to a column number in the table.
static int determineColumn(char letter){
    int column;
    if (letter >= 'A' && letter <= 'Z'){
        column = 0;
    }
    else if (letter >= 'a' && letter <= 'z'){
        column = 1;
    }
    else if (letter >= '0' && letter <= '9'){
        column = 2;
    }
    else if (letter == '+'){
        column = 3;
    }
    else if (letter == '-'){
        column = 4;
    }
    else if (letter == '/'){
        column = 5;
    }
    else if (letter == '<'){
        column = 6;
    }
    else if (letter == '>'){
        column = 7;
    }
    else if (letter == '.'){
        column = 8;
    }
    else if (letter == ','){
        column = 9;
    }
    else if (letter == '['){
        column = 10;
    }
    else if (letter == ']'){
        column = 11;
    }
    else if (letter == '{'){
        column = 12;
    }
    else if (letter == '}'){
        column = 13;
    }
    else if (letter == '('){
        column = 14;
    }
    else if (letter == ')'){
        column = 15;
    }
    else if (letter == '='){
        column = 16;
    }
    else if (letter == '*'){
        column = 17;
    }
    else if (letter == ':'){
        column = 18;
    }
    else if (letter == ';'){
        column = 19;
    }
    else if (letter == '%'){
        column = 20;
    }
    else if (letter == ' ' || letter == '\t' || letter == '\n'){
        column = 21;
    }
    else if (cin.peek() == EOF){
        column = 23;
    }
    else {
        column = 22;
    }

    return column;
}

//Main driver function for scanner.
s_token* scannerDriver(){
    s_token *newToken = new s_token;
    string tokenName;
    char inputChar;
    int column;
    state current, nextState;
    current.row = INITIAL;
    current.column = INITIAL;

    //Main driver loop.
    while (cin.peek() != EOF){
        inputChar = getChar();
        column = determineColumn(inputChar);
        nextState.row = current.row;
        nextState.column = column;
        
        //If a scanner error is found
        if (table[nextState.row][nextState.column] < 0){
            scannerErrorHandler(newToken, &nextState, inputChar);
            return newToken;
        }
        
        //If a final state is reached.
        else if (table[nextState.row][nextState.column] >= 50){
            
            //Decrement line count if last character was newline.
            if (inputChar == '\n'){
                lineCount--;
            }
            
            //Setting token after reaching final state.
            newToken->id = descriptors[(table[nextState.row][nextState.column])% 50];
            newToken->tokenInstance = tokenName;
            newToken->lineNumber = lineCount;
            cin.unget();
            foundKeyword(newToken);
            return newToken;
        }
        
        current.row = table[nextState.row][nextState.column];
        current.column = nextState.column;
        if(inputChar != '\n' && inputChar != '\t' && inputChar != ' '){
            tokenName += inputChar;
        }
    }

    //Generating EOF token.
    newToken->id = descriptors[20];
    newToken->tokenInstance = "EOF";
    newToken->lineNumber = lineCount;
    foundKeyword(newToken);
    return newToken;
}
