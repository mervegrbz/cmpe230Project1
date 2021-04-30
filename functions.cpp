#include "functions.h"
#include "outputService.h"
#include "assignments.h"
#include "evaluation.h"

string singleChoose = "\\s*choose\\((\\s*(" + RegexController::equationRegex + ",\\s*){3}\\s*" + RegexController::equationRegex + "\\))\\s*";
string multiChoose = "\\s*choose\\((\\s*((" + RegexController::equationRegex + "|" + singleChoose + "),\\s*){3}\\s*(" + RegexController::equationRegex + "|" + singleChoose + ")\\))\\s*";
regex singleChooseRegex(singleChoose);
regex multiChooseRegex(multiChoose);
string handleChooseLine(string currentLine)
{ // gelen line  a1 = 2 + choose(1,2,3,4)
    //choose(1,2,3,choose(1,2,3,4))
    // 2,3,4)
    string equationRegex = "\\s*((([%]{0,1}" + RegexController::variableRegex + ")|" + multiChoose + "|-?\\d+)(?:\\s*[+*-\\/]\\s*(([%]{0,1}" + RegexController::variableRegex + ")|" + multiChoose + "|-?\\d+)){0,}\\s*)";
    string assignmentRegex = RegexController::variableRegex + "\\s*=\\s*" + equationRegex;
    regex chooseAssign(assignmentRegex);
    regex equReg(equationRegex);
    regex printSyntax("print\\s*\\(" + equationRegex + "\\)\\s*");

    bool a = regex_match(currentLine, chooseAssign);
    bool b = regex_match(currentLine, printSyntax);
    // if (!a && !b)
    //     return "";

    smatch match;
    while (regex_search(currentLine, match, singleChooseRegex))
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
            // chooseOriginal = regex_replace(chooseOriginal,temp,holder);
            chooseIci = chooseIci.substr(chooseIci.find(",") + 1);
        }
        // chooseOriginal = choose(t1,t2,t3,5)
        string result = evaluateChoose(temps);
        regex t(chooseOriginal);
        // currentLine = regex_replace(currentLine, t, result);
        currentLine.replace(currentLine.find(chooseOriginal), chooseOriginal.length(), result);

        // cout << currentLine << endl;
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