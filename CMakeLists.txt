cmake_minimum_required(VERSION 2.8)

project(open-with-dialog)

add_definitions(-std=c++11)

include(GNUInstallDirs)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "-g -Wall")
set(QT_MINIMUM_VERSION "5.7.1")

# Find the QtWidgets library
find_package(Qt5 ${QT_MINIMUM_VERSION} CONFIG REQUIRED Widgets LinguistTools)
find_package(Dtk REQUIRED Core Widget Tools)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GIOMM giomm-2.4) # look into FindPkgConfig.cmake
pkg_check_modules(GIOUNIX gio-unix-2.0)


# Populate a CMake variable with the sources
set (OPENWITH_CPP_FILES
    main.cpp
    fileutils.cpp
    mainwindow.cpp
    desktopfile.cpp
    mimesappsmanager.cpp
)

set (OPENWITH_HEADER_FILES
    singleton.h
    fileutils.h
    mainwindow.h
    desktopfile.h
    mimesappsmanager.h
)

set (OPENWITH_QRC_FILES
)

# Program
set (EXE_NAME open-with-dialog)

# Translation
file (GLOB OPENWITH_TS_FILES translations/*.ts)
set (OPENWITH_CPP_FILES_FOR_I18N ${OPENWITH_CPP_FILES})

dtk_create_i18n_from_json(OPENWITH_SETTINGS_I18N_FILES default-config.json settings_translation.cpp)
list (APPEND OPENWITH_CPP_FILES_FOR_I18N ${OPENWITH_SETTINGS_I18N_FILES})
qt5_create_translation(OPENWITH_QM_FILES ${OPENWITH_CPP_FILES_FOR_I18N} ${OPENWITH_TS_FILES})

add_executable(${EXE_NAME}
    ${OPENWITH_CPP_FILES}
    ${OPENWITH_HEADER_FILES}
    ${OPENWITH_QRC_FILES}
)

target_include_directories(${EXE_NAME} PUBLIC
    ${DtkWidget_INCLUDE_DIRS}
    ${GIOMM_INCLUDE_DIRS}
    ${GIOUNIX_INCLUDE_DIRS}
)

target_link_libraries (${EXE_NAME}
    Qt5::Widgets
    ${DtkWidget_LIBRARIES}
    ${GIOMM_LIBRARIES}
    ${GIOUNIX_INCLUDE_DIRS}
)






