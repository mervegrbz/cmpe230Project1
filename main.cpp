#include <iostream>
#include <fstream>
#include <regex>
#include <string>

#include "assignments.h"
#include "outputService.h"
#include "evaluation.h"
#include "functions.h"

using namespace std;
//11 14 15 17

int main(int argCount, char *argv[])
{
    string fileName = argv[1];
    int nameLength = fileName.find_last_of(".my");
    string outFileName = fileName.substr(0, nameLength - 2) + ".ll";
    int syntaxErrorMessage = -1;
    ifstream inputFile;
    inputFile.open(fileName);
    OutputService::outputFile.open(outFileName);

    string currentLine;
    OutputService::currentLine = 0;

    try
    {
        while (getline(inputFile, currentLine))
        {
            OutputService::currentLine++;
            currentLine = regex_replace(currentLine, RegexController::zeroController, "0");
            currentLine.erase(std::remove(currentLine.begin(), currentLine.end(), '\t'), currentLine.end());
            currentLine.erase(std::remove(currentLine.begin(), currentLine.end(), '\r'), currentLine.end());
            if (currentLine == "" || currentLine[0] == '#')
                continue;
            if (currentLine.find("#") != string::npos)
                currentLine = currentLine.substr(0, currentLine.find("#"));
            if (currentLine.find("choose") != string::npos)
            {
                currentLine = handleChooseLine(currentLine);
                if (currentLine.find("choose") != string::npos)
                {
                    syntaxErrorMessage = OutputService::currentLine - 1;
                    break;
                }
            }

            if (checkAssignmentSyntax(currentLine))
                handleAssignmentLine(currentLine);

            else if (currentLine.find("while") != string::npos || currentLine.find("if") != string::npos)
            {
                int openIndex = currentLine.find_first_of("(");
                int closeIndex = currentLine.find_last_of(")");

                if (currentLine.find("{") == string::npos)
                {
                    syntaxErrorMessage = OutputService::currentLine - 1;
                    break;
                }
                string condition = currentLine.substr(openIndex + 1, closeIndex - openIndex - 1);
                evaluateExpression(condition);
                string whileLine = "";
                vector<string> whileLines;
                bool isClosed = false;
                while (getline(inputFile, whileLine))
                {
                    OutputService::currentLine++;
                    whileLine = regex_replace(whileLine, RegexController::zeroController, "0");
                    whileLine.erase(std::remove(whileLine.begin(), whileLine.end(), '\t'), whileLine.end());
                    whileLine.erase(std::remove(whileLine.begin(), whileLine.end(), '\r'), whileLine.end());
                    whileLine.erase(std::remove(whileLine.begin(), whileLine.end(), ' '), whileLine.end());
                    if (whileLine == "" || whileLine[0] == '#')
                        continue;
                    if (whileLine.find("#") != string::npos)
                        whileLine = whileLine.substr(0, whileLine.find("#"));
                    if (whileLine.find("while") != string::npos || whileLine.find("if") != string::npos)
                    {
                        syntaxErrorMessage = OutputService::currentLine - 1;
                        break;
                    }
                    if (whileLine.find_first_of("}") != string::npos)
                    {
                        isClosed = true;
                        break;
                    }
                    whileLines.push_back(whileLine);
                }
                if (syntaxErrorMessage != -1)
                    break;
                if (!isClosed)
                    syntaxErrorMessage = OutputService::currentLine - 1;

                OutputService::addLine("br label %cond_" + to_string(OutputService::currentLine));
                OutputService::addLine("cond_" + to_string(OutputService::currentLine) + ":");
                string result = OutputService::getTempName();
                string conditionTempName = evaluateExpression(condition);
                OutputService::addLine(result + " = icmp ne i32 " + conditionTempName + ", 0");
                OutputService::addLine("br i1 " + result + ", label %body_" + to_string(OutputService::currentLine) + ", label %end_" + to_string(OutputService::currentLine));
                OutputService::addLine("body_" + to_string(OutputService::currentLine) + ":");
                for (int i = 0; i < whileLines.size(); i++)
                {
                    string currentWhileLine = whileLines[i];
                    if (currentWhileLine.find("choose") != string::npos)
                    {
                        currentWhileLine = handleChooseLine(currentWhileLine);
                        if (currentWhileLine.find("choose") != string::npos)
                        {
                            syntaxErrorMessage = OutputService::currentLine - whileLines.size() + i - 1;
                            break;
                        }
                    }
                    if (checkAssignmentSyntax(currentWhileLine))
                        handleAssignmentLine(currentWhileLine);
                    else if (currentWhileLine.find("print(") != string::npos && regex_match(currentWhileLine, RegexController::printController))
                        handlePrintLine(currentWhileLine);
                    else
                        syntaxErrorMessage = OutputService::currentLine - whileLines.size() + i - 1;
                }
                if (syntaxErrorMessage != -1)
                    break;
                if (currentLine.find("while") != string::npos)
                    OutputService::addLine("br label %cond_" + to_string(OutputService::currentLine));

                else
                    OutputService::addLine("br label %end_" + to_string(OutputService::currentLine));

                OutputService::addLine("end_" + to_string(OutputService::currentLine) + ":");
            }
            else if (currentLine.find("print(") != string::npos && checkPrintSyntax(currentLine))
                handlePrintLine(currentLine);
            else
            {
                syntaxErrorMessage = OutputService::currentLine - 1;
                break;
            }
        }
    }
    catch (int line)
    {
        syntaxErrorMessage = line;
    }

    OutputService::outputFile << "; ModuleID = 'mylang2ir'\ndeclare i32 @printf(i8*, ...)\n@print.str = constant [4 x i8] c\"%d\\0A\\00\"\n@syntaxPrint = constant [23 x i8] c\"Line %d: syntax error\\0A\\00\"" << endl;
    OutputService::chooseFunction();
    OutputService::outputFile << "define i32 @main() {" << endl;
    if (syntaxErrorMessage == -1)
        OutputService::vectorsToFile();
    else
        OutputService::outputFile << "call i32 (i8*, ...)* @printf(i8* getelementptr ([23 x i8]* @syntaxPrint, i32 0, i32 0), i32 " + to_string(syntaxErrorMessage) + " )" << endl;

    OutputService::outputFile << "\nret i32 0\n}" << endl;
    OutputService::outputFile.close();
    inputFile.close();
    return 0;
}