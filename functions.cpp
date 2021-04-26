#include "functions.h"
#include "outputService.h"
#include "assignments.h"

string handleChooseLine(string currentLine){
// choose(choose(1,5853013,(2+6*8),choose(0, 4674117, choose(1,5853013,4281377,4280901),5853013)),0-1 * 0+1,5853013,(5*9*(5+5)))

    int openingBracket = currentLine.find_last_of("(");
    int closingBracket = currentLine.substr(openingBracket+1).find_first_of(")");
    string unknown = currentLine.substr(openingBracket+1,closingBracket - openingBracket);

    if(unknown.find(",") != string::npos){
        // Choose function var
        
    }
    

}
