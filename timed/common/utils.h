#pragma once
#include "source_location.hpp"
#include <chrono>
#include <string>
#include <iomanip>
#include <regex>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <sys/sysinfo.h>
#include <cstring>
#include <ios>
#include <iostream>


namespace tools {
	namespace select {
		enum log_type {
			out = 0,
			log = 1,
			err = 2
		};
	}

	using __int64 = long long int;
	using namespace std::chrono;

	// 匹配数字,没有匹配返回-1
	__int64 match_numerical(const std::string& str) {
		std::regex r{ "^(.*?)(\\d+)(.*)$" };

		if (std::smatch m; std::regex_match(str, m, r))
		{
			return std::stoi(m[2]);
		}
		else
		{
			return -1;
		}
	}

	// 是否含有某字符串内容
	bool is_match_literal(const std::string& value, const std::string& match) {
		std::regex r{ "^(.*?)(" + match + ")(.*)$" };

		if (std::smatch m; std::regex_match(value, m, r))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	// 获取日期
	decltype(auto) time_now(system_clock::time_point t_point = system_clock::now()) {
		time_t tt{ system_clock::to_time_t(t_point) };
		tm* t{ localtime(&tt) };
		return std::put_time(t, "%Y-%m-%d %X");
	}

	void location(std::string_view msg = "this a log message", const select::log_type& type = select::out, const nostd::source_location& sourec = nostd::source_location::current()) {
		switch (type)
		{
		case tools::select::out:
			std::cout << "[" << time_now()
				<< "]-" << sourec.function_name()
				<< "[(line:" << sourec.line()
				<< ")-(column:" << sourec.column()
				<< ")]:" << msg << std::endl;
			break;
		case tools::select::log:
			std::clog << "[" << time_now()
				<< "]-" << sourec.function_name()
				<< "[(line:" << sourec.line()
				<< ")-(column:" << sourec.column()
				<< ")]:" << msg << std::endl;
			break;
		case tools::select::err:
			std::cerr << "[" << time_now()
				<< "]-" << sourec.function_name()
				<< "[(line:" << sourec.line()
				<< ")-(column:" << sourec.column()
				<< ")]:" << msg << std::endl;
			break;
		default:
			break;
		}
	}
	
	void getRAMInfomation() {
		struct sysinfo s_info;
		char info_buff[100];
		if (sysinfo(&s_info) == 0)
		{
			sprintf(info_buff, "Total memory: %.ld Mib", s_info.totalram / 1024 / 1024);
			printf("%s\n", info_buff);

			sprintf(info_buff, "Buffer memory: %.ld Mib", s_info.bufferram / 1024 / 1024);
			printf("%s\n", info_buff);

			sprintf(info_buff, "Shared memory: %.ld Mib", s_info.sharedram / 1024 / 1024);
			printf("%s\n", info_buff);

			sprintf(info_buff, "Free memory: %.ld Mib", s_info.freeram / 1024 / 1024);
			printf("%s\n", info_buff);

			sprintf(info_buff, "Total swap memory: %.ld Mib", s_info.totalswap / 1024 / 1024);
			printf("%s\n", info_buff);

			sprintf(info_buff, "Free swap memory: %.ld Mib", s_info.freeswap / 1024 / 1024);
			printf("%s\n", info_buff);

			sprintf(info_buff, "System running time: %.ld minute", s_info.uptime / 60);
			printf("%s\n", info_buff);

			printf("\n");
		}
	}
}