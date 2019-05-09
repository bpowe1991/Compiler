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
#include <string>

using namespace std;

#define TOKEN_SIZE 3
#define CHILD_SIZE 4
#define ERROR_CODE1 1
#define ERROR_CODE2 2

static void errorHandler(int, p_node*);
static void verify(p_node*);
static int find(stack <s_token>, string, int);
static void treeTraversal(p_node*);
static string generateName(string);
static void exprCode(p_node*);
static void ACode(p_node*);
static void NCode(p_node*);
static void MCode(p_node*);
static void RCode(p_node*);
static void inCode(p_node*);
static void outCode(p_node*);
static void assignCode(p_node*);
static void ifCode(p_node*);
static void loopCode(p_node*);
static void ROCode(p_node*, string);

stringstream targetFile;

static bool inMainBlock = false;

//Global stack for indentifier variables in parse tree.
static stack <s_token> variables;
static int varCount = 0, labels = 0, temporaries = 0, globalCount = 0;

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
    
    //Checking semantics.
    verify(root);
    treeTraversal(root);
    
    //Resetting variable count and removing current block variables.
    if(root->label == "block"){
        for(x = 0; x < varCount; x++){
            variables.pop();
            targetFile << "POP" << endl;
        }
        varCount = previousVarCount;
    }
    else if(root->label == "program"){
        targetFile << "STOP" << endl;
        for(x = 1; x <= temporaries; x++){
            targetFile << "T" << x << " 0" << endl;
        }
    }
}

//Function that return the location of a variable on the stacks
static int find(stack <s_token> globalStack, string identifier, int depth){
    int index = 0;
    if(depth == -1){
       while(!globalStack.empty()){
           if(globalStack.top().tokenInstance == identifier){
               return index;
           } 
           index++;
           globalStack.pop();
       }
   }
   else{
      for(index = 0; index < depth; index++){
         if(globalStack.top().tokenInstance == identifier){
            return index;
         }
      }
      while(globalStack.size() != globalCount){
         globalStack.pop();
         index++;
      }
      while(!globalStack.empty()){
          if(globalStack.top().tokenInstance == identifier){
              return index;
          }
          index++;
          globalStack.pop();
      }

   }

    return -1;
}

//Function to check static semantics and add variables to stacks
static void verify(p_node *root){
    int x;
    if(root->label == "vars" && root->tokens[1] != NULL){
        if(find(variables, root->tokens[1]->tokenInstance, varCount) == -1){
            variables.push(*(root->tokens[1]));
            targetFile << "PUSH" << endl 
                       << "LOAD " << root->tokens[2]->tokenInstance << endl
                       << "STACKW 0" << endl;
            varCount++;
	    if(inMainBlock == false){
               globalCount++;
            }
        }
        else{
            errorHandler(ERROR_CODE1, root);
        }
    }
    else{
        for(x = 0; x < TOKEN_SIZE; x++){
            if(root->tokens[x] != NULL){
                if(root->tokens[x]->id == "Identifier"){
                    if(find(variables, root->tokens[x]->tokenInstance, -1) == -1){
                        errorHandler(ERROR_CODE2, root);
                    }
                }
            }
        }
    }
    return;
}

//Handler for semantic errors.
static void errorHandler(int errorCode, p_node *root){
    if(errorCode == ERROR_CODE1){
        cout << "Semantics Error! " << root->tokens[1]->tokenInstance 
                 << " redefined on line " 
                 << root->tokens[1]->lineNumber << "." <<endl;
    }
    else if(errorCode == ERROR_CODE2){
        cout << "Semantics Error! " << root->tokens[0]->tokenInstance
             << " undefined on line " 
             << root->tokens[0]->lineNumber << "." << endl;
    }
    exit(-1);
}

//Function for tree traversal
static void treeTraversal(p_node *root){
    int x;
    if(root->label == "expr"){
        exprCode(root);
    }
    else if(root->label == "A"){
        ACode(root);
    }
    else if(root->label == "N"){
        NCode(root);
    }
    else if(root->label == "M"){
        MCode(root);
    }
    else if(root->label == "R"){
        RCode(root);
    }
    else if(root->label == "in"){
        inCode(root);
    }
    else if(root->label == "out"){
        outCode(root);
    }
    else if(root->label == "assign"){
        assignCode(root);
    }
    else if(root->label == "If"){
        ifCode(root);
    }
    else if(root->label == "loop"){
        loopCode(root);
    }
    else{
        for(x = 0; x < CHILD_SIZE; x++){
            generateTarget(root->children[x]);
        }
    }
}

//Function to create temporary names and label names
static string generateName(string type){
    string name;
    stringstream ss;
    if(type == "T"){
        temporaries++;
	ss << temporaries;
        name = type + ss.str();
    }
    else{
        labels++;
        ss << labels;
        name = type + ss.str();
    }
    return name;
}

//Function for expr node code generation
static void exprCode(p_node *root){
    if(root->tokens[0] != NULL){
        string tempVar = generateName("T");
        generateTarget(root->children[1]);
        targetFile << "STORE " << tempVar << endl;
        generateTarget(root->children[0]);
        if(root->tokens[0]->tokenInstance == "+"){
            targetFile << "ADD "<< tempVar << endl;
        }
        else if(root->tokens[0]->tokenInstance == "-"){
            targetFile << "SUB "<< tempVar << endl;
        }
    }
    else{
        generateTarget(root->children[0]);
    }
}

//Function for A node code generation
static void ACode(p_node *root){
    if(root->tokens[0] != NULL){
        string tempVar = generateName("T");
        generateTarget(root->children[1]);
        targetFile << "STORE " << tempVar << endl;
        generateTarget(root->children[0]);
        targetFile << "MULT " << tempVar << endl;
    }
    else{
        generateTarget(root->children[0]);
    }
}

//Function for N node code generation
static void NCode(p_node *root){
    if(root->tokens[0] != NULL){
        string tempVar = generateName("T");
        generateTarget(root->children[1]);
        targetFile << "STORE " << tempVar << endl;
        generateTarget(root->children[0]);
        targetFile << "DIV " << tempVar << endl;
    }
    else{
        generateTarget(root->children[0]);
    }
}

//Function for M node code generation
static void MCode(p_node *root){
    if(root->tokens[0] != NULL){
        generateTarget(root->children[0]);
        targetFile << "MULT -1" << endl;
    }
    else{
        generateTarget(root->children[0]);
    }
}

//Function for R node code generation
static void RCode(p_node *root){
    if(root->tokens[0] != NULL){
        if(root->tokens[0]->id == "Integer"){
            targetFile << "LOAD " << root->tokens[0]->tokenInstance << endl;
        }
        else{
            targetFile << "STACKR " << find(variables, root->tokens[0]->tokenInstance, -1) << endl;
        }
    }
    else{
        generateTarget(root->children[0]);
    }
}

//Function for in node code generation
static void inCode(p_node *root){
    string tempVar = generateName("T");
    targetFile  << "READ " << tempVar << endl 
                << "LOAD " << tempVar << endl
                << "STACKW " 
                << find(variables, root->tokens[1]->tokenInstance, -1) << endl;
}

//Function for out node code generation
static void outCode(p_node *root){
        string tempVar = generateName("T");
        generateTarget(root->children[0]); 
        targetFile << "STORE " << tempVar << endl
                   << "WRITE " << tempVar << endl;
}

//Function for assign node code generation
static void assignCode(p_node *root){
    generateTarget(root->children[0]);
    targetFile << "STACKW " 
               << find(variables, root->tokens[0]->tokenInstance, -1) << endl;
}

//Function for if node code generation
static void ifCode(p_node *root){
    generateTarget(root->children[2]);
    string tempVar = generateName("T");
    targetFile << "STORE " << tempVar << endl;
    generateTarget(root->children[0]);
    targetFile << "SUB " << tempVar << endl;
    string label = generateName("L");
    ROCode(root->children[1], label);
    generateTarget(root->children[3]);
    targetFile << label << ": NOOP" << endl;
}

//Function for loop node code generation
static void loopCode(p_node *root){
    string label1 = generateName("L");
    generateTarget(root->children[2]);
    string tempVar = generateName("T");
    targetFile << "STORE " << tempVar << endl;
    generateTarget(root->children[0]);
    targetFile << label1 << ": SUB " << tempVar << endl;
    string label2 = generateName("L");
    ROCode(root->children[1], label2);
    generateTarget(root->children[3]);
    targetFile << "BR " << label1 << endl
               << label2 << ": NOOP" << endl;
}

//Function for RO node code generation
static void ROCode(p_node *root, string label){
    if(root->tokens[0]->tokenInstance == "<"){
        targetFile << "BRZPOS " << label << endl;
    }
    else if(root->tokens[0]->tokenInstance == ">"){
        targetFile << "BRZNEG " << label << endl;
    }
    else if(root->tokens[0]->tokenInstance == "="){
        if(root->tokens[1] != NULL){
            if(root->tokens[1]->tokenInstance == "<"){
                targetFile << "BRPOS " << label << endl;
            }
            else if(root->tokens[1]->tokenInstance == ">"){
                targetFile << "BRNEG " << label << endl;
            }
            else if(root->tokens[1]->tokenInstance == "="){
                targetFile << "BRZERO " << label << endl;
            }
        }
        else{
            targetFile << "BRPOS " << label << endl
                       << "BRNEG " << label << endl;
        }
    }
}
