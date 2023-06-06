// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "RBRJoykey.h"

IPlugin* g_pRBRPlugin = nullptr;

class IRBRGame;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}

IPlugin* RBR_CreatePlugin(IRBRGame* pGame) {
    if (g_pRBRPlugin == nullptr) g_pRBRPlugin = new RBRJoykey(pGame);
    return g_pRBRPlugin;
}