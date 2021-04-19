#include <fstream>
#include <vector>
using namespace std;

class OutputService{
    public:
        static vector<string> allocLines;
        static vector<string> storeLines;
        static vector<string> lines;

        static void addLine(string line);
        static void printer(string variableName);

        static ofstream outputFile;
        static void vectorsToFile();
};