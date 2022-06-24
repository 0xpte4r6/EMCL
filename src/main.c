#include <zconf.h>
#include <tchar.h>
#include "main.h"

typedef void(*P)(char *[]);

const int currentOS = WINDOWS;
_Bool onlyPrint = false;
CONFIGS configs;

char *jsonObjectFatherString, *assetsDir, *versionsDir;
const DownloadAPIProvider *downloadProviderP;

struct {
    char *key;

    void (*exec)(char *arguments[]);
} optionsMap[] = {
        {"-b",       startGameOption},
        {"-s",       selectOption},
        {"-l",       listOption},
        {"-p",       printOption},
        {"-u",       helpOption},
        {"-v",       AboutOption},
        {"-install", installOption}
};

P optionsMap_getValue(char *key) {
    int arrayLength = sizeof(optionsMap) / sizeof(optionsMap[0]);
    for (int i = 0; i < arrayLength; i++) {
        if (!strcmp(optionsMap[i].key, key)) {
            return optionsMap[i].exec;
        }
    }
    return unknownArgument;
}

int main(__attribute__((unused)) int argc, char *argv[]) {
    SetConsoleOutputCP(65001); //设置控制台编码为 UTF-8
    initConfig();
    versionsDir = calloc(sizeof(char) * 512, sizeof(char));
    assetsDir = calloc(sizeof(char) * 512, sizeof(char));
    sprintf(versionsDir, "%s/%s", configs.gameDir, "versions");
    sprintf(assetsDir, "%s/%s", configs.gameDir, "assets");
    if (argv[1] != NULL) {
        P p = optionsMap_getValue(argv[1]);
        p(argv);
    } else {
        helpOption(NULL);
    }
    /*if (access(versionJsonFile, F_OK == -1)) {
        printf("\nCan't find version JSON \"%s\"", versionJsonFile);
        return 1;
    }*/
#ifdef linux
    printf("linux");
#endif
    free(assetsDir);
    free(versionsDir);
    return 0;
}

void initConfig() {
    cJSON *configJsonObjectFather;
    FILE *configFile;
    if (access(getConfigFileFullPath(), F_OK) == -1) {
        configFile = fopen(getConfigFileFullPath(), "w+");
        configJsonObjectFather = cJSON_CreateObject();
        cJSON_AddNumberToObject(configJsonObjectFather, "maxMemory", 3072);
        cJSON_AddStringToObject(configJsonObjectFather, "gameDir", "F:/AppData/Roming/.minecraft");
        cJSON_AddStringToObject(configJsonObjectFather, "selectedVersion", "1.18.2");
        cJSON_AddStringToObject(configJsonObjectFather, "javaPath", "D:/jdk-17.0.2/bin/java.exe");
        cJSON_AddNumberToObject(configJsonObjectFather, "windowWidth", 854);
        cJSON_AddNumberToObject(configJsonObjectFather, "windowHeight", 480);
        cJSON_AddNumberToObject(configJsonObjectFather, "downloadSource", 0);
        cJSON_AddNumberToObject(configJsonObjectFather, "selectedAccount", 0);
        cJSON_AddArrayToObject(configJsonObjectFather,"accounts");

        cJSON *accountsArray = cJSON_GetObjectItem(configJsonObjectFather,"accounts");
        cJSON *accountsArrayObject1 = cJSON_CreateObject();
        cJSON_AddStringToObject(accountsArrayObject1,"playerName","XPlayer");
        cJSON_AddNumberToObject(accountsArrayObject1,"type",0);
        cJSON_AddItemToArray(accountsArray,accountsArrayObject1);



        jsonObjectFatherString = cJSON_Print(configJsonObjectFather);
        //printf("%s",jsonObjectFatherString);
        //delCharFromString(jsonObjectFatherString, '\t');
        //delCharFromString(jsonObjectFatherString, '\n');
        fprintf(configFile, "%s", jsonObjectFatherString);
        fclose(configFile);
    } else {
        configFile = fopen(getConfigFileFullPath(), "r");
        static char a[CONFIGFILE_SIZE];
        fread(a, sizeof(a), 1, configFile);
        jsonObjectFatherString = a;
        fclose(configFile);
        cJSON *cj = cJSON_Parse(jsonObjectFatherString);
        /*char *gameDir = cJSON_GetObjectItem(cj, "gameDir")->valuestring;
        if (gameDir[strlen(gameDir) - 1] == '/') {

        }*/
        configs.gameDir = cJSON_GetObjectItem(cj, "gameDir")->valuestring;
        configs.javaPath = cJSON_GetObjectItem(cj, "javaPath")->valuestring;
        //configs.playerName = cJSON_GetObjectItem(cj, "playerName")->valuestring;
        configs.selectedVersion = cJSON_GetObjectItem(cj, "selectedVersion")->valuestring;
        configs.maxMemory = cJSON_GetObjectItem(cj, "maxMemory")->valueint;
        configs.windowWidth = cJSON_GetObjectItem(cj, "windowWidth")->valueint;
        configs.windowHeight = cJSON_GetObjectItem(cj, "windowHeight")->valueint;
        configs.selectedAccount = cJSON_GetObjectItem(cj, "selectedAccount")->valueint;
        configs.playerName = cJSON_GetObjectItem(
                cJSON_GetArrayItem(cJSON_GetObjectItem(cj, "accounts"), configs.selectedAccount),
                "playerName")->valuestring;
        if (cJSON_GetObjectItem(
                cJSON_GetArrayItem(cJSON_GetObjectItem(cj, "accounts"), configs.selectedAccount),
                "type")->valueint == 1) {
            configs.uuid = cJSON_GetObjectItem(
                    cJSON_GetArrayItem(cJSON_GetObjectItem(cj, "accounts"), configs.selectedAccount),
                    "uuid")->valuestring;
            configs.accessToken = cJSON_GetObjectItem(
                    cJSON_GetArrayItem(cJSON_GetObjectItem(cj, "accounts"), configs.selectedAccount),
                    "accessToken")->valuestring;
        } else {
            configs.uuid = "null";
            configs.accessToken = "null";
        }

        cJSON *ds = cJSON_GetObjectItem(cj, "downloadSource");
        if (ds != NULL) {
            switch (ds->valueint) {
                case 1:
                    downloadProviderP = &bmclapiprovider;
                    break;
                case 2:
                    downloadProviderP = &mcbbsapiprovider;
                    break;
                default:
                    downloadProviderP = &officialapiprovider;
                    break;
            }
        }
    }
}

/*
static char *getConfigFileFullPath() {
    char *buffer;
    static char path[CONFIGFILE_SIZE];
    memset(path, 0, sizeof(path));
    if ((buffer = getcwd(NULL, 0)) == NULL) {
        perror("getcwd error");
        exit(0);
    }
    strcpy(path, buffer);
    str_replace_char(path, '\\', '/');
    strncat(path, "/", 2);
    strncat(path, CONFIGFILE, strlen(CONFIGFILE) + 1);
    free(buffer);
    return path;
}*/

static char *getConfigFileFullPath() {
    char *buffer = _pgmptr;
    static char path[CONFIGFILE_SIZE];
    memset(path, 0, sizeof(path));
    strcpy(path, buffer);
    str_replace_char(path, '\\', '/');
    unsigned int lastSlash = 0;
    for (unsigned int i = 0; i < strlen(path); i++) {
        if (path[i] == '/') {
            lastSlash = i;
        }
    }
    path[lastSlash] = '\0';
    strncat(path, "/", 2);
    strncat(path, CONFIGFILE, strlen(CONFIGFILE) + 1);
    //free(buffer);
    return path;
}