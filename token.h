/*
Programmer: Briton A. Powe          Program: frontEnd
Date: 4/15/19                       Class: Compilers
File: token.h
------------------------------------------------------------------------
Program Description:
This is the header file for the variable definitions for tokens and states. 
*/

#ifndef TOKEN
#define TOKEN

#include <string>

typedef struct scannerToken {
    std::string id;
    std::string tokenInstance;
    int lineNumber;
} s_token;

typedef struct state {
    int row, column;
} state;

#endif 