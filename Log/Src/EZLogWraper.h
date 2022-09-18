#ifndef EZLOG_H_
#define EZLOG_H_
#include <string>
#include "../Interface/EZLogInterface.h"

class EZLogWraper {
public:
    static EZLogWraper* instance();

    ~EZLogWraper();

    bool initLog(const EZLog::EZLogInitInfo& info);

    void setLogLevel(EZLog::EZLogLevel level);

    void log(EZLog::EZLogLevel level, const std::string& msg);

    bool shouldLog(EZLog::EZLogLevel level);

private:
    EZLog::EZLogLevel m_logLevel = EZLog::EZLogLevel_Info;
};

#endif