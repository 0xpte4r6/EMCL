//
// Created by Administrator on 2022/5/12.
//

#include "options.h"

static _Bool isDirectory(char *fileName);

void listOption(char *arguments[]) {
    DIR *dir;
    struct dirent *dp;
    char versionsDir[_char] = {'a', '\0'};
    strcpy(versionsDir, configs.gameDir);
    strncat(versionsDir, "/versions", 9);
    dir = opendir(versionsDir); //打开目录指针
    char fullPath[_char];
    strcpy(fullPath, versionsDir);

    while ((dp = readdir(dir)) != NULL) { //通过目录指针读目录
        strcpy(fullPath, versionsDir);
        sprintf(fullPath, "%s/%s", versionsDir, dp->d_name);
        if (isDirectory(fullPath) && strcmp(dp->d_name,"..") && strcmp(dp->d_name,".")) {
            printf("%s\n", dp->d_name);
        }
    }
    (void) closedir(dir); //关闭目录
}

static _Bool isDirectory(char *fileName) {
    struct stat buf;
    int result;
    result = stat(fileName, &buf);
    if (S_IFDIR & buf.st_mode) {
        return true;
    } else if (S_IFREG & buf.st_mode) {
        return false;
    }
}