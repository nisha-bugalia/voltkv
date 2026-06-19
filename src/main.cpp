#include <iostream>
#include "parser/parser.h"
#include "engines/engine.h"

using namespace std;

bool isNumber(const string& str) {
    if (str.empty()) return false;
    
    for (char const &c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}

int main(){
    CommandParser parser;
    DataEngine engine;
    cout<<"Welcome to the VoltKV In-Memory-Databse engine[Version 1.0]"<<endl;

    const string DUMP_FILE="dump.txt";
    cout<<"Loading data from disk..."<<endl;
    cout<<engine.loadFromFile(DUMP_FILE)<<"\n"<<endl;

    cout<<"Type 'EXIT' to quit the program\n"<<endl;
    string input;
    while (true)
    {
        cout<<"VoltKV>";
        getline(cin, input);
        vector<string> tokens=parser.parse(input);
        if(tokens.empty()) continue;
        string command=tokens[0];
        if (tokens[0] == "EXIT") {
            cout << "Shutting down VoltKV... Goodbye!" << endl;
            break;
        }
        

        if (command=="MULTI")
        {
            cout<<engine.multi()<<endl;
        }
        else if (command=="DISCARD")
        {
            /* code */
            cout<<engine.discard()<<endl;
        }
        else if (command=="EXEC")
        {
            /* code */
            vector<string> results=engine.exec(engine, parser);
            for(size_t i=0; i<results.size(); i++){
                cout<<i+1<<")"<<results[i]<<endl;
            }
        }
        else{
            if (engine.isTransactionActive())
            {
                /* code */
                if (command == "SET" || command == "GET" || command == "DEL" || 
                    command == "EXPIRE" || command == "TTL" || command == "EXISTS" || command == "SAVE") {
                    
                    engine.queueCommand(tokens);
                    cout << "QUEUED" << endl;
                } else {
                    cout << "(error) ERR unknown command '" << command << "'" << endl;
                }
            }
            else{

                if(tokens[0]=="SET"){
                    if(tokens.size()<3){
                        cout << "(error) ERR wrong number of arguments for 'SET' command" << endl;
                    }
                    else{
                        string key=tokens[1];
                        string val=tokens[2];
                        string res=engine.set(key, val);
                        cout<<res<<endl;
                    }
                }
                else if(tokens[0]=="GET"){
                    if(tokens.size()<2){
                        cout << "(error) ERR wrong number of arguments for 'GET' command" << endl;
                    }
                    else{
                        string key=tokens[1];
                        string res=engine.get(key);
                        cout<<res<<endl;
                    }
                }
                else if(tokens[0]=="DEL"){
                    if(tokens.size()<2){
                        cout << "(error) ERR wrong number of arguments for 'DEL' command" << endl;
                    }
                    else{
                        string key=tokens[1];
                        string res=engine.del(key);
                        cout<<res<<endl;
                    }
                }
                else if (command == "EXPIRE") {
                    if (tokens.size() < 3) {
                        cout << "(error) ERR wrong number of arguments for 'EXPIRE' command" << endl;
                    } else {
                        string key = tokens[1];
                        string secondsToken=tokens[2];
                        if (!isNumber(secondsToken))
                        {
                            /* code */
                            cout << "(error) ERR value is not an integer or out of range" << endl;
                        }
                        else{
                            int seconds = stoi(secondsToken); 
                            cout << engine.expire(key, seconds) << endl;
                        }
                    }
                }
                else if (command == "TTL") {
                    if (tokens.size() < 2) {
                        cout << "(error) ERR wrong number of arguments for 'TTL' command" << endl;
                    } else {
                        cout << engine.ttl(tokens[1]) << endl;
                    }
                }
                else if (command == "SAVE") {
                    cout << engine.saveToFile(DUMP_FILE) << endl;
                }
                else if (command == "EXISTS") {
                    if (tokens.size() < 2) {
                        cout << "(error) ERR wrong number of arguments for 'EXISTS' command" << endl;
                    } else {
                        cout << engine.exists(tokens[1]) << endl;
                    }
                }
                else if (command == "KEYS") {
                    vector<string> allKeys = engine.getAllKeys();
                    if (allKeys.empty()) {
                        cout << "(empty array)" << endl;
                    } else {
                        for (size_t i = 0; i < allKeys.size(); ++i) {
                            cout << i + 1 << ") \"" << allKeys[i] << "\"" << endl;
                        }
                    }
                }
                else {
                    cout << "(error) ERR unknown command '" << tokens[0] << "'" << endl;
                }
            }
        }
    }
    return 0;
}