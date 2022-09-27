#include "CoreInterface.h"
#include "CoreManger.h"
#include "../Src/Decode/FFUtils.h"

namespace EZCore {
    long createFileInstance(const std::string& filePath, long wndId) {
        return CoreManger::instance().createFileInstance(filePath, wndId);
    }

    void play(long fileHandle, PLAY_CALLBACK cbk) {
        CoreManger::instance().play(fileHandle, cbk);
    }

    bool pause(long fileHandle) {
        return CoreManger::instance().pause(fileHandle);
    }

    bool resume(long fileHandle) {
        return CoreManger::instance().resume(fileHandle);
    }

    bool seekTime(long fileHandle, int64_t time) {
        return CoreManger::instance().seekTime(fileHandle, time);
    }

    void refreshCurrentFrame(long fileHandle) {
        CoreManger::instance().refreshCurrentFrame(fileHandle);
    }

    PlayState getState(long fileHandle) {
        return CoreManger::instance().getState(fileHandle);
    }

    void destoryFileInstance(long fileHandle) {
        CoreManger::instance().destoryFileInstance(fileHandle);
    }

    bool getMediaInfo(const std::string& filePath, MediaInfo& info) {
        return FFUtils::getInstance()->getMediaInfo(filePath, info);
    }
}