
#include <vector>
#include <stack>
#include <iostream>
#include "evaluation.h"
#include "assignments.h"
#include "outputService.h"

string operators = "+-*/()";
vector<string> stringToVector(string s)
{
    int formerIndex = 0;
    vector<string> calculations;
    for (int currentIndex = 0; currentIndex < s.length(); currentIndex++)
    {
        if (operators.find(s[currentIndex]) == string::npos)
            continue;
        string added = s.substr(formerIndex, currentIndex - formerIndex);
        if (added != "")
            calculations.push_back(added);
        calculations.push_back(s.substr(currentIndex, 1));

        formerIndex = currentIndex + 1;
    }
    if (s.substr(formerIndex, s.length() - formerIndex) != "")
        calculations.push_back(s.substr(formerIndex, s.length() - formerIndex));
    return calculations;
}

int prec(string c)
{
    if (c == "*" || c == "/")
        return 2;
    else if (c == "+" || c == "-")
        return 1;
    else
        return -1;
}

vector<string> infixToPostfix(string s)
{
    vector<string> calc = stringToVector(s);
    std::stack<string> st;
    st.push("N");
    int l = calc.size();
    vector<string> ns;
    for (int i = 0; i < calc.size(); i++)
    {

        if (operators.find(calc[i]) == string::npos)
            ns.push_back(calc[i]);

        else if (calc[i] == "(")

            st.push("(");

        else if (calc[i] == ")")
        {
            while (st.top() != "N" && st.top() != "(")
            {
                string c = st.top();
                st.pop();
                ns.push_back(c);
            }
            if (st.top() == "(")
            {
                string c = st.top();
                st.pop();
            }
        }

        else
        {
            while (st.top() != "N" && prec(calc[i]) <=
                                          prec(st.top()))
            {
                string c = st.top();
                st.pop();
                ns.push_back(c);
            }
            st.push(calc[i]);
        }
    }

    while (st.top() != "N")
    {
        string c = st.top();
        st.pop();
        ns.push_back(c);
    }
    return ns;
}

string postfixEval(vector<string> ns)
{
    if (ns.size() == 1)
    {
        if (regex_match(ns[0], RegexController::variableController))
        {
            string tempName = OutputService::getTempName();
            if (Variable::existingVariables.find(ns[0]) == Variable::existingVariables.end())
            {
                OutputService::allocLines.push_back("%" + ns[0] + " = alloca i32");
                OutputService::storeLines.push_back("store i32 0, i32* %" + ns[0]);
                Variable::existingVariables.insert(ns[0]);
            }
            OutputService::addLine(tempName + " = load i32* %" + ns[0]);
            return tempName;
        }
        return ns[0];
    }
    stack<string> evaluations;
    for (int i = 0; i < ns.size(); i++)
    {
        if (operators.find(ns[i]) == string::npos)
        {
            evaluations.push(ns[i]);
            continue;
        }

        if (evaluations.size() < 2)
            throw OutputService::currentLine - 1;
        string operand1 = evaluations.top();

        if (regex_match(operand1, RegexController::variableController))
        {
            if (Variable::existingVariables.find(operand1) == Variable::existingVariables.end())
            {
                OutputService::allocLines.push_back("%" + operand1 + " = alloca i32");
                OutputService::storeLines.push_back("store i32 0, i32* %" + operand1);
                Variable::existingVariables.insert(operand1);
            }
            operand1 = OutputService::getTempName();
            OutputService::addLine(operand1 + " = load i32* %" + evaluations.top());
        }
        evaluations.pop();

        string operand2 = evaluations.top();
        if (regex_match(operand2, RegexController::variableController))
        {
            if (Variable::existingVariables.find(operand2) == Variable::existingVariables.end())
            {
                OutputService::allocLines.push_back("%" + operand2 + " = alloca i32");
                OutputService::storeLines.push_back("store i32 0, i32* %" + operand2);
                Variable::existingVariables.insert(operand2);
            }
            operand2 = OutputService::getTempName();
            OutputService::addLine(operand2 + "=  load i32* %" + evaluations.top());
        }
        evaluations.pop();

        string resultTempName = OutputService::getTempName();
        switch (ns[i][0])
        {

        case ('+'):
            OutputService::addLine(resultTempName + " = add i32 " + operand2 + ", " + operand1);
            break;
        case ('-'):
            OutputService::addLine(resultTempName + " = sub i32 " + operand2 + ", " + operand1);
            break;
        case ('*'):
            OutputService::addLine(resultTempName + " = mul i32 " + operand2 + ", " + operand1);
            break;
        case ('/'):
            OutputService::addLine(resultTempName + " = sdiv i32 " + operand2 + ", " + operand1);
            break;
        }
        evaluations.push(resultTempName);
    }
    if (evaluations.empty())
        throw OutputService::currentLine - 1;
    return evaluations.top();
}
string evaluateExpression(string exp)
{
    // if choose ile basladi:
    //     parametre ayir
    //     gerekirse recursive cagir
    //     en son da t5 don
    exp.erase(std::remove(exp.begin(), exp.end(), ' '), exp.end());
    exp.erase(std::remove(exp.begin(), exp.end(), '\t'), exp.end());
    if (exp[0] == '-')
        exp = to_string(0) + exp;

    return postfixEval(infixToPostfix(exp));
}
