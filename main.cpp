#include <iostream>
#include <fstream>
#include <regex>
#include <string>

#include "assignments.h"
#include "outputService.h"

using namespace std;

int main()
{
    string fileName = "m.my";
    ifstream inputFile;
    inputFile.open(fileName);
    OutputService::outputFile.open("deneme.ll");

    OutputService::outputFile << "; ModuleID = 'mylang2ir'\ndeclare i32 @printf(i8*, ...)\n@print.str = constant [4 x i8] c\"%d\\0A\\00\"\n"
                              << endl;
    OutputService::outputFile << "define i32 @main() {" << endl;
    ;

    string currentLine;
    int currentLineIndex = 0;
    while (getline(inputFile, currentLine))
    {
        currentLineIndex++;
        cout << "currentLine: " << currentLineIndex << "\t";

        bool isAssignmentLine = checkAssignmentSyntax(currentLine);
        if (isAssignmentLine)
        {
            currentLine.erase(std::remove(currentLine.begin(), currentLine.end(), ' '), currentLine.end());

            int equalSignIndex = currentLine.find("=");
            string varName = currentLine.substr(0, equalSignIndex);
            OutputService::allocLines.push_back("%" + varName + " = alloca i32");
            OutputService::storeLines.push_back("store i32 0, i32* %" + varName);
            string lastUsedTempVar = "t5"; //evaluateExpression(currentLine.substr(equalSignIndex+1));
            OutputService::addLine("store i32 %" + lastUsedTempVar + " i32* %" + varName);
        }

        else
        {
            cout << "Syntax error on line " << currentLineIndex << endl;
            break;
        }
    }

    
    OutputService::vectorsToFile();

    OutputService::outputFile.close();
    inputFile.close();
    return 0;
}