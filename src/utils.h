//
// Created by Administrator on 2022/5/9.
//

#ifndef C_CONSOLE_MINECRAFT_LAUNCHER_UTILS_H
#define C_CONSOLE_MINECRAFT_LAUNCHER_UTILS_H

void delCharFromString(char src[], char del);

void saveConfig(char *jsonObjectFather);

char *str_replace(char str1[], char str2[], char str3[]);

void str_replace_char(char str1[], char str2, char str3);

void downloadFile(char *path, char *url);

void createFolderRecursion(char *dirMame);

void uncompressZip(char *zipPath,char *uncompressTo);

int exeCmdWithResult(char *cmd, char *result);

int exec_cmd_1(char* cmd, char* result);

int exec_cmd_2(char* cmd);

_Bool archiveIsBroken(char *archivePath);

void assetsIndexSubString(const char *full, char previousTwo[3]);

void downloadProvider_str_replace(char str1[], const char *str2, const char *str3);

_Bool gameVersionIsExits(char *versionName);

#endif //C_CONSOLE_MINECRAFT_LAUNCHER_UTILS_H
