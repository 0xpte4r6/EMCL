//
// Created by Administrator on 2022/5/15.
//
#include "options.h"

char *pendingDownloadDirName;

void installOption(char *arguments[]) {
    if (arguments[2] != NULL) {
        if (!strcmp(arguments[2], "-list")) {
            getInstallableVersions(arguments);
        } else {
            char path[4096];
            pendingDownloadDirName = (arguments[3] != NULL && arguments[4] != NULL
                                      && !strcmp(arguments[3], "-n")) ? arguments[4] : arguments[2];
            sprintf(path, "%s/%s", versionsDir, pendingDownloadDirName);
            if (_access(path, F_OK) == -1) {
                mkdir(versionsDir);
                mkdir(path);
                installGame(arguments);
                printf("\nGame installation complete.");
            } else {
                char jsonPath[4096];
                sprintf(jsonPath, "%s/%s/%s.json", versionsDir, pendingDownloadDirName, pendingDownloadDirName);
                if (!strcmp(arguments[3], "--fixAssets")) {
                    downloadAssets(jsonPath, arguments);
                } else if (!strcmp(arguments[3], "--fixLibraries")) {
                    downloadLibraries(jsonPath, arguments);
                } else {
                    printf("The name \"%s\" already exist.", arguments[2]);
                }
            }
        }
    } else {
        unknownArgument(NULL);
    }
}


void installGame(char *arguments[]) {
    //下载 version_manifest.json 到 versions
    char filePath[4096];
    sprintf(filePath, "%s%s", versionsDir, "/version_manifest.json");
    downloadFile(filePath, downloadProviderP->versionManifest);

    //读取 version_manifest.json 并获得 版本.json 的下载链接
    FILE *versionJsonFilex = fopen(filePath, "r");
    char versionFileContent[_char] = {0};
    fread(versionFileContent, sizeof(versionFileContent), 1, versionJsonFilex);
    cJSON *cj = cJSON_Parse(versionFileContent);
    cJSON *versionsArray = cJSON_GetObjectItem(cj, "versions");
    char url[512];
    _Bool exits = false;
    for (int i = 0; i < cJSON_GetArraySize(versionsArray); i++) {
        cJSON *versionsArrayIndex1 = cJSON_GetArrayItem(versionsArray, i);
        if (!strcmp(cJSON_GetObjectItem(versionsArrayIndex1, "id")->valuestring, arguments[2])) {
            memset(url, 0, sizeof(url));
            strcpy(url, cJSON_GetObjectItem(versionsArrayIndex1, "url")->valuestring);
            downloadProvider_str_replace(url, officialapiprovider.versionJson, downloadProviderP->versionJson);
            //printf("%s", url);
            exits = true;
        }
    }
    if (!exits) { //不存在这个 Minecraft 版本
        printf("Target version does not exist.");
        char path[4096];
        sprintf(path, "%s/%s", versionsDir, pendingDownloadDirName);
        rmdir(path);
    } else {
        //下载 版本.json
        char jsonPath[4096];
        sprintf(jsonPath, "%s/%s/%s.json", versionsDir, pendingDownloadDirName, pendingDownloadDirName);
        downloadFile(jsonPath, url);
        downloadClient(jsonPath, arguments);
        downloadLibraries(jsonPath, arguments);
        downloadAssets(jsonPath, arguments);
    }
}

void downloadClient(char *versionJsonFullPath, char *arguments[]) {
    //读取版本.json
    FILE *versionJsonFilex = fopen(versionJsonFullPath, "r");
    char versionFileContent[_char] = {0};
    fread(versionFileContent, sizeof(versionFileContent), 1, versionJsonFilex);
    cJSON *cj = cJSON_Parse(versionFileContent);

    //---------------下载客户端---------------
    printf("Downloading client...\n");
    cJSON *downloadsBig = cJSON_GetObjectItem(cj, "downloads");
    char clientUrl[512];
    memset(clientUrl, 0, sizeof(clientUrl));
    strcpy(clientUrl, cJSON_GetObjectItem(
            cJSON_GetObjectItem(downloadsBig, "client"), "url")->valuestring);
    downloadProvider_str_replace(clientUrl, officialapiprovider.versionClient, downloadProviderP->versionClient);
    char jarPath[4096];
    sprintf(jarPath, "%s/%s/%s.jar", versionsDir, pendingDownloadDirName, pendingDownloadDirName);
    downloadFile(jarPath, clientUrl);
}

void downloadLibraries(char *versionJsonFullPath, char *arguments[]) {
    //读取版本.json
    FILE *versionJsonFilex = fopen(versionJsonFullPath, "r");
    char versionFileContent[_char] = {0};
    fread(versionFileContent, sizeof(versionFileContent), 1, versionJsonFilex);
    cJSON *cj = cJSON_Parse(versionFileContent);

    //---------------检测 libraries 是否下载，不适用于 rules 就不下载---------------
    printf("Downloading libraries...\n");
    cJSON *librariesArray = cJSON_GetObjectItem(cj, "libraries");
    for (int i = 0; i < cJSON_GetArraySize(librariesArray); i++) {
        _Bool add = true;
        cJSON *librariesArrayIndex1 = cJSON_GetArrayItem(librariesArray, i);
        cJSON *downloads = cJSON_GetObjectItem(librariesArrayIndex1, "downloads");
        if (downloads != NULL) { // 有些库是没有 downloads 的，比如 Fabric
            cJSON *artifact = cJSON_GetObjectItem(downloads, "artifact");
            char *path = (artifact != NULL) ? cJSON_GetObjectItem(artifact, "path")->valuestring : "null";

            //切换下载源
            char url[512];
            memset(url, 0, sizeof(url));
            strcpy(url, (artifact != NULL) ? cJSON_GetObjectItem(artifact, "url")->valuestring : "null");
            downloadProvider_str_replace(url, officialapiprovider.libraries,
                                         downloadProviderP->libraries);

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
            //下载并解压 natives
            if (cJSON_GetObjectItem(librariesArrayIndex1, "natives") != NULL
                && cJSON_GetObjectItem(cJSON_GetObjectItem(librariesArrayIndex1, "natives"), "windows") != NULL
                && currentOS == WINDOWS) {
                add = false;
                cJSON *classifiers = cJSON_GetObjectItem(downloads, "classifiers");
                char *cPath = cJSON_GetObjectItem(
                        cJSON_GetObjectItem(classifiers, "natives-windows"), "path")->valuestring;
                //切换下载源
                char cUrl[512];
                memset(cUrl, 0, sizeof(cUrl));
                strcpy(cUrl, cJSON_GetObjectItem(
                        cJSON_GetObjectItem(classifiers, "natives-windows"), "url")->valuestring);
                downloadProvider_str_replace(cUrl, officialapiprovider.libraries,
                                             downloadProviderP->libraries);
                char nativesLibFullPath[3072];
                sprintf(nativesLibFullPath, "%s/libraries/%s", configs.gameDir, cPath);
                createFolderRecursion(nativesLibFullPath);
                if (access(nativesLibFullPath, F_OK) == -1) {
                    downloadFile(nativesLibFullPath, cUrl);
                }
                char aNativesDir[4096];
                //和 --fixLibraries 有关 如果是通过 -install 调用该函数那么 pendingDownloadDirName 就不是 NULL
                sprintf(aNativesDir, "%s/%s/natives-windows", versionsDir, pendingDownloadDirName);
                createFolderRecursion(aNativesDir);
                uncompressZip(nativesLibFullPath, aNativesDir);
            }

            //正式下载 libraries
            if (add) {
                char jarLibFullPath[3072];
                sprintf(jarLibFullPath, "%s/libraries/%s", configs.gameDir, path);
                createFolderRecursion(jarLibFullPath);
                // jar 不存在或者损坏才下载
                if (archiveIsBroken(jarLibFullPath)) downloadFile(jarLibFullPath, url);
            }
        } else { // 如果没有 downloads 就通过 name 解析出 url,比如 Fabric 的 Libraries
            /*char *name = cJSON_GetObjectItem(librariesArrayIndex1, "name")->valuestring;
            char path[4096] = {0},temp2[2048] = {0};
            getFabricLibrariesPath(name, path);
            sprintf(temp2, "%s/libraries/%s;", configs.gameDir, path);
            strncat(classPath, temp2, strlen(temp2));*/
        }
    }
}

void downloadAssets(char *versionJsonFullPath, char *arguments[]) {
    //读取版本.json
    FILE *versionJsonFilex = fopen(versionJsonFullPath, "r");
    char versionFileContent[_char] = {0};
    fread(versionFileContent, sizeof(versionFileContent), 1, versionJsonFilex);
    cJSON *cj = cJSON_Parse(versionFileContent);

    //---------------下载 Assets Index---------------
    printf("Downloading assets index...\n");

    char assetsIndexDir[3072], assetsObjectDir[3072];
    sprintf(assetsIndexDir, "%s/indexes", assetsDir);
    sprintf(assetsObjectDir, "%s/objects", assetsDir);
    mkdir(assetsDir);
    mkdir(assetsObjectDir);
    mkdir(assetsIndexDir);

    //切换下载源
    char assetIndexUrl[512];
    memset(assetIndexUrl, 0, sizeof(assetIndexUrl));
    strcpy(assetIndexUrl, cJSON_GetObjectItem(cJSON_GetObjectItem(cj, "assetIndex"), "url")->valuestring);
    downloadProvider_str_replace(assetIndexUrl, officialapiprovider.assetsIndex, downloadProviderP->assetsIndex);

    char *assetsVersion = cJSON_GetObjectItem(cj, "assets")->valuestring;
    char assetsIndexPath[3072];
    sprintf(assetsIndexPath, "%s/indexes/%s.json", assetsDir, assetsVersion);
    if (access(assetsIndexPath, F_OK) == -1) {
        downloadFile(assetsIndexPath, assetIndexUrl);
    }
    FILE *assetsIndexFile = fopen(assetsIndexPath, "r");
    char assetsIndexContent[524288] = {0};
    char tempStr[524288], tempStr2[1024];
    fread(assetsIndexContent, sizeof(assetsIndexContent), 1, assetsIndexFile);
    cJSON *cjAssetsIndex = cJSON_Parse(assetsIndexContent);
    cJSON *objects = cJSON_GetObjectItem(cjAssetsIndex, "objects");
    cJSON *p = objects->child;
    //将内容塞进 assets.txt 并用 aria2c 下载
    FILE *tempFile;
    tempFile = fopen("assets.txt", "w+");
    while (p != NULL) {
        char *objHash = cJSON_GetObjectItem(p, "hash")->valuestring;
        char objDirName[3], objDirPath[3072], objFullPath[3072], objUrl[512];
        assetsIndexSubString(objHash, objDirName);
        sprintf(objFullPath, "%s/objects/%s/%s", assetsDir, objDirName, objHash);
        sprintf(objUrl, "%s%s/%s", downloadProviderP->assets, objDirName, objHash);
        sprintf(objDirPath, "%s/objects/%s", assetsDir, objDirName);
        if (access(objFullPath, F_OK) == -1) {
            sprintf(tempStr2, "%s\n dir=%s\n out=%s\n", objUrl, objDirPath, objHash);
            strncat(tempStr, tempStr2, strlen(tempStr2));
        }
        p = p->next;
    }
    fprintf(tempFile, "%s", tempStr);
    fclose(tempFile);
    system("aria2c -j 48 --input-file=assets.txt");
    remove("assets.txt");
}


//emcl -install -list 查看可安装的 Minecraft 版本
static void getInstallableVersions(char *arguments[]) {
    char downloadCommand[8192], filePath[8192];
    sprintf(filePath, "%s%s", versionsDir, "/version_manifest.json");
    sprintf(downloadCommand, "wget -O %s %s", filePath, downloadProviderP->versionManifest);
    system(downloadCommand);
    FILE *versionJsonFilex = fopen(filePath, "r");
    char versionFileContent[_char] = {0};
    fread(versionFileContent, sizeof(versionFileContent), 1, versionJsonFilex);
    cJSON *cj = cJSON_Parse(versionFileContent);
    cJSON *versionsArray = cJSON_GetObjectItem(cj, "versions");
    printf("[");
    for (int i = cJSON_GetArraySize(versionsArray) - 1; i > 0; i--) {
        cJSON *versionsArrayIndex1 = cJSON_GetArrayItem(versionsArray, i);
        if (!strcmp(cJSON_GetObjectItem(versionsArrayIndex1, "type")->valuestring, "release")) {
            printf("%s, ", cJSON_GetObjectItem(versionsArrayIndex1, "id")->valuestring);
        }
    }
    printf("%s]", cJSON_GetObjectItem(cJSON_GetObjectItem(cj, "latest"), "snapshot")->valuestring);
}