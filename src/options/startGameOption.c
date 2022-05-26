//
// Created by Administrator on 2022/5/11.
//
#include "options.h"

void threadA();

int my_CreateProcess(char *cmd);

HANDLE *mc;
_Bool mcb = true;
char *nativesDir, *versionJsonFile, *versionJarFile;

void startGameOption(char *arguments[]) {
    nativesDir = calloc(sizeof(char) * 512, sizeof(char));
    versionJsonFile = calloc(sizeof(char) * 512, sizeof(char));
    versionJarFile = calloc(sizeof(char) * 512, sizeof(char));
    char *pendingBootVersion;
    if (arguments[2] == NULL) {
        pendingBootVersion = configs.selectedVersion;
    } else if (arguments[2] != NULL) {
        pendingBootVersion = arguments[2];
    }
    if (gameVersionIsExits(pendingBootVersion)) {
        sprintf(versionJsonFile, "%s/%s/%s.json", versionsDir, pendingBootVersion,
                pendingBootVersion);
        sprintf(versionJarFile, "%s/%s/%s.jar", versionsDir, pendingBootVersion,
                pendingBootVersion);
        sprintf(nativesDir, "%s/%s/natives-windows", versionsDir, pendingBootVersion);

        char *launchCommand = getLaunchArguments(configs.javaPath, configs.gameDir, assetsDir,
                                                 getAssetsIndex(versionJsonFile),
                                                 pendingBootVersion, versionJsonFile, configs.maxMemory, nativesDir,
                                                 getLibrariesPath(versionJsonFile, versionJarFile), configs.playerName,
                                                 configs.uuid, configs.accessToken, 0, 0);
        if(!onlyPrint){
            my_CreateProcess(launchCommand);
        }else{
            printf(launchCommand);
        }
    } else {
        printf("The name \"%s\" does not exist.\n", pendingBootVersion);
    }
    free(nativesDir);
    free(versionJsonFile);
    free(versionJarFile);

    /*char *cd = calloc(sizeof(char) * 16384, sizeof(char));
    char tempversionJsonFile[512], tempversionJarFile[512], tempnativesDir[512],
            *versionJsonP = versionJsonFile, *versionJarP = versionJarFile,
            *selectVersionP = configs.selectedVersion, *nativesDirP = nativesDirX;
    if (arguments[2] != NULL) {
        sprintf(tempversionJsonFile, "%s/%s/%s.json", versionsDir, arguments[2], arguments[2]);
        if (access(tempversionJsonFile, F_OK) == 0) {
            sprintf(tempversionJarFile, "%s/%s/%s.jar", versionsDir, arguments[2], arguments[2]);
            sprintf(tempnativesDir, "%s/%s/natives-windows", versionsDir, arguments[2]);
            versionJsonP = tempversionJsonFile;
            versionJarP = tempversionJarFile;
            nativesDirP = tempnativesDir;
        } else if (access(tempversionJsonFile, F_OK) == -1) {
            printf("The name \"%s\" already exists, please change it.\n", arguments[2]);
            return;
        }
    } else {
        sprintf(tempversionJsonFile, "%s/%s/%s.json", versionsDir, configs.selectedVersion, configs.selectedVersion);
        printf("The selected version \"%s\" already exists, please change it.\n", configs.selectedVersion);
        return;
    }
    char *launchCommand = getLaunchArguments(configs.javaPath, configs.gameDir, assetsDir,
                                             getAssetsIndex(versionJsonP),
                                             selectVersionP, versionJsonP, configs.maxMemory, nativesDirP,
                                             getLibrariesPath(versionJsonP, versionJarP), configs.playerName,
                                             configs.uuid,
                                             configs.accessToken, 0, 0);


    if (!onlyPrint && arguments[3] == NULL) {
        sprintf(cd, "cd %s\n%s", configs.gameDir, launchCommand);
        printf("Launching game...\n");
        //system(cd);
        //system(launchCommand);
        my_CreateProcess(launchCommand);

    } else if (onlyPrint) {
        printf(launchCommand);
    } else if (arguments[3] != NULL && !strcmp(arguments[3], "--fixAssets")) {
        downloadAssets(versionJsonP, arguments);
    } else if (arguments[3] != NULL && !strcmp(arguments[3], "--fixLibraries")) {
        downloadLibraries(versionJsonP, arguments);
    }*/
    //free(cd);
}

int my_CreateProcess(char *cmd) {
    HANDLE pipeW = NULL, pipeR = NULL;
    SECURITY_ATTRIBUTES pipeSA;
    pipeSA.nLength = sizeof(pipeSA);
    pipeSA.lpSecurityDescriptor = NULL;
    pipeSA.bInheritHandle = TRUE;
    if (!CreatePipe(&pipeR, &pipeW, &pipeSA, 0)) {
        printf("pipe create failed!");
    }

    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    DWORD flags = DETACHED_PROCESS;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = NULL;
    si.hStdError = pipeW;
    si.hStdOutput = pipeW;

    //TCHAR cmd[] = TEXT(xcmd);
    BOOL ret = CreateProcess(NULL, cmd, NULL, NULL, TRUE, flags, NULL, NULL, &si, &pi);
    mc = pi.hProcess;

    if (ret) {
        //HANDLE th;
        //th = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)threadA,NULL,1,0);
        //ResumeThread(th);

        //WaitForSingleObject(pi.hProcess, INFINITE);
        SetConsoleTitleA("FUCK");
        unsigned long file_size = 1;
        char *buff = (char *) malloc(file_size);
        DWORD dwRead, ExitCode = 0;;
        do {
            GetExitCodeProcess(pi.hProcess, &ExitCode);
            if (!ReadFile(pipeR, buff, file_size, &dwRead, NULL)) {
                printf("ReadPipe error.\n");
                return 0;
            }
            buff[file_size] = '\0';
            printf(buff);
        } while (mcb);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(pipeW);
        CloseHandle(pipeR);
        return 0;
    }
    printf("执行失败....\n");
    return -1;
}

void threadA() {
    WaitForSingleObject(mc, INFINITE);
    Sleep(15);
    mcb = false;
}