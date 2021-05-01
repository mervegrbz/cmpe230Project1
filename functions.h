#include <string>
#include <regex>

using namespace std;
void printer(string variableName);

string handleChooseLine(string currentLine);
string evaluateChoose(vector<string> variables);
void handlePrintLine(string currentLine);
bool checkPrintSyntax(string printLine);