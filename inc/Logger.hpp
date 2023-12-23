#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <exception>
#include <unordered_map>
#include <functional>

#include <cstdio>
#include <cstdint>
#include <cstdarg>
#include <ctime>
#include <cstdlib>

#ifdef _WIN32
#include <process.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

class Logger {
public:
    enum {
        log__NONE = 0,
        log__ERROR,
        log__WARN,
        log__INFO,
        log__DEBUG,
        log__MAX,
    };
    enum {
        time__NONE = 0,
        time__ABSOLUTE,
        time__ABSOLUTE_MS,
        time__RELATIVE,
        time__MAX,
    };
    enum {
        enum__NONE = 0,
        enum__HASH,
        enum__MAP,
    };

public:
    static Logger & Instance() {
        static Logger instance;
        return instance;
    }
    void Name(const std::string & name, const bool append_pid=false) {

        if (GetEnv("LOG_OVR_OUT") != "") {
            // If LOG_OVR_OUT is defined, then omit (file)name, and use stdout.
            return;
        }

        name_ = name;
        std::stringstream ss;
        time_t now;
        time(&now);
        tm * local = localtime(&now);

        char basenameBuffer[128];
        if (append_pid) {
            sprintf(basenameBuffer, "%s_%04x"
                , name_.c_str()
                , GetProcessId()
            );
        } else {
            sprintf(basenameBuffer, "%s", name_.c_str());
        }

        char nameBuffer[256];
        sprintf(nameBuffer
            , "%s/%s_%02d%02d%02d_%02d%02d%02d.log"
            , GetLogFolder().c_str()
            , basenameBuffer
            , local->tm_year % 100
            , local->tm_mon + 1
            , local->tm_mday
            , local->tm_hour
            , local->tm_min
            , local->tm_sec
        );
        f_log_ = fopen(nameBuffer, "wt");
        if (f_log_ == NULL) {
            std::cerr << "Unable to open log file (";
            std::cerr << std::string(nameBuffer);
            std::cerr << "), using stdout!\n";
            f_log_ = stdout;
        }
    }
    void LogLevel(const int value) {
        auto sOverrideLevel = GetEnv("LOG_OVR_LEVEL");
        if (sOverrideLevel != "") {
            try {
                int ovrLevel = std::stoi(sOverrideLevel);
                level_ = ValidateLogLevel(ovrLevel);
            }
            catch (std::exception& e) {
                level_ = ValidateLogLevel(value);
            }
        } else {
            level_ = ValidateLogLevel(value);
        }
        LogMessage<Logger::log__INFO>("LogLevel is %s.", level_names_[level_].c_str());
    }
    void TimeFormat(const int value) {
        auto sOverrideTime = GetEnv("LOG_OVR_TS");
        if (sOverrideTime != "") {
            try {
                int ovrTime = std::stoi(sOverrideTime);
                time_format_ = ValidateTimeStamp(ovrTime);
            }
            catch (std::exception& e) {
                time_format_ = ValidateTimeStamp(value);
            }
        } else {
            time_format_ = ValidateTimeStamp(value);
        }
    }
    template<int level>
    void LogMessage(const std::string & message) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (level > log__NONE && level <= level_) {
            fprintf(f_log_
                , "%s|%s|%04x|%04x|%s|%s\n"
                , TimeStamp().c_str()
                , name_.c_str()
                , GetProcessId()
                , GetThreadNumber()
                , level_names_[level].c_str()
                , message.c_str()
            );
            fflush(f_log_);
            message_cb_(message);
        }
    }
    template<int level>
    void LogMessage(const char * format, ...) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (level > log__NONE && level <= level_) {
            enum { MAX_LOG_LINE = 512 };
            char message_buffer[MAX_LOG_LINE];
            va_list args;
            va_start(args, format);
            vsnprintf(message_buffer, MAX_LOG_LINE, format, args);
            va_end(args);
            std::string message(message_buffer);
            fprintf(f_log_
                , "%s|%s|%04x|%04x|%s|%s\n"
                , TimeStamp().c_str()
                , name_.c_str()
                , GetProcessId()
                , GetThreadNumber()
                , level_names_[level].c_str()
                , message.c_str()
            );
            fflush(f_log_);
            message_cb_(message);
        }
    }
    void SetLevelByName(const std::string& name) {
        auto f = level_ids_.find(name);
        if (f != level_ids_.end()) {
            LogLevel(level_ids_[name]);
        } else {
            LogMessage<Logger::log__WARN>("No such level (%s) exist, using DEBUG.", name.c_str() );
            LogLevel(log__DEBUG);
        }
    }
    void EnumThreadsBy(const int method) {
        thread_enum_method_ = method;
    }
    void SetCallbackHandler(std::function<void(std::string msg)> cb) {
        message_cb_ = cb;
    }

private:
    int thread_enum_method_;
    uint32_t thread_count_ = 1;
    std::unordered_map<std::thread::id, uint32_t> thread_list_;
    int time_format_;
    int level_;
    std::string name_ = "";
    FILE * f_log_;
    std::string level_names_[log__MAX];
    std::unordered_map<std::string, int> level_ids_;
    std::function<void(const std::string& )> message_cb_;

    uint32_t GetProcessId() {
        #ifdef _WIN32
        return (uint32_t) _getpid();
        #else
        return (uint32_t) getpid();
        #endif
    }
    uint32_t GetThreadNumber() {
        uint32_t threadId;
        switch (thread_enum_method_) {
        case enum__HASH:
            {
                std::hash<std::thread::id> hasher;
                threadId = hasher(std::this_thread::get_id());
                threadId &= 0xffff;
            } break;

        case enum__MAP:
        default:
            {
                auto id = std::this_thread::get_id();
                auto it = thread_list_.find(id);
                if (it == thread_list_.end()) {
                    thread_list_[id] = thread_count_;
                    threadId = thread_count_;
                    ++thread_count_;
                } else {
                    threadId = it->second;
                }
            } break;
        }

        return threadId;
    }

    std::string GetLogFolder() {
        std::string value = ".";
        char* pEnv = std::getenv("CPP_LOG_DIR");
        if (pEnv != 0) {
            value = std::string(pEnv);
        }
        return value;
    }

    std::string TimeStamp() {
        if (time_format_ == time__ABSOLUTE_MS) {
            return GetAbsTimeInMs();
        }
        else if (time_format_ == time__RELATIVE) {
            return GetRelTime();
        }
        else if (time_format_ == time__ABSOLUTE) {
            return GetAbsTime();
        }
        else {
            return std::string("---");
        }
    }
    std::string GetAbsTimeInMs() {
        auto t_now = std::chrono::system_clock::now();
        double ts = std::chrono::duration<double>(t_now.time_since_epoch()).count();
        char timeBuffer[256];
        sprintf(timeBuffer, "%15.4f", ts);
        return std::string(timeBuffer);
    }
    std::string GetAbsTime() {
        time_t now;
        time(&now);
        tm * local = localtime(&now);
        char timeBuffer[256];
        sprintf(timeBuffer
            , "%02d-%02d-%02d %02d:%02d:%02d"
            , local->tm_year % 100
            , local->tm_mon + 1
            , local->tm_mday
            , local->tm_hour
            , local->tm_min
            , local->tm_sec
        );
        return std::string(timeBuffer);
    }

    std::string GetRelTime() {
        auto t_now = std::chrono::steady_clock::now();
        double ts = 0.001 * std::chrono::duration_cast<std::chrono::milliseconds>(t_now - kReferenceTime).count();
        char timeBuffer[256];
        sprintf(timeBuffer, "%09.3f", ts);
        return std::string(timeBuffer);
    }

    std::string GetEnv(const std::string & name, const std::string & defaultValue="") {
        std::string value = defaultValue;
        char * pEnv = std::getenv(name.c_str());
        if (pEnv != 0) {
            value = std::string(pEnv);
        }
        return value;
    }

    const int ValidateLogLevel(const int level) {
        int valid_level = level;
        if (valid_level < log__NONE || valid_level >= log__MAX) {
            valid_level = log__DEBUG;
        }
        return valid_level;
    }
    const int ValidateTimeStamp(const int ts) {
        int ts_value = ts;
        if (ts_value < time__NONE || ts_value >= time__MAX) {
            ts_value = time__ABSOLUTE_MS;
        }
        return ts_value;
    }

    Logger()
    : kReferenceTime(std::chrono::steady_clock::now())
    {
        level_names_[log__NONE]  = "NONE ";
        level_names_[log__ERROR] = "ERROR";
        level_names_[log__WARN]  = "WARN ";
        level_names_[log__INFO]  = "INFO ";
        level_names_[log__DEBUG] = "DEBUG";

        level_ids_["none"] = log__NONE;
        level_ids_["error"] = log__ERROR;
        level_ids_["warn"] = log__WARN;
        level_ids_["info"] = log__INFO;
        level_ids_["debug"] = log__DEBUG;

        time_format_ = time__ABSOLUTE;
        level_ = log__DEBUG;
        f_log_ = stdout;

        thread_enum_method_ = enum__MAP;
        message_cb_ = [](const std::string&){};
    }
    ~Logger() {
        if (f_log_ != NULL && f_log_ != stdout) {
            fclose(f_log_);
        }
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> kReferenceTime;
    std::mutex mutex_;
};

#ifdef ENABLE_LOGS

#define LOGGER Logger::Instance()
#define L_USE_THREAD_HASH Logger::Instance().EnumThreadsBy(Logger::enum__HASH)
#define L_USE_THREAD_MAP Logger::Instance().EnumThreadsBy(Logger::enum__MAP)
#define L_NAME(n) Logger::Instance().Name(n)
#define L_NAME_PID(n) Logger::Instance().Name(n, true)
#define L_LEVEL(s) Logger::Instance().LogLevel(Logger::log__##s)
#define L_ABS_TIME Logger::Instance().TimeFormat(Logger::time__ABSOLUTE)
#define L_ABS_TIME_MS Logger::Instance().TimeFormat(Logger::time__ABSOLUTE_MS)
#define L_REL_TIME Logger::Instance().TimeFormat(Logger::time__RELATIVE)
#define L_NO_TIME Logger::Instance().TimeFormat(Logger::time__NONE)
#define L_ERROR Logger::Instance().LogMessage<Logger::log__ERROR>
#define L_WARN Logger::Instance().LogMessage<Logger::log__WARN>
#define L_INFO Logger::Instance().LogMessage<Logger::log__INFO>
#define L_DEBUG Logger::Instance().LogMessage<Logger::log__DEBUG>
#define L_CALLBACK(f) Logger::Instance().SetCallbackHandler(f)
#define L_NONE

#else

#define LOGGER
#define L_USE_THREAD_HASH
#define L_USE_THREAD_MAP
#define L_NAME(n)
#define L_NAME_PID(n)
#define L_LEVEL(s)
#define L_ABS_TIME
#define L_ABS_TIME_MS
#define L_REL_TIME
#define L_NO_TIME
#define L_ERROR
#define L_WARN
#define L_INFO
#define L_DEBUG
#define L_CALLBACK(f)
#define L_NONE

#endif

#endif // LOGGER_HPP
