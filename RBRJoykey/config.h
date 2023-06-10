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

  #define MAX_INPUT_WAITTING_TIME (3000) //ms

  typedef enum {
    MENU_PLUGIN_STATE = 0,
    MENU_KEYBIND_UP = 2,
    MENU_KEYBIND_DOWN,
    MENU_KEYBIND_LEFT,
    MENU_KEYBIND_RIGHT,
    MENU_KEYBIND_ESC,
    MENU_KEYBIND_ENTER,
    MENU_KEYBIND_SPACE,
    MENU_BUTT
  } MENUITEM;

  class Setting {
  private:
    INIUtil::INIManager *m_ini;
  public:
    MEMBER_GET_SET(bool, m_pluginOn);

    MEMBER_GET_SET(std::string, m_keyUp);
    MEMBER_GET_SET(std::string, m_keyDown);
    MEMBER_GET_SET(std::string, m_keyLeft);
    MEMBER_GET_SET(std::string, m_keyRight);
    MEMBER_GET_SET(std::string, m_keyEsc);
    MEMBER_GET_SET(std::string, m_keyEnter);
    MEMBER_GET_SET(std::string, m_keySpace);

  public:
    Setting(std::string filePath) {
      m_ini = new INIUtil::INIManager(filePath);
      LoadConfig();
    }
    virtual ~Setting(void) { SAFE_DELETE(m_ini); }

    void LoadConfig(void);

    void SaveConfig(void);

    void SaveConfig(MENUITEM type, const std::string& value);
  };
}