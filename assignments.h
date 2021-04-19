#include <string>
#include <map>
#include <regex>
#include <iostream>

using namespace std;
// Regex Definitions

bool checkExpressionSyntax(string expression);
bool checkVariableSyntax(string variable);
bool checkAssignmentSyntax(string assignmentLine);
int evaluateExpression(string expression);

#ifndef Variable_Class
#define Variable_Class
class Variable
{
public:
    static map<string, Variable *> currentVariables;
    static bool checkIfExists(string variableName);
    static int getValue(string variableName);
    static int currentTempCount;
    static string getCurrentTempName();

    Variable(string name);
    string name;

private:
    int value;
};
#endif 