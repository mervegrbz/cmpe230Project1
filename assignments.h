#include <string>
#include <unordered_set>
#include <regex>
#include <iostream>

using namespace std;
class RegexController
{
public:
    static string variableRegex;
    static string equationRegex;
    static string assignmentRegex;
    static string printRegex;
    static string zeroRegex;

    static regex assignmentController;
    static regex equationController;
    static regex variableController;
    static regex printController;
    static regex zeroController;
};
// Regex Definitions
void handleAssignmentLine(string currentLine);
bool checkExpressionSyntax(string expression);
bool checkVariableSyntax(string variable);
bool checkAssignmentSyntax(string assignmentLine);

#ifndef Variable_Class
#define Variable_Class
class Variable
{
public:
    static unordered_set<string> existingVariables;
    static int currentTempCount;
    static string getCurrentTempName();
};
#endif