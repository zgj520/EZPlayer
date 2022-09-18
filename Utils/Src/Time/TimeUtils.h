#ifndef TIMEUTILS_H_
#define TIMEUTILS_H_
#include <string>

namespace TimeUtils{
    std::string currentDateTimeString();

    int64_t getCurrentTimeUs();

    int64_t getCurrentTimeMs();
}

#endif