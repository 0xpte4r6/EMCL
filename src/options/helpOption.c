//
// Created by Administrator on 2022/5/12.
//
#include "options.h"

void helpOption(char *arguments[]) {
    printf("使用命令行来启动 Minecraft。"
            "\n\nEMCL <参数> 操作 (带*为可选参数)"
           "\n\t-u：获取使用手册"
           "\n\t-l：列出已安装的 MC 版本"
           "\n\t-install *<版本> *--fixAssets/--fixLibraries：启动游戏/修复资源或库"
           "\n\t-s <版本>：选择游戏版本"
           "\n\t-p *<版本>：打印启动参数"
           "\n\t-install <版本> *-n *<版本名称>：安装游戏"
           "\n\t-v：关于启动器");

}

void AboutOption(char *arguments[]){
    printf("                        ______   __  __    _____   _      \n"
           "                       |  ____| |  \\/  |  / ____| | |     \n"
           "                       | |__    | \\  / | | |      | |     \n"
           "                       |  __|   | |\\/| | | |      | |     \n"
           "                       | |____  | |  | | | |____  | |____ \n"
           "                       |______| |_|  |_|  \\_____| |______|\n"
           "                                                          \n"
           "                                                          ");
    printf("\n=====================================================================================");
    printf("\n-[WIP]Express Minecraft Launcher 1.0:A Launcher for Minecraft Java Edition(Developed by C)");
    printf("\n-https://github.com/Graetpro");
    printf("\n-https://www.github.com/MrShieh-X/console-minecraft-launcher");
    printf("\n=====================================================================================");
}