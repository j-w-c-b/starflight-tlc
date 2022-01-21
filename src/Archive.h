#ifndef ARCHIVE_H
#define ARCHIVE_H
#pragma once

#include <allegro5/allegro.h>
#include <stdio.h>
#include <string>

class Archive {
  public:
    Archive();
    virtual ~Archive();

    typedef enum
    {
        AM_NONE = 0,
        AM_STORE,
        AM_LOAD
    } ArchiveMode;

    bool Open(ALLEGRO_PATH *fileName, ArchiveMode mode);
    void Close();

    bool IsOpen();

    Archive &operator<<(double v);
    Archive &operator<<(int v);
    Archive &operator<<(bool v);
    Archive &operator<<(float v);
    Archive &operator<<(std::string &v);
    Archive &operator<<(const char *v);

    Archive &operator>>(double &v);
    Archive &operator>>(int &v);
    Archive &operator>>(bool &v);
    Archive &operator>>(float &v);
    Archive &operator>>(std::string &v);

    bool IsStoring();

  private:
    ALLEGRO_FILE *file;
    ArchiveMode arMode;

    bool VerifyTypeCode(char typeCode);
    void WriteTypeCode(char typeCode);
};

#endif
