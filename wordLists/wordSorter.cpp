#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

int main(void) {
    std::string inputFilename = "unsortedwordleanswers.txt";
    std::string outputFilename = "answerlist.txt";

    std::ifstream inputFile(inputFilename);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << inputFilename << std::endl;
        exit(1);
    }

    std::vector<std::string> words;
    std::string word;
    while (inputFile >> word) {
        words.push_back(word);
    }

    inputFile.close();

    std::cout << "Sorting started.\n";
    std::sort(words.begin(), words.end());
    std::cout << "Sorting finished!\n";

    std::ofstream outputFile(outputFilename);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening file: " << outputFilename << std::endl;
        exit(1);
    }

    for (const auto &sortedWord : words) {
        outputFile << sortedWord << std::endl;
    }

    outputFile.close();

    return 0;
}
