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

extern IPlugin* g_pRBRPlugin;

tRBRDirectXEndScene Func_OrigRBRDirectXStartScene = nullptr;
tRBRDirectXEndScene Func_OrigRBRDirectXEndScene = nullptr;

HRESULT __fastcall CustomRBRDirectXStartScene(void* objPointer) {
  // We need code at the beginning of this function that is not a call to another function.
  // If we just added "Countdown::DrawCountdown();" then hooking EndScene from another 
  // plugin will fail. This is because of some technicality of how the hooking works.

  /*
  if (g_pRBRGameMode->gameMode == 0x01) {
    return ((RBRDashboard*)g_pRBRPlugin)->CustomRBRDirectXStartScene(objPointer);
  }
  */

  return ::Func_OrigRBRDirectXStartScene(objPointer);
}

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
  m_dashtex = new IMAGE_TEXTURE();
  m_metatex = new IMAGE_TEXTURE();
  m_scalex = m_scaley = 1.0f;
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

RBRDashboard::~RBRDashboard(void) {
  LogUtil::ToFile("Destroying Plugin " + Config::PluginName + ".");
  delete Config::GIniConfig;
  delete m_setting;
  std::map<int, Config::CarSetting*>::iterator iter = m_carSettings.begin();
  while (iter++ != m_carSettings.end()) {
    delete iter->second;
  }
  m_carSettings.clear();
  delete m_dashtex;
  delete m_metatex;
}

const char* RBRDashboard::GetName(void) {
  if(Func_OrigRBRDirectXEndScene == nullptr) {
    // Do the initialization and texture creation only once because RBR may call GetName several times
    LogUtil::ToFile("Initializing the plugin");
    RBRAPI_InitializeObjReferences();

    InitDashboard();

    auto gtcDirect3DStartScene = new DetourXS((LPVOID)0x0040E880, ::CustomRBRDirectXStartScene, TRUE);
    Func_OrigRBRDirectXStartScene = (tRBRDirectXEndScene)gtcDirect3DStartScene->GetTrampoline();

    auto gtcDirect3DEndScene = new DetourXS((LPVOID)0x0040E890, ::CustomRBRDirectXEndScene, TRUE);
    Func_OrigRBRDirectXEndScene = (tRBRDirectXEndScene)gtcDirect3DEndScene->GetTrampoline();
  }

  return Config::PluginName.c_str();
}

void RBRDashboard::LoadINI(void) {
  Config::GIniConfig = new INIUtil::INIManager(Config::PluginConfig);
  m_setting = new Config::Setting(Config::GIniConfig);
  m_setting->LoadConfig();
  for (int i = 0; i < 8; i++) {
    if (Config::GIniConfig->IsSectionExist("car" + std::to_string(i))) {
      m_carSettings[i] = new Config::CarSetting(Config::GIniConfig, i);
      m_carSettings[i]->LoadConfig();
    }
  }
}

void RBRDashboard::InitDashboard(void) {
  POINT pa, pb;
  RBRAPI_MapRBRPointToScreenPoint(0, 0, &pa);
  RBRAPI_MapRBRPointToScreenPoint(640, 480, &pb);
  m_scalex = std::abs(pb.x - pa.x) / 1920.0f;
  m_scaley = std::abs(pb.y - pa.y) / 1440.0f;

  // load meta texture and dash texture for all cars.


  std::string dashtexfile = "RBRDashboard.png";

  std::wstring path = StringUtil::string_to_wide_string(Config::PluginFolder) + L"\\" + StringUtil::string_to_wide_string(dashtexfile);

  HRESULT hResult = g_pRBRIDirect3DDevice9->CreateTexture(640, 510, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_dashtex->pTexture, NULL);

  hResult = D3D9CreateRectangleVertexTexBufferFromFile(g_pRBRIDirect3DDevice9, path, 0, 0, 0, 0, m_metatex, 0);

  if(SUCCEEDED(hResult)) {
    LogUtil::ToFile(L"Success loading TEX: " + path);
  } else {
    LogUtil::ToFile(L"Failed loading TEX: " + path);
  }
}

HRESULT RBRDashboard::CustomRBRDirectXStartScene(void* objPointer) {
  HRESULT hResult = Func_OrigRBRDirectXStartScene(objPointer);

  return hResult;
}

HRESULT RBRDashboard::CustomRBRDirectXEndScene(void* objPointer) {
  HRESULT hResult;

  DrawDashboard();

  hResult = Func_OrigRBRDirectXEndScene(objPointer);
  return hResult;
}

void RBRDashboard::DrawDashboard(void) {
  IDirect3DSurface9 *drawSurface = nullptr;
  m_dashtex->pTexture->GetSurfaceLevel(0, &drawSurface);

  IDirect3DSurface9 *metaSurface = nullptr;
  m_metatex->pTexture->GetSurfaceLevel(0, &metaSurface);

  // background
  RECT src = { 0, 0, 640, 510 };
  RECT dst = { 0, 0, 640, 510 };
  g_pRBRIDirect3DDevice9->StretchRect(metaSurface, &src, drawSurface, &dst, D3DTEXF_LINEAR);

  // gear
  src = { 0, 570, 60, 670 };
  dst = { 250, 250, 310, 350 };
  g_pRBRIDirect3DDevice9->StretchRect(metaSurface, &src, drawSurface, &dst, D3DTEXF_LINEAR);

  //revmeter
  src = { 0, 505, 200, 565 };
  dst = { 60, 130, 260, 190 };
  g_pRBRIDirect3DDevice9->StretchRect(metaSurface, &src, drawSurface, &dst, D3DTEXF_LINEAR);



  metaSurface->Release();
  drawSurface->Release();

  IMAGE_TEXTURE tex = *m_dashtex;
  HRESULT hResult = D3D9CreateVertexesForTex(
    &tex, 0, 0, 800 * m_scalex, 800 * m_scaley, 0
  );
  if (SUCCEEDED(hResult)) {
    D3D9DrawVertexTex2D(g_pRBRIDirect3DDevice9, tex.pTexture, tex.vertexes2D);
  }
  else {
    LogUtil::ToFile("Failed creating vertexes.");
  }

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