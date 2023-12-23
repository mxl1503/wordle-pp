#include <iostream>
#include <fstream>

using namespace std; 
 
void generateStrings(ofstream &outputFile, char set[], string prefix, int length, int numChars) {     
    if (length == 0) {
        outputFile << prefix << std::endl;
        return;
    }
 
    for (int i = 0; i < numChars; i++) {
        string newPrefix;
         
        newPrefix = prefix + set[i];
        generateStrings(outputFile, set, newPrefix, length - 1, numChars);
    }
 
}
 
int main(void) {
    char charSet[] = {'B', 'G', 'Y'};

    ofstream outputFile("stringpermutations.txt");
    if (!outputFile.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        exit(1);
    }

    generateStrings(outputFile, charSet, "", 5, 3);
    outputFile.close();
}
 