//
// Created by Administrator on 2022/5/16.
//

#ifndef C_CONSOLE_MINECRAFT_LAUNCHER_DOWNLOADAPIPROVIDERS_H
#define C_CONSOLE_MINECRAFT_LAUNCHER_DOWNLOADAPIPROVIDERS_H

typedef struct {
    char *versionManifest, *assetsIndex, *assets, *libraries, *versionClient, *versionJson;
} DownloadAPIProvider;
const extern DownloadAPIProvider officialapiprovider, bmclapiprovider, mcbbsapiprovider;

#endif //C_CONSOLE_MINECRAFT_LAUNCHER_DOWNLOADAPIPROVIDERS_H
