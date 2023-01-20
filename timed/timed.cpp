// timed.cpp: 定义应用程序的入口点。
//
#include "timed.h"
#include "ram_head_file.h"
#include "source_location_log.h"
#include "file_exception.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <regex>

using byte = signed char;
using unsigned_long = unsigned long int;

std::vector<std::string> getProcessStatus(const std::string& pid) {
    std::filesystem::path p{ "/./proc/" + pid + "/status" };

    std::cout << "path is " << p << "\n";

    std::ifstream file_stream{ p };

    if (!file_stream.good() || !file_stream)
    {
        file_stream.close();
        throw FileException("Error while opening file!");
    }
    std::vector<std::string> result{};
    std::string line{};
    //file_stream >> result;

    while (std::getline(file_stream, line)){
        result.push_back(line);
    }

    file_stream.close();
    return result;
}

std::vector<std::string> getMemoryStatus(void) {
    std::filesystem::path p{ "/./proc/meminfo" };
    std::ifstream file_stream{ p };

    if (!file_stream.good() || !file_stream)
    {
        file_stream.close();
        throw FileException("Error while opening file!");
    }
    std::vector<std::string> result{};
    std::string line{};
    //file_stream >> result;

    while (std::getline(file_stream, line)) {
        result.push_back(line);
    }

    file_stream.close();
    return result;
}


int main(int argc, char* argv[])
{
    ram::getRAMInfomation();
    try {
        std::cout << "===================================\n";

        //获取内存相关信息
        auto memory_info{ getMemoryStatus() };
        for (auto item : memory_info) {
            if (tools::is_match_literal(item, "MemTotal")) {
                std::cout << "总物理内存:" << tools::match_numerical(item) << "KiB" << std::endl;
            }
            else if(tools::is_match_literal(item, "MemAvailable")) {
                std::cout << "剩余物理内存:" << tools::match_numerical(item) << "KiB" << std::endl;
                break;
            }
        }

        std::cout << "===================================\n";

        std::cout << "输pid获取进程信息:";
        std::string process_id;
        std::cin >> process_id;

        //获取进程相关信息
        auto process_info { getProcessStatus(process_id) };
        for (auto item : process_info) {
            //std::cout << item << std::endl;
            if (tools::is_match_literal(item, "VmRSS")) {
                std::cout << "进程已用物理内存:" << tools::match_numerical(item) << "KiB" << std::endl;
                break;
            }
            
        }
        std::cout << "===================================\n";
    }
    catch (const std::exception& e) {
        tools::location(e.what(), tools::select::err);
    }
    return 0;
}