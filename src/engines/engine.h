#ifndef ENGINE_H
#define ENGINE_H
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;
#include <chrono>
struct DataEntry
{
    /* data */
    string val;
    bool hasTTL;
    chrono::time_point<chrono::high_resolution_clock> expiryTime;
};
class DataEngine
{
private:
    /* data */
    unordered_map<string, DataEntry> ramStorage;
    bool inTransaction = false;
    vector<vector<string>> transactionQueue;
public:
    string set(const string& key, const string& value);
    string get(const string& key);
    string del(const string& key);

    string expire(const string& key, int second);
    string ttl(const string& key);
    string exists(const string& key);
    vector<string> getAllKeys();

    string saveToFile(const string& filename);
    string loadFromFile(const string& filename);

    string discard();
    string multi();

    bool isTransactionActive() const;
    void queueCommand(const vector<string>& cmdTokens);
    vector<string> exec(DataEngine& engine, const class CommandParser& parser);

};

#endif