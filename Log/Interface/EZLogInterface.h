#ifndef EZLOGINTERFACE_H_
#define EZLOGINTERFACE_H_
#include <string>

#ifdef EZLOG_EXPORT_API
#define EZLOG_EXPORT __declspec(dllexport)
#else
#define EZLOG_EXPORT __declspec(dllimport)
#endif // EZLOG_EXPORT

namespace EZLog {
    enum EZLogLevel
    {
        EZLogLevel_Debug = 1,
        EZLogLevel_Info,
        EZLogLevel_Warn,
        EZLogLevel_Error,
        EZLogLevel_Fata
    };
    enum EZLogType
    {
        EZLogType_Console = 1,
        EZLogType_NormalFile,
        EZLogType_MMapFile
    };
    struct EZLogInitInfo
    {
        EZLogType type = EZLogType_NormalFile;
        std::string logfilePath = "";
        unsigned int maxFileCount = 0;
    };

    bool EZLOG_EXPORT initLog(const EZLogInitInfo& info);

    void EZLOG_EXPORT setLogLevel(EZLogLevel level);

    void EZLOG_EXPORT logOut(EZLogLevel level, const char* format, ...);
}

/*=======================================================func defined===================================================================*/
#ifdef __windows__
#define __FILENAME__ (strrchr("\\" __FILE__, '\\') + 1)
#else
#define __FILENAME__ (strrchr("/" __FILE__, '/') + 1)
#endif

#define EZLOGD(format, ...) EZLog::logOut(EZLog::EZLogLevel_Debug, "[%s:%s:%d]" format,__FILENAME__,__func__,__LINE__, ##__VA_ARGS__);

#define EZLOGI(format, ...) EZLog::logOut(EZLog::EZLogLevel_Info, "[%s:%s:%d]" format,__FILENAME__,__func__,__LINE__, ##__VA_ARGS__);

#define EZLOGW(format, ...) EZLog::logOut(EZLog::EZLogLevel_Warn, "[%s:%s:%d]" format,__FILENAME__,__func__,__LINE__, ##__VA_ARGS__);

#define EZLOGE(format, ...) EZLog::logOut(EZLog::EZLogLevel_Error, "[%s:%s:%d]" format,__FILENAME__,__func__,__LINE__, ##__VA_ARGS__);

#define EZLOGF(format, ...) EZLog::logOut(EZLog::EZLogLevel_Fata, "[%s:%s:%d]" format,__FILENAME__,__func__,__LINE__, ##__VA_ARGS__);
#endif