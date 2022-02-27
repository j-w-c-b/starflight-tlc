cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

include(implib_utils.cmake)

file(GLOB TOPLEVEL_DATA
    LIST_DIRECTORIES false
    "${CMAKE_SOURCE_DIR}/data/*.ogg"
    "${CMAKE_SOURCE_DIR}/data/*.lua"
    "${CMAKE_SOURCE_DIR}/data/*.yaml"
)

foreach(dat ${TOPLEVEL_DATA})
    file(RELATIVE_PATH reldat "${CMAKE_SOURCE_DIR}/data" "${dat}")
    set_source_files_properties(
        "${dat}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/data/"
    )
endforeach()

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
        "${CMAKE_SOURCE_DIR}/data/${ARGV0}/*.ogg"
        "${CMAKE_SOURCE_DIR}/data/${ARGV0}/*.png"
        "${CMAKE_SOURCE_DIR}/data/${ARGV0}/*.lua"
        "${CMAKE_SOURCE_DIR}/data/${ARGV0}/*.ttf"
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
    set(MODULE_RESOURCE_HEADERS ${MODULE_RESOURCE_HEADERS} ${RESOURCE_HEADER} PARENT_SCOPE)
    file(
        GLOB ${ARGV0}_DATA
        LIST_DIRECTORIES false
        RELATIVE "${CMAKE_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/data/${ARGV0}/*.ogg"
        "${CMAKE_SOURCE_DIR}/data/${ARGV0}/*.png"
        "${CMAKE_SOURCE_DIR}/data/${ARGV0}/*.lua"
        "${CMAKE_SOURCE_DIR}/data/${ARGV0}/*.ttf"
    )
    add_custom_command(
        OUTPUT "${ARGV0}.dat"
        COMMAND ${CMAKE_COMMAND} 
        -E tar c "${CMAKE_CURRENT_BINARY_DIR}/${ARGV0}.dat" --format=zip -- ${${ARGV0}_DATA}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
set(MODULE_RESOURCE_DATA ${MODULE_RESOURCE_DATA} "${CMAKE_CURRENT_BINARY_DIR}/${ARGV0}.dat" PARENT_SCOPE)
    set_source_files_properties(
        ${ARGV0}.dat PROPERTIES MACOSX_PACKAGE_LOCATION Resources/data/${ARGV0}.dat
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
    Bitmap.cpp
    Bitmap.h
    Button.cpp
    Button.h
    CheckBox.cpp
    CheckBox.h
    CombatObject.cpp
    CombatObject.h
    DataMgr.cpp
    DataMgr.h
    DialogButtonPanel.cpp
    DialogButtonPanel.h
    Events.h
    Flux.cpp
    Flux.h
    Game.cpp
    Game.h
    GameState.cpp
    GameState.h
    ItemStackButtonList.cpp
    ItemStackButtonList.h
    Label.cpp
    Label.h
    Loan.cpp
    Loan.h
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
    ModuleCaptainDetails.cpp
    ModuleCaptainDetails.h
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
    ModuleProfessionChoice.cpp
    ModuleProfessionChoice.h
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
    Officer.cpp
    Officer.h
    OfficerInfo.cpp
    OfficerInfo.h
    PauseMenu.cpp
    PauseMenu.h
    Perlin.cpp
    PerlinTL.h
    PersonnelSlotButton.cpp
    PersonnelSlotButton.h
    PlanetSurfaceObject.cpp
    PlanetSurfaceObject.h
    PlanetSurfacePlayerVessel.cpp
    PlanetSurfacePlayerVessel.h
    PlayerShipSprite.cpp
    PlayerShipSprite.h
    Point2D.cpp
    Point2D.h
    QuestMgr.cpp
    QuestMgr.h
    ResourceManager.cpp
    ResourceManager.h
    RichTextLabel.cpp
    RichTextLabel.h
    Script.cpp
    Script.h
    ScrolledModule.cpp
    ScrolledModule.h
    SlidingModule.h
    Sprite.cpp
    Sprite.h
    Stardate.cpp
    Stardate.h
    TerrainVehicleSprite.cpp
    TerrainVehicleSprite.h
    TextEntry.cpp
    TextEntry.h
    TexturedSphere.cpp
    TexturedSphere.h
    TileScroller.cpp
    TileScroller.h
    Timer.cpp
    Timer.h
    UnemployedSlotButton.cpp
    UnemployedSlotButton.h
    UnemployedPanel.cpp
    UnemployedPanel.h
    Util.cpp
    Util.h
    debug.h
    noiseutils.cpp
    noiseutils.h
    rand.c
    lua.hpp
    ${MODULE_RESOURCE_HEADERS}
)

add_executable(
    ${APP_NAME} ${BUNDLE_ARG}
    main.cpp
    ${MODULE_RESOURCE_HEADERS}
    ${MODULE_RESOURCE_DATA}
    ${TOPLEVEL_DATA}
    ${ICON_PATH}
)

set_property(TARGET ${APP_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET lib${APP_NAME} PROPERTY CXX_STANDARD 17)

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
    allegro_physfs
    allegro_primitives
    allegro_ttf
    physfs
    lua
    m
    noise
    yaml_cpp
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
    allegro_physfs
    allegro_primitives
    physfs
    yaml_cpp
)

set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in)

set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE ${ICON_NAME})
set_source_files_properties(${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
set_source_files_properties(
    ${MODULE_RESOURCE_DATA} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/data
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
    allegro_physfs
    allegro_primitives
    allegro_ttf
    physfs
    yaml_cpp
)

foreach (pkg ${allegro_pkgs})
    string(TOUPPER ${pkg} pkg_prefix)

    if (${pkg} MATCHES "allegro.*")
        set(pkgspec "${pkg}-5>=5.2")
    elseif(${pkg} MATCHES "yaml_cpp")
        set(pkg_prefix "YAML_CPP")
        set(pkgspec "yaml-cpp")
    else()
        set(pkgspec "${pkg}")
    endif()

    pkg_check_modules(${pkg_prefix} REQUIRED IMPORTED_TARGET GLOBAL "${pkgspec}")
    add_library(${pkg} SHARED IMPORTED GLOBAL)
    
    list(GET ${pkg_prefix}_LINK_LIBRARIES 0 firstlib)

    set_property(TARGET ${pkg} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${${pkg_prefix}_INCLUDE_DIRS})

    if (WIN32)
        set(${pkg}_IMPLIB ${firstlib})
        set_property(TARGET ${pkg} PROPERTY IMPORTED_IMPLIB ${firstlib})
        implib_to_dll(${pkg}_IMPLIB)
        set_property(TARGET ${pkg} PROPERTY IMPORTED_LOCATION ${${pkg}_LIBRARY})
    else()
        set_property(TARGET ${pkg} PROPERTY IMPORTED_LOCATION ${firstlib})
    endif()
    LIST(APPEND ALLEGRO_LIBRARY_DIRS ${${pkg_prefix}_LIBRARY_DIRS})
    target_include_directories(lib${APP_NAME} PRIVATE ${${pkg_prefix}_INCLUDE_DIRS})
endforeach()

if (${CMAKE_SYSTEM_NAME} MATCHES Darwin)
    set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES "${APP_NAME}.app")
    install(TARGETS ${APP_NAME} BUNDLE DESTINATION .)
    install(CODE "
            include(BundleUtilities)
            set(BU_CHMOD_BUNDLE_ITEMS 1)
            fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/${APP_NAME}.app\" \"\" \"${ALLEGRO_LIBRARY_DIRS}\")
            ")

    set(CPACK_PACKAGE_EXECUTABLES "${CMAKE_INSTALL_PREFIX}/${APP_NAME}.app;Starflight - The Lost Colony")
    set(CPACK_PACKAGE_ICON "${ICON_PATH}")
elseif (${CMAKE_SYSTEM_NAME} MATCHES Windows)
    install(TARGETS ${APP_NAME} DESTINATION .)
    install(FILES ${MODULE_RESOURCE_DATA} DESTINATION data)
    install(FILES ${TOPLEVEL_DATA} DESTINATION data/)
    add_custom_command(
        TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${APP_NAME}> $<TARGET_FILE_DIR:${APP_NAME}>
        COMMAND_EXPAND_LISTS)
    install(FILES $<TARGET_RUNTIME_DLLS:${APP_NAME}> DESTINATION .)
    set (CPACK_PACKAGE_INSTALL_DIRECTORY ${CMAKE_PROJECT})
elseif (${CMAKE_SYSTEM_NAME} MATCHES Linux)
    install(TARGETS ${APP_NAME})
    install(FILES ${TOPLEVEL_DATA} DESTINATION data)
    install(FILES ${MODULE_RESOURCE_DATA} DESTINATION data)
    add_custom_command(
        TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/data/
        )
    add_custom_command(
        TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${TOPLEVEL_DATA} ${MODULE_RESOURCE_DATA} ${CMAKE_CURRENT_BINARY_DIR}/data/
        )
else()
    message(FATAL_ERROR "Unsupported OS")
endif()
include(CPack)
