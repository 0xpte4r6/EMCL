//
// Created by Administrator on 2022/4/24.
//

#include "mclauncher.h"

void unknownArgument(char *arguments[]) {
    printf("Unknown argument(s).\nUse option -u to get manual.");
}

char *getLibrariesPath(char *versionJsonFileFullPath, char *versionJarFilex) {
    FILE *versionJsonFilex = fopen(versionJsonFileFullPath, "r");
    char versionFileContent[_char] = {0};
    static char classPath[_char] = {0};
    char temp[4096] = {0};
    fread(versionFileContent, sizeof(versionFileContent), 1, versionJsonFilex);
    cJSON *cj = cJSON_Parse(versionFileContent);
    cJSON *librariesArray = cJSON_GetObjectItem(cj, "libraries");
    for (int i = 0; i < cJSON_GetArraySize(librariesArray); i++) {
        _Bool add = true;
        cJSON *librariesArrayIndex1 = cJSON_GetArrayItem(librariesArray, i);
        cJSON *downloads = cJSON_GetObjectItem(librariesArrayIndex1, "downloads");
        if (downloads != NULL) { // 有些库是没有 downloads 的，比如 Fabric
            cJSON *artifact = cJSON_GetObjectItem(downloads, "artifact");
            char *path = (artifact != NULL) ? cJSON_GetObjectItem(artifact, "path")->valuestring : "null";
            if (artifact != NULL) {
                //printf("%s\n", path);
            }
            // "rules":[]
            cJSON *rulesArray = cJSON_GetObjectItem(librariesArrayIndex1, "rules");
            if (rulesArray != NULL) {
                cJSON *allowActionObject = cJSON_GetArrayItem(rulesArray, 0);
                cJSON *allowOSObject = cJSON_GetObjectItem(allowActionObject, "os");
                if (allowOSObject != NULL) {
                    char *allowOSName = cJSON_GetObjectItem(allowOSObject, "name")->valuestring;
                    if (!strcmp(allowOSName, "osx") && currentOS != MACOS) add = false;
                }
                cJSON *disallowActionObject = cJSON_GetArrayItem(rulesArray, 1);
                cJSON *disallowOSObject = cJSON_GetObjectItem(disallowActionObject, "os");
                if (disallowActionObject != NULL) {
                    char *disallowOSName = cJSON_GetObjectItem(disallowOSObject, "name")->valuestring;
                    if (!strcmp(disallowOSName, "osx") && currentOS == MACOS) add = false;
                }
            }
            if (cJSON_GetObjectItem(librariesArrayIndex1, "natives") != NULL) add = false;
            if (add) {
                sprintf(temp, "%s/libraries/%s;", configs.gameDir, path);
                strncat(classPath, temp, strlen(temp));
            }
        } else { // 如果没有 downloads 就通过 name 解析出 path
            char *name = cJSON_GetObjectItem(librariesArrayIndex1, "name")->valuestring;
            char path[4096] = {0},temp2[2048] = {0};
            getFabricLibrariesPath(name, path);
            sprintf(temp2, "%s/libraries/%s;", configs.gameDir, path);
            strncat(classPath, temp2, strlen(temp2));
        }
    }
    strncat(classPath, versionJarFilex, strlen(versionJarFilex));
    //printf("\n\n%s", classPath);
    fclose(versionJsonFilex);
    return classPath;
}

char *getMainClass(char *versionJsonFileFullPath) {
    FILE *versionJsonFilex = fopen(versionJsonFileFullPath, "r");
    static char versionFileContent[_char];
    fread(versionFileContent, sizeof(versionFileContent), 1, versionJsonFilex);
    cJSON *cj = cJSON_Parse(versionFileContent);
    char *mainClass = cJSON_GetObjectItem(cj, "mainClass")->valuestring;
    fclose(versionJsonFilex);
    return mainClass;
}

_Bool doRules(cJSON *rulesArray) {
    if (rulesArray != NULL) {
        cJSON *allowActionObject = cJSON_GetArrayItem(rulesArray, 0);
        cJSON *allowOSObject = cJSON_GetObjectItem(allowActionObject, "os");
        if (allowOSObject != NULL) {
            char *allowOSName = cJSON_GetObjectItem(allowOSObject, "name") != NULL
                                ? cJSON_GetObjectItem(allowOSObject, "name")->valuestring : "unknown";
            //char *allowOSArch = cJSON_GetObjectItem(allowOSObject, "arch")->valuestring;
            char *allowOSVersion = (cJSON_GetObjectItem(allowOSObject, "version") != NULL) ? cJSON_GetObjectItem(
                    allowOSObject, "version")->valuestring : NULL;
            if (!strcmp(allowOSName, "osx") && currentOS != MACOS) return false;
            if (!strcmp(allowOSName, "osx") && currentOS == MACOS) return true;
            if (!strcmp(allowOSName, "windows") && currentOS == WINDOWS) return true;
            if (allowOSVersion != NULL && !strcmp(allowOSName, "windows") && !strcmp(allowOSVersion, "^10\\.") &&
                currentOS == WINDOWS)
                return true;
        }
        /*cJSON *disallowActionObject = cJSON_GetArrayItem(rulesArray, 1);
        cJSON *disallowOSObject = cJSON_GetObjectItem(disallowActionObject, "os");
        if (disallowActionObject != NULL) {
            char *disallowOSName = cJSON_GetObjectItem(disallowOSObject, "name")->valuestring;
            char *disallowOSArch = cJSON_GetObjectItem(disallowOSObject, "arch")->valuestring;
            char *disallowOSVersion = cJSON_GetObjectItem(disallowOSObject, "version")->valuestring;
            if (!strcmp(disallowOSName, "osx") && currentOS == MACOS) return false;
        }*/
    }
    return false;
}

char *getJVMArguments(char *versionJsonFilex, char *nativesDir, char *classPath) {
    FILE *versionFile = fopen(versionJsonFilex, "r");
    static char versionFileContent[_char];
    static char temp[_char];
    fread(versionFileContent, sizeof(versionFileContent), 1, versionFile);
    cJSON *fatherObject = cJSON_Parse(versionFileContent);
    cJSON *argumentsObject = cJSON_GetObjectItem(fatherObject, "arguments");
    cJSON *argumentsJVMArray = cJSON_GetObjectItem(argumentsObject, "jvm");
    for (int i = 0; i < cJSON_GetArraySize(argumentsJVMArray); i++) {
        cJSON *argumentsArrayIndex1 = cJSON_GetArrayItem(argumentsJVMArray, i);
        cJSON *rules = cJSON_GetObjectItem(argumentsArrayIndex1, "rules");
        _Bool add = (rules != NULL) ? doRules(rules) : true;
        if (add) {
            cJSON *values = cJSON_GetObjectItem(argumentsArrayIndex1, "value");
            if (values != NULL) {
                for (int j = 0; j < cJSON_GetArraySize(values); j++) {
                    char *argString = cJSON_GetArrayItem(values, j)->valuestring;
                    strncat(temp, "\"", 2);
                    strncat(temp, argString, strlen(argString));
                    strncat(temp, "\" ", 3);
                }
            } else {
                char *argString = argumentsArrayIndex1->valuestring;
                if (!strcmp(argString, "${classpath}")) {
                    strncat(temp, classPath, strlen(classPath));
                    strncat(temp, " ", 2);
                } else if (!strcmp(argString, "-Djava.library.path=${natives_directory}")) {
                    strncat(temp, "-Djava.library.path=", 22);
                    strncat(temp, nativesDir, strlen(nativesDir));
                    strncat(temp, " ", 2);
                } else if (!strcmp(argString, "-Dminecraft.launcher.version=${launcher_version}")) {
                    strncat(temp, "-Dminecraft.launcher.version=", 30);
                    strncat(temp, "1.0", 4);
                    strncat(temp, " ", 2);
                } else if (!strcmp(argString, "-Dminecraft.launcher.brand=${launcher_name}")) {
                    strncat(temp, "-Dminecraft.launcher.brand=", 28);
                    strncat(temp, "[WIP]EMCL1.0", 13);
                    strncat(temp, " ", 2);
                } else {
                    strncat(temp, "\"", 3);
                    strncat(temp, argumentsArrayIndex1->valuestring,
                            strlen(argumentsArrayIndex1->valuestring));
                    strncat(temp, "\" ", 3);
                }
            }
        }
    }

//printf(temp);
    fclose(versionFile);
    return temp;
}

char *getGameArguments(char *versionJsonFilex, char *playerName, char *versionName, char *gameDir, char *assetsDirx,
                       char *assetsIndex, char *uuid, char *accessToken) {
    FILE *versionFile = fopen(versionJsonFilex, "r");
    static char versionFileContent[_char];
    static char temp[_char];
    fread(versionFileContent, sizeof(versionFileContent), 1, versionFile);
    cJSON *fatherObject = cJSON_Parse(versionFileContent);
    cJSON *argumentsObject = cJSON_GetObjectItem(fatherObject, "arguments");
    cJSON *argumentsJVMArray = cJSON_GetObjectItem(argumentsObject, "game");
    for (int i = 0; i < cJSON_GetArraySize(argumentsJVMArray); i++) {
        cJSON *argumentsArrayIndex1 = cJSON_GetArrayItem(argumentsJVMArray, i);
        cJSON *rules = cJSON_GetObjectItem(argumentsArrayIndex1, "rules");
        _Bool add = (rules != NULL) ? doRules(rules) : true;
        if (add) {
            cJSON *values = cJSON_GetObjectItem(argumentsArrayIndex1, "value");
            if (values != NULL) {
                for (int j = 0; j < cJSON_GetArraySize(values); j++) {
                    strncat(temp, cJSON_GetArrayItem(values, j)->valuestring,
                            strlen(cJSON_GetArrayItem(values, j)->valuestring));
                    strncat(temp, " ", 2);
                }
            } else {
                char *argString = argumentsArrayIndex1->valuestring;
                if (!strcmp(argString, "${auth_player_name}")) {
                    strncat(temp, playerName, strlen(playerName));
                    strncat(temp, " ", 2);
                } else if (!strcmp(argString, "${version_name}")) {
                    strncat(temp, versionName, strlen(versionName));
                    strncat(temp, " ", 2);
                } else if (!strcmp(argString, "${game_directory}")) {
                    strncat(temp, gameDir, strlen(gameDir));
                    strncat(temp, " ", 2);
                } else if (!strcmp(argString, "${assets_root}")) {
                    strncat(temp, assetsDirx, strlen(assetsDirx));
                    strncat(temp, " ", 2);
                } else if (!strcmp(argString, "${assets_index_name}")) {
                    strncat(temp, assetsIndex, strlen(assetsIndex));
                    strncat(temp, " ", 2);
                } else if (!strcmp(argString, "${auth_uuid}")) {
                    strncat(temp, uuid, strlen(uuid));
                    strncat(temp, " ", 2);
                } else if (!strcmp(argString, "${auth_access_token}")) {
                    strncat(temp, accessToken, strlen(accessToken));
                    strncat(temp, " ", 2);
                } else if (!strcmp(argString, "${user_type}")) {
                    strncat(temp, "mojang", 7);
                    strncat(temp, " ", 2);
                } else if (!strcmp(argString, "${version_type}")) {
                    strncat(temp, "[WIP]EMCL1.0", 13);
                    strncat(temp, " ", 2);
                } else {
                    strncat(temp, argumentsArrayIndex1->valuestring,
                            strlen(argumentsArrayIndex1->valuestring));
                    strncat(temp, " ", 2);
                }
            }
        }
    }
    //printf(temp);
    fclose(versionFile);
    return temp;
}

char *getAssetsIndex(char *versionJsonFilex) {
    FILE *versionFile = fopen(versionJsonFilex, "r");
    static char versionFileContent[_char];
    fread(versionFileContent, sizeof(versionFileContent), 1, versionFile);
    cJSON *fatherObject = cJSON_Parse(versionFileContent);
    char *assetsIndex = cJSON_GetObjectItem(fatherObject, "assets")->valuestring;
    return assetsIndex;
}

void getFabricLibrariesPath(char *name, OUT char path[]) {
    int onceColon = 0, twiceColon = 0;
    char firstSection[30] = {0}, secondSection[30] = {0}, thirdSection[30] = {0}, slashName[30] = {0};
    for (int i = 0; i < strlen(name); i++) {
        if (name[i] == ':' && onceColon == 0) {
            onceColon = i;
            //printf("\\%d,\\",onceColon);
        } else if (name[i] == ':' && twiceColon == 0) {
            twiceColon = i;
        }
    }
    for (int i = 0; i < onceColon; i++) {
        firstSection[i] = name[i];
    }
    firstSection[onceColon] = 0;
    //printf("\n%s\n",firstSection);
    strcpy(slashName, firstSection); // 这行代码必须在这里！！！否则 firstSection 会数据丢失！！！
    for (int i = onceColon + 1; i < twiceColon; i++) {
        secondSection[i - onceColon - 1] = name[i];
    }
    secondSection[twiceColon] = 0;
    for (int i = twiceColon + 1; i < strlen(name); i++) {
        thirdSection[i - twiceColon - 1] = name[i];
    }
    thirdSection[strlen(name) - strlen(firstSection) - strlen(secondSection) + 1] = 0;
    str_replace_char(slashName, '.', '/');
    //printf("\n%s", name);
    //printf("\n%d  %d", onceColon, twiceColon);
    //printf("\n\\%s\\  \\%s\\  \\%s\\  \\%s\\", firstSection, secondSection, thirdSection, slashName);
    sprintf(path, "%s/%s/%s/%s-%s.jar", slashName, secondSection, thirdSection, secondSection, thirdSection);
}


char *
getLaunchArguments(char *javaPath, char *gameDir, char *assetsDirx, char *assetsIndex, char *versionName,
                   char *versionJsonFilex, int maxMemory, char *nativesDirectory, char *classPath,
                   char *playerName, char *uuid, char *accessToken, int windowWidth, int windowHeight) {
    static char temp[_char];
    sprintf(temp, "%s -Xmn128m -Xmx%dm %s %s %s", javaPath, maxMemory,
            getJVMArguments(versionJsonFilex, nativesDirectory, classPath),
            getMainClass(versionJsonFilex),
            getGameArguments(versionJsonFilex, playerName, versionName, gameDir, assetsDirx, assetsIndex, uuid,
                             accessToken));

    return temp;
}