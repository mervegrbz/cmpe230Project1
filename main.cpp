#include <iostream>
#include <fstream>
#include <regex>
#include <string>

using namespace std;

string variableRegex = "([a-zA-Z]{1}[0-9a-zA-Z]{0,})";
string equationRegex = "((" + variableRegex + "|\\d+)(?:\\s*[+*-\\/]\\s*(" + variableRegex + "|[0-9]{1,})){0,}\\s*)";
string assignmentRegex = variableRegex + "\\s*=\\s*" + equationRegex;
regex assignmentController(assignmentRegex);
regex equationController(equationRegex);

bool mehmet(string expression)
{
    int bracketClosing = expression.find_first_of(")");
    if (bracketClosing == string::npos)
        return regex_match(expression, equationController);
    // Contains at least 1 )
    int bracketOpening = expression.substr(0, bracketClosing).find_last_of("(");
    if (bracketOpening == string::npos)
        return false;
    string insideParanthesis = expression.substr((bracketOpening + 1), bracketClosing - bracketOpening - 1);
    if (!mehmet(insideParanthesis))
        return false;
    expression = expression.substr(0, bracketOpening) + "0" + expression.substr(bracketClosing + 1);
    return mehmet(expression);
}
int main()
{
    // string fileName = "m.my";
    // ifstream inputFile;
    // inputFile.open(fileName);

    // string currentLine;

    // while(getline(inputFile,currentLine)){
    //     cout << currentLine << endl;
    // }

    // inputFile.close();
    // ^variableRegex\s*=\s*$

    while (true)
    {
        string a;
        getline(cin, a);
        cout << "given: " << a << endl;

        cout << "Result: " << mehmet(a) << endl;
        ;
    }
    return 0;
}