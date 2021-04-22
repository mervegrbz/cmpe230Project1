#include <iostream>
#include <fstream>
#include <regex>
#include <string>

#include "assignments.h"
#include "outputService.h"
#include "evaluation.h"

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

    string currentLine;
    int currentLineIndex = 0;
    while (getline(inputFile, currentLine))
    {
        currentLineIndex++;

        if (checkAssignmentSyntax(currentLine))
            handleAssignmentLine(currentLine);

        else if (currentLine.find("while") != string::npos)
        {
            // While Loop

            int openIndex = currentLine.find_first_of("(");
            int closeIndex = currentLine.find_first_of(")");
            string condition = currentLine.substr(openIndex + 1, closeIndex - openIndex-1);
            string whileLine = "";
            vector<string> whileLines;

            while (true)
            {
                getline(inputFile, whileLine);
                if (whileLine.find_first_of("}") != string::npos)
                    break;
                whileLines.push_back(whileLine);
                currentLineIndex++;
            }

            OutputService::addLine("br label %whcond_" + to_string(currentLineIndex));
            OutputService::addLine("whcond_" + to_string(currentLineIndex) + ":");
            string conditionTempName = evaluateExpression(condition); //TODO beware of spaces and tabs
            // %t2 = icmp ne i32 %t1, 0
            string result = OutputService::getTempName();
            OutputService::addLine(result + " = icmp ne i32 " + conditionTempName + ", 0");
            OutputService::addLine("br i1 " + result + ", label %whbody_" + to_string(currentLineIndex) + ", label %whend_" + to_string(currentLineIndex));
            OutputService::addLine("whbody_" + to_string(currentLineIndex) + ":");
            for (int i = 0; i < whileLines.size(); i++)
            {
                if (checkAssignmentSyntax(whileLines[i]))
                    handleAssignmentLine(whileLines[i]);
                else
                    cout << "Syntax error on line " << currentLineIndex - whileLines.size() + i << endl;
            }
            OutputService::addLine("br label %whcond_" + to_string(currentLineIndex));
            OutputService::addLine("whend_" + to_string(currentLineIndex)+":");
        }

        else
        {
            break;
        }
    }

    OutputService::vectorsToFile();

    OutputService::outputFile.close();
    inputFile.close();
    return 0;
}