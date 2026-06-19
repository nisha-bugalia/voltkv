#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
using namespace std;
class CommandParser
{
public:
    vector<string> parse(const string& input);
};
#endif