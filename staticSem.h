/*
Programmer: Briton A. Powe          Program: frontEnd
Date: 4/23/19                       Class: Compilers
File: staticSem.h
------------------------------------------------------------------------
Program Description:
This is the header file for verifying static semantics. 
*/

#ifndef STATIC_SEM
#define STATIC_SEM

#include <fstream>
#include <sstream>
#include "parser.h"

extern std::stringstream targetFile;

void generateTarget(p_node*);

#endif 