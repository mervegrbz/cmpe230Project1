#include "functions.h"
#include "outputService.h"
#include "assignments.h"
#include "evaluation.h"

string chooseParam = "((?!choose|,)[\\s*%a-zA-Z0-9+\\-*\\/\\(\\)])+?";
regex denemeRegexController("choose\\((\\s*" + chooseParam + "\\s*,\\s*){3}\\s*" + chooseParam + "\\s*\\)");
string handleParanthesis(string currentLine)
{
    int cutFrom = 0;
    while (cutFrom < currentLine.length())
    {
        string temp = currentLine.substr(cutFrom);
        int openBracket = temp.find("(");
        if (openBracket == string::npos)
            return currentLine;
        temp = temp.substr(openBracket);
        int closeBracket = temp.find(")");
        if (closeBracket == string::npos)
            return ""; // That means an error
        string inside = temp.substr(0, closeBracket + 1);
        if (inside != "" && inside.find(",") == string::npos && checkExpressionSyntax(inside))
        {
            string t = evaluateExpression(inside);
            regex temp(inside);
            currentLine.replace(currentLine.find(inside), inside.length(), t);
            cutFrom = 0;
        }
        else
            cutFrom++;
    }
    return currentLine;
}

string handleChooseLine(string currentLine)
{

    string head = "", tail = "";
    if (currentLine.find("print(") != string::npos)
    {
        head = "print(";
        tail = ")";
        int openBracket = currentLine.find("(");
        if (currentLine.substr(openBracket).find_last_of(")") == string::npos)
            return "";
        int closingBracket = currentLine.find_last_of(")");
        currentLine = currentLine.substr(openBracket + 1, closingBracket - openBracket - 1);
    }
    else if (currentLine.find("if(") != string::npos)
    {
        if (currentLine.substr(0, 3) != "if(")
            throw OutputService::currentLine - 1;
        head = "if(";
        tail = "){";
        int openBracket = currentLine.find("(");
        int closingBracket = currentLine.find_last_of(")");
        int curly = currentLine.find("{");
        if (openBracket == string::npos || closingBracket == string::npos || curly == string::npos)
            return "";
        currentLine = currentLine.substr(openBracket + 1, closingBracket - openBracket - 1);
    }
    else if (currentLine.find("while(") != string::npos)
    {
        if (currentLine.substr(0, 6) != "while(")
            throw OutputService::currentLine - 1;
        head = "while(";
        tail = "){";
        int openBracket = currentLine.find("(");
        int closingBracket = currentLine.find_last_of(")");
        int curly = currentLine.find("{");
        if (openBracket == string::npos || closingBracket == string::npos || curly == string::npos)
            return "";
        currentLine = currentLine.substr(openBracket + 1, closingBracket - openBracket - 1);
    }

    currentLine = handleParanthesis(currentLine);
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
            temps.push_back(holder);
            chooseIci = chooseIci.substr(chooseIci.find(",") + 1);
        }
        if (temps.size() != 4)
            return "";
        string result = evaluateChoose(temps);
        currentLine.replace(currentLine.find(chooseOriginal), chooseOriginal.length(), result);
        currentLine = handleParanthesis(currentLine);
    }
    return head + currentLine + tail;
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