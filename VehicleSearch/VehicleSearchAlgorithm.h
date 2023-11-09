#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <string>
#include <locale>
#include <codecvt>

std::wstring stringToWstring(const std::string&);
std::string wstringToString(const std::wstring&);

using json = nlohmann::json;

struct SearchResult
{
    int id = 0;
    std::wstring name;
    std::wstring imagePath;
};

class TrieNode {
public:
    std::unordered_map<char, std::shared_ptr<TrieNode>> children;
    bool isEndOfWord;
    std::vector<int> ids;

    TrieNode() : isEndOfWord(false) {}
};

class VehicleDatabase {
private:
    std::shared_ptr<TrieNode> root;
    std::unordered_map<int, json> vehicleMap;

public:
    VehicleDatabase() : root(std::make_shared<TrieNode>()) {};

    void insert(const std::string&, int, const nlohmann::json&);

    std::vector<int> search(const std::string&);

    nlohmann::json getVehicle(int);
};

class VehicleSearchAlgorithm {
private:
    VehicleDatabase database;

public:
    VehicleSearchAlgorithm(const std::string& dataFilePath);
    std::vector<SearchResult> runSearch(std::string);

private:
    void initializeDatabase(const std::string& dataFilePath);
};

