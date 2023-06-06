#pragma once

#include <string>
#include <Windows.h>
#include "Utils/INIUtil.h"

#ifndef MEMBER_GET_SET
#define MEMBER_GET_SET(member_type, member) \
  member_type member; \
  const member_type get_##member(){return member;} \
  void set_##member(member_type value){ this->member = value;}
#endif

namespace Config {
  const std::string PluginName = "RBRJoykey";
  const std::string PluginsFolder = ".\\Plugins";
  const std::string PluginFolder = PluginsFolder + "\\" + PluginName;
  const std::string PluginConfig = PluginFolder + "\\" + "RBRJoykey.ini";

  class Setting {
  private:
    INIUtil::INIManager *m_ini;
  public:
    MEMBER_GET_SET(bool, m_pluginOn);
  public:
    Setting(std::string filePath) {
      m_ini = new INIUtil::INIManager(filePath);
      LoadConfig();
    }
    virtual ~Setting(void) { SAFE_DELETE(m_ini); }

    void LoadConfig(void);
  };
}