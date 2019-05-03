/*
Programmer: Briton A. Powe          Program: P1
Date: 3/14/19                       Class: Compilers
File: testScanner.cpp
------------------------------------------------------------------------
Program Description:
This is the source file for the test Scanner. The function loops over input
and outputs tokens to cout. If an error of EOF occurs, the function exits
back to main.cpp. 
*/

#include <iostream>
#include <string>
#include "token.h"
#include "scanner.h"
#include "testScanner.h"

using namespace std;

//Main looping function over input
void testScanner(){
    s_token *nextToken;
    while (1){
        
        //Calling scanner.
        nextToken = scannerDriver();
        
        //Outputting results from scanner.
        cout << "<" << nextToken->id << nextToken->tokenInstance <<  ", line " 
             << nextToken->lineNumber << ">" << endl;
        
        //Exit loop if termination condition of error or EOF is met.
        if (nextToken->id == "End of File,  "){
            break;
        }
    }
}
