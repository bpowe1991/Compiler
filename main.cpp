/*
Programmer: Briton A. Powe          Program: frontEnd
Date: 4/15/19                       Class: Compilers
File: main.cpp
------------------------------------------------------------------------
Program Description:
This is the main source file for the program frontEbd. Input (keepboard, redirection
, or file stream) is set to cin and made into tokens. The tokens are organized into
a parse tree by the parser.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include "token.h"
#include "testScanner.h"
#include "parser.h"
#include "staticSem.h"

using namespace std;

int main(int argc, char *argv[]){
    ifstream input;
    char c;
    string token;
    bool error = false;
    p_node* root;
    
    //Checking number of command-line arguements.
    if (argc < 1 || argc > 2){
        
        cout << (string)argv[0]+"Error! Incorrect argument list" << endl;
        return -1;
    }
    else if (argc == 2){
        
        //If input file arguement is given.
        input.open(((string)argv[1]+".input1").c_str());
        
        //Checking if file opened.
        if (!input.is_open()){
            
            cout << (string)argv[0]+" : Error! Cannot open file" << endl;
            error = true;
        }

        //Setting cin read buffer to input read buffer.
        cin.rdbuf(input.rdbuf());
    }
    
    //Exiting if error occurred.
    if (error){
        input.close();
        exit(-1);
    }

    //Calling testScanner to generate tokens from input.
    root = parser();
    generateTarget(root);
    deleteTree(root);

    return 0;
}
