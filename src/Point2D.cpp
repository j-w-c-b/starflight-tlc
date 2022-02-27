#include "Point2D.h"

InputArchive &
operator>>(InputArchive &ar, Point2D &point) {
    int schema = 0;
    std::string LoadedClassName;
    ar >> LoadedClassName;

    if (LoadedClassName != std::string(Point2D::ClassName)) {
        std::error_code ec(al_get_errno(), std::system_category());
        throw std::system_error(
            ec,
            "Invalid save file: expected " + std::string(Point2D::ClassName)
                + ", got " + LoadedClassName);
    }

    int loaded_schema;
    ar >> loaded_schema;

    if (loaded_schema > schema) {
        std::error_code ec(al_get_errno(), std::system_category());
        throw std::system_error(
            ec,
            "Invalid save file: unknown schema: "
                + std::string(Point2D::ClassName) + " version "
                + std::to_string(loaded_schema));
    }

    ar >> point.x;
    ar >> point.y;

    return ar;
}

OutputArchive &
operator<<(OutputArchive &ar, const Point2D &point) {
    int schema = 0;

    ar << std::string(Point2D::ClassName);
    ar << schema;

    ar << point.x;
    ar << point.y;

    return ar;
}
