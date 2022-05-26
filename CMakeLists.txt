cmake_minimum_required(VERSION 3.22)
project(c_console_minecraft_launcher C)

set(CMAKE_C_STANDARD 99)

add_executable(c_console_minecraft_launcher
        src/main.c
        src/modules/mclauncher.c
        src/libraries/cjson/cJSON.c
        src/modules/mclauncher.h
        src/libraries/cjson/cJSON.h src/utils.c src/utils.h src/options/startGameOption.c src/main.h src/options/selectOption.c src/options/listOption.c src/options/printOption.c src/options/options.h src/options/helpOption.c src/options/installOption.c src/modules/downloadApiProviders.c src/modules/downloadApiProviders.h)