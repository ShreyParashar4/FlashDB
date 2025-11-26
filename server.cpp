#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <list>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <fstream> // NEW: For File I/O

using namespace std;

class FlashDB {
private:
    size_t capacity;
    list<pair<string, string>> lru_list;
    unordered_map<string, list<pair<string, string>>::iterator> cache_map;
    mutex db_mutex;
    const string DB_FILE = "flashdb.data"; // The save file

public:
    FlashDB(size_t cap) : capacity(cap) {
        // ON STARTUP: Try to load data from file
        loadData();
    }

    void loadData() {
        ifstream infile(DB_FILE);
        if (!infile.is_open()) return; // File doesn't exist yet, that's fine

        string key, value;
        while (infile >> key >> value) {
            // Direct insert (bypassing LRU logic for speed during load)
            if (lru_list.size() == capacity) {
                string last = lru_list.back().first;
                cache_map.erase(last);
                lru_list.pop_back();
            }
            lru_list.push_front({key, value});
            cache_map[key] = lru_list.begin();
        }
        cout << "[SYSTEM] Data restored from disk." << endl;
        infile.close();
    }

    void saveData() {
        ofstream outfile(DB_FILE);
        for (auto const& [key, val] : lru_list) {
            outfile << key << " " << val << endl;
        }
        outfile.close();
        cout << "[SYSTEM] Data saved to disk." << endl;
    }

    string processCommand(string commandLine) {
        stringstream ss(commandLine);
        string action, key, value;
        ss >> action;

        lock_guard<mutex> lock(db_mutex);

        if (action == "SET") {
            ss >> key >> value;
            if (key.empty() || value.empty()) return "ERROR\n";
            
            if (cache_map.find(key) != cache_map.end()) {
                lru_list.erase(cache_map[key]);
            } else if (lru_list.size() == capacity) {
                string last = lru_list.back().first;
                cache_map.erase(last);
                lru_list.pop_back();
            }
            lru_list.push_front({key, value});
            cache_map[key] = lru_list.begin();
            return "OK\n";
        } 
        else if (action == "GET") {
            ss >> key;
            if (key.empty()) return "ERROR\n";
            if (cache_map.find(key) != cache_map.end()) {
                string val = cache_map[key]->second;
                lru_list.erase(cache_map[key]);
                lru_list.push_front({key, val});
                cache_map[key] = lru_list.begin();
                return val + "\n";
            }
            return "(nil)\n";
        }
        else if (action == "SAVE") { // NEW COMMAND
            saveData();
            return "OK: DB Saved\n";
        }
        return "ERROR\n";
    }
};

FlashDB db(100); // Increased capacity to 100 for real usage

void handle_client(int client_fd) {
    char buffer[1024] = {0};
    while (true) {
        memset(buffer, 0, 1024);
        int bytes_read = read(client_fd, buffer, 1024);
        if (bytes_read <= 0) {
            close(client_fd);
            return;
        }
        string response = db.processCommand(string(buffer));
        send(client_fd, response.c_str(), response.length(), 0);
    }
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return -1;

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        cerr << "Bind failed" << endl;
        return -1;
    }

    listen(server_fd, 5);
    cout << "FlashDB v1.0 (Persistent) running on Port 8080..." << endl;

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len);
        if (client_fd < 0) continue;
        
        thread t(handle_client, client_fd);
        t.detach(); 
    }
    return 0;
}