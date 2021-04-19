#include "assignments.h"

string variableRegex = "\\s*([a-zA-Z]{1}[0-9a-zA-Z]{0,})\\s*";
string equationRegex = "\\s*((" + variableRegex + "|\\d+)(?:\\s*[+*-\\/]\\s*(" + variableRegex + "|[0-9]{1,})){0,}\\s*)";
string assignmentRegex = variableRegex + "\\s*=\\s*" + equationRegex;


vector<pair<string,string>> Variable::variables;
int Variable::currentTempCount = 0;

regex assignmentController(assignmentRegex);
regex equationController(equationRegex);
regex variableController(variableRegex);



bool checkExpressionSyntax(string expression)
{
    int bracketClosing = expression.find_first_of(")");
    if (bracketClosing == string::npos)
        return regex_match(expression, equationController);
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
    return regex_match(variable, variableController);
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

int evaluateExpression(string expression)
{
    // expression.erase(std::remove(expression.begin(), expression.end(), ' '), expression.end());

    // smatch res;
    // string::const_iterator searchStart( expression.cbegin() );
    // vector<string> foundVariables;
    // while ( regex_search( searchStart, expression.cend(), res, variableController ) )
    // {
    //     // cout << ( searchStart == expression.cbegin() ? "" : " " ) << res[0];
    //     foundVariables.push_back(res[0]);
    //     searchStart = res.suffix().first;
    // }
    // for(string variable : foundVariables){
    //     expression = regex_replace(expression,regex(variable),"" + Variable::getValue(variable));   
    // }

    // cout << expression << endl;

    

    return 0;
}