// RBRJoykey.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "config.h"
#include "Utils/LogUtil.h"
#include "Utils/StringUtil.h"
#include "Utils/INIUtil.h"
#include "Lib/Detourxs/detourxs.h"
#include "RBRJoykey.h"

extern IPlugin* g_pRBRPlugin;

tRBRDirectXEndScene Func_OrigRBRDirectXStartScene = nullptr;
tRBRDirectXEndScene Func_OrigRBRDirectXEndScene = nullptr;

HRESULT __fastcall CustomRBRDirectXStartScene(void* objPointer) {
  // We need code at the beginning of this function that is not a call to another function.
  // If we just added "Countdown::DrawCountdown();" then hooking EndScene from another 
  // plugin will fail. This is because of some technicality of how the hooking works.

  return ::Func_OrigRBRDirectXStartScene(objPointer);
}

HRESULT __fastcall CustomRBRDirectXEndScene(void* objPointer) {
  // We need code at the beginning of this function that is not a call to another function.
  // If we just added "Countdown::DrawCountdown();" then hooking EndScene from another 
  // plugin will fail. This is because of some technicality of how the hooking works.

  return ::Func_OrigRBRDirectXEndScene(objPointer);
}

RBRJoykey::RBRJoykey(IRBRGame* pGame) : m_pGame(pGame) {
  LogUtil::ToFile("Creating Plugin " + Config::PluginName + ".");

  if(CreateDirectory(Config::PluginFolder.c_str(), NULL) ||
    ERROR_ALREADY_EXISTS == GetLastError()
    ) {
    // success
  } else {
    LogUtil::ToFile("Failed creating plugin folder.");
  }

  LoadINI();
}

RBRJoykey::~RBRJoykey(void) {
  LogUtil::ToFile("Destroying Plugin " + Config::PluginName + ".");
}

const char* RBRJoykey::GetName(void) {
  if(Func_OrigRBRDirectXEndScene == nullptr) {
    // Do the initialization and texture creation only once because RBR may call GetName several times
    LogUtil::ToFile("Initializing the plugin");

    RBRAPI_InitializeObjReferences();

    InitJoykey();

    auto gtcDirect3DStartScene = new DetourXS((LPVOID)0x0040E880, ::CustomRBRDirectXStartScene, TRUE);
    Func_OrigRBRDirectXStartScene = (tRBRDirectXEndScene)gtcDirect3DStartScene->GetTrampoline();

    auto gtcDirect3DEndScene = new DetourXS((LPVOID)0x0040E890, ::CustomRBRDirectXEndScene, TRUE);
    Func_OrigRBRDirectXEndScene = (tRBRDirectXEndScene)gtcDirect3DEndScene->GetTrampoline();
  }

  return Config::PluginName.c_str();
}

void RBRJoykey::LoadINI(void) {
  m_setting = new Config::Setting(Config::PluginConfig);
}

void RBRJoykey::InitJoykey(void) {
  POINT pa, pb;
  RBRAPI_MapRBRPointToScreenPoint(0, 0, &pa);
  RBRAPI_MapRBRPointToScreenPoint(640, 480, &pb);
  m_scalex = std::abs(pb.x - pa.x) / 1920.0f;
  m_scaley = std::abs(pb.y - pa.y) / 1440.0f;
}