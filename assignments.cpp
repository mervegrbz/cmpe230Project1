#include "assignments.h"
#include "evaluation.h"
#include "outputService.h"
unordered_set<string> Variable::existingVariables;
vector<pair<string, string>> Variable::variables;
int Variable::currentTempCount = 0;

string RegexController::variableRegex = "\\s*([a-zA-Z]{1}[0-9a-zA-Z]{0,})\\s*";
string RegexController::equationRegex = "\\s*((" + variableRegex + "|\\d+)(?:\\s*[+*-\\/]\\s*(" + variableRegex + "|[0-9]{1,})){0,}\\s*)";
string RegexController::assignmentRegex = variableRegex + "\\s*=\\s*" + equationRegex;

regex RegexController::assignmentController(RegexController::assignmentRegex);
regex RegexController::equationController(RegexController::equationRegex);
regex RegexController::variableController(RegexController::variableRegex);

void handleAssignmentLine(string currentLine)
{
    currentLine.erase(std::remove(currentLine.begin(), currentLine.end(), ' '), currentLine.end());

    int equalSignIndex = currentLine.find("=");
    string varName = currentLine.substr(0, equalSignIndex);
    if (Variable::existingVariables.find(varName) == Variable::existingVariables.end())
    {
        OutputService::allocLines.push_back("%" + varName + " = alloca i32");
        OutputService::storeLines.push_back("store i32 0, i32* %" + varName);
        Variable::existingVariables.insert(varName);
    }
    string lastUsedTempVar = evaluateExpression(currentLine.substr(equalSignIndex + 1));
    OutputService::addLine("store i32 " + lastUsedTempVar + ", i32* %" + varName);
}

bool checkExpressionSyntax(string expression)
{
    int bracketClosing = expression.find_first_of(")");
    if (bracketClosing == string::npos)
        return regex_match(expression, RegexController::equationController);
    // Contains at least 1 )
    int bracketOpening = expression.substr(0, bracketClosing).find_last_of("(");
    if (bracketOpening == string::npos)
        return false;
    string insideParanthesis = expression.substr((bracketOpening + 1), bracketClosing - bracketOpening - 1);
    if (!checkExpressionSyntax(insideParanthesis))
        return false;
    expression = expression.substr(0, bracketOpening) + "0" + expression.substr(bracketClosing + 1);
    return checkExpressionSyntax(expression);
}

bool checkVariableSyntax(string variable)
{
    return regex_match(variable, RegexController::variableController);
}

bool checkAssignmentSyntax(string assignmentLine)
{
    int equalSignIndex = assignmentLine.find("=");
    if (equalSignIndex == string::npos)
        return false;
    bool isVariableSyntaxValid = checkVariableSyntax(assignmentLine.substr(0, equalSignIndex));
    bool isExpressionSyntaxValid = checkExpressionSyntax(assignmentLine.substr(equalSignIndex + 1));
    return isVariableSyntaxValid && isExpressionSyntaxValid;
}
