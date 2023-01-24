// timed.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

//不要乱动到其他的头文件了,不然爆炸了没你好果子吃(

#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <regex>

#include "exception/file_exception.h"
#include "common/utils.h"

/*
以下三方库的头文件的顺序不要改!
千万不要改, 改了就不能通过编译了,
我也不知道原理是什么,反正这个顺序不要改
*/

#include "source_location.hpp"
#include "yaml-cpp/yaml.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "fmt/format.h"

const std::filesystem::path CONFIG_YAML_PATH {"./config.yaml"};

//到点(2:30)会全部记录到文件中
static auto logger = spdlog::daily_logger_mt("daily_logger", "logs/daily.txt", 2, 30);

namespace std {
	using fmt::format;
	using fmt::format_error;
	using fmt::formatter;
	using nostd::source_location;
}

namespace log_relevant {
	enum log_type :int {
		info = 0,
		debug = 1,
		warn = 2,
		critical = 3,
		error = 4
	};

	void daily_log_record(
		std::string_view record_literal,
		log_type type = log_type::info) {

		switch (type) {
		case info:
			spdlog::info(record_literal);
			logger->info(record_literal);
			break;
		case debug:
			spdlog::debug(record_literal);
			logger->debug(record_literal);
			break;
		case warn:
			spdlog::warn(record_literal);
			logger->warn(record_literal);
			break;
		case critical:
			spdlog::critical(record_literal);
			logger->critical(record_literal);
			break;
		case error:
			spdlog::error(record_literal);
			logger->error(record_literal);
			break;
		}
	}
}

namespace define_value {
	static std::string proc{ "/" };
	void init() {
		try {
			std::ifstream file_stream{ CONFIG_YAML_PATH };

			if (!file_stream.good() || !file_stream)
			{
				file_stream.close();
				throw FileException("Error while opening file!");
			}
			YAML::Parser parser(file_stream);

			YAML::Node doc; 

			auto&& path = doc["path"];

			//std::cout << std::format("path:{}", doc["path"]) << std::endl;

			//proc = path["proc"].as<std::string>();

			//std::cout << std::format("path路径:{}", proc) << std::endl;
		}
		catch (const FileException& e) {
			log_relevant::daily_log_record("config.yaml configuration file does not exist", log_relevant::log_type::error);
		}
		catch (const YAML::ParserException& e) {
			log_relevant::daily_log_record(std::format("yaml exception->detail:{}",e.what()), log_relevant::log_type::error);
		}
		catch (const std::exception& e) {
			log_relevant::daily_log_record(std::format("A serious error occurred:{}", e.what()), log_relevant::log_type::error);
		}
	}
}

inline void initialized(void) {
	spdlog::info("正在初始化");

	
	//设置全局日志等级
	define_value::init();

	std::cout << std::format("文件路径:{}",define_value::proc) << std::endl;

	logger->set_level(spdlog::level::info);
	spdlog::set_level(spdlog::level::info);
	spdlog::info("初始化完成");
}

// TODO: 在此处引用程序需要的其他标头。
// 
//吸氧大法(对程序优化性能)
//#define OXYGEN_INHALATION_METHOD

#ifdef OXYGEN_INHALATION_METHOD
	#pragma GCC optimize(1)
	#pragma GCC optimize(2)
	#pragma GCC optimize(3)
	#pragma GCC optimize("Ofast")
	#pragma GCC optimize("inline")
	#pragma GCC optimize("-fgcse")
	#pragma GCC optimize("-fgcse-lm")
	#pragma GCC optimize("-fipa-sra")
	#pragma GCC optimize("-ftree-pre")
	#pragma GCC optimize("-ftree-vrp")
	#pragma GCC optimize("-fpeephole2")
	#pragma GCC optimize("-ffast-math")
	#pragma GCC optimize("-fsched-spec")
	#pragma GCC optimize("unroll-loops")
	#pragma GCC optimize("-falign-jumps")
	#pragma GCC optimize("-falign-loops")
	#pragma GCC optimize("-falign-labels")
	#pragma GCC optimize("-fdevirtualize")
	#pragma GCC optimize("-fcaller-saves")
	#pragma GCC optimize("-fcrossjumping")
	#pragma GCC optimize("-fthread-jumps")
	#pragma GCC optimize("-funroll-loops")
	#pragma GCC optimize("-fwhole-program")
	#pragma GCC optimize("-freorder-blocks")
	#pragma GCC optimize("-fschedule-insns")
	#pragma GCC optimize("inline-functions")
	#pragma GCC optimize("-ftree-tail-merge")
	#pragma GCC optimize("-fschedule-insns2")
	#pragma GCC optimize("-fstrict-aliasing")
	#pragma GCC optimize("-fstrict-overflow")
	#pragma GCC optimize("-falign-functions")
	#pragma GCC optimize("-fcse-skip-blocks")
	#pragma GCC optimize("-fcse-follow-jumps")
	#pragma GCC optimize("-fsched-interblock")
	#pragma GCC optimize("-fpartial-inlining")
	#pragma GCC optimize("no-stack-protector")
	#pragma GCC optimize("-freorder-functions")
	#pragma GCC optimize("-findirect-inlining")
	#pragma GCC optimize("-frerun-cse-after-loop")
	#pragma GCC optimize("inline-small-functions")
	#pragma GCC optimize("-finline-small-functions")
	#pragma GCC optimize("-ftree-switch-conversion")
	#pragma GCC optimize("-foptimize-sibling-calls")
	#pragma GCC optimize("-fexpensive-optimizations")
	#pragma GCC optimize("-funsafe-loop-optimizations")
	#pragma GCC optimize("inline-functions-called-once")
	#pragma GCC optimize("-fdelete-null-pointer-checks")
	#pragma G++ optimize(1)
	#pragma G++ optimize(2)
	#pragma G++ optimize(3)
	#pragma G++ optimize("Ofast")
	#pragma G++ optimize("inline")
	#pragma G++ optimize("-fgcse")
	#pragma G++ optimize("-fgcse-lm")
	#pragma G++ optimize("-fipa-sra")
	#pragma G++ optimize("-ftree-pre")
	#pragma G++ optimize("-ftree-vrp")
	#pragma G++ optimize("-fpeephole2")
	#pragma G++ optimize("-ffast-math")
	#pragma G++ optimize("-fsched-spec")
	#pragma G++ optimize("unroll-loops")
	#pragma G++ optimize("-falign-jumps")
	#pragma G++ optimize("-falign-loops")
	#pragma G++ optimize("-falign-labels")
	#pragma G++ optimize("-fdevirtualize")
	#pragma G++ optimize("-fcaller-saves")
	#pragma G++ optimize("-fcrossjumping")
	#pragma G++ optimize("-fthread-jumps")
	#pragma G++ optimize("-funroll-loops")
	#pragma G++ optimize("-fwhole-program")
	#pragma G++ optimize("-freorder-blocks")
	#pragma G++ optimize("-fschedule-insns")
	#pragma G++ optimize("inline-functions")
	#pragma G++ optimize("-ftree-tail-merge")
	#pragma G++ optimize("-fschedule-insns2")
	#pragma G++ optimize("-fstrict-aliasing")
	#pragma G++ optimize("-fstrict-overflow")
	#pragma G++ optimize("-falign-functions")
	#pragma G++ optimize("-fcse-skip-blocks")
	#pragma G++ optimize("-fcse-follow-jumps")
	#pragma G++ optimize("-fsched-interblock")
	#pragma G++ optimize("-fpartial-inlining")
	#pragma G++ optimize("no-stack-protector")
	#pragma G++ optimize("-freorder-functions")
	#pragma G++ optimize("-findirect-inlining")
	#pragma G++ optimize("-frerun-cse-after-loop")
	#pragma G++ optimize("inline-small-functions")
	#pragma G++ optimize("-finline-small-functions")
	#pragma G++ optimize("-ftree-switch-conversion")
	#pragma G++ optimize("-foptimize-sibling-calls")
	#pragma G++ optimize("-fexpensive-optimizations")
	#pragma G++ optimize("-funsafe-loop-optimizations")
	#pragma G++ optimize("inline-functions-called-once")
	#pragma G++ optimize("-fdelete-null-pointer-checks")
#endif