#include <iostream>
#include <fstream>
#include <regex>
#include <string>

#include "assignments.h"
#include "outputService.h"
#include "evaluation.h"
#include "functions.h"

using namespace std;

int main(int argCount, char *argv[])
{
    string fileName = argv[1];
    int nameLength = fileName.find_last_of(".my");
    string outFileName = fileName.substr(0, nameLength - 2) + ".ll";
    // In case of a syntax error, this variable will hold the status.
    int syntaxErrorMessage = -1;
    ifstream inputFile;
    inputFile.open(fileName);
    OutputService::outputFile.open(outFileName);

    string currentLine;
    // We had to make this a static variable to throw errors from other functions.
    OutputService::currentLine = 0;

    // Everything happens in a massive try/catch block. We thought printing syntax error in a case we cannot handle is better than seeing something like segmentation fault.
    try
    {
        while (getline(inputFile, currentLine))
        {
            OutputService::currentLine++;
            // In case you use zero-1 instead of -1.
            currentLine = regex_replace(currentLine, RegexController::zeroController, "0");
            currentLine.erase(std::remove(currentLine.begin(), currentLine.end(), '\t'), currentLine.end());
            currentLine.erase(std::remove(currentLine.begin(), currentLine.end(), '\r'), currentLine.end());
            currentLine.erase(std::remove(currentLine.begin(), currentLine.end(), ' '), currentLine.end());
            // Empty or comment line, skip
            if (currentLine == "" || currentLine[0] == '#')
                continue;
            // If line has a command at the end, get rid of command part
            if (currentLine.find("#") != string::npos)
                currentLine = currentLine.substr(0, currentLine.find("#"));
            // Handling choose function before checking anything else may not be a great idea but obviously much easier to implement.
            if (currentLine.find("choose") != string::npos)
            {
                currentLine = handleChooseLine(currentLine);
                // There cannot be a choose function after the above function. If there is, it indicates a syntax error the function couldn't handle.
                if (currentLine.find("choose") != string::npos)
                {
                    syntaxErrorMessage = OutputService::currentLine - 1;
                    break;
                }
            }
            // Handles the assignment lines. In most of the project we used regex to check syntaxes; however, regex is not very useful while dealing with
            // multiple layers of paranthesis. We had to use combination of regex and postfix to verify the syntax of an assignment line.You can find more
            //information about this on checkAssignmentSyntax function.
            if (checkAssignmentSyntax(currentLine))
                handleAssignmentLine(currentLine);
            // If line includes while or if
            else if (currentLine.find("while(") != string::npos || currentLine.find("if(") != string::npos)
            {
                                int openIndex = currentLine.find_first_of("(");
                int closeIndex = currentLine.find_last_of(")");
                // Line must include { character
                if (currentLine.find("{") == string::npos)
                {
                    syntaxErrorMessage = OutputService::currentLine - 1;
                    break;
                }

                string condition = currentLine.substr(openIndex + 1, closeIndex - openIndex - 1);
                // We had to use this silly way to check syntax of condition part and print the line if necessary. We will call this function with the same parameter to properly use on line 114
                evaluateExpression(condition);
                string whileLine = "";
                // Store lines to in a vector to use down below
                vector<string> whileLines;
                // If } is not exist, this boolean will hold the status
                bool isClosed = false;
                while (getline(inputFile, whileLine))
                {
                    OutputService::currentLine++;
                    whileLine = regex_replace(whileLine, RegexController::zeroController, "0");
                    whileLine.erase(std::remove(whileLine.begin(), whileLine.end(), '\t'), whileLine.end());
                    whileLine.erase(std::remove(whileLine.begin(), whileLine.end(), '\r'), whileLine.end());
                    whileLine.erase(std::remove(whileLine.begin(), whileLine.end(), ' '), whileLine.end());
                    // Skip empty line
                    if (whileLine == "" || whileLine[0] == '#')
                        continue;
                    // Cut the comment part
                    if (whileLine.find("#") != string::npos)
                        whileLine = whileLine.substr(0, whileLine.find("#"));
                    // No nested if and while blocks
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
                    // This part is very similar to former part so no need to comment again.
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
                    // If line has print in it
                    else if (currentWhileLine.find("print(") != string::npos && regex_match(currentWhileLine, RegexController::printController))
                        handlePrintLine(currentWhileLine);
                    else
                        syntaxErrorMessage = OutputService::currentLine - whileLines.size() + i - 1;
                }
                if (syntaxErrorMessage != -1)
                    break;
                // Add the loop functionality if it is a while loop
                if (currentLine.find("while") != string::npos)
                    OutputService::addLine("br label %cond_" + to_string(OutputService::currentLine));
                else
                    OutputService::addLine("br label %end_" + to_string(OutputService::currentLine));
                OutputService::addLine("end_" + to_string(OutputService::currentLine) + ":");
            }
            // If line has print in it
            else if (currentLine.find("print(") != string::npos && checkPrintSyntax(currentLine))
                handlePrintLine(currentLine);
            // All posibilities are checked and none of them were true
            else
            {
                syntaxErrorMessage = OutputService::currentLine - 1;
                break;
            }
        }
    }
    catch (int line)
    {
        // In case of an error we couldn't handle
        syntaxErrorMessage = line;
    }

    OutputService::outputFile << "; ModuleID = 'mylang2ir'\ndeclare i32 @printf(i8*, ...)\n@print.str = constant [4 x i8] c\"%d\\0A\\00\"\n@syntaxPrint = constant [23 x i8] c\"Line %d: syntax error\\0A\\00\"" << endl;
    // Adds choose function on top of the .ll file
    OutputService::chooseFunction();
    OutputService::outputFile << "define i32 @main() {" << endl;
    if (syntaxErrorMessage == -1)
        // Prints every line to .ll file
        OutputService::vectorsToFile();
    else
        OutputService::outputFile << "call i32 (i8*, ...)* @printf(i8* getelementptr ([23 x i8]* @syntaxPrint, i32 0, i32 0), i32 " + to_string(syntaxErrorMessage) + " )" << endl;

    OutputService::outputFile << "\nret i32 0\n}" << endl;
    OutputService::outputFile.close();
    inputFile.close();
    return 0;
}