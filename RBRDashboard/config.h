#pragma once

#include <string>
#include <Windows.h>
#include "Utils/INIUtil.h"
#include "RBR/D3D9Helpers.h"
#include "RBR/D3D9Font/D3DFont.h"
#include <d3d11.h>
#include "SpriteFont.h"
#include "SpriteBatch.h"

using namespace DirectX;

#ifndef MEMBER_GET_SET
#define MEMBER_GET_SET(member_type, member) \
  member_type member; \
  const member_type get_##member(){return member;} \
  void set_##member(member_type value){ this->member = value;}
#endif

namespace Config {
  const std::string PluginName = "RBRDashboard";
  const std::string PluginsFolder = ".\\Plugins";
  const std::string PluginFolder = PluginsFolder + "\\" + PluginName;
  const std::string PluginConfig = PluginFolder + "\\" + "RBRDashboard.ini";

  const int MAX_GEAR_NUM = 8;
  const int MAX_RPM_NUM = 18;

  class Setting {
  private:
    INIUtil::INIManager *m_ini;
  public:
    MEMBER_GET_SET(bool, m_pluginOn);
    MEMBER_GET_SET(bool, m_showIn2D);
    MEMBER_GET_SET(bool, m_showInVr);
  public:
    Setting(std::string filePath) {
      m_ini = new INIUtil::INIManager(filePath);
      LoadConfig();
    }
    virtual ~Setting(void) { SAFE_DELETE(m_ini); }

    void LoadConfig(void);

    bool IsCarConfigExist(const int carId);

    std::string GetCarConfigFolder(const int carId);
  };

  class CarSetting {
  private:
    INIUtil::INIManager *m_ini;

  public:
    int m_carid;
    
    // 2d game overlay needed
    PIMAGE_TEXTURE m_metatex;
    PIMAGE_TEXTURE m_dashtex;
    CD3DFont* m_timeFont;
    CD3DFont* m_speedFont;
    CD3DFont* m_distanceFont;
    CD3DFont* m_engineFont;

    // vr game overlay needed
    ID3D11Texture2D *m_pD3D11TextureMeta, * m_pD3D11TextureDash;
    ID3D11ShaderResourceView *m_pD3D11ShaderResourceView;
    DX11::SpriteBatch *m_spriteBatch;
    DX11::SpriteFont *m_timeSpriteFont;
    DX11::SpriteFont *m_speedSpriteFont;
    DX11::SpriteFont *m_distanceSpriteFont;
    DX11::SpriteFont *m_engineSpriteFont;

  public:
    // display
    MEMBER_GET_SET(POINT, m_hudPos);
    MEMBER_GET_SET(POINT, m_hudSize);
    MEMBER_GET_SET(float, m_scale);

    // vr position
    MEMBER_GET_SET(float, m_vrPositionX);
    MEMBER_GET_SET(float, m_vrPositionY);
    MEMBER_GET_SET(float, m_vrPositionZ);
    MEMBER_GET_SET(float, m_vrRotationX);
    MEMBER_GET_SET(float, m_vrRotationY);
    MEMBER_GET_SET(float, m_vrRotationZ);
    MEMBER_GET_SET(float, m_vrScale);

    // background
    MEMBER_GET_SET(bool, m_backgroudShow);
    MEMBER_GET_SET(RECT, m_backgroundSrc);
    MEMBER_GET_SET(RECT, m_backgroundDst);

    // gear
    MEMBER_GET_SET(bool, m_gearShow);
    MEMBER_GET_SET(RECT*, m_gearSrc);
    MEMBER_GET_SET(RECT*, m_gearDst);

    // recmeter
    MEMBER_GET_SET(bool, m_rpmShow);
    MEMBER_GET_SET(RECT*, m_rpmSrc);
    MEMBER_GET_SET(RECT*, m_rpmDst);

    // time
    MEMBER_GET_SET(bool, m_timeShow);
    MEMBER_GET_SET(int, m_timeTextFontSize);
    MEMBER_GET_SET(D3DCOLOR, m_timeTextFontColor);
    MEMBER_GET_SET(POINT, m_timeTextPos);

    // speed
    MEMBER_GET_SET(bool, m_speedShow);
    MEMBER_GET_SET(int, m_speedTextFontSize);
    MEMBER_GET_SET(D3DCOLOR, m_speedTextFontColor);
    MEMBER_GET_SET(POINT, m_speedTextPos);

    // distance
    MEMBER_GET_SET(bool, m_distanceShow);
    MEMBER_GET_SET(int, m_distanceTextFontSize);
    MEMBER_GET_SET(D3DCOLOR, m_distanceTextFontColor);
    MEMBER_GET_SET(POINT, m_distancePos0);
    MEMBER_GET_SET(POINT, m_distancePos1);

    // engine temp
    MEMBER_GET_SET(bool, m_engineTempShow);
    MEMBER_GET_SET(int, m_engineTempTextFontSize);
    MEMBER_GET_SET(D3DCOLOR, m_engineTempTextFontColor);
    MEMBER_GET_SET(POINT, m_engineTempPos);

  public:
    CarSetting(int carid, std::string filePath) : m_carid(carid) {
      m_ini = new INIUtil::INIManager(filePath);
      m_metatex = new IMAGE_TEXTURE();
      m_dashtex = new IMAGE_TEXTURE();
      m_timeFont = m_speedFont = m_distanceFont = m_engineFont = nullptr;
      m_gearSrc = new RECT[MAX_GEAR_NUM]();
      m_gearDst = new RECT[MAX_GEAR_NUM]();
      m_rpmSrc = new RECT[MAX_RPM_NUM]();
      m_rpmDst = new RECT[MAX_RPM_NUM]();
      m_pD3D11TextureMeta = nullptr;
      m_pD3D11TextureDash = nullptr;
      m_pD3D11ShaderResourceView = nullptr;
      LoadConfig();
    }
    virtual ~CarSetting(void) {
      SAFE_DELETE(m_ini);
      SAFE_RELEASE(m_metatex->pTexture);
      SAFE_RELEASE(m_dashtex->pTexture);
      SAFE_RELEASE(m_pD3D11ShaderResourceView);
      SAFE_RELEASE(m_pD3D11TextureMeta);
      SAFE_RELEASE(m_pD3D11TextureDash);
      SAFE_DELETE(m_metatex);
      SAFE_DELETE(m_dashtex);
      SAFE_DELETE(m_timeFont);
      SAFE_DELETE(m_speedFont);
      SAFE_DELETE(m_distanceFont);
      SAFE_DELETE(m_engineFont);
      SAFE_DELETE(m_spriteBatch);
      SAFE_DELETE(m_timeSpriteFont);
      SAFE_DELETE(m_speedSpriteFont);
      SAFE_DELETE(m_distanceFont);
      SAFE_DELETE(m_engineFont);
      delete []m_gearSrc;
      delete []m_gearDst;
      delete []m_rpmSrc;
      delete []m_rpmDst;
    }

    void LoadConfig(void);
  };
}