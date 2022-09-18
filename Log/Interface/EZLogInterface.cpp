#include "EZLogInterface.h"
#include "Src/EZLogWraper.h"
#include <stdarg.h>

std::string formatArgStr(const char* format, va_list args) {
    char buffer[1024] = { 0 };
    vsprintf(buffer, format, args);
    return std::string(buffer);
}

namespace EZLog {
    bool initLog(const EZLogInitInfo& info) {
        return EZLogWraper::instance()->initLog(info);
    }

    void setLogLevel(EZLogLevel level) {
        EZLogWraper::instance()->setLogLevel(level);
    }

    void logOut(EZLogLevel level, const char* format, ...) {
        if (!EZLogWraper::instance()->shouldLog(level)) {
            return;
        }
        va_list args;
        va_start(args, format);
        auto str = formatArgStr(format, args);
        va_end(args);
        EZLogWraper::instance()->log(level, str);
    }
}