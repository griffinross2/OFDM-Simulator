#include "log_file.h"
#include <stdexcept>
#include <cstring>
#include <cstdarg>

FileManager::FileManager()
{
    // Nothin
}

FileManager::~FileManager()
{
    for (auto &log_file : log_files)
    {
        if (log_file.fp)
        {
            fclose(log_file.fp);
        }
    }
    printf("Closing Files!\n");
}

void FileManager::write_log(const char *filename, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    for (auto &log_file : log_files)
    {
        if (strcmp(log_file.filename, filename) == 0)
        {
            if (log_file.fp)
            {
                vfprintf(log_file.fp, format, args);
                return;
            }
        }
    }

    // If the file is not found, create a new log file
    LogFile new_log_file;
    new_log_file.filename = filename;
    new_log_file.fp = fopen(filename, "w");

    if (new_log_file.fp)
    {
        vfprintf(new_log_file.fp, format, args);
        log_files.push_back(new_log_file);
    }
    else
    {
        throw std::runtime_error("Failed to open log file");
    }
}
