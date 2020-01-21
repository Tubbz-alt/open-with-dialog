
project(open-with-dialog)


# Populate a CMake variable with the sources
set (OPENWITH_CPP_FILES
    Sources/main.cpp
    Sources/fileutils.cpp
    Sources/mainwindow.cpp
    Sources/desktopfile.cpp
    Sources/mimesappsmanager.cpp
    Sources/commandlinemanager.cpp
)

set (OPENWITH_HEADER_FILES
    Headers/singleton.h
    Headers/fileutils.h
    Headers/mainwindow.h
    Headers/desktopfile.h
    Headers/mimesappsmanager.h
    Headers/commandlinemanager.h
)


#generate exec
add_executable(${EXE_NAME}
    ${OPENWITH_CPP_FILES}
    ${OPENWITH_HEADER_FILES}
    ${OPENWITH_QRC_FILES}
)

target_include_directories(${EXE_NAME} PUBLIC
    Headers/
    ${DtkWidget_INCLUDE_DIRS}
    ${GIOMM_INCLUDE_DIRS}
    ${GIOUNIX_INCLUDE_DIRS}
)

target_link_libraries (${EXE_NAME}
    Qt5::Widgets Qt5::DBus Qt5::X11Extras
    ${DtkWidget_LIBRARIES}
    ${GIOMM_LIBRARIES}
    ${GIOUNIX_INCLUDE_DIRS}
)

#generate lib
add_library(${LIB_NAME} SHARED
    ${OPENWITH_CPP_FILES}
    ${OPENWITH_HEADER_FILES}
    ${OPENWITH_QRC_FILES}
)

target_include_directories(${LIB_NAME} PUBLIC
    Headers/
    ${DtkWidget_INCLUDE_DIRS}
    ${GIOMM_INCLUDE_DIRS}
    ${GIOUNIX_INCLUDE_DIRS}
)

target_link_libraries (${LIB_NAME}
    Qt5::Widgets Qt5::DBus Qt5::X11Extras
    ${DtkWidget_LIBRARIES}
    ${GIOMM_LIBRARIES}
    ${GIOUNIX_INCLUDE_DIRS}
)

#message(STATUS "====" ${CMAKE_BINARY_DIR})
#message(STATUS "----" ${CMAKE_INSTALL_LIBDIR})


# install exec and lib
install (TARGETS ${EXE_NAME} ${LIB_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )

# install dev files
install (FILES ${OPENWITH_HEADER_FILES}
        DESTINATION include/open-with-dialog
        )

# install pkgconfig file
install (FILES "${CMAKE_BINARY_DIR}/open-with-dialog.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

