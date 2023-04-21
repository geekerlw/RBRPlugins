#pragma once

#include <string>
#include <Windows.h>
#include "Utils/INIUtil.h"

#ifndef MEMBER_GET_SET
#define MEMBER_GET_SET(member_type, member) \
  member_type member; \
  member_type get_##member(){return member;} \
  void set_##member(member_type value){ this->member = value;}
#endif

namespace Config {
  const std::string PluginName = "RBRDashboard";
  const std::string PluginsFolder = ".\\Plugins";
  const std::string PluginFolder = PluginsFolder + "\\" + PluginName;
  const std::string PluginConfig = PluginFolder + "\\" + "RBRDashboard.ini";

  const int MAX_GEAR_NUM = 8;
  const int MAX_RPM_NUM = 18;
  INIUtil::INIManager *GIniConfig = nullptr;

  class Setting {
  private:
    INIUtil::INIManager *m_ini;
  public:
    MEMBER_GET_SET(bool, m_showInVr);
  public:
    Setting(INIUtil::INIManager *ini) : m_ini(ini) { }
    virtual ~Setting(void) { m_ini = nullptr; }

    void LoadConfig(void);
  };

  class CarSetting {
  private:
    INIUtil::INIManager *m_ini;
    int m_carid;

  public:
    MEMBER_GET_SET(std::string, m_textureFile);
    MEMBER_GET_SET(RECT, m_hubPos);
    MEMBER_GET_SET(float, m_scalex);
    MEMBER_GET_SET(float, m_scaley);

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
    MEMBER_GET_SET(RECT, m_timeTextArea);
    MEMBER_GET_SET(float, m_timeTextSplitWidth);
    MEMBER_GET_SET(RECT, m_timeTextPos);

    // speed
    MEMBER_GET_SET(bool, m_speedShow);
    MEMBER_GET_SET(RECT, m_speedTextArea);
    MEMBER_GET_SET(float, m_speedTextSplitWidth);
    MEMBER_GET_SET(RECT, m_speedTextPos);

    // distance
    MEMBER_GET_SET(bool, m_distanceShow);
    MEMBER_GET_SET(RECT, m_distanceTextArea);
    MEMBER_GET_SET(float, m_distanceTextSplitWidth);
    MEMBER_GET_SET(RECT, m_distancePos0);
    MEMBER_GET_SET(RECT, m_distancePos1);

    // engine temp
    MEMBER_GET_SET(bool, m_engineTempShow);
    MEMBER_GET_SET(RECT, m_engineTempTextArea);
    MEMBER_GET_SET(float, m_engineTempTextSplitWidth);
    MEMBER_GET_SET(RECT, m_engineTempPos);

    // warning
    MEMBER_GET_SET(bool, m_warningIconShow);
    MEMBER_GET_SET(RECT, m_warningIconArea);
    MEMBER_GET_SET(float, m_warningIconSplitWidth);
    MEMBER_GET_SET(RECT, m_warningIconPos);

  public:
    CarSetting(INIUtil::INIManager *ini, int carid) : m_ini(ini), m_carid(carid) { 
      m_gearSrc = new RECT[MAX_GEAR_NUM]();
      m_gearDst = new RECT[MAX_GEAR_NUM]();
      m_rpmSrc = new RECT[MAX_RPM_NUM]();
      m_rpmDst = new RECT[MAX_RPM_NUM]();
    }
    virtual ~CarSetting(void) {
      m_ini = nullptr;
      delete []m_gearSrc;
      delete []m_gearDst;
      delete []m_rpmSrc;
      delete []m_rpmDst;
    }

    void LoadConfig(void);
  };
}