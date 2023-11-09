#include "VehicleSearchAlgorithm.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <comutil.h>  
#pragma comment(lib, "comsuppw.lib")

std::string wstringToString(const std::wstring& ws)
{
    _bstr_t t = ws.c_str();
    char* pchar = (char*)t;
    std::string result = pchar;
    return result;
}

std::wstring stringToWstring(const std::string& s)
{
    _bstr_t t = s.c_str();
    wchar_t* pwchar = (wchar_t*)t;
    std::wstring result = pwchar;
    return result;
}

void VehicleDatabase:: insert(const std::string& key, int id, const json& vehicle) {
        auto node = root;
        for (char c : key) {
            if (!node->children[c]) {
                node->children[c] = std::make_shared<TrieNode>();
            }
            node = node->children[c];
            node->ids.push_back(id);
        }
        node->isEndOfWord = true;
        vehicleMap[id] = vehicle;
    }

std::vector<int> VehicleDatabase::search(const std::string& key) {
    std::string lowerKey = key;
    std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower); // to lower
    auto node = root;
    for (char c : lowerKey) {
        if (!node->children[c]) {
            return {}; // 返回空向量表示没有匹配项
        }
        node = node->children[c];
    }
    return node->ids;
}

json VehicleDatabase::getVehicle(int id) {
    return vehicleMap[id];
}

VehicleSearchAlgorithm::VehicleSearchAlgorithm(const std::string& dataFilePath) {
    initializeDatabase(dataFilePath);
}

std::vector<SearchResult> VehicleSearchAlgorithm::runSearch(std::string query) {
    std::transform(query.begin(), query.end(), query.begin(), ::tolower); // to lower
    std::vector<int> matches = database.search(query);
    std::vector<SearchResult> results;

    for (int id : matches) {
        json vehicle = database.getVehicle(id);
        SearchResult result;
        result.id = vehicle["id"];
        result.name = stringToWstring(vehicle["model"]);
        result.imagePath = stringToWstring(vehicle["image_path"]);
        results.push_back(result);
    }
    return results;
}


void VehicleSearchAlgorithm::initializeDatabase(const std::string& dataFilePath) {
    std::ifstream inFile(dataFilePath);
    if (!inFile.is_open()) {
        std::cerr << "无法打开数据文件。" << std::endl;
        return;
    }

    json vehiclesJson;
    inFile >> vehiclesJson;
    inFile.close();

    for (const auto& vehicleJson : vehiclesJson) {
        int id = vehicleJson["id"];
        std::string model = vehicleJson["model"];
        std::transform(model.begin(), model.end(), model.begin(), ::tolower); // to lower
        database.insert(std::to_string(id), id, vehicleJson);
        database.insert(model, id, vehicleJson);
    }
}