#include "EZLogWraper.h"
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include "spdlog/spdlog.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <filesystem>

std::shared_ptr<spdlog::logger> m_fileLogger = nullptr;

EZLogWraper* EZLogWraper::instance() {
    static EZLogWraper* self = new EZLogWraper;
    return self;
}

EZLogWraper::~EZLogWraper() {
    spdlog::drop_all();
}

bool EZLogWraper::initLog(const EZLog::EZLogInitInfo& info) {
#if defined(__windows__)
    try {
        if (info.type == EZLog::EZLogType_Console) {
            m_fileLogger = spdlog::stdout_color_mt("console");
        }
        else if(info.type == EZLog::EZLogType_NormalFile || info.type == EZLog::EZLogType_MMapFile){
            std::string logfileRealPath = info.logfilePath;
            std::filesystem::path u8LogFilePath = std::filesystem::u8path(info.logfilePath);
            if (u8LogFilePath.empty()) {
                return false;
            }
            std::error_code err;
            std::filesystem::create_directories(u8LogFilePath.parent_path(), err);
            logfileRealPath = u8LogFilePath.string();
            bool bMMap = info.type == EZLog::EZLogType_MMapFile;
            if (info.maxFileCount > 0) {
                m_fileLogger = spdlog::rotating_logger_mt("file_logger", logfileRealPath, 50 * 1024 * 1024, info.maxFileCount, false);
            }
            else {
                m_fileLogger = spdlog::basic_logger_mt("file_logger", logfileRealPath, false, bMMap);
            }
        }
        else {
            return false;
        }
    }
    catch (...) {

    }
#endif
    // all logger set
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e][%l][%t]%v");
    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_every(std::chrono::seconds(2));
    spdlog::flush_on(spdlog::level::warn);
    spdlog::set_default_logger(m_fileLogger);
    // single logger set
    if (m_fileLogger != nullptr) {
        m_fileLogger->flush_on(spdlog::level::warn);
        return true;
    }
    return false;
}

void EZLogWraper::setLogLevel(EZLog::EZLogLevel level){
    m_logLevel = level;
}

void EZLogWraper::log(EZLog::EZLogLevel level, const std::string& msg) {
    if (m_logLevel > level || m_fileLogger == nullptr) {
        return;
    }
    switch (level)
    {
    case EZLog::EZLogLevel_Debug:
        m_fileLogger->debug(msg);
        break;
    case EZLog::EZLogLevel_Info:
        m_fileLogger->info(msg);
        break;
    case EZLog::EZLogLevel_Warn:
        m_fileLogger->warn(msg);
        break;
    case EZLog::EZLogLevel_Error:
        m_fileLogger->error(msg);
        break;
    case EZLog::EZLogLevel_Fata:
        m_fileLogger->critical(msg);
        break;
    default:
        break;
    }
}

bool EZLogWraper::shouldLog(EZLog::EZLogLevel level) {
    if (m_logLevel > level || m_fileLogger == nullptr) {
        return false;
    }
    return true;
}