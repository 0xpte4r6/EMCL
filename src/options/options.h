//
// Created by Administrator on 2022/5/12.
//

#ifndef C_CONSOLE_MINECRAFT_LAUNCHER_OPTIONS_H
#define C_CONSOLE_MINECRAFT_LAUNCHER_OPTIONS_H

#include <dirent.h>
#include <sys/stat.h>
#include "../main.h"

void startGameOption(char *arguments[]);

void listOption(char *arguments[]);

void helpOption(char *arguments[]);

void selectOption(char *arguments[]);

void printOption(char *arguments[]);

void installOption(char *arguments[]);

void AboutOption(char *arguments[]);

static void getInstallableVersions(char *arguments[]);

void installGame(char *arguments[]);

void downloadClient(char *versionJsonFullPath, char *arguments[]);

void downloadAssets(char *versionJsonFullPath, char *arguments[]);

void downloadLibraries(char *versionJsonFullPath, char *arguments[]);

#endif //C_CONSOLE_MINECRAFT_LAUNCHER_OPTIONS_H
