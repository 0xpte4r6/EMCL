//
// Created by Administrator on 2022/5/16.
//

#include "downloadApiProviders.h"

const DownloadAPIProvider officialapiprovider = {
        "https://launchermeta.mojang.com/mc/game/version_manifest.json",
        "https://launchermeta.mojang.com/",
        "https://resources.download.minecraft.net/",
        "https://libraries.minecraft.net/",
        "https://launcher.mojang.com/",
        "https://launchermeta.mojang.com/"
};

const DownloadAPIProvider bmclapiprovider = {
        "https://bmclapi2.bangbang93.com/mc/game/version_manifest.json",
        "https://bmclapi2.bangbang93.com/",
        "https://bmclapi2.bangbang93.com/",
        "https://bmclapi2.bangbang93.com/maven/",
        "https://bmclapi2.bangbang93.com/",
        "https://bmclapi2.bangbang93.com/"
};

const DownloadAPIProvider mcbbsapiprovider = {
        "https://download.mcbbs.net/mc/game/version_manifest.json",
        "https://download.mcbbs.net/",
        "https://download.mcbbs.net/assets/",
        "https://download.mcbbs.net/maven/",
        "https://download.mcbbs.net/",
        "https://download.mcbbs.net/"
};