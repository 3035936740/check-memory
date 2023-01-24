#pragma once
#include <filesystem>
#include <fstream>
#include <exception>
#include <stdexcept>
#include "yaml-cpp/yaml.h"
#include "../common/utils.h"
#include "../exception/file_exception.h"


namespace define_value {
	static std::string proc_path{};

	void initialized(void) {
		try {
			std::ifstream fin("config.yaml");

			if (!fin.good() || !fin)
			{
				fin.close();
				throw FileException("Error while opening file!");
			}

			YAML::Parser parser(fin);

			YAML::Node doc;
			YAML::Node& path {doc["path"]};

			path["proc"] >> proc_path;
		}
		catch (const YAML::ParserException& e) {
			log_relevant::daily_log_record("找不到yaml对应参数", log_relevant::log_type::error);
		}
		catch (const FileException& e) {
			log_relevant::daily_log_record("不存在文件config.yaml", log_relevant::log_type::error);
		}
		catch (const std::exception& e) {
			log_relevant::daily_log_record("发生严重错误:" + std::string(e.what()), log_relevant::log_type::error);
		}
	}
}