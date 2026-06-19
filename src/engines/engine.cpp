#include "engine.h"
#include <fstream>
#include <vector>
string DataEngine::set(const string& key, const string& val){
    DataEntry entry;
    entry.val=val;
    entry.hasTTL=false;
    ramStorage[key]=entry;
    return "OK";
}
string DataEngine::get(const string& key){
    if(ramStorage.find(key)==ramStorage.end()){
        return "(nil)";
    }
    DataEntry entry=ramStorage[key];
    if(entry.hasTTL){
        auto curTime= chrono::high_resolution_clock::now();
        if(curTime > entry.expiryTime){
            ramStorage.erase(key);
            return "(nil)";
        }
    }
    return entry.val;
}
string DataEngine::del(const string& key){
    if(ramStorage.erase(key)==1){
        return "(integer) 1";
    }
    return "(integer) 0";
}
string DataEngine::expire(const string& key, int seconds){
    if (ramStorage.find(key) == ramStorage.end()) {
        return "(integer) 0";
    }
    auto currentTime = std::chrono::high_resolution_clock::now();
    if (ramStorage[key].hasTTL && currentTime > ramStorage[key].expiryTime) {
        ramStorage.erase(key);
        return "(integer) 0";
    }
    ramStorage[key].hasTTL=true;
    ramStorage[key].expiryTime=currentTime+chrono::seconds(seconds);
    return "(integer) 1";
}
string DataEngine::ttl(const string& key){
    if (ramStorage.find(key) == ramStorage.end()) {
        return "(integer) -2";
    }
    DataEntry entry = ramStorage[key];
    if (!entry.hasTTL) {
        return "(integer) -1";
    }
    auto currentTime = chrono::high_resolution_clock::now();
    if (currentTime > entry.expiryTime) {
        ramStorage.erase(key);
        return "(integer) -2";
    }
    auto timeLeft=chrono::duration_cast<chrono::seconds>(entry.expiryTime-currentTime);
    string res=to_string(timeLeft.count());
    return "(integer) "+ res;
}

string DataEngine::saveToFile(const string& filename){
    ofstream outfile(filename);
    if(!outfile.is_open()) return "(error) ERR could not create or open dump file";
    auto currentTime=chrono::high_resolution_clock::now();
    for(auto &p: ramStorage){
        string key=p.first;
        DataEntry entry=p.second;
        if(entry.hasTTL && currentTime>entry.expiryTime){
            continue;
        }
        outfile<<key<<"|"<<entry.val<<endl;
    }
    outfile.close();
    return "OK";
}
string DataEngine::loadFromFile(const string& filename){
    ifstream infile(filename);
    if(!infile.is_open()) return "No save file found. Starting with empty database.";
    int loadedcount=0;
    string line;
    while(getline(infile, line)){
        size_t delimeter_pos=line.find('|');
        if(delimeter_pos!=string::npos){
            string key=line.substr(0, delimeter_pos);
            DataEntry entry;
            entry.val=line.substr(delimeter_pos+1);
            entry.hasTTL=false;
            ramStorage[key]=entry;
            loadedcount++;
        }
    }
    infile.close();
    return "Successfully restored " + to_string(loadedcount) + " keys from disk.";
}

string DataEngine::exists(const string& key){
    if (ramStorage.find(key) == ramStorage.end()) {
        return "(integer) 0";
    }
    DataEntry entry = ramStorage[key];
    if (entry.hasTTL) {
        auto currentTime = chrono::high_resolution_clock::now();
        if (currentTime > entry.expiryTime) {
            ramStorage.erase(key);
            return "(integer) 0";
        }
    }

    return "(integer) 1";
}

vector<string> DataEngine::getAllKeys(){
    vector<string> activeKeys;
    auto currentTime = chrono::high_resolution_clock::now();
    for (const auto& p : ramStorage) {
    const string& key = p.first;
    const DataEntry& entry = p.second;

    if (entry.hasTTL && currentTime > entry.expiryTime) {
        continue;
    }

    activeKeys.push_back(key);
}

    return activeKeys;
}

string DataEngine::multi(){
    if(inTransaction){
        return "(err) MULTI calls can not be nested";
    }
    inTransaction=true;
    transactionQueue.clear();
    return "OK";
}
string DataEngine::discard(){
    if (!inTransaction) {
        return "(error) ERR DISCARD without MULTI";
    }
    inTransaction = false;
    transactionQueue.clear(); // Dump all queued commands without running them
    return "OK";
}
bool DataEngine::isTransactionActive() const {
    return inTransaction;
}

// Add this missing function inside src/engines/engine.cpp!
void DataEngine::queueCommand(const vector<string>& cmdTokens) {
    transactionQueue.push_back(cmdTokens);
}

vector<string> DataEngine::exec(DataEngine& engine, const CommandParser& parser) {
    vector<string> finalResponses;

    if (!inTransaction) {
        finalResponses.push_back("(error) ERR EXEC without MULTI");
        return finalResponses;
    }

    // If the transaction block was empty, return an empty array notification
    if (transactionQueue.empty()) {
        inTransaction = false; // Turn off transaction mode safely
        finalResponses.push_back("(empty array)");
        return finalResponses;
    }

    // Loop through every single queued command one-by-one sequentially
    for (size_t i = 0; i < transactionQueue.size(); ++i) {
        vector<string> tokens = transactionQueue[i];
        string command = tokens[0];

        // Route the queued command exactly like main.cpp does
        if (command == "SET") {
            if (tokens.size() < 3) {
                finalResponses.push_back("(error) ERR wrong number of arguments for 'SET' command");
            } else {
                finalResponses.push_back(engine.set(tokens[1], tokens[2]));
            }
        }
        else if (command == "GET") {
            if (tokens.size() < 2) {
                finalResponses.push_back("(error) ERR wrong number of arguments for 'GET' command");
            } else {
                finalResponses.push_back(engine.get(tokens[1]));
            }
        }
        else if (command == "DEL") {
            if (tokens.size() < 2) {
                finalResponses.push_back("(error) ERR wrong number of arguments for 'DEL' command");
            } else {
                finalResponses.push_back(engine.del(tokens[1]));
            }
        }
        else if (command == "EXPIRE") {
            if (tokens.size() < 3) {
                finalResponses.push_back("(error) ERR wrong number of arguments for 'EXPIRE' command");
            } else {
                int seconds = stoi(tokens[2]);
                finalResponses.push_back(engine.expire(tokens[1], seconds));
            }
        }
        else if (command == "TTL") {
            if (tokens.size() < 2) {
                finalResponses.push_back("(error) ERR wrong number of arguments for 'TTL' command");
            } else {
                finalResponses.push_back(engine.ttl(tokens[1]));
            }
        }
        else if (command == "EXISTS") {
            if (tokens.size() < 2) {
                finalResponses.push_back("(error) ERR wrong number of arguments for 'EXISTS' command");
            } else {
                finalResponses.push_back(engine.exists(tokens[1]));
            }
        }
        else {
            finalResponses.push_back("(error) ERR unknown command '" + command + "'");
        }
    }

    // Cleanup: Clear transaction state flags and empty out the memory holding queue
    inTransaction = false;
    transactionQueue.clear();

    return finalResponses;
}