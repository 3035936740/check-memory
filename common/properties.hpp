#pragma once

#ifndef CXX_PROPERTIES
#define CXX_PROPERTIES
#include <filesystem>
#include <fstream>
#include <string>
#include "nlohmann/json.hpp"
#include "../exception/file_exception.h"
using nlohmann::json;

const std::filesystem::path CONFIG_PATH{ "./config.json" };

class properties {
public:
    properties(const std::filesystem::path config_path = CONFIG_PATH) :
        input_file_stream { config_path } , 
        file_path         { config_path } {
        hasFileExists(this->file_path); 
        data = json::parse(input_file_stream);
    };

    virtual ~properties() {
        input_file_stream.close();
        file_path.clear();
    };

    void setConfigPath(const std::filesystem::path& file_path) {
        this->file_path = file_path;
        hasFileExists(this->file_path);
        input_file_stream.open(this->file_path);
    };

    const std::filesystem::path& getConfigPath() const {
        return file_path;
    };

    const json& getJsonData() const {
        return data;
    };
private:
    std::filesystem::path file_path;
    std::ifstream input_file_stream;
    json data;

    // 文件存在为无事发生,否则抛出异常
    void hasFileExists(const std::filesystem::path& file_path) {
        if (!std::filesystem::exists(file_path)) {
            throw FileException("文件路径不存在!");
        }
    };
};

#endif