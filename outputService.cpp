#include "outputService.h"
#include <iostream>
vector<string> OutputService::allocLines;
vector<string> OutputService::storeLines;
vector<string> OutputService::lines;
ofstream OutputService::outputFile;
int OutputService::tempCount = 1;
int OutputService::currentLine = 0;
string OutputService::getTempName()
{
    string varName = "%t9t1" + to_string(OutputService::tempCount++);
    // cout << varName;
    return varName;
}

void OutputService::addLine(string line)
{
    OutputService::lines.push_back(line);
}
void OutputService::printer(string variableName)
{
    OutputService::addLine("call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 %" + variableName + " )");
}
void OutputService::chooseFunction()
{
    OutputService::outputFile << "define i32 @choose( i32 %expr1, i32 %expr2, i32 %expr3, i32 %expr4){\n entry:\n %a = icmp eq i32 %expr1, 0\n br i1 %a, label %return1, label %entry2\n return1:\n ret i32 %expr2\n entry2:\n %b = icmp sgt i32 %expr1, 0\n br i1 %b, label %return2, label %exit\n return2:\n ret i32 %expr3\n exit:\n ret i32 %expr4\n }\n\n"<<endl;
}
void OutputService::vectorsToFile()
{
    // OutputService::addLine("\nret i32 0\n}");

    // Starting to write to .ll file. Do not add from now on.
    for (string line : OutputService::allocLines)
        OutputService::outputFile << line << endl;
    OutputService::outputFile << endl;

    for (string line : OutputService::storeLines)
        OutputService::outputFile << line << endl;
    OutputService::outputFile << endl;

    for (string line : OutputService::lines)
        OutputService::outputFile << line << endl;
    OutputService::outputFile << endl;
}