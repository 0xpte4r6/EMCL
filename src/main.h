//
// Created by Administrator on 2022/5/11.
//

#ifndef C_CONSOLE_MINECRAFT_LAUNCHER_MAIN_H
#define C_CONSOLE_MINECRAFT_LAUNCHER_MAIN_H

#define WINDOWS 0
#define LINUX 1
#define MACOS 2
#define CONFIGFILE "ccmcl.json"
#define CONFIGFILE_SIZE 4096
#define _char 393216
#define OUT

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include "modules/mclauncher.h"
#include "modules/downloadApiProviders.h"
#include "libraries/cjson/cJSON.h"
#include "options/options.h"
#include "utils.h"
#include <direct.h>
#include <windows.h>

extern const int currentOS;
extern char *jsonObjectFatherString,*assetsDir,*versionsDir;
extern const DownloadAPIProvider *downloadProviderP;

extern _Bool onlyPrint;

typedef struct {
    char *gameDir,*javaPath,*playerName,*uuid,*accessToken,*selectedVersion,*assetsDir;
    int maxMemory,windowWidth,windowHeight,selectedAccount;
} CONFIGS;

extern CONFIGS configs;

void initConfig();

#endif //C_CONSOLE_MINECRAFT_LAUNCHER_MAIN_H
