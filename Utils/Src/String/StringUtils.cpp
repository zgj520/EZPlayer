#include "StringUtils.h"
#include <codecvt>

std::string wstring2String(const std::wstring& input_wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(input_wstr);
}

std::wstring string2WString(const std::string& input_str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(input_str);
}