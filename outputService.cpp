#include "outputService.h"
#include <iostream>
// Since we need to have the allocs and store 0 lines at the top of
// the main, we had to use 3 vectors to protect the order.
vector<string> OutputService::allocLines; // %number = alloca i32
vector<string> OutputService::storeLines; // store i32 0, i32* %number
vector<string> OutputService::lines;      // Every other line
ofstream OutputService::outputFile;
int OutputService::tempCount = 1;   // Since temp variables must be unique, we increment the count
int OutputService::currentLine = 0; // Line index
string OutputService::getTempName()
{
    // Temporary variable name must be something that cannot be in testcases so we just made something up.
    // Muhammet speaking, I wanted to use "improversation" but it affects the file size much more than I thought :(((
    string varName = "%t9t1" + to_string(OutputService::tempCount++);
    return varName;
}

void OutputService::addLine(string line)
{
    OutputService::lines.push_back(line);
}
void OutputService::chooseFunction()
{   
    // Add choose function to .ll file
    OutputService::outputFile << "define i32 @choose( i32 %expr1, i32 %expr2, i32 %expr3, i32 %expr4){\n entry:\n %a = icmp eq i32 %expr1, 0\n br i1 %a, label %return1, label %entry2\n return1:\n ret i32 %expr2\n entry2:\n %b = icmp sgt i32 %expr1, 0\n br i1 %b, label %return2, label %exit\n return2:\n ret i32 %expr3\n exit:\n ret i32 %expr4\n }\n\n"
                              << endl;
}
void OutputService::vectorsToFile()
{
    // Starting to write to .ll file. Adding alloc lines
    for (string line : OutputService::allocLines)
        OutputService::outputFile << line << endl;
    OutputService::outputFile << endl;
    //Adding store lines
    for (string line : OutputService::storeLines)
        OutputService::outputFile << line << endl;
    OutputService::outputFile << endl;
    // Adding other lines
    for (string line : OutputService::lines)
        OutputService::outputFile << line << endl;
    OutputService::outputFile << endl;
}