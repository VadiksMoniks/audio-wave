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
    src/SoundSys/AudioPlayer.cpp
    src/Core/PlayerQueue.cpp
    src/Core/Handler.cpp
    src/SoundSys/WAVFormat.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/SoundSys/MP3Format.cpp

    include/audio-wave/SoundSys/IAudioDecoder.hpp
    include/audio-wave/SoundSys/AudioPlayer.hpp
    include/audio-wave/Core/PlayerQueue.hpp
    include/audio-wave/Core/Handler.hpp
    include/audio-wave/SoundSys/WAVFormat.hpp
    include/audio-wave/SoundSys/MP3Format.hpp
    include/audio-wave/Libraries/ID3Reader.hpp
    include/audio-wave/Libraries/RIFFReader.hpp
    include/audio-wave/Libraries/minimp3.h
    include/audio-wave/Libraries/minimp3_ex.h
    include/audio-wave/mainwindow.h
    include/audio-wave/Core/TrackInfo.hpp
    include/audio-wave/Core/PlayerState.hpp

    mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(audio-wave
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${APP_ICON_RESOURCE_WINDOWS}
        include/audio-wave/Core/Player.hpp
        src/Core/Player.cpp
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
