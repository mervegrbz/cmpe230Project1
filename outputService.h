#include <fstream>
#include <vector>
using namespace std;

class OutputService{
    public:
        static vector<string> allocLines;
        static vector<string> storeLines;
        static vector<string> lines;

        static void addLine(string line);
        static string getTempName();
        static int tempCount;
        static int currentLine;
        static ofstream outputFile;
        static void vectorsToFile();
        static void chooseFunction();
};