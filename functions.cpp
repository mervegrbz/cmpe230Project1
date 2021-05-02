#include "functions.h"
#include "outputService.h"
#include "assignments.h"
#include "evaluation.h"
// chooseParam is the parameter that can be given to choose function. One thing we are doing here is we are trying to get the deepest or easier choose.
// Which doesn't contain a choose inside it. That's why we say a parameter cannot have the word "choose"
string chooseParam = "((?!choose|,)[\\s*%a-zA-Z0-9+\\-*\\/\\(\\)])+?";
regex findBasicChooseRegex("choose\\((\\s*" + chooseParam + "\\s*,\\s*){3}\\s*" + chooseParam + "\\s*\\)");
// This function takes the line and tries to handle algebra and paranthesis. It iterates
// the string and finds a matching paranthesis. Then, checks if the inside is a valid algebraic
// expression. If so, evaluates the expression and start over.
string handleParanthesis(string currentLine)
{
    int cutFrom = 0;
    while (cutFrom < currentLine.length() - 1)
    {
        string temp = currentLine.substr(cutFrom);
        int openBracket = temp.find("(");
        // If no (, nothing to do for this function
        if (openBracket == string::npos)
            return currentLine;
        temp = temp.substr(openBracket);
        int closeBracket = temp.find(")");
        if (closeBracket == string::npos)
            // If there exists a ( but not a ), this means a syntax error. Just return an empty string as the output.
            return "";
        string inside = temp.substr(0, closeBracket + 1);
        // If the inside doesn't contain a comma and is a valid expression, evaluate it.
        if (inside != "" && inside.find(",") == string::npos && checkExpressionSyntax(inside))
        {
            string t = evaluateExpression(inside);
            currentLine.replace(currentLine.find(inside), inside.length(), t);
            // Start over just in case
            cutFrom = 0;
        }
        else
            cutFrom++;
    }
    return currentLine;
}
// This function takes a line with choose keyword and tries to handle all choose functions and to return a line without any choose words
string handleChooseLine(string currentLine)
{
    // We had problem with both choose and print, or if and while, have paranthesis and those //
    //parantheses mix up very often. Taking the inside of print, while, or if and putting the pieces together at the end is easier.
    string head = "", tail = "";
    if (currentLine.find("print(") != string::npos)
    {
        // If there is something before the print word, throw syntax eror. For example, pprint(1)
        if (currentLine.substr(0, 6) != "print(")
            throw OutputService::currentLine - 1;
        head = "print(";
        tail = ")";
        int openBracket = currentLine.find("(");
        if (currentLine.substr(openBracket).find_last_of(")") == string::npos)
            return ""; // Print doesn't have the necessary )
        int closingBracket = currentLine.find_last_of(")");
        currentLine = currentLine.substr(openBracket + 1, closingBracket - openBracket - 1);
    }
    else if (currentLine.find("if(") != string::npos)
    {
        // If there is something before the if word, throw syntax eror. For example, iif(1)
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
        // If there is something before the while word, throw syntax eror. For example, if while(1)
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
    // Try getting rid of expressions' paranthesis before dealing with choose calls.
    currentLine = handleParanthesis(currentLine);
    smatch match;
    while (regex_search(currentLine, match, findBasicChooseRegex))
    {
        // Find a choose call
        string chooseIci = match[0];
        string chooseOriginal = match[0];
        chooseIci = chooseIci.substr(chooseIci.find_first_of("(") + 1);
        chooseIci[chooseIci.find_last_of(")")] = ',';
        vector<string> parameters;
        // Split by commas and evaluate expressions
        while (chooseIci.find(",") != string::npos)
        {
            string eq = chooseIci.substr(0, chooseIci.find(","));
            string holder = evaluateExpression(eq);
            parameters.push_back(holder);
            chooseIci = chooseIci.substr(chooseIci.find(",") + 1);
        }
        // If there are more or less than 4 parameters, return error
        if (parameters.size() != 4)
            return "";
        // call choose function with parameters
        string result = evaluateChoose(parameters);
        // For example, replace a = choose(1,1,1,1) with a = %t1
        currentLine.replace(currentLine.find(chooseOriginal), chooseOriginal.length(), result);
        currentLine = handleParanthesis(currentLine);
    }
    return head + currentLine + tail;
}

string evaluateChoose(vector<string> variables)
{
    // Call the llvm choose function with required parameters
    string resultTemp = OutputService::getTempName();
    string l;
    // Directly accessing to a vector is not very safe so we must have a try catch around it. 
    try
    {
        l = resultTemp + "= call i32 @choose(i32 " + variables[0] + ", i32 " + variables[1] + ", i32 " + variables[2] + ", i32 " + variables[3] + ")\n";
    }
    catch (const exception &e)
    {
        throw OutputService::currentLine - 1;
    }

    OutputService::addLine(l);
    return resultTemp;
}

void handlePrintLine(string currentLine)
{
    // Call print function of llvm
    int openingBracket = currentLine.find_first_of("(");
    int closingBracket = currentLine.find_last_of(")");
    string inside = currentLine.substr(openingBracket + 1, closingBracket - openingBracket);
    string resultName = evaluateExpression(inside);
    OutputService::addLine("call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 " + resultName + " )");
}

bool checkPrintSyntax(string printLine)
{
    // A print line must have "print(", an expression, and ")"
    int openingBracket = printLine.find_first_of("(");
    int closingBracket = printLine.find_last_of(")");
    if (openingBracket == string::npos || closingBracket == string::npos)
        return false;
    string inside = printLine.substr(openingBracket + 1, closingBracket - openingBracket - 1);
    return checkExpressionSyntax(inside);
}