#pragma once
#include <chrono>
#include "include/source_location.hpp"
#include <string>
#include <iomanip>
#include <regex>

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

	// Æ¥ÅäÊý×Ö,Ã»ÓÐÆ¥Åä·µ»Ø-1
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

	// ÊÇ·ñº¬ÓÐÄ³×Ö·û´®ÄÚÈÝ
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
	
}