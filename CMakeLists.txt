cmake_minimum_required(VERSION 3.16)
project(audio-wave VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Путь к иконке Windows (если она осталась в корне, оставьте так, если переехала в src — измените путь)
set(APP_ICON_RESOURCE_WINDOWS ${CMAKE_CURRENT_SOURCE_DIR}/icon.rc)
set(SDL2_PATH "D:/Libraries/SDL2-2.30.10/x86_64-w64-mingw32")

# Поиск Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Обновленный список исходных файлов (все пути теперь начинаются с src/)
set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui

    # Core
    src/Core/Player.cpp
    src/Core/Player.hpp
    src/Core/PlayerQueue.cpp
    src/Core/PlayerQueue.hpp
    src/Core/Handler.cpp
    src/Core/Handler.hpp
    src/Core/TrackInfo.hpp
    src/Core/PlayerState.hpp

    # SoundSys
    src/SoundSys/AudioPlayer.cpp
    src/SoundSys/AudioPlayer.hpp
    src/SoundSys/IAudioDecoder.hpp
    src/SoundSys/WAVFormat.cpp
    src/SoundSys/WAVFormat.hpp
    src/SoundSys/MP3Format.cpp
    src/SoundSys/MP3Format.hpp

    # Libraries
    src/Libraries/ID3Reader.hpp
    src/Libraries/RIFFReader.hpp
    src/Libraries/minimp3.h
    src/Libraries/minimp3_ex.h

    #Common
    src/Common/types.h
)

# Создание исполняемого файла в зависимости от версии Qt
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(audio-wave MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${APP_ICON_RESOURCE_WINDOWS}
        resources.qrc
    )
else()
    add_executable(audio-wave
        ${PROJECT_SOURCES}
        ${APP_ICON_RESOURCE_WINDOWS}
        resources.qrc
    )
endif()

add_definitions(-DSDL_MAIN_HANDLED)

# Настройка путей к заголовочным файлам
target_include_directories(audio-wave PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src                  # Чтобы можно было писать #include "Core/Player.hpp"
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SoundSys
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Libraries
    "${SDL2_PATH}/include"
)

# Линковка библиотек
target_link_libraries(audio-wave PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    "${SDL2_PATH}/lib/libSDL2.dll.a"
    "${SDL2_PATH}/lib/libSDL2main.a"
)

# Свойства таргета
set_target_properties(audio-wave PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(audio-wave)
endif()
