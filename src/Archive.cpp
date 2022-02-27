#include <string>
#include <system_error>

#include <allegro5/allegro.h>

#include "Archive.h"

using namespace std;

ALLEGRO_DEBUG_CHANNEL("Archive")

InputArchive::InputArchive(ALLEGRO_PATH *path)
    : m_path(al_path_cstr(path, '/')) {
    const char *fmode = "rb";

    m_file = al_fopen(al_path_cstr(path, '/'), fmode);
    if (m_file == nullptr) {
        error_code ec(al_get_errno(), system_category());
        throw std::system_error(ec, "Error opening save file.");
    }
}

InputArchive::~InputArchive() {
    if (m_file) {
        if (!al_fclose(m_file)) {
            ALLEGRO_ERROR("Error closing save file %s\n", m_path.c_str());
        }
    }
}

bool
InputArchive::verify_type_code(TypeCode type_code) {
    char loaded_type_code = 0;
    size_t num_read = al_fread(m_file, &loaded_type_code, sizeof(type_code));
    if (num_read != 1)
        return false;
    return loaded_type_code == type_code;
}

InputArchive &
InputArchive::operator>>(double &v) {
    if (verify_type_code(TYPECODE_DOUBLE))
        al_fread(m_file, &v, sizeof(v));

    return *this;
}

InputArchive &
InputArchive::operator>>(int &v) {
    if (verify_type_code(TYPECODE_INT))
        al_fread(m_file, &v, sizeof(v));

    return *this;
}

InputArchive &
InputArchive::operator>>(float &v) {
    if (verify_type_code(TYPECODE_FLOAT))
        al_fread(m_file, &v, sizeof(v));

    return *this;
}

InputArchive &
InputArchive::operator>>(bool &v) {
    if (verify_type_code(TYPECODE_BOOL))
        al_fread(m_file, &v, sizeof(v));

    return *this;
}

InputArchive &
InputArchive::operator>>(std::string &v) {
    if (verify_type_code(TYPECODE_STRING)) {
        v = "";
        while (true) {
            char c;
            size_t numRead = al_fread(m_file, &c, sizeof(c));

            if (numRead != 1)
                break;

            if (c == '\0')
                break;

            v.push_back(c);
        }
    }

    return *this;
}

OutputArchive::OutputArchive(ALLEGRO_PATH *path)
    : m_path(al_path_cstr(path, '/')) {
    const char *fmode = "wb";

    m_file = al_fopen(al_path_cstr(path, '/'), fmode);
    if (m_file == nullptr) {
        error_code ec(al_get_errno(), system_category());
        throw std::system_error(ec, "Error opening save file.");
    }
}

OutputArchive::~OutputArchive() {
    if (m_file) {
        if (!al_fclose(m_file)) {
            ALLEGRO_ERROR("Error closing save file %s\n", m_path.c_str());
        }
    }
}

void
OutputArchive::write_type_code(TypeCode type_code) {
    char tc = static_cast<char>(type_code);
    al_fwrite(m_file, &tc, sizeof(tc));
}

OutputArchive &
OutputArchive::operator<<(double v) {
    write_type_code(TYPECODE_DOUBLE);
    al_fwrite(m_file, &v, sizeof(v));

    return *this;
}

OutputArchive &
OutputArchive::operator<<(int v) {
    write_type_code(TYPECODE_INT);
    al_fwrite(m_file, &v, sizeof(v));

    return *this;
}

OutputArchive &
OutputArchive::operator<<(float v) {
    write_type_code(TYPECODE_FLOAT);
    al_fwrite(m_file, &v, sizeof(v));

    return *this;
}

OutputArchive &
OutputArchive::operator<<(bool v) {
    write_type_code(TYPECODE_BOOL);
    al_fwrite(m_file, &v, sizeof(v));

    return *this;
}

OutputArchive &
OutputArchive::operator<<(const std::string &v) {
    return operator<<(v.c_str());
}

OutputArchive &
OutputArchive::operator<<(const char *v) {
    write_type_code(TYPECODE_STRING);
    al_fwrite(m_file, v, strlen(v) + 1);

    return *this;
}

void
OutputArchive::close() {
    if (!al_fclose(m_file)) {
        /* physfs addon bug prevents error detection here */
        /* error_code ec(al_get_errno(), system_category()); */
        /* throw std::system_error(ec, "Error closing save file."); */
    }
    m_file = nullptr;
}

// vi: ft=cpp
