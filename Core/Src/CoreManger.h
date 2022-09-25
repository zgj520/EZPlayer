#pragma once
#include <map>
#include <string>
#include "PlayCore.h"

class CoreManger
{
public:
    static CoreManger& instance();

    long createFileInstance(const std::string& filePath, long wndId);

    void destoryFileInstance(long fileHandle);

    void play(long fileHandle, EZCore::PLAY_CALLBACK cbk);

    bool pause(long fileHandle);

    bool resume(long fileHandle);

    bool seekTime(long fileHandle, int64_t time);

    void refreshCurrentFrame(long fileHandle);

    EZCore::PlayState getState(long fileHandle);

private:
    CoreManger();
    ~CoreManger();

private:
    static CoreManger m_instance;
    std::map<long, PlayCore*> m_mapFileInstance;
};