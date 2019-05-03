/*
Programmer: Briton A. Powe          Program: frontEnd
Date: 4/15/19                       Class: Compilers
File: parser.h
------------------------------------------------------------------------
Program Description:
This is the header file for the parser. 
*/

#ifndef PARSER
#define PARSER

#include <string>
#include "token.h"

typedef struct parserNode {
    std::string label;
    s_token *tokens[3];
    struct parserNode *children[4];
} p_node;

p_node* parser();
void deleteTree(p_node*);

#endif 