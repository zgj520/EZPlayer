// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#ifndef SPDLOG_HEADER_ONLY
#    include <spdlog/details/file_helper.h>
#endif

#include <spdlog/details/os.h>
#include <spdlog/common.h>

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <string>
#include <thread>
#include <tuple>

namespace spdlog {
namespace details {

SPDLOG_INLINE file_helper::file_helper(const file_event_handlers &event_handlers)
    : event_handlers_(event_handlers)
{}

SPDLOG_INLINE file_helper::~file_helper()
{
    close();
}

SPDLOG_INLINE void file_helper::open(const filename_t &fname, bool truncate, bool bMMap)
{
    close();
    filename_ = fname;

    auto *mode = SPDLOG_FILENAME_T("ab");
    auto *trunc_mode = SPDLOG_FILENAME_T("wb");

    if (event_handlers_.before_open)
    {
        event_handlers_.before_open(filename_);
    }
#if defined(__windows__)
    if (bMMap) {
        if (create_file_use_mmp(truncate)) {
            return;
        }
    }
#endif

    for (int tries = 0; tries < open_tries_; ++tries)
    {
        // create containing folder if not exists already.
        os::create_dir(os::dir_name(fname));
        if (truncate)
        {
            // Truncate by opening-and-closing a tmp file in "wb" mode, always
            // opening the actual log-we-write-to in "ab" mode, since that
            // interacts more politely with eternal processes that might
            // rotate/truncate the file underneath us.
            std::FILE *tmp;
            if (os::fopen_s(&tmp, fname, trunc_mode))
            {
                continue;
            }
            std::fclose(tmp);
        }
        if (!os::fopen_s(&fd_, fname, mode))
        {
            if (event_handlers_.after_open)
            {
                event_handlers_.after_open(filename_, fd_);
            }
            return;
        }

        details::os::sleep_for_millis(open_interval_);
    }

    throw_spdlog_ex("Failed opening file " + os::filename_to_str(filename_) + " for writing", errno);
}

SPDLOG_INLINE void file_helper::reopen(bool truncate)
{
    if (filename_.empty())
    {
        throw_spdlog_ex("Failed re opening file - was not opened before");
    }
    this->open(filename_, truncate);
}

SPDLOG_INLINE void file_helper::flush()
{
#if defined(__windows__)
    if (buf_) {
        FlushViewOfFile(buf_, offset_);
        return;
    }
#endif
    if (std::fflush(fd_) != 0)
    {
        throw_spdlog_ex("Failed flush to file " + os::filename_to_str(filename_), errno);
    }
}

SPDLOG_INLINE void file_helper::close()
{
#if defined(__windows__)
    if (buf_) {
        UnmapViewOfFile(buf_);
        buf_ = nullptr;
    }
    if (file_mapping_handle_) {
        CloseHandle(file_mapping_handle_);
        file_mapping_handle_ = nullptr;
    }
    if (file_handle_) {
        CloseHandle(file_handle_);
        file_handle_ = nullptr;
    }
#endif
    if (fd_ != nullptr)
    {
        if (event_handlers_.before_close)
        {
            event_handlers_.before_close(filename_, fd_);
        }

        std::fclose(fd_);
        fd_ = nullptr;

        if (event_handlers_.after_close)
        {
            event_handlers_.after_close(filename_);
        }
    }
}

SPDLOG_INLINE void file_helper::write(const memory_buf_t &buf)
{
    size_t msg_size = buf.size();
    auto data = buf.data();
#if defined(__windows__)
    if (file_mapping_handle_ && buf_) {
        if (msg_size + offset_ < sys_gran_) {
            memcpy_s(buf_ + offset_, sys_gran_ - offset_, data, msg_size);
            offset_ += msg_size;
            return;
        }
        else {
            memcpy_s(buf_ + offset_, sys_gran_ - offset_, data, sys_gran_ - offset_);
            if (allocate_mmp_bucket_for_file()) {
                memcpy_s(buf_, sys_gran_, data + sys_gran_ - offset_, offset_ + msg_size - sys_gran_);
                offset_ = offset_ + msg_size - sys_gran_;
                return;
            }
            else {
                reopen(true);
            }
        }
    }
#endif
    if (std::fwrite(data, 1, msg_size, fd_) != msg_size)
    {
        throw_spdlog_ex("Failed writing to file " + os::filename_to_str(filename_), errno);
    }
}

SPDLOG_INLINE size_t file_helper::size() const
{
#if defined(__windows__)
    if (file_mapping_handle_ && buf_) {
        return file_size_ - sys_gran_ + offset_;
    }
#endif
    if (fd_ == nullptr)
    {
        throw_spdlog_ex("Cannot use size() on closed file " + os::filename_to_str(filename_));
    }
    return os::filesize(fd_);
}

SPDLOG_INLINE const filename_t &file_helper::filename() const
{
    return filename_;
}

//
// return file path and its extension:
//
// "mylog.txt" => ("mylog", ".txt")
// "mylog" => ("mylog", "")
// "mylog." => ("mylog.", "")
// "/dir1/dir2/mylog.txt" => ("/dir1/dir2/mylog", ".txt")
//
// the starting dot in filenames is ignored (hidden files):
//
// ".mylog" => (".mylog". "")
// "my_folder/.mylog" => ("my_folder/.mylog", "")
// "my_folder/.mylog.txt" => ("my_folder/.mylog", ".txt")
SPDLOG_INLINE std::tuple<filename_t, filename_t> file_helper::split_by_extension(const filename_t &fname)
{
    auto ext_index = fname.rfind('.');

    // no valid extension found - return whole path and empty string as
    // extension
    if (ext_index == filename_t::npos || ext_index == 0 || ext_index == fname.size() - 1)
    {
        return std::make_tuple(fname, filename_t());
    }

    // treat cases like "/etc/rc.d/somelogfile or "/abc/.hiddenfile"
    auto folder_index = fname.find_last_of(details::os::folder_seps_filename);
    if (folder_index != filename_t::npos && folder_index >= ext_index - 1)
    {
        return std::make_tuple(fname, filename_t());
    }

    // finally - return a valid base and extension tuple
    return std::make_tuple(fname.substr(0, ext_index), fname.substr(ext_index));
}

SPDLOG_INLINE bool file_helper::create_file_use_mmp(bool truncate) {
#if defined(__windows__)
    auto flag = truncate ? CREATE_ALWAYS : OPEN_ALWAYS;
#ifdef SPDLOG_WCHAR_FILENAMES
    file_handle_ = CreateFileW(filename_.c_str(), GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, flag, FILE_ATTRIBUTE_NORMAL, NULL);
#else
    file_handle_ = CreateFile(filename_.c_str(), GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, flag, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
    if (INVALID_HANDLE_VALUE == file_handle_) {
        return false;
    }
    LARGE_INTEGER fileSize = { 0 };
    if (GetFileSizeEx(file_handle_, &fileSize)) {
        file_size_ = fileSize.QuadPart;
    }
    if (allocate_mmp_bucket_for_file()) {
        return true;
    }
    CloseHandle(file_handle_);
    file_handle_ = nullptr;
#endif
    return false;
}

SPDLOG_INLINE bool file_helper::allocate_mmp_bucket_for_file() {
#if defined(__windows__)
    if (file_handle_ == nullptr || file_handle_ == INVALID_HANDLE_VALUE) {
        return false;
    }
    if (sys_gran_ == 0) {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        sys_gran_ = sysInfo.dwAllocationGranularity * 2;
    }
    if (buf_ != nullptr) {
        UnmapViewOfFile(buf_);
        buf_ = nullptr;
    }
    if (file_mapping_handle_ && file_mapping_handle_ != INVALID_HANDLE_VALUE) {
        CloseHandle(file_mapping_handle_);
        file_mapping_handle_ = nullptr;
    }
    file_size_ += sys_gran_;

    file_mapping_handle_ = CreateFileMapping(file_handle_, NULL, PAGE_READWRITE, 0, file_size_, NULL);
    if (file_mapping_handle_ == nullptr) {
        SPDLOG_THROW(spdlog_ex("Faild create file mapping " + os::filename_to_str(filename_), GetLastError()));
        return false;
    }
    buf_ = static_cast<char*>(MapViewOfFile(file_mapping_handle_, FILE_MAP_WRITE | FILE_MAP_COPY, 0, file_size_ - sys_gran_, sys_gran_));
    if (buf_ == nullptr) {
        CloseHandle(file_mapping_handle_);
        file_mapping_handle_ = nullptr;
        file_handle_ = nullptr;
        SPDLOG_THROW(spdlog_ex("Faild map view of file " + os::filename_to_str(filename_), GetLastError()));
        return false;
    }
    return true;
#endif
}

} // namespace details
} // namespace spdlog
