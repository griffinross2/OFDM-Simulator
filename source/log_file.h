#pragma once

#include <vector>
#include <stdio.h>

typedef struct
{
    const char *filename;
    FILE *fp;
} LogFile;

class FileManager
{
public:
    FileManager();
    ~FileManager();
    void write_log(const char *filename, const char *format, ...);

private:
    std::vector<LogFile> log_files;
};