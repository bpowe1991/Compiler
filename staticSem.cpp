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

static void errorHandler(int, p_node*, std::ofstream*);
static void verify(p_node*, std::ofstream*);
static int find(stack <s_token>, string);
static string generateName(string);
static void exprCode(p_node*, ofstream*);
static void ACode(p_node*, ofstream*);
static void NCode(p_node*, ofstream*);
static void MCode(p_node*, ofstream*);
static void RCode(p_node*, ofstream*);
static void inCode(p_node*, ofstream*);
static void outCode(p_node*, ofstream*);
static void assignCode(p_node*, ofstream*);

static bool inMainBlock = false;

//Global stack for indentifier variables in parse tree.
static stack <s_token> variables;
static stack <string> values;
static stack <string> assemblyTempVarValues, globalVars, globalVarValues;
static int varCount = 0, labels = 0, temporaries = 0;

void generateTarget(p_node *root, ofstream* targetFile){ 
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
    
    //Checking semantics.
    verify(root, targetFile);

    for(x = 0; x < CHILD_SIZE; x++){
        generateTarget(root->children[x], targetFile);
    }
    
    //Resetting variable count and removing current block variables.
    if(root->label == "block"){
        for(x = 0; x < varCount; x++){
            variables.pop();
            *targetFile << "POP" << endl;
        }
        varCount = previousVarCount;
    }
}

static int find(stack <s_token> globalStack, string identifier){
    int index = 0;
    while(!globalStack.empty()){
        if(globalStack.top().tokenInstance == identifier){
            return index;
        }
        index++;
        globalStack.pop();
    }

    return -1;
}

static void verify(p_node *root, ofstream *targetFile){
    int x;
    if(root->label == "vars" && root->tokens[1] != NULL){
        if(find(variables, root->tokens[1]->tokenInstance) == -1){
            variables.push(*(root->tokens[1]));
            *targetFile << "PUSH" << endl 
                        << "LOAD " << root->tokens[2] << endl
                        << "STACKW 0" << endl;
            varCount++;
        }
        else{
            errorHandler(ERROR_CODE1, root, targetFile);
        }
    }
    else{
        for(x = 0; x < TOKEN_SIZE; x++){
            if(root->tokens[x] != NULL){
                if(root->tokens[x]->id == "Identifier"){
                    if(find(variables, root->tokens[x]->tokenInstance) == -1){
                        errorHandler(ERROR_CODE2, root, targetFile);
                    }
                }
            }
        }
    }
    return;
}

//Handler for semantic errors.
static void errorHandler(int errorCode, p_node *root, ofstream *targetFile){
    if(errorCode == ERROR_CODE1){
        cout << "Semantics Error! " << root->tokens[1]->tokenInstance 
                 << " redefined on line " 
                 << root->tokens[1]->lineNumber << "." <<endl;
    }
    else if(errorCode == ERROR_CODE2){
        cout << "Semantics Error! " << root->tokens[1]->tokenInstance
             << " undefined on line " 
             << root->tokens[1]->lineNumber << "." << endl;
    }
    targetFile->close();
    targetFile = NULL;
    exit(-1);
}

//Function to create temporary names and label names
static string generateName(string type){
    string name;
    if(type == "T"){
        temporaries++;
        name = type + to_string(temporaries);
    }
    else{
        labels++;
        name = type + to_string(labels);
    }
    return name;
}

static void exprCode(p_node *root, ofstream *targetFile){
    if(root->tokens[0] != NULL){
        string tempVar = generateName("T");
        generateTarget(root->children[1], targetFile);
        *targetFile << "STORE " << tempVar << endl;
        generateTarget(root->children[0], targetFile);
        if(root->tokens[0]->tokenInstance == "+"){
            *targetFile << "ADD "<< tempVar << endl;
        }
        else if(root->tokens[0]->tokenInstance == "-"){
            *targetFile << "SUB "<< tempVar << endl;
        }
    }
    else{
        generateTarget(root->children[0], targetFile);
    }
}

static void ACode(p_node *root, ofstream *targetFile){
    if(root->tokens[0] != NULL){
        string tempVar = generateName("T");
        generateTarget(root->children[1], targetFile);
        *targetFile << "STORE " << tempVar << endl;
        generateTarget(root->children[0], targetFile);
        *targetFile << "MULT " << tempVar << endl;
    }
    else{
        generateTarget(root->children[0], targetFile);
    }
}

static void NCode(p_node *root, ofstream *targetFile){
    if(root->tokens[0] != NULL){
        string tempVar = generateName("T");
        generateTarget(root->children[1], targetFile);
        *targetFile << "STORE " << tempVar << endl;
        generateTarget(root->children[0], targetFile);
        *targetFile << "DIV " << tempVar << endl;
    }
    else{
        generateTarget(root->children[0], targetFile);
    }
}

static void MCode(p_node *root, ofstream *targetFile){
    if(root->tokens[0] != NULL){
        generateTarget(root->children[0], targetFile);
        *targetFile << "MULT -1" << endl;
    }
    else{
        generateTarget(root->children[0], targetFile);
    }
}

static void RCode(p_node *root, ofstream *targetFile){
    if(root->tokens[0] != NULL){
        if(root->tokens[0]->id == "Integer"){
            *targetFile << "LOAD " << root->tokens[0]->tokenInstance << endl;
        }
        else{
            *targetFile << "STACKR " << find(variables, root->tokens[0]->tokenInstance) << endl;
        }
    }
    else{
        generateTarget(root->children[0], targetFile);
    }
}

static void inCode(p_node *root, ofstream *targetFile){
    string tempVar = generateName("T");
    *targetFile  << "READ " << tempVar << endl 
                 << "LOAD " << tempVar << endl
                 << "STACKW " << find(variables, root->tokens[1]->tokenInstance);
}

static void outCode(p_node *root, ofstream *targetFile){
        string tempVar = generateName("T");
        generateTarget(root->children[0], targetFile); 
        *targetFile << "STORE " << tempVar << endl
                    << "WRITE " << tempVar << endl;
}

static void assignCode(p_node *root, ofstream *targetFile){
    generateTarget(root->children[0], targetFile);
    *targetFile << "STACKW " << find(variables, root->tokens[0]->tokenInstance);
}

// if(root->label == "expr"){
    //     if(root->tokens[0] != NULL){
    //         string tempVar = generateName("T");
    //         generateTarget(root->children[1], targetFile);
    //         *targetFile << "STORE " << tempVar << endl;
    //         generateTarget(root->children[0], targetFile);
    //         if(root->tokens[0]->tokenInstance == "+"){
    //             *targetFile << "ADD "<< tempVar << endl;
    //         }
    //         else if(root->tokens[0]->tokenInstance == "-"){
    //             *targetFile << "SUB "<< tempVar << endl;
    //         }
    //     }
    //     else{
    //         generateTarget(root->children[0], targetFile);
    //     }
    // }
    // else if(root->label == "A"){
    //     if(root->tokens[0] != NULL){
    //         string tempVar = generateName("T");
    //         generateTarget(root->children[1], targetFile);
    //         *targetFile << "STORE " << tempVar << endl;
    //         generateTarget(root->children[0], targetFile);
    //         *targetFile << "MULT " << tempVar << endl;
    //     }
    //     else{
    //         generateTarget(root->children[0], targetFile);
    //     }
    // }
    // else if(root->label == "N"){
    //     if(root->tokens[0] != NULL){
    //         string tempVar = generateName("T");
    //         generateTarget(root->children[1], targetFile);
    //         *targetFile << "STORE " << tempVar << endl;
    //         generateTarget(root->children[0], targetFile);
    //         *targetFile << "DIV " << tempVar << endl;
    //     }
    //     else{
    //         generateTarget(root->children[0], targetFile);
    //     }
    // }
    // else if(root->label == "M"){
    //     if(root->tokens[0] != NULL){
    //         generateTarget(root->children[0], targetFile);
    //         *targetFile << "MULT -1" << endl;
    //     }
    //     else{
    //         generateTarget(root->children[0], targetFile);
    //     }
    // }
    // else if(root->label == "R"){
    //     if(root->tokens[0] != NULL){
    //         if(root->tokens[0]->id == "Integer"){
    //             *targetFile << "LOAD " << root->tokens[0]->tokenInstance << endl;
    //         }
    //         else{
    //            *targetFile << "STACKR " << find(variables, root->tokens[0]->tokenInstance) << endl;
    //         }
    //     }
    //     else{
    //         generateTarget(root->children[0], targetFile);
    //     }
    // }
    // else if(root->label == "in"){
    //     string tempVar = generateName("T");
    //     *targetFile  << "READ " << tempVar << endl 
    //                  << "LOAD " << tempVar << endl
    //                  << "STACKW " << find(variables, root->tokens[1]->tokenInstance);
    // }
    // else if(root->label == "out"){
    //     string tempVar = generateName("T");
    //     generateTarget(root->children[0]); 
    //     *targetFile << "STORE " << tempVar << endl
    //                 << "WRITE " << tempVar << endl;
    // }
    // else if(root->label == "assign"){
    //     generateTarget(root->children[0]);
    //     *targetFile << "STACKW " << find(variables, root->tokens[0]->tokenInstance);
    // }
    // else{
    //     //Traversing child nodes.
    //     for (x = 0; x < CHILD_SIZE; x++){
    //         generateTarget(root->children[x]);
    //     }
    // }