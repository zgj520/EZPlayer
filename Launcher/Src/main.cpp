#include "../Log/Interface/EZLogInterface.h"
#include "../Utils/Src/Time/TimeUtils.h"
#include "../Utils/Src/String/StringUtils.h"
#if defined(__windows__)
#include <windows.h>
#include <Shlobj.h>
#endif


using mainEnterPoint = int(*)(int argc, char* argv[]);
const char* entrySymbolName = "main";
#if defined(__windows__)
const WCHAR* enterDllName = L"Framework.dll";
#else
const char* enterDllName = L"Framework.dylib";
#endif

void initLog() {
    std::string logPath = "";
#if defined(__windows__)
    WCHAR buffer[MAX_PATH];
    SHGetSpecialFolderPath(0, buffer, CSIDL_LOCAL_APPDATA, false);
    logPath = wstring2String(buffer) + "\\EZPlayer\\UserData\\Log\\" + TimeUtils::currentDateTimeString() + ".log";
#endif
    EZLog::initLog({ EZLog::EZLogType_MMapFile,logPath, 0 });
    EZLog::setLogLevel(EZLog::EZLogLevel_Debug);
}

#ifdef __windows__
int wmain(int argc, char* argv[]) {

    initLog();

    int ret = -100;
    wchar_t exePath[MAX_PATH + 1] = {0};
    ::GetModuleFileName(NULL, exePath, MAX_PATH);

    std::wstring path = std::wstring(exePath);
    auto pos = path.find_last_of(L"\\");
    if (pos != std::wstring::npos) {
        path = path.substr(0, pos);
    }
    path += L"\\";
    path += enterDllName;

    HMODULE hMod = ::LoadLibrary(path.c_str());
    if (hMod == NULL) {
        return ret;
    }
    auto enterPoint = reinterpret_cast<mainEnterPoint>(::GetProcAddress(hMod, entrySymbolName));
    if (enterPoint == nullptr) {
        return ret - 1;
    }
    return enterPoint(argc, argv);
}
#endif // __windows__


