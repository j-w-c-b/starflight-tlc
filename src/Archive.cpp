#include <string.h>

#include "Archive.h"

#define TYPECODE_DOUBLE 'D'
#define TYPECODE_INT 'I'
#define TYPECODE_BOOL 'B'
#define TYPECODE_STRING 'S'
#define TYPECODE_FLOAT 'F'

using namespace std;

Archive::Archive() : file(NULL), arMode(AM_NONE) {}

Archive::~Archive() { Close(); }

bool
Archive::Open(ALLEGRO_PATH *fileName, ArchiveMode mode) {
    Close();

    std::string fmode;
    if (mode == AM_STORE) {
        fmode = "wb";
    } else if (mode == AM_LOAD) {
        fmode = "rb";
    }

    file = al_fopen(
        al_path_cstr(fileName, ALLEGRO_NATIVE_PATH_SEP), fmode.c_str());
    if (file == NULL)
        return false;

    arMode = mode;
    return true;
}

void
Archive::Close() {
    if (file != NULL) {
        al_fclose(file);
        file = NULL;
    }
    arMode = AM_NONE;
}

bool
Archive::IsOpen() {
    return (file != NULL);
}

bool
Archive::IsStoring() {
    return arMode == AM_STORE;
}

bool
Archive::VerifyTypeCode(char typeCode) {
    if (IsOpen()) {
        char loadedTypeCode = '\0';
        size_t numRead = al_fread(file, &loadedTypeCode, sizeof(typeCode));
        if (numRead != 1)
            return false;
        return loadedTypeCode == typeCode;
    } else
        return false;
}

void
Archive::WriteTypeCode(char typeCode) {
    if (IsOpen())
        al_fwrite(file, &typeCode, sizeof(typeCode));
}

Archive &
Archive::operator<<(double v) {
    if (IsOpen()) {
        WriteTypeCode(TYPECODE_DOUBLE);
        al_fwrite(file, &v, sizeof(v));
    }

    return *this;
}

Archive &
Archive::operator<<(int v) {
    if (IsOpen()) {
        WriteTypeCode(TYPECODE_INT);
        al_fwrite(file, &v, sizeof(v));
    }

    return *this;
}

Archive &
Archive::operator<<(float v) {
    if (IsOpen()) {
        WriteTypeCode(TYPECODE_FLOAT);
        al_fwrite(file, &v, sizeof(v));
    }

    return *this;
}

Archive &
Archive::operator<<(bool v) {
    if (IsOpen()) {
        WriteTypeCode(TYPECODE_BOOL);
        al_fwrite(file, &v, sizeof(v));
    }

    return *this;
}

Archive &
Archive::operator<<(std::string &v) {
    return operator<<(v.c_str());
}

Archive &
Archive::operator<<(const char *v) {
    if (IsOpen()) {
        WriteTypeCode(TYPECODE_STRING);
        al_fwrite(file, v, strlen(v) + 1);
    }

    return *this;
}

Archive &
Archive::operator>>(double &v) {
    if (VerifyTypeCode(TYPECODE_DOUBLE))
        al_fread(file, &v, sizeof(v));

    return *this;
}

Archive &
Archive::operator>>(int &v) {
    if (VerifyTypeCode(TYPECODE_INT))
        al_fread(file, &v, sizeof(v));

    return *this;
}

Archive &
Archive::operator>>(float &v) {
    if (VerifyTypeCode(TYPECODE_FLOAT))
        al_fread(file, &v, sizeof(v));

    return *this;
}

Archive &
Archive::operator>>(bool &v) {
    if (VerifyTypeCode(TYPECODE_BOOL))
        al_fread(file, &v, sizeof(v));

    return *this;
}

Archive &
Archive::operator>>(std::string &v) {
    if (VerifyTypeCode(TYPECODE_STRING)) {
        v = "";
        while (true) {
            char c;
            size_t numRead = al_fread(file, &c, sizeof(c));

            if (numRead != 1)
                break;

            if (c == '\0')
                break;

            v.push_back(c);
        }
    }

    return *this;
}
