#include "parser.h"
#include <sstream>
#include <vector>
#include <string>
vector<string> CommandParser::parse(const string& input){
    vector<string> tokens;
    string currentToken = "";
    bool inQuotes = false;

    for (size_t i = 0; i < input.length(); ++i) {
        char ch = input[i];

        if (ch == '"') {
            inQuotes = !inQuotes;
            if (!inQuotes && currentToken.empty()) {
                tokens.push_back(""); 
            }
            continue; 
        }

        if (ch == ' ' && !inQuotes) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken = ""; 
            }
        } else {
            currentToken += ch;
        }
    }

    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }

    return tokens;
}