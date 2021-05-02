
#include <vector>
#include <stack>
#include <iostream>
#include "evaluation.h"
#include "assignments.h"
#include "outputService.h"

// Operators that can be in an expression
string operators = "+-*/()";
// Divide the expression to operands and operators
vector<string> stringToVector(string s)
{
    int formerIndex = 0;
    vector<string> calculations;
    for (int currentIndex = 0; currentIndex < s.length(); currentIndex++)
    {
        // If current character is not an operator, do nothing
        if (operators.find(s[currentIndex]) == string::npos)
            continue;
        // When the current character is an operator, take the string between this and former operator, which is an operand
        string added = s.substr(formerIndex, currentIndex - formerIndex);
        if (added != "")
            calculations.push_back(added);
        // Add the operator
        calculations.push_back(s.substr(currentIndex, 1));
        // Update the former operator index
        formerIndex = currentIndex + 1;
    }
    // Add the last operand since it does not have an operator at its right
    if (s.substr(formerIndex, s.length() - formerIndex) != "")
        calculations.push_back(s.substr(formerIndex, s.length() - formerIndex));
    return calculations;
}

int prec(string c)
{
    // Distinguish between operators
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
        // If the current string is not an operator, directly add to output
        if (operators.find(calc[i]) == string::npos)
            ns.push_back(calc[i]);
        // If it is a (, we need to add )
        else if (calc[i] == "(")
            st.push("(");
        // If it is a ), we need to handle operators and operans in the middle
        else if (calc[i] == ")")
        {
            // Get everything from the stack and add to string until the stack is empty
            while (st.top() != "N" && st.top() != "(")
            {
                string c = st.top();
                st.pop();
                ns.push_back(c);
            }
            // We dont need to add paranthesis again so just pop it
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
// Evaluate postfix
string postfixEval(vector<string> ns)
{
    // If only one string has come, like "5" or "a1"
    if (ns.size() == 1)
    {
        // If it is a variable, alloc, store, and load it if necessary
        if (regex_match(ns[0], RegexController::variableController))
        {
            string tempName = OutputService::getTempName();
            // First time encountiring this variable so we must alloc and store 0 to it
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
        // If current string is not an operator, push to stack and continue
        if (operators.find(ns[i]) == string::npos)
        {
            evaluations.push(ns[i]);
            continue;
        }
        // If current string is an operator, we need 2 operands to evaluate this.
        // If there are less then 2 operands, we encountered a syntax error.
        if (evaluations.size() < 2)
            throw OutputService::currentLine - 1;
        string operand1 = evaluations.top();
        // If operand is a variable, we need to load it to a temp variable
        if (regex_match(operand1, RegexController::variableController))
        {
            // Initialization, same as line 111
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
        // If operand is a variable, we need to load it to a temp variable
        if (regex_match(operand2, RegexController::variableController))
        {
            if (Variable::existingVariables.find(operand2) == Variable::existingVariables.end())
            {
                // Initialization, same as line 111
                OutputService::allocLines.push_back("%" + operand2 + " = alloca i32");
                OutputService::storeLines.push_back("store i32 0, i32* %" + operand2);
                Variable::existingVariables.insert(operand2);
            }
            operand2 = OutputService::getTempName();
            OutputService::addLine(operand2 + "=  load i32* %" + evaluations.top());
        }
        evaluations.pop();
        // We need to store the result of the operation into a temp variable
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
    // Only the final result must be in the stack. If it isn't, that means a syntax error
    if (evaluations.empty())
        throw OutputService::currentLine - 1;
    return evaluations.top();
}
string evaluateExpression(string exp)
{
    exp.erase(std::remove(exp.begin(), exp.end(), ' '), exp.end());
    exp.erase(std::remove(exp.begin(), exp.end(), '\t'), exp.end());
    // Handle negative numbers
    if (exp[0] == '-')
        exp = to_string(0) + exp;

    return postfixEval(infixToPostfix(exp));
}
