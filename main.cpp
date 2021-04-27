#include <iostream>
#include <fstream>
#include <regex>
#include <string>

#include "assignments.h"
#include "outputService.h"
#include "evaluation.h"
#include "functions.h"

using namespace std;

int main()
{
    string syntaxErrorMessage = "";
    string fileName = "deliverables/inputs/testcase4.my";
    ifstream inputFile;
    inputFile.open(fileName);
    OutputService::outputFile.open("deneme.ll");

    string currentLine;
    int currentLineIndex = 0;
    while (getline(inputFile, currentLine))
    {

        currentLineIndex++;
        currentLine.erase(std::remove(currentLine.begin(), currentLine.end(), '\t'), currentLine.end());
        currentLine.erase(std::remove(currentLine.begin(), currentLine.end(), '\r'), currentLine.end());
        if (currentLine == "" || currentLine[0] == '#')
            continue;
        if (currentLine.find("#") != string::npos)
            currentLine = currentLine.substr(0, currentLine.find("#"));

        if (currentLine.find("choose") != string::npos)
        {
            currentLine = handleChooseLine(currentLine);
        }

        if (checkAssignmentSyntax(currentLine))
            handleAssignmentLine(currentLine);

        else if (currentLine.find("while") != string::npos || currentLine.find("if") != string::npos)
        {
            // While Loop

            int openIndex = currentLine.find_first_of("(");
            int closeIndex = currentLine.find_first_of(")");
            string condition = currentLine.substr(openIndex + 1, closeIndex - openIndex - 1);
            string whileLine = "";
            vector<string> whileLines;

            while (true)
            {
                getline(inputFile, whileLine);
                whileLine.erase(std::remove(whileLine.begin(), whileLine.end(), '\t'), whileLine.end());
                whileLine.erase(std::remove(whileLine.begin(), whileLine.end(), '\r'), whileLine.end());
                if (whileLine.find("#") != string::npos)
                    whileLine = whileLine.substr(0, whileLine.find("#"));
                if (whileLine.find_first_of("}") != string::npos)
                    break;
                whileLines.push_back(whileLine);
                currentLineIndex++;
            }

            OutputService::addLine("br label %cond_" + to_string(currentLineIndex));
            OutputService::addLine("cond_" + to_string(currentLineIndex) + ":");
            string conditionTempName = evaluateExpression(condition); //TODO beware of spaces and tabs
            // %t2 = icmp ne i32 %t1, 0
            string result = OutputService::getTempName();
            OutputService::addLine(result + " = icmp ne i32 " + conditionTempName + ", 0");
            OutputService::addLine("br i1 " + result + ", label %body_" + to_string(currentLineIndex) + ", label %end_" + to_string(currentLineIndex));
            OutputService::addLine("body_" + to_string(currentLineIndex) + ":");
            for (int i = 0; i < whileLines.size(); i++)
            {
                string currentWhileLine = whileLines[i];
                if (currentWhileLine.find("choose") != string::npos)
                {
                    currentWhileLine = handleChooseLine(currentWhileLine);
                }
                if (checkAssignmentSyntax(currentWhileLine))
                    handleAssignmentLine(currentWhileLine);
                else if (currentWhileLine.find("print(") != string::npos && regex_match(currentWhileLine, RegexController::printController))
                {

                    handlePrintLine(currentWhileLine);
                }
                else
                    syntaxErrorMessage = "Syntax error on line " + to_string(currentLineIndex - whileLines.size() + i);
            }
            if (currentLine.find("while") != string::npos)
                OutputService::addLine("br label %cond_" + to_string(currentLineIndex));

            else
                OutputService::addLine("br label %end_" + to_string(currentLineIndex));

            OutputService::addLine("end_" + to_string(currentLineIndex) + ":");
        }
        else if (currentLine.find("print(") != string::npos && regex_match(currentLine, RegexController::printController))
        {

            handlePrintLine(currentLine);
        }

        else
        {
            syntaxErrorMessage = "Syntax error on line " + to_string(currentLineIndex);

            break;
        }
    }
    if (syntaxErrorMessage == "")
    {
        OutputService::outputFile << "; ModuleID = 'mylang2ir'\ndeclare i32 @printf(i8*, ...)\n@print.str = constant [4 x i8] c\"%d\\0A\\00\"\n"
                                  << endl;
        OutputService::chooseFunction();
        OutputService::outputFile << "define i32 @main() {" << endl;

        OutputService::vectorsToFile();
    }
    else
        OutputService::outputFile << syntaxErrorMessage << endl;

    OutputService::outputFile.close();
    inputFile.close();
    return 0;
}