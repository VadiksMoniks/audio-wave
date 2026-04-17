cmake_minimum_required(VERSION 3.16)

project(audio-wave VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(APP_ICON_RESOURCE_WINDOWS
    ${CMAKE_CURRENT_SOURCE_DIR}/icon.rc
)

set(SDL2_PATH "D:/Libraries/SDL2-2.30.10/x86_64-w64-mingw32")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    src/AudioPlayer.cpp
    src/PlayerQueue.cpp
    src/Handler.cpp
    src/WAVFormat.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/MP3Format.cpp

    include/audio-wave/IAudioDecoder.hpp
    include/audio-wave/AudioPlayer.hpp
    include/audio-wave/PlayerQueue.hpp
    include/audio-wave/Handler.hpp
    include/audio-wave/WAVFormat.hpp
    include/audio-wave/MP3Format.hpp
    include/audio-wave/ID3Reader.hpp
    include/audio-wave/minimp3.h
    include/audio-wave/minimp3_ex.h
    include/audio-wave/mainwindow.h
    include/audio-wave/TrackInfo.hpp

    mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(audio-wave
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${APP_ICON_RESOURCE_WINDOWS}
        include/audio-wave/Player.hpp
        src/Player.cpp
        resources.qrc

    )
else()
    add_executable(audio-wave ${PROJECT_SOURCES} ${APP_ICON_RESOURCE_WINDOWS}
        resources.qrc
    )
endif()

add_definitions(-DSDL_MAIN_HANDLED)

target_include_directories(audio-wave PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/audio-wave
    "${SDL2_PATH}/include"
)

target_link_libraries(audio-wave PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    "${SDL2_PATH}/lib/libSDL2.dll.a"
    "${SDL2_PATH}/lib/libSDL2main.a"
)

set_target_properties(audio-wave PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(audio-wave)
endif()
