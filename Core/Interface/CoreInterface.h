#ifndef COREINTERFACE_H_
#define COREINTERFACE_H_
#include <string>
#include <functional>

#ifdef CORE_EXPORT_API
#define EZCORE_EXPORT __declspec(dllexport)
#else
#define EZCORE_EXPORT __declspec(dllimport)
#endif // CORE_EXPORT

namespace EZCore {
    enum PlayState {
        PlayState_None,
        PlayState_Ready,
        PlayState_Playing,
        PlayState_Paused,
        PlayState_EOF
    };
    struct MediaInfo {
        float fps = 0;
        int64_t duration = 0;
        std::string name;
        std::string long_name;
        int64_t frameCount = 0;
        int width = 0;
        int height = 0;
    };
    using PLAY_CALLBACK = std::function<void(int64_t, int64_t)>;

    long EZCORE_EXPORT createFileInstance(const std::string &filePath, long wndId);

    void EZCORE_EXPORT play(long fileHandle, PLAY_CALLBACK cbk);

    bool EZCORE_EXPORT pause(long fileHandle);

    bool EZCORE_EXPORT resume(long fileHandle);

    bool EZCORE_EXPORT seekTime(long fileHandle, int64_t time);

    void EZCORE_EXPORT refreshCurrentFrame(long fileHandle);

    PlayState EZCORE_EXPORT getState(long fileHandle);

    void EZCORE_EXPORT destoryFileInstance(long fileHandle);

    // Utils
    bool EZCORE_EXPORT getMediaInfo(const std::string& filePath, MediaInfo& info);
}

#endif