#include <iostream>
#include <fstream>
#include <regex>
#include <string>

#include "assignments.h"
using namespace std;

int main()
{
    string fileName = "m.my";
    ifstream inputFile;
    inputFile.open(fileName);

    string currentLine;
    int currentLineIndex = 1;
    while (getline(inputFile, currentLine))
    {
        cout << "currentLine: " << currentLineIndex << "\t";
        // evaluateExpression(currentLine);
        // cout << "Given Line: " << currentLine;
        bool isValidLine = checkAssignmentSyntax(currentLine);
        if (isValidLine)
            cout << "Syntax is Correct!" << endl;
        else
        {
            cout << "Syntax error on line " << currentLineIndex << endl;
            break;
        }
        currentLineIndex++;
    }

    inputFile.close();
    return 0;
}