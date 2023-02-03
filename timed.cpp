// timed.cpp: 定义应用程序的入口点。
//
#include "timed.h"
//#define DEBUG

std::mutex mtx;              // 全局互斥锁.
std::condition_variable cv;  // 全局条件变量.

void sub();

inline void killProcess(void) {
    for (const auto& iter : processes_pid) {
        std::string cmd{ std::format("kill -{} {}",config_variable::signal,iter) };
        std::cout << std::format(">> {}",cmd) << "\n";
        if (system(cmd.c_str()) == -1) {
            log_relevant::daily_log_record("指令有误",log_relevant::error);
        }
        else
        {
            log_relevant::daily_log_record(std::format("PID:{} 已被杀死", iter));
        }
    }
}

int main(int argc, char* argv[])
{
    // initialized
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    initialized();

    std::thread sub_thread(sub);
    __uint64 max_memory_size{ config_variable::max_memory_size };
    float max_memory_usage{ config_variable::max_memory_usage };
    bool wait_for_sleep_flag{ config_variable::wait_for_sleep > 0 };
    auto wait_for_sleep{ std::chrono::milliseconds(config_variable::wait_for_sleep) };
    auto chrono_timer{ std::chrono::milliseconds(config_variable::timer) };
    auto chrono_await{std::chrono::milliseconds(config_variable::await)};
    auto chrono_kill_await{std::chrono::milliseconds(config_variable::kill_await)};
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

    /* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
    log_relevant::daily_log_record(std::format("{}", "启动成功"));

    if (processes_pid.empty())
    {
        log_relevant::daily_log_record("进程不存在", log_relevant::error);
        for (const auto& iter : executes) {

            tools::exec_simple(iter.c_str());

            std::this_thread::sleep_for(chrono_await);
            
            if (reloadProcessPID()) {
                spdlog::info("进程PID为:");
                for (const auto& iter : processes_pid) {
                    spdlog::info(iter);
                }
            }

            log_relevant::daily_log_record("进程启动成功", log_relevant::info);
        }
    }

    sub_thread.detach();

    try {
        while (true) {
            #ifdef DEBUG
            auto start = std::chrono::high_resolution_clock::now();
            #endif

            //PID是否存在(不存在重启)
            for (const auto& iter : processes_pid) {
                auto cmd{ "ps -ef | grep " + std::to_string(iter) + " | grep -v grep" };
                std::string res{ tools::exec(cmd.c_str()) };
                if (res.empty())
                {
                    std::cout << std::endl;
                    log_relevant::daily_log_record("进程不存在", log_relevant::error);
                    for (const auto& iter : executes) {
                        tools::exec_simple(iter.c_str());

                        std::this_thread::sleep_for(chrono_await);
                        log_relevant::daily_log_record(std::format("进程启动成功"), log_relevant::info);
                        if (reloadProcessPID()) {
                            spdlog::info("进程PID为:");
                            for (const auto& iter : processes_pid) {
                                spdlog::info(iter);
                            }
                        }
                        else {
                            log_relevant::daily_log_record(std::format("文件不存在"), log_relevant::error);
                        }
                    }
                    std::cout << std::endl;
                }
            }

            if (performance)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(config_variable::timer));
            }
            else {
                std::unique_lock<std::mutex> lck(mtx);
                cv.wait_for(lck, chrono_timer, [&wait_for_sleep,&wait_for_sleep_flag] {
                    if (wait_for_sleep_flag)
                    {
                        std::this_thread::sleep_for(wait_for_sleep);
                    }
                    return config_variable::end;
                    });
            }
            if (config_variable::end) {
                break;
            }

            //获取meminfo的信息
            auto memory_info{ info::getMemoryStatus() };
            __uint64 memory_available{}, memory_used{};
            float memory_usage{};
            for (const auto& item : memory_info) {
                if (tools::is_match_literal(item, "MemAvailable")) {
                    memory_available = tools::match_numerical(item);
                    memory_used = max_memory_size - memory_available;
                    memory_usage = static_cast<float>(memory_used) / static_cast<float>(max_memory_size);
                    break;
                }
            }

            if (memory_usage >= max_memory_usage)
            {
                std::cout << std::endl;
                log_relevant::daily_log_record(std::format("现内存使用率为:{}%,超过了{}%,正在试图kill进程", memory_usage * 100, max_memory_usage * 100));
                killProcess();
                std::this_thread::sleep_for(chrono_kill_await);
                std::cout << std::endl;
            }

        #ifdef DEBUG
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;
            std::cout << "Waited " << elapsed.count() << " ms\n";
        #endif
        }
    }
    catch (const std::runtime_error& e) {
        log_relevant::daily_log_record(std::format("运行时异常,detail:{}", e.what()), log_relevant::error);
    }catch (const std::exception& e) {
        log_relevant::daily_log_record(std::format("发生严重错误,detail:{}", e.what()), log_relevant::error);
    }


    
    /* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
    std::cout.flush();
    release_resource();
    return 0;
}

void sub() {
    std::this_thread::sleep_for(2500ms);
    std::cout << "\n\n#-- 输入help查看帮助列表 --#\n";
    while (true)
    {
        std::cout << ">> ";
        std::string command;
        std::cin >> command;

        //全部转换为小写
        transform(command.begin(), command.end(), command.begin(), ::tolower);

        if (command.compare("dd") == 0) {
            bool flag_temp{ false };
            std::thread t([&flag_temp] {
                while (true) {
                    if (flag_temp)break;
                    if (system("clear") == -1) {
                        log_relevant::daily_log_record("指令有误", log_relevant::error);
                    }
                    auto memory_info{ info::getMemoryStatus() };
                    __uint64 memory_available{}, memory_used{};
                    for (const auto& item : memory_info) {
                        if (tools::is_match_literal(item, "MemAvailable")) {
                            memory_available = tools::match_numerical(item);
                            memory_used = config_variable::max_memory_size - memory_available;
                            break;
                        }
                    }
                    __uint64 VmHWM{}, VmRSS{};
                    //获取进程相关信息
                    for (const auto& iter : config_variable::processes_pid) {
                        auto process_info{ getProcessStatus(std::to_string(iter)) };
                        if (process_info.empty()) {
                            log_relevant::daily_log_record("进程状态获取失败", log_relevant::error);
                        }
                        else {
                            for (const auto& item : process_info) {
                                if (tools::is_match_literal(item, "VmRSS")) {
                                    VmRSS = tools::match_numerical(item);
                                    break;
                                }
                            }
                            for (const auto& item : process_info) {
                                if (tools::is_match_literal(item, "VmHWM")) {
                                    VmHWM = tools::match_numerical(item);
                                    break;
                                }
                            }
                        }
                    }

                    std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n";
                    if (reloadProcessPID()) {
                        std::cout << "当前进程的PID:";
                        for (const auto& iter : processes_pid) {
                            std::cout << iter << "\t";
                        }
                    }
                    std::cout << "\n";

                    std::cout << "内存使用率:" << static_cast<float>(memory_used) / static_cast<float>(config_variable::max_memory_size) * 100 << "%\n"
                        << "最大物理内存:" << config_variable::max_memory_size << "KiB\n"
                        << "剩余物理内存:" << memory_available << "KiB\n"
                        << "已用物理内存:" << memory_used << "KiB\n"
                        << "进程已用物理内存:" << VmRSS << "KiB\n"
                        << "进程物理内存峰值:" << VmHWM << "KiB\n";
                    std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n"
                              << "按回车(Enter)键关闭" << std::endl;
                    std::this_thread::sleep_for(1s);
                }
            });
            t.detach();
            getchar();
            getchar();
            flag_temp = true;
            if (system("clear") == -1) {
                log_relevant::daily_log_record("指令有误", log_relevant::error);
            }
            std::cout << "#-- 输入help查看帮助列表 --#" << std::endl;
        }
        else if (command.compare("kill") == 0) {
            killProcess();
        }
        else if (command.compare("info") == 0)
        {
            //获取meminfo的信息
            auto memory_info{ info::getMemoryStatus() };
            __uint64 memory_available{},memory_used{};
            for (const auto& item : memory_info) {
                if (tools::is_match_literal(item, "MemAvailable")) {
                    memory_available = tools::match_numerical(item);
                    memory_used = config_variable::max_memory_size - memory_available;
                    break;
                }
            }
            __uint64 VmHWM{}, VmRSS{};
            //获取进程相关信息
            for (const auto& iter : config_variable::processes_pid) {
                auto process_info{ getProcessStatus(std::to_string(iter)) };
                if (process_info.empty()) {
                    log_relevant::daily_log_record("进程状态获取失败", log_relevant::error);
                }
                else {
                    for (const auto& item : process_info) {
                        if (tools::is_match_literal(item, "VmRSS")) {
                            VmRSS = tools::match_numerical(item);
                            break;
                        }
                    }
                    for (const auto& item : process_info) {
                        if (tools::is_match_literal(item, "VmHWM")) {
                            VmHWM = tools::match_numerical(item);
                            break;
                        }
                    }
                }
            }

            std::cout << "\n\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n";
            if (reloadProcessPID()) {
                std::cout << "当前进程的PID:";
                for (const auto& iter : processes_pid) {
                    std::cout << iter << "\t";
                }
            }
            std::cout << "\n";

            std::cout << "内存使用率:" << static_cast<float>(memory_used) / static_cast<float>(config_variable::max_memory_size) * 100 << "%\n"
                << "最大物理内存:" << config_variable::max_memory_size << "KiB\n"
                << "剩余物理内存:" << memory_available << "KiB\n"
                << "已用物理内存:" << memory_used << "KiB\n"
                << "进程已用物理内存:" << VmRSS << "KiB\n"
                << "进程物理内存峰值:" << VmHWM << "KiB\n\n"
                << "(*)配置信息:\n"
                << std::format("-- 每天 {} 的时候将爆炸信息写入日志\n", config_variable::write_log_time)
                << "-- 每 " << config_variable::timer << "ms 自检一次\n"
                << "-- 等待程序启动(初始化) " << config_variable::await << "ms 后开始检测\n"
                << "-- 等待程序终止(释放资源) " << config_variable::kill_await << "ms 后开始检测\n"
                << "-- 内存使用率达到 " << config_variable::max_memory_usage * 100 << "% 的时候重启程序\n"
                << "-- kill的信号: " << config_variable::signal << "\n\t*(1:新加载进程,9:强制杀死一个进程,15:正常停止一个进程)\n"
                << "-- 是否开启了性能模式: " << config_variable::performance << "\n\t*(一般填写false,填写true也只能提升1ms,为了1ms的性能值得吗)\n";
            std::cout <<  "-- 读取存储信息的目录:" << config_variable::meminfo << "\n"
                << "-- PID生成目录:" << config_variable::process_pid_file_path << "\n"
                << "-- 运行进程的脚本:" << "\n";
            for (const auto& iter : config_variable::executes){
                std::cout << "\t====>  " << iter << "\n";
            }
            std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n";

        }
        else if (command.compare("quit") == 0 || command.compare("exit") == 0) {
            std::unique_lock <std::mutex> lck(mtx);

            config_variable::end = true;

            cv.notify_all();

            break;
        }
        else if (command.compare("quit_kill") == 0 || command.compare("exit_kill") == 0) {
            std::unique_lock <std::mutex> lck(mtx);
            killProcess();

            config_variable::end = true;
            cv.notify_all();
            break;
        }else if (command.compare("clear") == 0) {
            if (system("clear") == -1) {
                log_relevant::daily_log_record("指令有误", log_relevant::error);
            }
            else {
                std::cout << "#-- 输入help查看帮助列表 --#\n";
            }
        }
        else if (command.compare("help") == 0) {
            std::cout << "\n\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n";
            std::cout << "- info 获取有关信息\n"
                << "- quit/exit 结束此程序\n"
                << "- quit_kill/exit_kill 结束此程序并杀死进程\n"
                << "- help 帮助列表\n"
                << "- kill 杀死进程\n"
                << "- clear 清屏\n"
                << "- dd 动态检测内存状态\n";
            std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n\n";
        }
        else {
            std::cout << "\n- (!)this command doesn't exist\n\n";
        }
        std::cout.flush();
    }
};
