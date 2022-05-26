//
// Created by Administrator on 2022/5/12.
//

#include "options.h"

void selectOption(char *arguments[]) {
    char *versionPath = calloc(sizeof(char) * 512, sizeof(char));
    sprintf(versionPath, "%s/versions/%s", configs.gameDir, arguments[2]);
    if (arguments[2] != NULL) {
        if (access(versionPath, F_OK) == -1) {
            printf("Target game version does not exist.");
        } else {
            configs.selectedVersion = arguments[2];
            cJSON *cjx = cJSON_Parse(jsonObjectFatherString);
            cJSON *selectVersionObj = cJSON_GetObjectItem(cjx, "selectedVersion");
            cJSON_SetValuestring(selectVersionObj, arguments[2]);
            jsonObjectFatherString = cJSON_Print(cjx);
            saveConfig(jsonObjectFatherString);
            cJSON_Delete(cjx);
        }
    } else {
        unknownArgument(NULL);
    }
    free(versionPath);
}