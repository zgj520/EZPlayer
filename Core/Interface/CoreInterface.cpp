#include "CoreInterface.h"
#include "CoreManger.h"
namespace EZCore {
    long createFileInstance(const std::string& filePath, long wndId) {
        return CoreManger::instance().createFileInstance(filePath, wndId);
    }

    void play(long fileHandle) {
        CoreManger::instance().play(fileHandle);
    }

    void pause(long fileHandle) {

    }

    void refreshCurrentFrame(long fileHandle) {
        CoreManger::instance().refreshCurrentFrame(fileHandle);
    }

    PlayState getState(long fileHandle) {
        return CoreManger::instance().getState(fileHandle);
    }

    void destoryFileInstance(long fileHandle) {

    }
}