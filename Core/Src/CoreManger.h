#pragma once
#include <map>
#include "PlayCore.h"
#include <string>

class CoreManger
{
public:
    static CoreManger& instance();

    long createFileInstance(const std::string& filePath, long wndId);

    void play(long fileHandle);

    void refreshCurrentFrame(long fileHandle);

    PlayState getState(long fileHandle);

private:
    CoreManger();
    ~CoreManger();

private:
    static CoreManger m_instance;
    std::map<long, PlayCore*> m_mapFileInstance;
};