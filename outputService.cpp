#include "outputService.h"

vector<string> OutputService::allocLines;
vector<string> OutputService::storeLines;
vector<string> OutputService::lines;
ofstream OutputService::outputFile;

void OutputService::addLine(string line)
{
    OutputService::lines.push_back(line);
}
void OutputService::printer(string variableName)
{
    OutputService::addLine("call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 %" + variableName + " )");
}

void OutputService::vectorsToFile()
{
    OutputService::addLine("\nret i32 0\n}");

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