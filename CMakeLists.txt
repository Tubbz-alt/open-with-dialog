cmake_minimum_required(VERSION 2.8)

project(open-with-dialog)

add_definitions(-std=c++11)

include(GNUInstallDirs)

# Install settings
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr CACHE PATH "default install prefix" FORCE)
endif ()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "-g -Wall")

find_package(Qt5 ${QT_MINIMUM_VERSION} CONFIG REQUIRED Widgets DBus X11Extras LinguistTools)
find_package(Dtk REQUIRED Core Widget Tools)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GIOMM giomm-2.4) # look into FindPkgConfig.cmake
pkg_check_modules(GIOUNIX gio-unix-2.0)

# Translation
file (GLOB OPENWITH_TS_FILES translations/*.ts)
set (OPENWITH_CPP_FILES_FOR_I18N ${OPENWITH_CPP_FILES})
qt5_create_translation(OPENWITH_QM_FILES ${OPENWITH_CPP_FILES_FOR_I18N} ${OPENWITH_TS_FILES})

# set variable name for Exec and Lib
set (EXE_NAME open-with-dialog)
set (LIB_NAME openwithdialog)

configure_file ("open-with-dialog.pc.in" "open-with-dialog.pc" @ONLY)

add_subdirectory (src)





