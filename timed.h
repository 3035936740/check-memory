//不要乱动到其他的头文件了,不然爆炸了没你好果子吃(
#pragma once

//是否开启吸氧大法(对程序优化性能)
//#define OXYGEN_INHALATION_METHOD

#include <iostream>
#include <memory>
#include <fstream>
#include <filesystem>
#include <vector>
#include <regex>
#include <thread>

#include <mutex>
#include <condition_variable>

#include <exception>
#include <stdexcept>
#include <algorithm>

#include "exception/file_exception.h"
#include "common/properties.hpp"
#include "common/utils.h"

/*
以下三方库的头文件的顺序不要改!
千万不要改, 改了就不能通过编译了,
我也不知道原理是什么,反正这个顺序不要改
*/

#include "source_location.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "fmt/format.h"

using namespace std::chrono_literals;
using namespace std::string_literals;

using byte = signed char;
using __uint64 = unsigned long int;

void reloadProcessPIDMsg(void);
bool reloadProcessPID(void);
bool release_resource();
inline namespace info {
	std::vector<std::string> getProcessStatus(const std::string& pid);
	std::vector<std::string> getMemoryStatus(void);
}

//到点(2:30)会全部记录到文件中
static std::shared_ptr<spdlog::logger> logger {};

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

inline namespace config_variable {
	static bool end{ false };
	static bool performance{ false };
	static std::string write_log_time{};
	static std::string meminfo{};
	static std::string process_pid_file_path{};
	static std::vector<std::string> executes{};
	static std::vector<__uint64> processes_pid{};
	static __uint64 max_memory_size{};
	static __uint64 timer{};
	static __uint64 await{};
	static __uint64 kill_await{};
	static __uint64 wait_for_sleep{};
	static int signal{};
	static float max_memory_usage{};
};

namespace define_value {
	void init(void) {
		properties prop;
		json data = prop.getJsonData();

		int hour{ data["log_write_time"][0] },
			minute{ data["log_write_time"][1] };
		
		write_log_time = std::format("{}:{}", hour, minute);

		logger = spdlog::daily_logger_mt("daily_logger", "timed_logs/daily.txt", hour, minute);
		meminfo = data["proc"];
		process_pid_file_path = data["process_pid_file_path"];
		executes = data["execute"];
		timer = data["timer"];
		await = data["await"];
		kill_await = data["kill_await"];
		config_variable::signal = data["signal"];
		performance = data["performance"];
		max_memory_usage = data["max_memory_usage"];
		wait_for_sleep = data["wait_for_sleep"];

		auto memory_info{ info::getMemoryStatus() };
		for (auto item : memory_info) {
			if (tools::is_match_literal(item, "MemTotal")) {
				max_memory_size = tools::match_numerical(item);
				break;
			}
		}

		reloadProcessPIDMsg();
	}
};

void reloadProcessPIDMsg(void) {
	spdlog::info("======================");
	if (reloadProcessPID()) {
		spdlog::info("成功获取进程PID,PID为:");
		for (const auto& iter : processes_pid) {
			spdlog::info(iter);
		}
	}
	else {
		log_relevant::daily_log_record(std::format("{}路径文件不存在", process_pid_file_path), log_relevant::error);
	}
	spdlog::info("======================");
}

// 重新获取pid,获取成功返回true
bool reloadProcessPID(void){
	std::ifstream file_stream{ process_pid_file_path };

	if (!file_stream.good() || !file_stream)
	{
		file_stream.close();
		return false;
	}

	processes_pid.clear();
	std::string line{};

	while (std::getline(file_stream, line)) {
		processes_pid.push_back(std::stoull(line));
	}

	file_stream.close();
	return true;
}

inline void initialized(void) {
	spdlog::info("正在初始化");

	//JSON获取变量初始化
	define_value::init();

	//设置全局日志等级
	logger->set_level(spdlog::level::info);
	spdlog::set_level(spdlog::level::info);
	spdlog::info("初始化完成");
}

inline namespace info {
	std::vector<std::string> getMemoryStatus(void) {
		std::filesystem::path p{ meminfo };
		std::ifstream file_stream{ p };

		if (!file_stream.good() || !file_stream)
		{
			file_stream.close();
			log_relevant::daily_log_record("无法找到文件", log_relevant::error);
			release_resource();
			throw FileException("error while opening file!");
		}
		std::vector<std::string> result{};
		std::string line{};

		while (std::getline(file_stream, line)) {
			result.push_back(line);
		}

		file_stream.close();
		return result;
	};

	//获取进程状态
	std::vector<std::string> getProcessStatus(const std::string& pid) {
		std::filesystem::path p{ "/./proc/" + pid + "/status" };

		std::ifstream file_stream{ p };

		std::vector<std::string> result{};

		if (!file_stream.good() || !file_stream)
		{
			file_stream.close(); 
			return result;
		}
		std::string line{};

		while (std::getline(file_stream, line)) {
			result.push_back(line);
		}

		file_stream.close();
		return result;
	};
}

//释放资源,释放成功返回true
bool release_resource() {
	try {
		logger.reset();
	}
	catch (const std::exception& e) {
		spdlog::info("资源释放失败,detail:" + std::string(e.what()));
		return false;
	}

	spdlog::info("资源释放成功");
	return true;
}

// TODO: 在此处引用程序需要的其他标头。

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