#include "functions.h"
#include "outputService.h"
#include "assignments.h"
#include "evaluation.h"
string singleChoose = "\\s*choose\\((\\s*(" + RegexController::equationRegex + ",\\s*){3}\\s*" + RegexController::equationRegex + "\\))\\s*";
string chooseParam = "((?!choose|,)[\\s%a-zA-Z0-9+\\-*\\/\\(\\)])+?";
string denemeRegex = "choose\\((\\s*" + chooseParam + "\\s*,\\s*){3}\\s*" + chooseParam + "\\s*\\)";
regex denemeRegexController(denemeRegex);
regex singleChooseRegex(singleChoose);

string merveFuncu(string currentLine){
    int openBracket= currentLine.find_last_of("(");
    int closingBracket = currentLine.substr(openBracket).find_first_of(")") + openBracket ;
    string inside = currentLine.substr(openBracket,closingBracket - openBracket +1);
    if (inside.find(",") == string::npos)
    {
        string t = evaluateExpression(inside);
        regex temp(inside);
        return regex_replace(currentLine,temp,t);
    }
    return currentLine;
    
}

string handleChooseLine(string currentLine)
{
    currentLine = merveFuncu(currentLine);
    smatch match;
    while (regex_search(currentLine, match, denemeRegexController))
    {
        string chooseIci = match[0];
        string chooseOriginal = match[0];
        chooseIci = chooseIci.substr(chooseIci.find_first_of("(") + 1);
        chooseIci[chooseIci.find_last_of(")")] = ',';
        vector<string> temps;
        while (chooseIci.find(",") != string::npos)
        {
            string eq = chooseIci.substr(0, chooseIci.find(","));
            string holder = evaluateExpression(eq);
            regex temp(eq);
            temps.push_back(holder);
            chooseIci = chooseIci.substr(chooseIci.find(",") + 1);
        }
        if (temps.size() != 4)
            return "";
        string result = evaluateChoose(temps);
        regex t(chooseOriginal);
        currentLine.replace(currentLine.find(chooseOriginal), chooseOriginal.length(), result);
        currentLine = merveFuncu(currentLine);

    }
    return currentLine;
}

string evaluateChoose(vector<string> variables)
{
    string resultTemp = OutputService::getTempName();
    string l = resultTemp + "= call i32 @choose(i32 " + variables[0] + ", i32 " + variables[1] + ", i32 " + variables[2] + ", i32 " + variables[3] + ")\n";
    OutputService::addLine(l);
    return resultTemp;
}

void handlePrintLine(string currentLine)
{
    int openingBracket = currentLine.find_first_of("(");
    int closingBracket = currentLine.find_last_of(")");
    string inside = currentLine.substr(openingBracket + 1, closingBracket - openingBracket);
    string resultName = evaluateExpression(inside);
    OutputService::addLine("call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 " + resultName + " )");
}

bool checkPrintSyntax(string printLine)
{
    int openingBracket = printLine.find_first_of("(");
    int closingBracket = printLine.find_last_of(")");
    if (openingBracket == string::npos || closingBracket == string::npos)
        return false;
    string inside = printLine.substr(openingBracket + 1, closingBracket - openingBracket - 1);
    return checkExpressionSyntax(inside);
}