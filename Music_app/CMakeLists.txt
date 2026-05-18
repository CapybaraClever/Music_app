cmake_minimum_required(VERSION 3.16)

project(MusicArchive VERSION 1.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/msvc2022_64")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Для чистого кода достаточно автоматического MOC
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Исходники кода
set(SOURCES
    main.cpp
    Music_app.cpp
    addsongdialog.cpp
)

# Заголовки
set(HEADERS
    Music_app.h
    addsongdialog.h
    song.h
)

# Собираем exe (без всяких .ui и .qrc)
add_executable(MusicArchive WIN32 ${SOURCES} ${HEADERS})

target_link_libraries(MusicArchive PRIVATE Qt6::Widgets)

# Автоматическое копирование DLL библиотек Qt в папку с .exe файлом для Windows
if(WIN32)
    add_custom_command(TARGET MusicArchive POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:Qt6::Widgets>"
        "$<TARGET_FILE_DIR:MusicArchive>"
    )
    # Копируем также базовую библиотеку Qt6Core и Qt6Gui
    add_custom_command(TARGET MusicArchive POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:Qt6::Core>"
        "$<TARGET_FILE_DIR:MusicArchive>"
    )
    add_custom_command(TARGET MusicArchive POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:Qt6::Gui>"
        "$<TARGET_FILE_DIR:MusicArchive>"
    )
endif()

# Автоматический запуск windeployqt для сборки всех плагинов (включая platforms/qwindows.dll)
if(WIN32)
    # Находим путь к утилите windeployqt.exe внутри твоей папки Qt
    get_target_property(_qt_bindir Qt6::Core LOCATION)
    get_filename_component(_qt_bindir "${_qt_bindir}" DIRECTORY)
    
    add_custom_command(TARGET MusicArchive POST_BUILD
        COMMAND "${_qt_bindir}/windeployqt.exe" --debug "$<TARGET_FILE:MusicArchive>"
        COMMENT "Running windeployqt..."
    )
endif()