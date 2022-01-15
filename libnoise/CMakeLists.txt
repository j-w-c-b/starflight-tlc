cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(libnoise VERSION 0.9.0)

add_library(
    noise
    src/noise.h
    src/vectortable.h
    src/misc.h
    src/module/blend.h
    src/module/scalebias.cpp
    src/module/perlin.h
    src/module/translatepoint.cpp
    src/module/invert.h
    src/module/power.h
    src/module/min.h
    src/module/select.cpp
    src/module/translatepoint.h
    src/module/ridgedmulti.h
    src/module/invert.cpp
    src/module/curve.cpp
    src/module/clamp.h
    src/module/modulebase.cpp
    src/module/blend.cpp
    src/module/multiply.h
    src/module/turbulence.h
    src/module/displace.cpp
    src/module/modulebase.h
    src/module/cache.h
    src/module/add.h
    src/module/terrace.cpp
    src/module/max.cpp
    src/module/scalepoint.cpp
    src/module/voronoi.cpp
    src/module/abs.h
    src/module/checkerboard.cpp
    src/module/turbulence.cpp
    src/module/curve.h
    src/module/cylinders.cpp
    src/module/scalepoint.h
    src/module/multiply.cpp
    src/module/abs.cpp
    src/module/power.cpp
    src/module/cache.cpp
    src/module/billow.cpp
    src/module/spheres.cpp
    src/module/checkerboard.h
    src/module/rotatepoint.cpp
    src/module/cylinders.h
    src/module/spheres.h
    src/module/exponent.h
    src/module/clamp.cpp
    src/module/exponent.cpp
    src/module/voronoi.h
    src/module/max.h
    src/module/module.h
    src/module/perlin.cpp
    src/module/displace.h
    src/module/scalebias.h
    src/module/add.cpp
    src/module/min.cpp
    src/module/ridgedmulti.cpp
    src/module/billow.h
    src/module/const.h
    src/module/terrace.h
    src/module/rotatepoint.h
    src/module/const.cpp
    src/module/select.h
    src/noisegen.h
    src/latlon.cpp
    src/model
    src/model/line.cpp
    src/model/cylinder.cpp
    src/model/sphere.h
    src/model/cylinder.h
    src/model/plane.h
    src/model/model.h
    src/model/line.h
    src/model/sphere.cpp
    src/model/plane.cpp
    src/Sources
    src/basictypes.h
    src/mathconsts.h
    src/latlon.h
    src/exception.h
    src/interp.h
    src/noisegen.cpp
)

file(
    GLOB_RECURSE NOISE_HEADERS
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)
foreach (HDR ${NOISE_HEADERS})
    message("Copying ${HDR} to noise/${HDR}")
    configure_file(src/${HDR} noise/${HDR} COPYONLY)
endforeach()
