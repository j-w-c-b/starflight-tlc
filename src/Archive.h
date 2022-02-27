#ifndef ARCHIVE_H
#define ARCHIVE_H
#pragma once

#include <allegro5/allegro.h>
#include <string>

enum TypeCode : char
{
    TYPECODE_DOUBLE = 'D',
    TYPECODE_INT = 'I',
    TYPECODE_BOOL = 'B',
    TYPECODE_STRING = 'S',
    TYPECODE_FLOAT = 'F',
};

class InputArchive {
  public:
    explicit InputArchive(ALLEGRO_PATH *path);
    virtual ~InputArchive();

    InputArchive &operator>>(double &v);
    InputArchive &operator>>(int &v);
    InputArchive &operator>>(bool &v);
    InputArchive &operator>>(float &v);
    InputArchive &operator>>(std::string &v);

  private:
    ALLEGRO_FILE *m_file;
    std::string m_path;

    bool verify_type_code(TypeCode type_code);
};

class OutputArchive {
  public:
    explicit OutputArchive(ALLEGRO_PATH *path);
    virtual ~OutputArchive();

    void close();

    OutputArchive &operator<<(double v);
    OutputArchive &operator<<(int v);
    OutputArchive &operator<<(bool v);
    OutputArchive &operator<<(float v);
    OutputArchive &operator<<(const std::string &v);
    OutputArchive &operator<<(const char *v);

  private:
    ALLEGRO_FILE *m_file;
    std::string m_path;

    void write_type_code(TypeCode type_code);
};

#endif
// vi: ft=cpp
