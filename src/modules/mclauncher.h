//
// Created by Administrator on 2022/4/24.
//

#ifndef C_CONSOLE_MINECRAFT_LAUNCHER_MCLAUNCHER_H
#define C_CONSOLE_MINECRAFT_LAUNCHER_MCLAUNCHER_H

#include <stdbool.h>
#include "../main.h"
#include "../libraries/cjson/cJSON.h"

void unknownArgument(char *arguments[]);

char *getLibrariesPath(char *versionJsonFileFullPath,char *versionJarFilex);

char *getMainClass(char *versionJsonFile);

_Bool doRules(cJSON *rulesArray);

char *getJVMArguments(char *versionJsonFilex, char *nativesDir, char *classPath);

char *getGameArguments(char *versionJsonFilex, char *playerName, char *versionName, char *gameDir, char *assetsDirx,
                       char *assetsIndex, char *uuid, char *accessToken);

char *getAssetsIndex(char *versionJsonFilex);

void getFabricLibrariesPath(char *name,OUT char *path);

char *
getLaunchArguments(char *javaPath, char *gameDir, char *assetsDirx, char *assetsIndex, char *versionName,
                   char *versionJsonFilex, int maxMemory, char *nativesDirectory, char *classPath,
                   char *playerName, char *uuid, char *accessToken, int windowWidth, int windowHeight);

#endif //C_CONSOLE_MINECRAFT_LAUNCHER_MCLAUNCHER_H
