//
// Created by Administrator on 2022/5/9.
//

#include "utils.h"
#include "modules/mclauncher.h"
#include "main.h"
#include <tchar.h>
#include <windows.h>
#include <string.h>

void saveConfig(char *jsonObjectFather) {
    FILE *configFile = fopen(CONFIGFILE, "w+");
    //delCharFromString(jsonObjectFather, '\t');
    //delCharFromString(jsonObjectFather, '\n');
    fprintf(configFile, "%s", jsonObjectFather);
    fclose(configFile);
}

void delCharFromString(char src[], char del) {
    int srcLen = 0;
    while (src[srcLen++] != '\0');
    char cx[srcLen];
    int v = 0;
    for (int i = 0; i < strlen(src) + 1; i++) {
        if (src[i] != del) {
            cx[i - v] = src[i];
        } else {
            cx[i] = src[i];
            v++;
        }
    }
    for (int i = 0; i < srcLen; i++) {
        src[i] = cx[i];
    }
}

/*void str_replace(char *str1, char *str2, char *str3) {
    char str1x[] = "-Dminecraft.launcher.brand=${launcher_name}";
    char str2x[] = "${launcher_name}";
    char str3x[] = "CCMCL";
    char temp[100];
    unsigned int startReplace, endReplace = 0;
    for (int h = 0; h < strlen(str1x) - strlen(str2x) + 1; h++) {
        for (int i = 0; i < strlen(str2x); i++) {
            temp[i] = str1x[i + h];
        }
        startReplace = h;
        endReplace = startReplace + strlen(str3x) - 1;
        if (!strcmp(temp, str2x)) {
            for (int i = 0; i < strlen(str3x); i++) {
                str1x[startReplace + i] = str3x[i];
            }
            str1x[endReplace+1] = '\0';
            break;
        }
        printf("\n\n%s  %d  %d", temp, startReplace, endReplace);
    }
    printf("\n\n%s", str1x);
}*/


char *str_replace(char str1[], char str2[], char str3[]) {
    char str1x[] = "-Dminecraft.launcher.brand=${launcher_name}";
    char str2x[] = "${launcher_name}";
    char str3x[] = "CCMCL";
    char temp[100];
    char temp2[638];
    unsigned int startReplace = 0;
    for (int h = 0; h < strlen(str1x) - strlen(str2x) + 1; h++) {
        for (int i = 0; i < strlen(str2x); i++) {
            temp[i] = str1x[i + h];
        }
        startReplace = h;
        if (!strcmp(temp, str2x)) {
            for (int i = 0; i < strlen(str2x); i++) {
                str1x[startReplace + i] = '\0';
            }
        }
        printf("\n\n%s  %d  %d", temp, startReplace);
    }
    sprintf(temp2, "%s%s", str1x, str3x);
    printf("\n%s", temp2);
    return NULL;
}


void str_replace_char(char str1[], char str2, char str3) {
    for (int i = 0; i < strlen(str1); i++) {
        if (str1[i] == str2) {
            str1[i] = str3;
        }
    }
}

void downloadFile(char *path, char *url) {
    char temp[4096];
    sprintf(temp, "wget -O %s %s", path, url);
    system(temp);
}

void createFolderRecursion(char *dirMame) { //递归创建文件夹  格式： xxx/xxx/xxx
    char shortDirName[3072];
    for (int i = 0; i < strlen(dirMame); i++) {
        if (dirMame[i] == '/') {
            memset(shortDirName, 0, sizeof(shortDirName));
            strncat(shortDirName, dirMame, i);
            if (_access(shortDirName, F_OK) == -1) {
                _mkdir(shortDirName);
            }
        }
    }
}

void uncompressZip(char *zipPath, char *uncompressTo) {
    char temp[4096];
    sprintf(temp, "unzip -o %s -d %s", zipPath, uncompressTo);
    system(temp);
}

//跨平台
int exeCmdWithResult(char *cmd, char *result) {
    char buffer[128];                        //定义缓冲区
    FILE *pipe = _popen(cmd, "r");            //打开管道，并执行命令
    if (!pipe) return 0;                     //返回0表示运行失败
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe)) {            //将管道输出到result中
            strcat(result, buffer);
        }
    }
    _pclose(pipe);                          //关闭管道
    return 1;                               //返回1表示运行成功
}


//Windows Only
int exec_cmd_1(char *cmd, char *result) {
    SECURITY_ATTRIBUTES sa; // 此结构体包含一个对象的安全描述符，并指定检索到制定这个结构的句柄是否是可继承的
    sa.nLength = sizeof(SECURITY_ATTRIBUTES); // 结构体大小
    sa.lpSecurityDescriptor = NULL; // 安全描述符
    sa.bInheritHandle = TRUE; // 安全描述的对象能否被创建的进程继承，TRUE表示能被继承

    HANDLE h_read, h_write;
    if (!CreatePipe(&h_read, &h_write, &sa, 0)) // 创建匿名管道
        return 0;

    STARTUPINFO si = {sizeof(STARTUPINFO)}; // 此结构体用于指定新进程的主窗口特性 //si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si);
    si.wShowWindow = SW_HIDE; // 窗口设为隐藏
    si.hStdError = NULL;//h_write; // 标识控制台窗口的缓存，可指定管道
    si.hStdOutput = h_write; // 同上
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;// 使用wShowWindow成员，使用hStdInput、hStdOutput、hStdError成员

    PROCESS_INFORMATION pi; // 此结构返回有关新进程及其主线程的信息
    if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) // 创建进程
        return -1;
    CloseHandle(h_write);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    char buffer[1024] = {0};
    DWORD i = 0, bytes_read;
    while (i < 2048) {
        if (ReadFile(h_read, buffer, 1023, &bytes_read, NULL) == NULL)
            break;
        strcat(result, buffer);
    }
    CloseHandle(h_read);
    return 1;
}

/**
* @brief 通过创建进程的方式无控制台窗口执行cmd
*
* @param[in] cmd 命令
* @return 1|0|-1 成功|管道创建失败|进程创建失败
* Windows Only
*/
int exec_cmd_2(char *cmd) {
    STARTUPINFO si = {sizeof(STARTUPINFO)}; // 此结构体用于指定新进程的主窗口特性 //si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si);
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW;

    PROCESS_INFORMATION pi; // 此结构包含有关新进程及其主线程的信息
    if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
        return -1;

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 1;
}

_Bool archiveIsBroken(char *archivePath) {
    char result[3072], temp[3072];
    sprintf(temp, "zipinfo -t %s", archivePath);
    exeCmdWithResult(temp, result);
    if (strstr(result, "bytes uncompressed") != NULL) return false;
    else return true;
}

void assetsIndexSubString(const char *full, char previousTwo[3]) {
    previousTwo[0] = full[0];
    previousTwo[1] = full[1];
    previousTwo[2] = 0;
}


void downloadProvider_str_replace(char str1[], const char *str2, const char *str3) {
    char str1replaced[512], str1part2[512];
    memset(str1replaced, 0, sizeof(str1replaced));
    memset(str1part2, 0, sizeof(str1part2));
    unsigned int i;
    for (i = strlen(str2); i < strlen(str1); i++) {
        str1part2[i - strlen(str2)] = str1[i];
    }
    str1part2[i + 1] = 0;
    strncat(str1replaced, str3, strlen(str3));
    strncat(str1replaced, str1part2, strlen(str1part2));
    strcpy(str1, str1replaced);
}

_Bool gameVersionIsExits(char *versionName) {
    char tempversionJsonFile[512];
    sprintf(tempversionJsonFile,"%s/%s/%s.json",versionsDir,versionName,versionName);
    if (access(tempversionJsonFile, F_OK) == -1) {
        return false;
    }
    return true;
}

/*void delAllFilesExceptDLL(char *dir){

}*/


/*
int ax();

int getLength(){
    return 0;
}

typedef struct STRING{
    char *s;
    int length;
    int (*getLength)();
}String;

String *new_String(char *c){
    String *p = malloc(sizeof (String));
    p->s = c;
    p->length = strlen(c);
    p->getLength = getLength;
    return p;
}

int ax(){
    String *string = new_String("asdf");
    int i = string->length;
    string->getLength();
    printf("%d",i);
    return 0;
}*/

/*int file_size(char *filename);

int file_size(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fclose(fp);

    return size;
}*/

/*char *a = calloc(sizeof(char)*128,sizeof(char));*/
//char *javaPathx = "D:\\jdk-17.0.2\\bin\\java.exe";