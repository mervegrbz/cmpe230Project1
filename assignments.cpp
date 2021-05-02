#include "assignments.h"
#include "evaluation.h"
#include "outputService.h"

// We mainly used regex to check syntaxes of variables and equations


// Set of all variables in order to avoid multiple allocation
unordered_set<string> Variable::existingVariables;
// We have many temp variables to we need to keep track of that number
int Variable::currentTempCount = 0;
// A variable must start with a letter and continue with a digit or letter as much as wanted.
string RegexController::variableRegex = "\\s*([a-zA-Z]{1}[0-9a-zA-Z]{0,})\\s*";
// An equation can include (a variable or a number) and zero or more times ((+ or - or / or *) and (a variable or a number))
string singleEquation = "\\s*((([%]{0,1}" + RegexController::variableRegex + ")|-?\\d+)(?:\\s*[+*-\\/]\\s*(([%]{0,1}" + RegexController::variableRegex + ")|-?\\d+)){0,}\\s*)";
// Same as above, only difference is (2+a) is a valid expression just like 2+a
string RegexController::equationRegex = "(" + singleEquation + "|\\(" + singleEquation + "\\)" + ")";
// A print line can have print keyword and an expression
string RegexController::printRegex = "\\s*print\\(" + RegexController::equationRegex + "\\)\\s*";
// To replace "zero"s with "0"s
string RegexController::zeroRegex = "zero";

regex RegexController::equationController(RegexController::equationRegex);
regex RegexController::variableController(RegexController::variableRegex);
regex RegexController::printController(RegexController::printRegex);
regex RegexController::zeroController(RegexController::zeroRegex);

void handleAssignmentLine(string currentLine)
{   
    // Remove all spaces and tabs
    currentLine.erase(std::remove(currentLine.begin(), currentLine.end(), ' '), currentLine.end());
    currentLine.erase(std::remove(currentLine.begin(), currentLine.end(), '\t'), currentLine.end());

    int equalSignIndex = currentLine.find("=");
    // Left part of the equal sign
    string varName = currentLine.substr(0, equalSignIndex);
    // If the variable name has never been used before, create and store it
    if (Variable::existingVariables.find(varName) == Variable::existingVariables.end())
    {
        OutputService::allocLines.push_back("%" + varName + " = alloca i32");
        OutputService::storeLines.push_back("store i32 0, i32* %" + varName);
        Variable::existingVariables.insert(varName);
        
    }
    // Get the result from evaluateExpression, something like "%t3"
    string lastUsedTempVar = evaluateExpression(currentLine.substr(equalSignIndex + 1));
    OutputService::addLine("store i32 " + lastUsedTempVar + ", i32* %" + varName);
}
// Checks the syntax of the right of the equals sign
bool checkExpressionSyntax(string expression)
{
    int bracketClosing = expression.find_first_of(")");
    if (bracketClosing == string::npos)
        // if there are no brackets to handle, we can directly use regex
        return regex_match(expression, RegexController::equationController);
    // Contains at least 1 )
    int bracketOpening = expression.substr(0, bracketClosing).find_last_of("(");
    if (bracketOpening == string::npos)
        return false;
    string insideParanthesis = expression.substr((bracketOpening + 1), bracketClosing - bracketOpening - 1);
    // Check the syntax of the new part
    if (!checkExpressionSyntax(insideParanthesis))
        return false;
    // Replacing expressions with the new temp names
    expression = expression.substr(0, bracketOpening) + "0" + expression.substr(bracketClosing + 1);
    // We need to re-call it since our line has been updated and may have changed
    return checkExpressionSyntax(expression);
}
//We can use regex to see if a variable name is valid or not.
bool checkVariableSyntax(string variable)
{
    return regex_match(variable, RegexController::variableController);
}
// Divide the line by equals sign and check the variable and expression syntax. If both are them valid, then the line is valid
bool checkAssignmentSyntax(string assignmentLine)
{
    int equalSignIndex = assignmentLine.find("=");
    if (equalSignIndex == string::npos)
        return false; // Couldn't find the equals sign
    bool isVariableSyntaxValid = checkVariableSyntax(assignmentLine.substr(0, equalSignIndex));
    bool isExpressionSyntaxValid = checkExpressionSyntax(assignmentLine.substr(equalSignIndex + 1));
    return isVariableSyntaxValid && isExpressionSyntaxValid;
}
