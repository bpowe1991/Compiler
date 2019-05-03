/*
Programmer: Briton A. Powe          Program: frontEnd
Date: 4/23/19                       Class: Compilers
File: staticSem.cpp
------------------------------------------------------------------------
Program Description:
This is the source file for verifying static semantics. Terminates normally,
or exits with error output.
*/

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include "scanner.h"
#include "staticSem.h"
#include <stack>

using namespace std;

#define TOKEN_SIZE 3
#define CHILD_SIZE 4
#define ERROR_CODE1 1
#define ERROR_CODE2 2

static int verify(stack <s_token>, string);
static void errorHandler(int, p_node*, int);
static void verifyStaticSem(p_node*);
static bool inMainBlock = false;

//Global stack for indentifier variables in parse tree.
static stack <s_token> variables;
static stack <string> assemblyTempVarValues, globalVars, globalVarValues;
static int varCount = 0, accumulator = 0, temporaries = 1;

void generateTarget(p_node *root){ 
    //Return when leaf has been reached.    
    if(root == NULL){
        return;
    }
    
    int x, previousVarCount = 0;
    
    //Reset variable counter if in block node.
    if (root->label == "block"){
        inMainBlock = true;
        previousVarCount = varCount;
        varCount = 0;
    }
    
    verifyStaticSem(root);
    
    if(root->label == "expr"){
        if(root->tokens[0] != NULL){
            string tempVar = "T"+to_string(temporaries);
            temporaries++;
            generateTarget(root->children[1]);
            //Write("STORE "+tempVar);
            generateTarget(root->children[0]);
            if(root->tokens[0]->tokenInstance == "+"){
                //Write("ADD "+tempVar);
            }
            else if(root->tokens[0]->tokenInstance == "-"){
                //Write("SUB "+tempVar);
            }
        }
        else{
            generateTarget(root->children[0]);
        }
    }
    else if(root->label == "A"){
        if(root->tokens[0] != NULL){
            string tempVar = "T"+to_string(temporaries);
            temporaries++;
            generateTarget(root->children[1]);
            //Write("STORE "+tempVar);
            generateTarget(root->children[0]);
            //Write("MULT "+tempVar);
        }
        else{
            generateTarget(root->children[0]);
        }
    }
    else if(root->label == "N"){
        if(root->tokens[0] != NULL){
            string tempVar = "T"+to_string(temporaries);
            temporaries++;
            generateTarget(root->children[1]);
            //Write("STORE "+tempVar);
            generateTarget(root->children[0]);
            //Write("DIV "+tempVar);
        }
        else{
            generateTarget(root->children[0]);
        }
    }
    else if(root->label == "M"){
        if(root->tokens[0] != NULL){
            generateTarget(root->children[0]);
            //Write("MULT -1");
        }
        else{
            generateTarget(root->children[0]);
        }
    }
    else if(root->label == "R"){
        if(root->tokens[0] != NULL){
            if(root->tokens[0]->id == "Identifier" || root->tokens[0]->id == "Integer"){
                //Write("LOAD "+root->tokens[0]->tokenInstance);
            }
        }
        else{
            generateTarget(root->children[0]);
        }
    }
    else{
        //Traversing child nodes.
        for (x = 0; x < CHILD_SIZE; x++){
            generateTarget(root->children[x]);
        }
    }

    //Resetting variable count and removing current block variables.
    if(root->label == "block"){
        for(x = 0; x < varCount; x++){
            variables.pop();
        }
        varCount = previousVarCount;
    }
}

//Function to verify if a indentifier is already in the stack.
static int verify(stack <s_token> varList, string currentId){
    int x;
    while(!varList.empty()){
        if (varList.top().tokenInstance == currentId){
            return varList.top().lineNumber;
        }
        varList.pop();
    }

    return -1;
}

static void verifyStaticSem(p_node *root){
    int x, lineNum;
    if(root->label == "vars" && root->tokens[1] != NULL){
        if((lineNum = verify(variables, root->tokens[1]->tokenInstance)) == -1){
            variables.push(*(root->tokens[1]));
            
            //Adding local temporary ids and values
            if(inMainBlock){
                assemblyTempVarValues.push(root->tokens[2]->tokenInstance);
            }
            else{
                globalVars.push(root->tokens[1]->tokenInstance);
                globalVarValues.push(root->tokens[2]->tokenInstance);
            }
            
            varCount++;
            return;
        }
        //If variable is already in global stack.
        else{
            errorHandler(ERROR_CODE1, root, lineNum);
        }
    }
    //Checking usage of an identifer and verifying if valid.
    else{
        for (x = 0; x < TOKEN_SIZE; x++){
            if (root->tokens[x] != NULL){
                if(root->tokens[x]->id == "Identifier"){
                    if(verify(variables, root->tokens[x]->tokenInstance)== -1){
                        errorHandler(ERROR_CODE2, root, x);
                    }
                }
            }
        }
    }
    return;
}

//Handler for semantic errors.
static void errorHandler(int errorCode, p_node *root, int lineNumber){
    if(errorCode == 1){
        cout << "Semantics Error! " << root->tokens[1]->tokenInstance 
                 << " already declared on line " 
                 << lineNumber << ". Error on line " 
                 << root->tokens[1]->lineNumber << "." << endl;
    }
    else if(errorCode == 2){
        cout << "Semantics Error! " << root->tokens[lineNumber]->tokenInstance
             << " has not been declared on line " 
             << root->tokens[lineNumber]->lineNumber << "." << endl;
    }
    exit(-1);
}