// RBRDashboard.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "config.h"
#include "RBR/D3D9Helpers.h"
#include "Utils/LogUtil.h"
#include "Utils/StringUtil.h"
#include "Utils/INIUtil.h"
#include "Lib/Detourxs/detourxs.h"
#include "RBRDashboard.h"

tRBRDirectXEndScene Func_OrigRBRDirectXEndScene = nullptr;
HRESULT __fastcall CustomRBRDirectXEndScene(void* objPointer) {
  // We need code at the beginning of this function that is not a call to another function.
  // If we just added "Countdown::DrawCountdown();" then hooking EndScene from another 
  // plugin will fail. This is because of some technicality of how the hooking works.

  if(g_pRBRGameMode->gameMode == 0x01) {
    return ((RBRDashboard *)g_pRBRPlugin)->CustomRBRDirectXEndScene(objPointer);
  }

  return ::Func_OrigRBRDirectXEndScene(objPointer);
}

RBRDashboard::RBRDashboard(IRBRGame* pGame) : m_pGame(pGame) {
  LogUtil::ToFile("Creating Plugin " + Config::PluginName + ".");

  if(CreateDirectory(Config::PluginFolder.c_str(), NULL) ||
    ERROR_ALREADY_EXISTS == GetLastError()
    ) {
    // success
  } else {
    LogUtil::ToFile("Failed creating plugin folder.");
  }

  LoadINI();
  m_ini = new INIUtil::INIManager(Config::PluginFolder + L"\RBRDashboard.ini");
}

RBRDashboard::~RBRDashboard(void) {
  LogUtil::ToFile("Destroying Plugin " + Config::PluginName + ".");
  delete m_ini;
}

const char* RBRDashboard::GetName(void) {
  if(Func_OrigRBRDirectXEndScene == nullptr) {
    // Do the initialization and texture creation only once because RBR may call GetName several times
    LogUtil::ToFile("Initializing the plugin");
    RBRAPI_InitializeObjReferences();

    InitDashboard();

    auto gtcDirect3DEndScene = new DetourXS((LPVOID)0x0040E890, ::CustomRBRDirectXEndScene, TRUE);
    Func_OrigRBRDirectXEndScene = (tRBRDirectXEndScene)gtcDirect3DEndScene->GetTrampoline();
  }

  return Config::PluginName.c_str();
}


void RBRDashboard::LoadINI(void) {

}

void RBRDashboard::InitDashboard(void) {
  std::string dashtexfile = "RBRDashboard.dds";
  m_ini->Get("Texture", "path", dashtexfile);

  std::wstring path = StringUtil::string_to_wide_string(Config::PluginFolder) + StringUtil::string_to_wide_string(dashtexfile);

  HRESULT hResult = D3D9CreateRectangleVertexTexBufferFromFile(g_pRBRIDirect3DDevice9,
    path, 0, 0, 0, 0, &m_dashtex, 0);

  if(SUCCEEDED(hResult)) {
    LogUtil::ToFile(L"Success loading Tex: " + path);
  } else {
    LogUtil::ToFile(L"Failed loading TEX: " + path);
  }
}

HRESULT RBRDashboard::CustomRBRDirectXEndScene(void* objPointer) {
  DrawDashboard();
}

void RBRDashboard::DrawDashboard(void) {
  // draw gear

  // draw recmeter

  // draw tuibometer

  // draw speedmeter

  // draw distance 1

  // draw distance 2

  // draw engine temp

  // draw warnning

  // draw alpha color
}