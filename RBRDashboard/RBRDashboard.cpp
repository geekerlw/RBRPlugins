// RBRDashboard.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "config.h"
#include "RBR/D3D9Helpers.h"
#include "Utils/LogUtil.h"
#include "Utils/StringUtil.h"
#include "Utils/INIUtil.h"
#include "Utils/D3DUtil.h"
#include "Lib/Detourxs/detourxs.h"
#include "RBRDashboard.h"
#include <d3d11.h>
#include "WICTextureLoader.h"
#include "SpriteFont.h"
#include "SpriteBatch.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
#pragma comment(lib, "d3d11.lib")

extern IPlugin* g_pRBRPlugin;

tRBRDirectXEndScene Func_OrigRBRDirectXStartScene = nullptr;
tRBRDirectXEndScene Func_OrigRBRDirectXEndScene = nullptr;

HRESULT __fastcall CustomRBRDirectXStartScene(void* objPointer) {
  // We need code at the beginning of this function that is not a call to another function.
  // If we just added "Countdown::DrawCountdown();" then hooking EndScene from another 
  // plugin will fail. This is because of some technicality of how the hooking works.

  if (g_pRBRGameMode->gameMode == 0x0A) {
    return ((RBRDashboard*)g_pRBRPlugin)->CustomRBRDirectXStartSceneBeforeStart(objPointer);
  }

  return ::Func_OrigRBRDirectXStartScene(objPointer);
}

HRESULT __fastcall CustomRBRDirectXEndScene(void* objPointer) {
  // We need code at the beginning of this function that is not a call to another function.
  // If we just added "Countdown::DrawCountdown();" then hooking EndScene from another 
  // plugin will fail. This is because of some technicality of how the hooking works.

  if(g_pRBRGameMode->gameMode == 0x01) {
    return ((RBRDashboard *)g_pRBRPlugin)->CustomRBRDirectXEndSceneDriving(objPointer);
  }
  if (g_pRBRGameMode->gameMode == 0x0C) {
    return ((RBRDashboard*)g_pRBRPlugin)->CustomRBRDirectXEndSceneBackMenu(objPointer);
  }

  return ::Func_OrigRBRDirectXEndScene(objPointer);
}

RBRDashboard::RBRDashboard(IRBRGame* pGame) : m_pGame(pGame) {
  m_curCarSetting = nullptr;
  m_setting = nullptr;
  m_scalex = m_scaley = 1.0f;
  m_tickTime = 0;
  m_pID3D11Device = nullptr;
  m_pID3D11DeviceContext = nullptr;
  m_Vr = new RBRVRDash();

  m_metatex = new IMAGE_TEXTURE();
  m_dashtex = new IMAGE_TEXTURE();
  m_timeFont = m_speedFont = m_distanceFont = m_engineFont = nullptr;

  m_pD3D11TextureMeta = nullptr;
  m_pD3D11TextureDash = nullptr;
  m_pD3D11ShaderResourceView = nullptr;
  m_spriteBatch = nullptr;
  m_timeSpriteFont = m_speedSpriteFont = m_distanceSpriteFont = m_engineSpriteFont = nullptr;

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

  if (m_setting->get_m_pluginOn() && IsHMDAvailable()) {
    m_Vr->Shutdown();
  }
  
  DisableDashboard();
  SAFE_DELETE(m_metatex);
  SAFE_DELETE(m_dashtex);

  DisableVROverlay();
  SAFE_RELEASE(m_pID3D11DeviceContext);
  SAFE_RELEASE(m_pID3D11Device);

  SAFE_DELETE(m_Vr);
  SAFE_DELETE(m_setting);
  std::map<int, Config::CarSetting*>::iterator iter = m_carSettings.begin();
  while (iter != m_carSettings.end()) {
    SAFE_DELETE(iter->second);
    iter++;
  }
  m_carSettings.clear();
}

const char* RBRDashboard::GetName(void) {
  if(Func_OrigRBRDirectXEndScene == nullptr) {
    // Do the initialization and texture creation only once because RBR may call GetName several times
    LogUtil::ToFile("Initializing the plugin");

    if (!m_setting->get_m_pluginOn()) {
      return Config::PluginName.c_str(); // plugin func not enabled, just return.
    }

    RBRAPI_InitializeObjReferences();

    if (IsHMDAvailable()) {
      InitVrSystem();
    }
    else {
      InitDashboard();
    }

    auto gtcDirect3DStartScene = new DetourXS((LPVOID)0x0040E880, ::CustomRBRDirectXStartScene, TRUE);
    Func_OrigRBRDirectXStartScene = (tRBRDirectXEndScene)gtcDirect3DStartScene->GetTrampoline();

    auto gtcDirect3DEndScene = new DetourXS((LPVOID)0x0040E890, ::CustomRBRDirectXEndScene, TRUE);
    Func_OrigRBRDirectXEndScene = (tRBRDirectXEndScene)gtcDirect3DEndScene->GetTrampoline();
  }

  return Config::PluginName.c_str();
}

void RBRDashboard::LoadINI(void) {
  m_setting = new Config::Setting(Config::PluginConfig);
  for (int i = 0; i < 8; i++) {
    if (m_setting->IsCarConfigExist(i)) {
      std::string configFile = Config::PluginFolder + "\\" + m_setting->GetCarConfigFolder(i) + "\\config.ini";
      Config::CarSetting *carSetting = new Config::CarSetting(i, configFile);
      m_carSettings[i] = carSetting;
    }
  }
}

void RBRDashboard::InitDashboard(void) {
  POINT pa, pb;
  RBRAPI_MapRBRPointToScreenPoint(0, 0, &pa);
  RBRAPI_MapRBRPointToScreenPoint(640, 480, &pb);
  m_scalex = std::abs(pb.x - pa.x) / 1920.0f;
  m_scaley = std::abs(pb.y - pa.y) / 1440.0f;
}

void RBRDashboard::InitVrSystem(void) {
  HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
    D3D11_SDK_VERSION, &m_pID3D11Device, nullptr, &m_pID3D11DeviceContext);
  if (FAILED(hr)) {
    LogUtil::ToFile("Failed create D3D11 Device and Context");
    return;
  }

  if (m_Vr->Init()) {
    LogUtil::ToFile("Success initialize VR System.");
  }
}

void RBRDashboard::EnableDashboard(Config::CarSetting* car) {
  std::wstring configFolder = StringUtil::string_to_wide_string(Config::PluginFolder) + L"\\" + StringUtil::string_to_wide_string(m_setting->GetCarConfigFolder(car->m_carid));
  std::wstring textureFile = configFolder + L"\\digidash.png";

  HRESULT hResultDash = g_pRBRIDirect3DDevice9->CreateTexture(car->get_m_hudSize().x, car->get_m_hudSize().y,
    1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_dashtex->pTexture, NULL);
  HRESULT hResultMeta = D3D9CreateRectangleVertexTexBufferFromFile(g_pRBRIDirect3DDevice9, textureFile, 0, 0, 0, 0, m_metatex, 0);
  if (SUCCEEDED(hResultDash) && SUCCEEDED(hResultMeta)) {
    LogUtil::ToFile(L"Success loading TEX: " + textureFile);
  }
  else {
    LogUtil::ToFile(L"Failed loading TEX: " + textureFile);
    return;
  }

  // load fonts
  m_timeFont = new CD3DFont(L"Trebuchet MS", car->get_m_timeTextFontSize(), 0 /*D3DFONT_BOLD*/);
  m_timeFont->InitDeviceObjects(g_pRBRIDirect3DDevice9);
  m_timeFont->RestoreDeviceObjects();

  m_speedFont = new CD3DFont(L"Trebuchet MS", car->get_m_speedTextFontSize(), 0 /*D3DFONT_BOLD*/);
  m_speedFont->InitDeviceObjects(g_pRBRIDirect3DDevice9);
  m_speedFont->RestoreDeviceObjects();

  m_distanceFont = new CD3DFont(L"Trebuchet MS", car->get_m_distanceTextFontSize(), 0 /*D3DFONT_BOLD*/);
  m_distanceFont->InitDeviceObjects(g_pRBRIDirect3DDevice9);
  m_distanceFont->RestoreDeviceObjects();

  m_engineFont = new CD3DFont(L"Trebuchet MS", car->get_m_engineTempTextFontSize(), 0 /*D3DFONT_BOLD*/);
  m_engineFont->InitDeviceObjects(g_pRBRIDirect3DDevice9);
  m_engineFont->RestoreDeviceObjects();
}

void RBRDashboard::DisableDashboard(void) {
  SAFE_DELETE(m_timeFont);
  SAFE_DELETE(m_speedFont);
  SAFE_DELETE(m_distanceFont);
  SAFE_DELETE(m_engineFont);
  SAFE_RELEASE(m_metatex->pTexture);
  SAFE_RELEASE(m_dashtex->pTexture);
}

void RBRDashboard::EnableVROverlay(Config::CarSetting *car) {
  std::wstring configFolder = StringUtil::string_to_wide_string(Config::PluginFolder) + L"\\" + StringUtil::string_to_wide_string(m_setting->GetCarConfigFolder(car->m_carid));
  std::wstring textureFile = configFolder + L"\\digidash.png";

  D3D11_TEXTURE2D_DESC desc;
  ZeroMemory(&desc, sizeof(desc));
  desc.Width = (UINT)car->get_m_hudSize().x;
  desc.Height = (UINT)car->get_m_hudSize().y;
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  desc.SampleDesc.Count = 1;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
  desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
  HRESULT hResultDash = m_pID3D11Device->CreateTexture2D(&desc, nullptr, &m_pD3D11TextureDash);
  HRESULT hResultMeta = CreateWICTextureFromFile(m_pID3D11Device, m_pID3D11DeviceContext, textureFile.data(), reinterpret_cast<ID3D11Resource**>(&m_pD3D11TextureMeta), &m_pD3D11ShaderResourceView, 0);
  if (SUCCEEDED(hResultDash) && SUCCEEDED(hResultMeta)) {
    LogUtil::ToFile(L"Success loading VR TEX: " + textureFile);
  }
  else {
    LogUtil::ToFile(L"Failed loading VR TEX: " + textureFile);
    return;
  }

  // load fonts
  m_spriteBatch = new DX11::SpriteBatch(m_pID3D11DeviceContext);
  m_timeSpriteFont = new DX11::SpriteFont(m_pID3D11Device, (configFolder + L"\\time.spritefont").c_str());
  m_speedSpriteFont = new DX11::SpriteFont(m_pID3D11Device, (configFolder + L"\\speed.spritefont").c_str());
  m_distanceSpriteFont = new DX11::SpriteFont(m_pID3D11Device, (configFolder + L"\\distance.spritefont").c_str());
  m_engineSpriteFont = new DX11::SpriteFont(m_pID3D11Device, (configFolder + L"\\engine.spritefont").c_str());

  // Create the render target view
  ID3D11RenderTargetView* renderTargetView;
  D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
  renderTargetViewDesc.Format = desc.Format;
  renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
  HRESULT hr = m_pID3D11Device->CreateRenderTargetView(m_pD3D11TextureDash, &renderTargetViewDesc, &renderTargetView);
  if (FAILED(hr)) {
    LogUtil::ToFile(L"Failed to create render target");
    return;
  }

  // Define the viewport dimensions
  D3D11_VIEWPORT viewport;
  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.Width = (float)car->get_m_hudSize().x;
  viewport.Height = (float)car->get_m_hudSize().y;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;

  // Set the viewport on the device context
  m_pID3D11DeviceContext->RSSetViewports(1, &viewport);

  // Set the render target
  m_pID3D11DeviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);

  m_spriteBatch->SetViewport(viewport);

  // configure vr pose
  Matrix matrix = Matrix::CreateTranslation(0, 0, 0);
  matrix *= Matrix::CreateRotationX(car->get_m_vrRotationX());
  matrix *= Matrix::CreateRotationY(car->get_m_vrRotationY());
  matrix *= Matrix::CreateRotationZ(car->get_m_vrRotationZ());
  matrix *= Matrix::CreateScale(car->get_m_vrScale());
  matrix *= Matrix::CreateTranslation(car->get_m_vrPositionX(), car->get_m_vrPositionY(), car->get_m_vrPositionZ());
  matrix = matrix.Transpose();

  vr::HmdMatrix34_t transform = MatrixToHmdMatrix34(matrix);
  m_Vr->UpdatePose(&transform);
  m_Vr->ShowOverlay();
}

void RBRDashboard::DisableVROverlay(void) {
  if (m_Vr) {
    m_Vr->HideOverlay();
  }

  SAFE_DELETE(m_timeSpriteFont);
  SAFE_DELETE(m_speedSpriteFont);
  SAFE_DELETE(m_distanceSpriteFont);
  SAFE_DELETE(m_engineSpriteFont);
  SAFE_DELETE(m_spriteBatch);
  SAFE_RELEASE(m_pD3D11ShaderResourceView);
  SAFE_RELEASE(m_pD3D11TextureMeta);
  SAFE_RELEASE(m_pD3D11TextureDash);
}

bool RBRDashboard::IsHMDAvailable()
{
  return vr::VRSystem() != NULL;
}

vr::HmdMatrix34_t RBRDashboard::MatrixToHmdMatrix34(Matrix& m)
{
  vr::HmdMatrix34_t pose;
  pose.m[0][0] = m._11;
  pose.m[0][1] = m._12;
  pose.m[0][2] = m._13;
  pose.m[0][3] = m._14;
  pose.m[1][0] = m._21;
  pose.m[1][1] = m._22;
  pose.m[1][2] = m._23;
  pose.m[1][3] = m._24;
  pose.m[2][0] = m._31;
  pose.m[2][1] = m._32;
  pose.m[2][2] = m._33;
  pose.m[2][3] = m._34;

  return pose;
}

HRESULT RBRDashboard::CustomRBRDirectXStartSceneBeforeStart(void* objPointer) {
  HRESULT hResult = Func_OrigRBRDirectXStartScene(objPointer);

  // loading car setting when stage count down, only once.
  if (m_curCarSetting == nullptr) {
    std::map<int, Config::CarSetting*>::const_iterator iter;
    if ((iter = m_carSettings.find(g_pRBRGameModeExt->carID)) != m_carSettings.end()) {
      m_curCarSetting = iter->second;
      IsHMDAvailable() ? EnableVROverlay(m_curCarSetting) : EnableDashboard(m_curCarSetting);
    }
  }
  else {
    if (m_curCarSetting->m_carid != g_pRBRGameModeExt->carID) {
      IsHMDAvailable() ? DisableVROverlay() : DisableDashboard();
      m_curCarSetting = nullptr; // carid not match, retry next tick.
    }
  }

  return hResult;
}

HRESULT RBRDashboard::CustomRBRDirectXEndSceneDriving(void* objPointer) {
  HRESULT hResult;

  if (IsHMDAvailable()) {
    m_Vr->HandleVrEvent();
    DrawVROverlay();
  } else {
    DrawDashboard();
  }

  hResult = Func_OrigRBRDirectXEndScene(objPointer);
  return hResult;
}

HRESULT RBRDashboard::CustomRBRDirectXEndSceneBackMenu(void* objPointer) {
  HRESULT hResult;

  if (m_curCarSetting) {
    IsHMDAvailable() ? DisableVROverlay() : DisableDashboard();
    m_curCarSetting = nullptr; // carid not match, retry next tick.
  }

  hResult = Func_OrigRBRDirectXEndScene(objPointer);
  return hResult;
}

void RBRDashboard::DrawDashboard(void) {
  if (m_curCarSetting == nullptr)
    return;

  RECT src = { 0, 0, 0, 0 };
  RECT dst = { 0, 0, 0, 0 };
  IDirect3DSurface9 *drawSurface = nullptr;
  m_dashtex->pTexture->GetSurfaceLevel(0, &drawSurface);

  IDirect3DSurface9* originRenterTarget = nullptr;
  g_pRBRIDirect3DDevice9->GetRenderTarget(0, &originRenterTarget);

  if ((m_tickTime++ % m_setting->get_m_frameRatio()) == 0) {
    IDirect3DSurface9* metaSurface = nullptr;
    m_metatex->pTexture->GetSurfaceLevel(0, &metaSurface);

    g_pRBRIDirect3DDevice9->SetRenderTarget(0, drawSurface);

    // background
    if (m_curCarSetting->get_m_backgroudShow()) {
      src = m_curCarSetting->get_m_backgroundSrc();
      dst = m_curCarSetting->get_m_backgroundDst();
      g_pRBRIDirect3DDevice9->StretchRect(metaSurface, &src, drawSurface, &dst, D3DTEXF_LINEAR);
    }

    // gear
    if (m_curCarSetting->get_m_gearShow()) {
      int gear = g_pRBRCarInfo->gear;
      g_pRBRIDirect3DDevice9->StretchRect(metaSurface, m_curCarSetting->get_m_gearSrc() + gear, drawSurface, m_curCarSetting->get_m_gearDst() + gear, D3DTEXF_LINEAR);
    }

    // revmeter
    if (m_curCarSetting->get_m_rpmShow()) {
      for (int i = 0; i < Config::MAX_RPM_NUM && i < std::round(g_pRBRCarInfo->rpm / 500); i++) {
        g_pRBRIDirect3DDevice9->StretchRect(metaSurface, m_curCarSetting->get_m_rpmSrc() + i, drawSurface, m_curCarSetting->get_m_rpmDst() + i, D3DTEXF_LINEAR);
      }
    }

    IMAGE_TEXTURE tex = *m_dashtex;
    HRESULT hResult = D3D9CreateVertexesForTex(&tex, 0, 0, (float)m_curCarSetting->get_m_hudSize().x, (float)m_curCarSetting->get_m_hudSize().y, 0);

    if (SUCCEEDED(hResult)) {
      D3D9DrawVertexTex2D(g_pRBRIDirect3DDevice9, tex.pTexture, tex.vertexes2D);
    }
    else {
      LogUtil::ToFile("Failed creating vertexes.");
      return;
    }

    if (m_curCarSetting->get_m_timeShow()) {
      std::string timestr = GetSecondsAsMISSMS(g_pRBRCarInfo->raceTime);
      m_timeFont->DrawText(m_curCarSetting->get_m_timeTextPos().x, m_curCarSetting->get_m_timeTextPos().y, m_curCarSetting->get_m_timeTextFontColor(), timestr.c_str(), D3DFONT_CLEARTARGET);
    }

    if (m_curCarSetting->get_m_speedShow()) {
      char speedstr[16];
      snprintf(speedstr, sizeof(speedstr), "%0.2f KM/h", g_pRBRCarInfo->speed);
      m_speedFont->DrawText(m_curCarSetting->get_m_speedTextPos().x, m_curCarSetting->get_m_speedTextPos().y, m_curCarSetting->get_m_speedTextFontColor(), speedstr, D3DFONT_CLEARTARGET);
    }

    if (m_curCarSetting->get_m_distanceShow()) {
      char distancestr[16];
      snprintf(distancestr, sizeof(distancestr), "%0.2f KM", g_pRBRCarInfo->distanceFromStartControl);
      m_distanceFont->DrawText(m_curCarSetting->get_m_distancePos0().x, m_curCarSetting->get_m_distancePos0().y, m_curCarSetting->get_m_distanceTextFontColor(), distancestr, D3DFONT_CLEARTARGET);
      snprintf(distancestr, sizeof(distancestr), "%0.2f KM", g_pRBRCarInfo->distanceToFinish);
      m_distanceFont->DrawText(m_curCarSetting->get_m_distancePos1().x, m_curCarSetting->get_m_distancePos1().y, m_curCarSetting->get_m_distanceTextFontColor(), distancestr, D3DFONT_CLEARTARGET);
    }

    if (m_curCarSetting->get_m_engineTempShow()) {
      char engineTempstr[16];
      snprintf(engineTempstr, sizeof(engineTempstr), "%02d", (int)g_pRBRCarInfo->temp);
      m_engineFont->DrawText(m_curCarSetting->get_m_engineTempPos().x, m_curCarSetting->get_m_engineTempPos().y, m_curCarSetting->get_m_engineTempTextFontColor(), engineTempstr, D3DFONT_CLEARTARGET);
    }

    g_pRBRIDirect3DDevice9->SetRenderTarget(0, originRenterTarget);
    metaSurface->Release();
  }

  dst = { m_curCarSetting->get_m_hudPos().x, m_curCarSetting->get_m_hudPos().y,
    (long)(m_curCarSetting->get_m_hudSize().x * m_curCarSetting->get_m_scale() * m_scalex) + m_curCarSetting->get_m_hudPos().x,
    (long)(m_curCarSetting->get_m_hudSize().y * m_curCarSetting->get_m_scale() * m_scaley) + m_curCarSetting->get_m_hudPos().y };
  
  g_pRBRIDirect3DDevice9->StretchRect(drawSurface, NULL, originRenterTarget, &dst, D3DTEXF_LINEAR);

  originRenterTarget->Release();
  drawSurface->Release();
}

void RBRDashboard::DrawVROverlay(void) {
  if (m_curCarSetting == nullptr || !IsHMDAvailable())
    return;

  if ((m_tickTime++ % m_setting->get_m_frameRatio()) == 0) {
    ID3D11Texture2D *metaTexture = m_pD3D11TextureMeta;
    ID3D11Texture2D *dashTexture = m_pD3D11TextureDash;
    D3D11_BOX src, dst;
    ZeroMemory(&src, sizeof(D3D11_BOX));
    ZeroMemory(&dst, sizeof(D3D11_BOX));

    // background
    if (m_curCarSetting->get_m_backgroudShow()) {
      src = D3DUtil::D3D9ToD3D11Rect(m_curCarSetting->get_m_backgroundSrc());
      dst = D3DUtil::D3D9ToD3D11Rect(m_curCarSetting->get_m_backgroundDst());
      m_pID3D11DeviceContext->CopySubresourceRegion(dashTexture, 0, dst.left, dst.top, 0, metaTexture, 0, &src);
    }

    // gear
    if (m_curCarSetting->get_m_gearShow()) {
      int gear = g_pRBRCarInfo->gear;
      src = D3DUtil::D3D9ToD3D11Rect(m_curCarSetting->get_m_gearSrc() + gear);
      dst = D3DUtil::D3D9ToD3D11Rect(m_curCarSetting->get_m_gearDst() + gear);
      m_pID3D11DeviceContext->CopySubresourceRegion(dashTexture, 0, dst.left, dst.top, 0, metaTexture, 0, &src);
    }

    // revmeter
    if (m_curCarSetting->get_m_rpmShow()) {
      for (int i = 0; i < Config::MAX_RPM_NUM && i < std::round(g_pRBRCarInfo->rpm / 500); i++) {
        src = D3DUtil::D3D9ToD3D11Rect(m_curCarSetting->get_m_rpmSrc() + i);
        dst = D3DUtil::D3D9ToD3D11Rect(m_curCarSetting->get_m_rpmDst() + i);
        m_pID3D11DeviceContext->CopySubresourceRegion(dashTexture, 0, dst.left, dst.top, 0, metaTexture, 0, &src);
      }
    }

    m_spriteBatch->Begin();

    if (m_curCarSetting->get_m_timeShow()) {
      std::string timestr = GetSecondsAsMISSMS(g_pRBRCarInfo->raceTime);
      XMFLOAT2 textpos((float)m_curCarSetting->get_m_timeTextPos().x, (float)m_curCarSetting->get_m_timeTextPos().y);
      m_timeSpriteFont->DrawString(m_spriteBatch, timestr.c_str(), textpos, DirectX::Colors::White);
    }

    if (m_curCarSetting->get_m_speedShow()) {
      char speedstr[16];
      snprintf(speedstr, sizeof(speedstr), "%0.2f KM/h", g_pRBRCarInfo->speed);
      XMFLOAT2 textpos((float)m_curCarSetting->get_m_speedTextPos().x, (float)m_curCarSetting->get_m_speedTextPos().y);
      m_timeSpriteFont->DrawString(m_spriteBatch, speedstr, textpos, DirectX::Colors::White);
    }

    if (m_curCarSetting->get_m_distanceShow()) {
      char distancestr[16];
      snprintf(distancestr, sizeof(distancestr), "%0.2f KM", g_pRBRCarInfo->distanceFromStartControl);
      XMFLOAT2 textpos((float)m_curCarSetting->get_m_distancePos0().x, (float)m_curCarSetting->get_m_distancePos0().y);
      m_timeSpriteFont->DrawString(m_spriteBatch, distancestr, textpos, DirectX::Colors::White);
      snprintf(distancestr, sizeof(distancestr), "%0.2f KM", g_pRBRCarInfo->distanceToFinish);
      textpos = XMFLOAT2((float)m_curCarSetting->get_m_distancePos1().x, (float)m_curCarSetting->get_m_distancePos1().y);
      m_timeSpriteFont->DrawString(m_spriteBatch, distancestr, textpos, DirectX::Colors::White);
    }

    if (m_curCarSetting->get_m_engineTempShow()) {
      char engineTempstr[16];
      snprintf(engineTempstr, sizeof(engineTempstr), "%02d", (int)g_pRBRCarInfo->temp);
      XMFLOAT2 textpos((float)m_curCarSetting->get_m_engineTempPos().x, (float)m_curCarSetting->get_m_engineTempPos().y);
      m_timeSpriteFont->DrawString(m_spriteBatch, engineTempstr, textpos, DirectX::Colors::White);
    }

    m_spriteBatch->End();
  }

  m_Vr->SubmitOverlay(m_pD3D11TextureDash);
}