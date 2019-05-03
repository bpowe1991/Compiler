/*
Programmer: Briton A. Powe          Program: frontEnd
Date: 4/15/19                       Class: Compilers
File: parser.cpp
------------------------------------------------------------------------
Program Description:
This is the main source file for the parser of frontEnd. Parser receives
tokens from the scanner and checks for correct semantics.
*/

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include "scanner.h"
#include "parser.h"

using namespace std;

static p_node* program();
static p_node* block();
static p_node* vars();
static p_node* expr();
static p_node* A();
static p_node* N();
static p_node* M();
static p_node* R();
static p_node* stats();
static p_node* stat();
static p_node* MStat();
static p_node* in();
static p_node* out();
static p_node* If();
static p_node* loop();
static p_node* assign();
static p_node* RO();
static p_node* newPNode(string);

//Main token pointer for scanners.
static s_token * token;

//Main parser function.
p_node* parser(){
    p_node * root;
    token = scannerDriver();
    root = program();
    return root;
}

//Starting production for parser.
static p_node* program(){
    p_node *node = newPNode("program");
    node->children[0] = vars();
    node->children[1] = block();

    if(token->id == "End of File"){
        return node;
    }
    
    cout << "Parser Error! EOF token expected. Recieved token " 
         << token->tokenInstance << "  line " << token->lineNumber << endl;
    exit(-1);
}

//Function for block production.
static p_node* block(){
    if(token->tokenInstance == "Begin"){
        p_node *node = newPNode("block");;
        token = scannerDriver();
        node->children[0] = vars();
        node->children[1] = stats();
        
        if(token->tokenInstance == "End"){
            token = scannerDriver();
            return node;
        }
        
        cout << "Parser Error! End token expected. Recieved token " 
             << token->tokenInstance << "  line " << token->lineNumber << endl;
        exit(-1);
    }

    cout << "Parser Error! Begin token expected. Recieved token " 
         << token->tokenInstance << "  line " << token->lineNumber << endl;
    exit(-1);
}

//Function for var production.
static p_node* vars(){
    if(token->tokenInstance == "INT"){
        p_node *node = newPNode("vars");
        token = scannerDriver();
        
        if(token->id == "Identifier"){
            node->tokens[1] = token;
            token = scannerDriver();
            
            if(token->id == "Integer"){
                node->tokens[2] = token;
                token = scannerDriver();
                node->children[0] = vars();
                return node;
            }
            else{
                cout << "Parser Error! Integer token expected. Recieved token " 
                     << token->tokenInstance << "  line " << token->lineNumber << endl;
                exit(-1);
            }
        }
        else{
            cout << "Parser Error! Identifier token expected. Recieved token " 
                 << token->tokenInstance << "  line " << token->lineNumber << endl;
            exit(-1);
        }
    }

    return NULL;
}

//Function for expr production
static p_node* expr(){
    p_node *node = newPNode("expr");
    node->children[0] = A();
    
    if(token->id == "Plus Operator" || token->id == "Minus Operator"){
        node->tokens[0] = token;
        token = scannerDriver();
        node->children[1] = expr();
        return node;
    }
    return node;
}

//Function for A production.
static p_node* A(){
    p_node *node = newPNode("A");
    node->children[0] = N();
    
    if(token->id == "Asterisk"){
        node->tokens[0] = token;
        token = scannerDriver();
        node->children[1] = A();
        return node;
    }
    return node;
}

//Function for N production.
static p_node* N(){
    p_node *node = newPNode("N");
    node->children[0] = M();
    
    if(token->id == "Forward Slash"){
        node->tokens[0] = token;
        token = scannerDriver();
        node->children[1] = N();
        return node;
    }
    return node;
}

//Function for M production.
static p_node* M(){
    p_node *node = newPNode("M");
    if(token->id == "Minus Operator"){
        node->tokens[0] = token;
        token = scannerDriver();
        node->children[0] = M();
        return node;
    }
    node->children[0] = R();
    return node;
}

//Function for R production.
static p_node* R(){
    p_node *node = newPNode("R");
    if(token->id == "Left Brace"){
        token = scannerDriver();
        node->children[0] = expr();
        
        if(token->id == "Right Brace"){
            token = scannerDriver();
            return node;
        }
        else{
            
            cout << "Parser Error! \"]\" token expected. Recieved token " 
                 << token->tokenInstance << "  line " << token->lineNumber << endl;
            exit(-1);
        }
    }
    else if(token->id == "Identifier"){
        node->tokens[0] = token;
        token = scannerDriver();
        return node;
    }
    else if(token->id == "Integer"){
        node->tokens[0] = token;
        token = scannerDriver();
        return node;
    }
    
    cout << "Parser Error! \"[\", Identifier, or Integer token expected. Recieved token " 
                 << token->tokenInstance << "  line " << token->lineNumber << endl;
    exit(-1);
}

//Function for stats production.
static p_node* stats(){
    p_node *node = newPNode("stats");
    node->children[0] = stat();
    
    if(token->id == "Colon"){
        token = scannerDriver();
        node->children[1] = MStat();
        return node;
    }
    
    cout << "Parser Error! \":\" token expected. Recieved token " 
                 << token->tokenInstance << "  line " << token->lineNumber << endl;
    exit(-1);
}

//Function for mstat production.
static p_node* MStat(){
    if((token->id == "Keyword" && token->tokenInstance != "End") ||
        token->id == "Identifier"){
        p_node *node = newPNode("mstat");
        node->children[0] = stat();
        
        if(token->id == "Colon"){
            token = scannerDriver();
            node->children[1] = MStat();
            return node;
        }
        else{
            cout << "Parser Error! \":\" token expected. Recieved token " 
                 << token->tokenInstance << "  line " << token->lineNumber << endl;
            exit(-1);
        }
    }

    return NULL;
}

//Function for stat production.
static p_node* stat(){
    p_node *node = newPNode("stat");
    if(token->tokenInstance == "Read"){      
        node->children[0] = in();
        return node;
    }
    else if(token->tokenInstance == "Output"){
        node->children[0] = out();
        return node;
    }
    else if(token->tokenInstance == "IFF"){
        node->children[0] = If();
        return node;
    }
    else if(token->tokenInstance == "Loop"){
        node->children[0] = loop();
        return node;
    }
    else if(token->id == "Identifier"){
        node->children[0] = assign();
        return node;
    }
    else if(token->tokenInstance == "Begin"){
        node->children[0] = block();
        return node;
    }
    else{
        cout << "Parser Error! stat Keyword token or Identifier token expected. Recieved token " 
             << token->tokenInstance << "  line " << token->lineNumber << endl;
        exit(-1);
    }
}

//Function for in production.
static p_node* in(){
    if(token->tokenInstance == "Read"){
        p_node *node = newPNode("in");
        node->tokens[0] = token;
        token = scannerDriver();
        
        if(token->id == "Left Brace"){
            token = scannerDriver();
            
            if(token->id == "Identifier"){
                node->tokens[1] = token;
                token = scannerDriver();
                
                if(token->id == "Right Brace"){
                    token = scannerDriver();
                    return node;
                }
                else{
                    cout << "Parser Error! \"]\" token expected. Recieved token " 
                        << token->tokenInstance << "  line " << token->lineNumber << endl;
                    exit(-1);
                }
            }
            else{
                cout << "Parser Error! Identifier token expected. Recieved token " 
                    << token->tokenInstance << "  line " << token->lineNumber << endl;
                exit(-1);
            }
        }
        else{
            cout << "Parser Error! \"[\" token expected. Recieved token " 
                << token->tokenInstance << "  line " << token->lineNumber << endl;
            exit(-1);
        }
    }
    else{
        cout << "Parser Error! Read token expected. Recieved token " 
             << token->tokenInstance << "  line " << token->lineNumber << endl;
        exit(-1);
    }
}

//Function for out production.
static p_node* out(){
    if(token->tokenInstance == "Output"){
        p_node *node = newPNode("out");
        node->tokens[0] = token;
        token = scannerDriver();
        
        if(token->id == "Left Brace"){
            token = scannerDriver();
            node->children[0] = expr();
            
            if(token->id == "Right Brace"){
                token = scannerDriver();
                return node;
            }
            else{
                cout << "Parser Error! \"]\" token expected. Recieved token " 
                        << token->tokenInstance << "  line " << token->lineNumber << endl;
                exit(-1);
            }
        }
        else{
            cout << "Parser Error! \"[\" token expected. Recieved token " 
                 << token->tokenInstance << "  line " << token->lineNumber << endl;
            exit(-1);
        }
    }
    else{
        cout << "Parser Error! Output token expected. Recieved token " 
             << token->tokenInstance << "  line " << token->lineNumber << endl;
        exit(-1);
    }
}

//Function for if production.
static p_node* If(){
    if(token->tokenInstance == "IFF"){
        p_node *node = newPNode("If");
        node->tokens[0] = token;
        token = scannerDriver();
        
        if(token->id == "Left Brace"){
            token = scannerDriver();
            node->children[0] = expr();
            node->children[1] = RO();
            node->children[2] = expr();
            
            if(token->id == "Right Brace"){
                token = scannerDriver();
                node->children[3] = stat();
                return node;
            }
            else{
                cout << "Parser Error! \"]\" token expected. Recieved token " 
                        << token->tokenInstance << "  line " << token->lineNumber << endl;
                exit(-1);
            }
        }
        else{
            cout << "Parser Error! \"[\" token expected. Recieved token " 
                << token->tokenInstance << "  line " << token->lineNumber << endl;
            exit(-1);
        }
    }
    else{
        cout << "Parser Error! IFF token expected. Recieved token " 
             << token->tokenInstance << "  line " << token->lineNumber << endl;
        exit(-1);
    }
}

//Function for in production.
static p_node* loop(){
    if(token->tokenInstance == "Loop"){
        p_node *node = newPNode("loop");
        node->tokens[0] = token;
        token = scannerDriver();
        
        if(token->id == "Left Brace"){
            token = scannerDriver();
            node->children[0] = expr();
            node->children[1] = RO();
            node->children[2] = expr();
            
            if(token->id == "Right Brace"){
                token = scannerDriver();
                node->children[3] = stat();
                return node;
            }
            else{
                cout << "Parser Error! \"]\" token expected. Recieved token " 
                        << token->tokenInstance << "  line " << token->lineNumber << endl;
                exit(-1);
            }
        }
        else{
            cout << "Parser Error! \"[\" token expected. Recieved token " 
                << token->tokenInstance << "  line " << token->lineNumber << endl;
            exit(-1);
        }
    }
    else{
        cout << "Parser Error! Loop token expected. Recieved token " 
             << token->tokenInstance << "  line " << token->lineNumber << endl;
        exit(-1);
    }
}

//Function for assign production.
static p_node* assign(){
    if(token->id == "Identifier"){
        p_node *node = newPNode("assign");
        node->tokens[0] = token;
        token = scannerDriver();
        
        if(token->id == "Equal Operator"){
            node->tokens[1] = token;
            token = scannerDriver();
            node->children[0] = expr();
            return node;
        }
        else{
            cout << "Parser Error! \"=\" token expected. Recieved token " 
                << token->tokenInstance << "  line " << token->lineNumber << endl;
            exit(-1);
        }
    }
    else{
        cout << "Parser Error! Identifier token expected. Recieved token " 
             << token->tokenInstance << "  line " << token->lineNumber << endl;
        exit(-1);
    }
}

//Function for relational operator production.
static p_node* RO(){
    p_node *node = newPNode("RO");
    if(token->id == "Greater Than Operator" || token->id == "Less Than Operator"){
        node->tokens[0] = token;
        token = scannerDriver();
        return node;
    }
    else if(token->id == "Equal Operator"){
        node->tokens[0] = token;
        token = scannerDriver();
        
        if(token->id == "Greater Than Operator" || 
           token->id == "Less Than Operator" ||
           token->id == "Equal Operator"){
            node->tokens[1] = token;
            token = scannerDriver();
            return node;
        }
        else{
            return node;
        }
    }
    else{
        cout << "Parser Error! \"=\", \"<\", or \">\" token expected. Recieved token " 
             << token->tokenInstance << "  line " << token->lineNumber << endl;
        exit(-1);
    }
}

//Function to produce new parser node
static p_node* newPNode(string label){ 
    int x;
    p_node *newNode =  new p_node; 
    newNode->label = label;
    
    for(x = 0; x < 3; x++){
        newNode->tokens[x] = NULL;
    }
    for(x = 0; x < 4; x++){
        newNode->children[x] = NULL;
    }
    
    return newNode; 
}

//Function to delete parse tree.
void deleteTree(p_node* node){ 
    if (node == NULL) return; 
    
    int x;
    for(x = 0; x < 4; x++){
        deleteTree(node->children[x]);
    }

    free(node); 
}  