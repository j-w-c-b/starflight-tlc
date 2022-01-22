cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

file(
    GLOB TOPLEVEL_DATA
    LIST_DIRECTORIES false
    "${CMAKE_SOURCE_DIR}/data/*/"
)

# This function generates a header containing the upper-cased base names
# of all the files in ../data/${ARGV0} and mapping of those to the names
# relative to the data directory.
# 
# As side effects
# * the name of the generated header is added to the MODULE_RESOURCE_HEADERS
#   variable.
function(create_resource_header)
    # call with find_module_resources(encounter)
    # will generate EncounterResources.h which depends on data/encounter/*
    string(TOUPPER "${ARGV0}_DEPENDENCIES" resourcedepname)
    set(RESOURCE_HEADER "${ARGV0}_resources.h")

    file(
        GLOB ${resourcedepname}
        LIST_DIRECTORIES false
        "${CMAKE_SOURCE_DIR}/data/${ARGV0}/*"
    )

    add_custom_command(
        OUTPUT ${RESOURCE_HEADER}
        COMMAND ${CMAKE_COMMAND} 
            -DRESOURCE_NAME=${ARGV0} 
            -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR} 
            -P ${CMAKE_CURRENT_SOURCE_DIR}/generate_resources.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/data
        DEPENDS ${${resourcedepname}}
    )
    set(MODULE_RESOURCE_HEADERS ${MODULE_RESOURCE_HEADERS} ${RESOURCE_HEADER} ${${resourcedepname}} PARENT_SCOPE)
    set_source_files_properties(
        ${${resourcedepname}} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/data/${ARGV0}
    )
endfunction()

create_resource_header("auxiliary")
create_resource_header("bank")
create_resource_header("cantina")
create_resource_header("captaincreation")
create_resource_header("captainslounge")
create_resource_header("cargohold")
create_resource_header("controlpanel")
create_resource_header("credits")
create_resource_header("crewhire")
create_resource_header("encounter")
create_resource_header("engineer")
create_resource_header("gui")
create_resource_header("medical")
create_resource_header("messagegui")
create_resource_header("pausemenu")
create_resource_header("planetorbit")
create_resource_header("planetsurface")
create_resource_header("questviewer")
create_resource_header("settings")
create_resource_header("shipconfig")
create_resource_header("spacetravel")
create_resource_header("starmap")
create_resource_header("starport")
create_resource_header("startup")
create_resource_header("titlescreen")
create_resource_header("topgui")
create_resource_header("tradedepot")

set(APP_NAME "${CMAKE_PROJECT_NAME}")
set(ICON_NAME "starflighttlc.ico")
set(ICON_PATH ${CMAKE_SOURCE_DIR}/${ICON_NAME})

if (${CMAKE_SYSTEM_NAME} MATCHES Darwin)
    set(BUNDLE_ARG MACOSX_BUNDLE)
endif()

add_library(lib${APP_NAME} STATIC
    AdvancedTileScroller.cpp
    AdvancedTileScroller.h
    Archive.cpp
    Archive.h
    AudioSystem.cpp
    AudioSystem.h
    Button.cpp
    Button.h
    CombatObject.cpp
    CombatObject.h
    DataMgr.cpp
    DataMgr.h
    Events.h
    Flux.cpp
    Flux.h
    Game.cpp
    Game.h
    GameState.cpp
    GameState.h
    Label.cpp
    Label.h
    Math.cpp
    MathTL.h
    MessageBoxWindow.cpp
    MessageBoxWindow.h
    ModeMgr.cpp
    ModeMgr.h
    Module.cpp
    Module.h
    ModuleAuxiliaryDisplay.cpp
    ModuleAuxiliaryDisplay.h
    ModuleBank.cpp
    ModuleBank.h
    ModuleCantina.cpp
    ModuleCantina.h
    ModuleCaptainCreation.cpp
    ModuleCaptainCreation.h
    ModuleCaptainsLounge.cpp
    ModuleCaptainsLounge.h
    ModuleCargoWindow.cpp
    ModuleCargoWindow.h
    ModuleControlPanel.cpp
    ModuleControlPanel.h
    ModuleCredits.cpp
    ModuleCredits.h
    ModuleCrewHire.cpp
    ModuleCrewHire.h
    ModuleEncounter.cpp
    ModuleEncounter.h
    ModuleEngineer.cpp
    ModuleEngineer.h
    ModuleGameOver.cpp
    ModuleGameOver.h
    ModuleInterplanetaryTravel.cpp
    ModuleInterplanetaryTravel.h
    ModuleInterstellarTravel.cpp
    ModuleInterstellarTravel.h
    ModuleMedical.cpp
    ModuleMedical.h
    ModuleMessageGUI.cpp
    ModuleMessageGUI.h
    ModulePlanetOrbit.cpp
    ModulePlanetOrbit.h
    ModulePlanetSurface.cpp
    ModulePlanetSurface.h
    ModuleQuestLog.cpp
    ModuleQuestLog.h
    ModuleSettings.cpp
    ModuleSettings.h
    ModuleShipConfig.cpp
    ModuleShipConfig.h
    ModuleStarmap.cpp
    ModuleStarmap.h
    ModuleStarport.cpp
    ModuleStarport.h
    ModuleStartup.cpp
    ModuleStartup.h
    ModuleTitleScreen.cpp
    ModuleTitleScreen.h
    ModuleTopGUI.cpp
    ModuleTopGUI.h
    ModuleTradeDepot.cpp
    ModuleTradeDepot.h
    PauseMenu.cpp
    PauseMenu.h
    Perlin.cpp
    PerlinTL.h
    PlanetSurfaceObject.cpp
    PlanetSurfaceObject.h
    PlanetSurfacePlayerVessel.cpp
    PlanetSurfacePlayerVessel.h
    PlayerShipSprite.cpp
    PlayerShipSprite.h
    Point2D.h
    QuestMgr.cpp
    QuestMgr.h
    ResourceManager.cpp
    ResourceManager.h
    Script.cpp
    Script.h
    ScrollBox.cpp
    ScrollBox.h
    Sprite.cpp
    Sprite.h
    Stardate.cpp
    Stardate.h
    TerrainVehicleSprite.cpp
    TerrainVehicleSprite.h
    TexturedSphere.cpp
    TexturedSphere.h
    TileScroller.cpp
    TileScroller.h
    Timer.cpp
    Timer.h
    Util.cpp
    Util.h
    Vector3.cpp
    Vector3.h
    debug.h
    noiseutils.cpp
    noiseutils.h
    tinyxml/tinystr.cpp
    tinyxml/tinystr.h
    tinyxml/tinyxml.cpp
    tinyxml/tinyxml.h
    tinyxml/tinyxmlerror.cpp
    tinyxml/tinyxmlparser.cpp
    rand.c
    lua.hpp
    ${MODULE_RESOURCE_HEADERS}
)

add_executable(
    ${APP_NAME} ${BUNDLE_ARG}
    main.cpp
    ${MODULE_RESOURCE_HEADERS}
    ${TOPLEVEL_DATA}
    ${ICON_PATH}
)

set_property(TARGET ${APP_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET lib${APP_NAME} PROPERTY CXX_STANDARD 11)

target_include_directories(
    ${APP_NAME}
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/libnoise/>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/lua/include/>
)

target_include_directories(
    lib${APP_NAME}
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/libnoise/>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/lua/include/>
)

if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    SET(DEBUGMODE "-DDEBUGMODE")
endif()

target_compile_options(${APP_NAME} PRIVATE ${DEBUGMODE} ${STL_TARGET_CXXFLAGS})
target_compile_options(lib${APP_NAME} PRIVATE ${DEBUGMODE} ${STL_TARGET_CXXFLAGS})
target_link_options(${APP_NAME} PRIVATE ${STL_TARGET_LDFLAGS})
target_link_libraries(
    ${APP_NAME} PUBLIC
    PRIVATE lib${APP_NAME}
    allegro
    allegro_acodec
    allegro_audio
    allegro_dialog
    allegro_font
    allegro_image
    allegro_main
    allegro_primitives
    allegro_ttf
    lua
    m
    noise
)

add_dependencies(
    ${APP_NAME}
    lib${APP_NAME}
    allegro_main
)

add_dependencies(
    lib${APP_NAME}
    lua
    noise
    allegro
    allegro_acodec
    allegro_audio
    allegro_font
    allegro_dialog
    allegro_primitives
)

set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE TRUE)

set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE ${ICON_NAME})
set_source_files_properties(${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
set_source_files_properties(
    ${TOPLEVEL_DATA} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/data/
)

# Use pkgconfig to find the allegro libraries and headers, and add them to
# our build. For macos, we include them in the application bundle
include(FindPkgConfig)

set(allegro_pkgs
    allegro
    allegro_acodec
    allegro_audio
    allegro_dialog
    allegro_font
    allegro_image
    allegro_main
    allegro_primitives
    allegro_ttf
)
foreach (pkg ${allegro_pkgs})
    string(TOUPPER ${pkg} pkg_prefix)

    pkg_check_modules(${pkg_prefix} REQUIRED IMPORTED_TARGET GLOBAL "${pkg}-5>=5.2")
    add_library(${pkg} ALIAS "PkgConfig::${pkg_prefix}")
    LIST(APPEND ALLEGRO_LIBRARY_DIRS ${${pkg_prefix}_LIBRARY_DIRS})
    target_include_directories(lib${APP_NAME} PRIVATE ${${pkg_prefix}_INCLUDE_DIRS})
endforeach()

if (${CMAKE_SYSTEM_NAME} MATCHES Darwin)
    set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES "${APP_NAME}.app")
    install(TARGETS ${APP_NAME} BUNDLE DESTINATION .)
    install(CODE "
            include(BundleUtilities)
            fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/${APP_NAME}.app\" \"\" \"${ALLEGRO_LIBRARY_DIRS}\")
            ")

    set(CPACK_PACKAGE_EXECUTABLES "${CMAKE_INSTALL_PREFIX}/${APP_NAME}.app;Starflight - The Lost Colony")
    set(CPACK_PACKAGE_ICON "${ICON_PATH}")
    include(CPack)
else()
    add_custom_command(
        TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../data ${CMAKE_CURRENT_BINARY_DIR}/data)
endif()
