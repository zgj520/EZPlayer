#include "CoreManger.h"

CoreManger CoreManger::m_instance;
CoreManger& CoreManger::instance() {
    return m_instance;
}

long CoreManger::createFileInstance(const std::string& filePath, long wndId) {
    static long index = 1;
    auto id = index;
    auto pPlayCore = new PlayCore(filePath, wndId);
    m_mapFileInstance[id] = pPlayCore;
    index++;
    return id;
}

void CoreManger::destoryFileInstance(long fileHandle) {
    auto iter = m_mapFileInstance.find(fileHandle);
    if (iter == m_mapFileInstance.end()) {
        return;
    }
    delete iter->second;
    m_mapFileInstance.erase(iter);
}

void CoreManger::play(long fileHandle) {
    auto iter = m_mapFileInstance.find(fileHandle);
    if (iter == m_mapFileInstance.end()) {
        return;
    }
    iter->second->play();
}

void CoreManger::refreshCurrentFrame(long fileHandle) {
    auto iter = m_mapFileInstance.find(fileHandle);
    if (iter == m_mapFileInstance.end()) {
        return;
    }
    iter->second->refreshCurrentFrame();
}

PlayState CoreManger::getState(long fileHandle) {
    auto iter = m_mapFileInstance.find(fileHandle);
    if (iter == m_mapFileInstance.end()) {
        return PlayState_None;
    }
    iter->second->getState();
}

CoreManger::CoreManger()
{
}

CoreManger::~CoreManger()
{
}