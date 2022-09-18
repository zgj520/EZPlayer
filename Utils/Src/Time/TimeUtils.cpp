#include "TimeUtils.h"
#include <chrono>
#include <sstream>
#include <iomanip>
namespace TimeUtils {
    std::string currentDateTimeString() {
        auto now = std::chrono::system_clock::now();
        auto tSeconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
        auto current_time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&current_time), "%Y%m%d-%H%M%S");
        auto tMilli = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        auto ms = tMilli - tSeconds;
        ss << "-" << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    int64_t getCurrentTimeUs() {
        std::chrono::system_clock::time_point time_point_now = std::chrono::system_clock::now(); // 获取当前时间点
        std::chrono::system_clock::duration duration_since_epoch = time_point_now.time_since_epoch(); // 从1970-01-01 00:00:00到当前时间点的时长
        time_t microseconds_since_epoch = std::chrono::duration_cast<std::chrono::microseconds>(duration_since_epoch).count() / std::chrono::system_clock::period::den; // 将时长转换为微秒数
        return microseconds_since_epoch;
    }

    int64_t getCurrentTimeMs() {
        return getCurrentTimeUs() / 1000;
    }
}
