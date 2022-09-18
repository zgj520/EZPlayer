#ifndef COREINTERFACE_H_
#define COREINTERFACE_H_
#include <string>

#ifdef CORE_EXPORT_API
#define EZCORE_EXPORT __declspec(dllexport)
#else
#define EZCORE_EXPORT __declspec(dllimport)
#endif // CORE_EXPORT

enum PlayState{
    PlayState_None,
    PlayState_Ready,
    PlayState_Playing,
    PlayState_Paused,
    PlayState_Stop
};

namespace EZCore {
    long EZCORE_EXPORT createFileInstance(const std::string &filePath, long wndId);

    void EZCORE_EXPORT play(long fileHandle);

    void EZCORE_EXPORT pause(long fileHandle);

    void EZCORE_EXPORT refreshCurrentFrame(long fileHandle);

    PlayState EZCORE_EXPORT getState(long fileHandle);

    void EZCORE_EXPORT destoryFileInstance(long fileHandle);
}

#endif