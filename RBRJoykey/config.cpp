#include "config.h"

namespace Config {

void Setting::LoadConfig(void) {
    if (m_ini == nullptr)
        return;

    bool bValue = false;
    std::string strValue = "";

    set_m_pluginOn(m_ini->Get("setting", "PluginOn", bValue));
    
    set_m_keyUp(m_ini->Get("keymap", "Up", strValue));
    set_m_keyDown(m_ini->Get("keymap", "Down", strValue));
    set_m_keyLeft(m_ini->Get("keymap", "Left", strValue));
    set_m_keyRight(m_ini->Get("keymap", "Right", strValue));
    set_m_keyEsc(m_ini->Get("keymap", "Esc", strValue));
    set_m_keyEnter(m_ini->Get("keymap", "Enter", strValue));
    set_m_keySpace(m_ini->Get("keymap", "Space", strValue));
    set_m_keyLshift(m_ini->Get("keymap", "Lshift", strValue));
    set_m_keyLctrl(m_ini->Get("keymap", "Lctrl", strValue));
    set_m_keyPageUp(m_ini->Get("keymap", "PageUp", strValue));
    set_m_keyPageDown(m_ini->Get("keymap", "PageDown", strValue));
    set_m_keyNum4(m_ini->Get("keymap", "Num4", strValue));
    set_m_keyNum6(m_ini->Get("keymap", "Num6", strValue));
    set_m_keyNum8(m_ini->Get("keymap", "Num8", strValue));
    set_m_keyNum2(m_ini->Get("keymap", "Num2", strValue));
    set_m_keyNumadd(m_ini->Get("keymap", "Numadd", strValue));
    set_m_keyNumsub(m_ini->Get("keymap", "Numsub", strValue));
    set_m_keyNum0(m_ini->Get("keymap", "Num0", strValue));
    set_m_keyNumMultiply(m_ini->Get("keymap", "NumMultiply", strValue));
}

void Setting::SaveConfig(void) {
  m_ini->Save(true);
}

void Setting::SaveConfig(MENUITEM type, const std::string& value) {
  switch (type) {
  case MENU_PLUGIN_STATE:
    m_ini->Set("setting", "PluginOn", m_pluginOn ? "true" : "false");
    break;
  case MENU_KEYBIND_UP:
    m_keyUp = value;
    m_ini->Set("keymap", "Up", m_keyUp);
    break;
  case MENU_KEYBIND_DOWN:
    m_keyDown = value;
    m_ini->Set("keymap", "Down", m_keyDown);
    break;
  case MENU_KEYBIND_LEFT:
    m_keyLeft = value;
    m_ini->Set("keymap", "Left", m_keyLeft);
    break;
  case MENU_KEYBIND_RIGHT:
    m_keyRight = value;
    m_ini->Set("keymap", "Right", m_keyRight);
    break;
  case MENU_KEYBIND_ESC:
    m_keyEsc = value;
    m_ini->Set("keymap", "Esc", m_keyEsc);
    break;
  case MENU_KEYBIND_ENTER:
    m_keyEnter = value;
    m_ini->Set("keymap", "Enter", m_keyEnter);
    break;
  case MENU_KEYBIND_SPACE:
    m_keySpace = value;
    m_ini->Set("keymap", "Space", m_keySpace);
    break;
  case MENU_KEYBIND_LSHIFT:
    m_keyLshift = value;
    m_ini->Set("keymap", "Lshift", m_keyLshift);
    break;
  case MENU_KEYBIND_LCTRL:
    m_keyLctrl = value;
    m_ini->Set("keymap", "Lctrl", m_keyLctrl);
    break;
  case MENU_KEYBIND_PAGEUP:
    m_keyPageUp = value;
    m_ini->Set("keymap", "PageUp", m_keyPageUp);
    break;
  case MENU_KEYBIND_PAGEDOWN:
    m_keyPageDown = value;
    m_ini->Set("keymap", "PageDown", m_keyPageDown);
    break;
  case MENU_KEYBIND_NUM4:
    m_keyNum4 = value;
    m_ini->Set("keymap", "Num4", m_keyNum4);
    break;
  case MENU_KEYBIND_NUM6:
    m_keyNum6 = value;
    m_ini->Set("keymap", "Num6", m_keyNum6);
    break;
  case MENU_KEYBIND_NUM8:
    m_keyNum8 = value;
    m_ini->Set("keymap", "Num8", m_keyNum8);
    break;
  case MENU_KEYBIND_NUM2:
    m_keyNum2 = value;
    m_ini->Set("keymap", "Num2", m_keyNum2);
    break;
  case MENU_KEYBIND_NUMADD:
    m_keyNumadd = value;
    m_ini->Set("keymap", "Numadd", m_keyNumadd);
    break;
  case MENU_KEYBIND_NUMSUB:
    m_keyNumsub = value;
    m_ini->Set("keymap", "Numsub", m_keyNumsub);
    break;
  case MENU_KEYBIND_NUM0:
    m_keyNum0 = value;
    m_ini->Set("keymap", "Num0", m_keyNum0);
    break;
  case MENU_KEYBIND_MULTIPLY:
    m_keyNumMultiply = value;
    m_ini->Set("keymap", "NumMultiply", m_keyNumMultiply);
    break;
  default:
    break;
  }
}

}