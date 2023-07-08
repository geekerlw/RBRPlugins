// RBRJoykey.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "config.h"
#include "Utils/LogUtil.h"
#include "Utils/StringUtil.h"
#include "Utils/INIUtil.h"
#include "Lib/Detourxs/detourxs.h"
#include "RBRJoykey.h"
#include "SDL.h"

extern IPlugin* g_pRBRPlugin;

typedef struct {
  const char keyname[32];
  const std::string (Config::Setting::*get_func)(void);
  void (Config::Setting::*set_func)(std::string);
  WORD simkey;
} MenuAction_t;

static const MenuAction_t g_menuActions[Config::MENU_BUTT] = {
  {"Plugin Status", nullptr, nullptr, VK_NONAME}, // do not use.
  {"Keys Group", nullptr, nullptr, VK_NONAME}, // do not use.
  {"Up", &Config::Setting::get_m_keyUp, &Config::Setting::set_m_keyUp, VK_UP},
  {"Down", &Config::Setting::get_m_keyDown, &Config::Setting::set_m_keyDown, VK_DOWN},
  {"Left", &Config::Setting::get_m_keyLeft, &Config::Setting::set_m_keyLeft, VK_LEFT},
  {"Right", &Config::Setting::get_m_keyRight, &Config::Setting::set_m_keyRight, VK_RIGHT},
  {"Esc", &Config::Setting::get_m_keyEsc, &Config::Setting::set_m_keyEsc, VK_ESCAPE},
  {"Enter", &Config::Setting::get_m_keyEnter, &Config::Setting::set_m_keyEnter, VK_RETURN},
  {"Space", &Config::Setting::get_m_keySpace, &Config::Setting::set_m_keySpace, VK_SPACE},
  {"Left Shift", &Config::Setting::get_m_keyLshift, &Config::Setting::set_m_keyLshift, VK_LSHIFT},
  {"Left Ctrl", &Config::Setting::get_m_keyLctrl, &Config::Setting::set_m_keyLctrl, VK_LCONTROL},
  {"Page Up", &Config::Setting::get_m_keyPageUp, &Config::Setting::set_m_keyPageUp, VK_PRIOR},
  {"Page Down", &Config::Setting::get_m_keyPageDown, &Config::Setting::set_m_keyPageDown, VK_NEXT},
  {"Num 4", &Config::Setting::get_m_keyNum4, &Config::Setting::set_m_keyNum4, VK_NUMPAD4},
  {"Num 6", &Config::Setting::get_m_keyNum6, &Config::Setting::set_m_keyNum6, VK_NUMPAD6},
  {"Num 8", &Config::Setting::get_m_keyNum8, &Config::Setting::set_m_keyNum8, VK_NUMPAD8},
  {"Num 2", &Config::Setting::get_m_keyNum2, &Config::Setting::set_m_keyNum2, VK_NUMPAD2},
  {"Num +", &Config::Setting::get_m_keyNumadd, &Config::Setting::set_m_keyNumadd, VK_ADD},
  {"Num -", &Config::Setting::get_m_keyNumsub, &Config::Setting::set_m_keyNumsub, VK_SUBTRACT},
  {"Num 0", &Config::Setting::get_m_keyNum0, &Config::Setting::set_m_keyNum0, VK_NUMPAD0},
  {"Num *", &Config::Setting::get_m_keyNumMultiply, &Config::Setting::set_m_keyNumMultiply, VK_MULTIPLY},
};

RBRJoykey::RBRJoykey(IRBRGame* pGame) : m_pGame(pGame), m_initialized(false), m_listenSetting(false) {
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
  Input::SDLBackend::GetInstance()->unregListener(this);
  Input::SDLBackend::GetInstance()->Stop();
  SAFE_DELETE(m_setting);
}

const char* RBRJoykey::GetName(void) {
  if(!m_initialized) {
    // Do the initialization and texture creation only once because RBR may call GetName several times
    LogUtil::ToFile("Initializing the plugin");

    RBRAPI_InitializeObjReferences();
    Input::SDLBackend::GetInstance()->Start();

    InitJoykey();
    m_initialized = true;
  }

  return Config::PluginName.c_str();
}

void RBRJoykey::LoadINI(void) {
  m_setting = new Config::Setting(Config::PluginConfig);
}

void RBRJoykey::InitJoykey(void) {
  m_menuSelection = 0;
  m_settingtime = ::GetTickCount32();
  memset(&m_keystates, 0x0, sizeof(KeyState_t) * Config::MENU_BUTT);
  Input::SDLBackend::GetInstance()->regListener(this);
}

void RBRJoykey::OnWork(void) {
  Input::SDLListener::OnWork();

  for (int i = Config::MENU_KEYBIND_UP; i < Config::MENU_KEYBIND_MULTIPLY; i++) {
    if (m_keystates[i].pressed && ::GetTickCount32() - m_keystates[i].lastPressTime > 200) { // long time pressed
      SendKeyInput(g_menuActions[i].simkey, 0);
      m_keystates[i] = { true, ::GetTickCount32() };
    }
  }
}

void RBRJoykey::OnEvent(SDL_Event& event) {
  Input::SDLListener::OnEvent(event);
  if (!m_setting->get_m_pluginOn())
    return;

  if (!m_listenSetting) {
    switch (event.type) {
    case SDL_QUIT:
      break;
    case SDL_JOYBUTTONDOWN:
      JoystickButtonPressed(event);
      break;
    case SDL_JOYBUTTONUP:
      JoystickButtonRelease(event);
      break;
    default:
      break;
    }
  }
  else { // in keybind setting menu
    if (event.type == SDL_JOYBUTTONDOWN) {
      char keyname[64] = { 0 };
      snprintf(keyname, sizeof(keyname), "%s # %d", SDL_JoystickName(SDL_JoystickFromInstanceID(event.jbutton.which)), event.jbutton.button);
      m_setting->SaveConfig((Config::MENUITEM)m_menuSelection, keyname);
      m_setting->SaveConfig();
      m_listenSetting = false;
    }
  }
}

void RBRJoykey::SendKeyInput(WORD key, DWORD flags) {
  INPUT input = { 0 };
  input.type = INPUT_KEYBOARD;
  input.ki.wVk = key;
  input.ki.dwFlags = flags;
  SendInput(1, &input, sizeof(INPUT));
}

void RBRJoykey::JoystickButtonPressed(SDL_Event &event) {
  char keyname[64] = { 0 };
  snprintf(keyname, sizeof(keyname), "%s # %d", SDL_JoystickName(SDL_JoystickFromInstanceID(event.jbutton.which)), event.jbutton.button);
  for (int i = Config::MENU_KEYBIND_UP; i < Config::MENU_KEYBIND_MULTIPLY; i++) {
    if ((m_setting->*g_menuActions[i].get_func)() == keyname) {
      SendKeyInput(g_menuActions[i].simkey, 0);
      m_keystates[i] = { true, ::GetTickCount32() };
    }
  }
}

void RBRJoykey::JoystickButtonRelease(SDL_Event &event) {
  char keyname[64] = { 0 };
  snprintf(keyname, sizeof(keyname), "%s # %d", SDL_JoystickName(SDL_JoystickFromInstanceID(event.jbutton.which)), event.jbutton.button);

  for (int i = Config::MENU_KEYBIND_UP; i < Config::MENU_KEYBIND_MULTIPLY; i++) {
    if ((m_setting->*g_menuActions[i].get_func)() == keyname) {
      SendKeyInput(g_menuActions[i].simkey, KEYEVENTF_KEYUP);
      m_keystates[i] = { false, ::GetTickCount32() };
    }
  }
}

void RBRJoykey::DrawFrontEndPage(void) {
  float lineposx = 65.0f;
  float lineposy = 49.0f;
  float lineheight = 21.0f;
  int startpos = 0;
  int endpos = 0;

  // Draw blackout (coordinates specify the 'window' where you don't want black background but the "RBR world" to be visible)
  m_pGame->DrawBlackOut(520.0f, 0.0f, 190.0f, 480.0f);

  // Draw custom plugin header line
  m_pGame->SetMenuColor(IRBRGame::MENU_HEADING);
  m_pGame->SetFont(IRBRGame::FONT_BIG);
  m_pGame->WriteText(lineposx, lineposy, Config::PluginName.c_str());

  // Draw key bind menu
  m_pGame->SetMenuColor(IRBRGame::MENU_TEXT);
  m_pGame->SetFont(IRBRGame::FONT_BIG);

  // Draw red menu selection line
  m_pGame->SetMenuColor(IRBRGame::MENU_SELECTION);
  if (Config::MENU_KEYBIND_NUM4 <= m_menuSelection) {
    m_pGame->DrawSelection(0.0f, 68.0f + (static_cast<float>(m_menuSelection - Config::MENU_KEYBIND_NUM4 + 2) * lineheight), 180.0f);
  }
  else {
    m_pGame->DrawSelection(0.0f, 68.0f + (static_cast<float>(m_menuSelection) * lineheight), 180.0f);
  }

  lineposy += lineheight;
  m_pGame->SetMenuColor(IRBRGame::MENU_TEXT);
  m_pGame->WriteText(lineposx, lineposy, "Plugin Status");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_pluginOn() ? "ON" : "OFF");

  lineposy += lineheight;
  m_pGame->SetMenuColor(IRBRGame::MENU_HEADING);
  m_pGame->WriteText(lineposx, lineposy, "KEY");
  if (Config::MENU_KEYBIND_UP <= m_menuSelection && m_menuSelection <= Config::MENU_KEYBIND_PAGEDOWN) {
    m_pGame->WriteText(lineposx + 160.f, lineposy, "General Keys");
  }
  else if (Config::MENU_KEYBIND_NUM4 <= m_menuSelection && m_menuSelection <= Config::MENU_KEYBIND_MULTIPLY) {
    m_pGame->WriteText(lineposx + 160.f, lineposy, "VR Keys");
  }
  else {
    m_pGame->WriteText(lineposx + 160.0f, lineposy, "Input");
  }

  if (m_setting->get_m_pluginOn()) {
    m_pGame->SetMenuColor(IRBRGame::MENU_TEXT);
  }
  else
  {
    m_pGame->SetColor(0x20, 0x20, 0x20, 180);
  }

  // draw keys menu
  if (Config::MENU_PLUGIN_STATE <= m_menuSelection && m_menuSelection <= Config::MENU_KEYBIND_PAGEDOWN) {
    startpos = Config::MENU_KEYBIND_UP;
    endpos = Config::MENU_KEYBIND_PAGEDOWN;
  }
  else if (Config::MENU_KEYBIND_NUM4 <= m_menuSelection && m_menuSelection <= Config::MENU_KEYBIND_MULTIPLY) {
    startpos = Config::MENU_KEYBIND_NUM4;
    endpos = Config::MENU_KEYBIND_MULTIPLY;
  }

  for (int i = startpos; i <= endpos; i++) {
    lineposy += lineheight;
    m_pGame->WriteText(lineposx, lineposy, g_menuActions[i].keyname);
    m_pGame->WriteText(lineposx + 160.0f, lineposy, (m_setting->*g_menuActions[i].get_func)().c_str());
  }

  if (m_listenSetting) {
    DWORD duration = ::GetTickCount32() - m_settingtime;
    if (duration > MAX_INPUT_WAITTING_TIME) {
      m_listenSetting = false;
    }
    else {
      char notice[64] = { 0 };
      snprintf(notice, sizeof(notice), "Press A key to bind, remain time %d S\n", (MAX_INPUT_WAITTING_TIME - duration) / 1000);
      m_pGame->WriteText(lineposx, 400.0f, notice);
    }
  }

  // Draw footer readme
  m_pGame->SetMenuColor(IRBRGame::MENU_HEADING);
  m_pGame->SetFont(IRBRGame::FONT_SMALL);
  m_pGame->WriteText(lineposx, 421.0f, "Use 'Enter' to set select Input.");
  m_pGame->WriteText(lineposx, 421.0f + lineheight, "Use 'Left' to clear select setting.");
  m_pGame->WriteText(lineposx, 421.0f + lineheight * 2, "Use 'Right' to change select setting.");
}

void RBRJoykey::HandleFrontEndEvents(char txtKeyboard, bool bUp, bool bDown, bool bLeft, bool bRight, bool bSelect) {
  if (m_listenSetting)
    return;

  //
  // Menu focus line moved up or down
  //
  if (bUp && (--m_menuSelection) < 0)
    m_menuSelection = 0;  // No wrapping logic

  if (bDown && (++m_menuSelection) >= Config::MENU_BUTT)
    m_menuSelection = Config::MENU_BUTT - 1;

  char keyname[64] = { 0 };
  switch (m_menuSelection) {
  case Config::MENU_PLUGIN_STATE:
    if (bLeft || bRight) {
      m_setting->m_pluginOn = !m_setting->get_m_pluginOn();
      m_setting->SaveConfig((Config::MENUITEM)m_menuSelection, m_setting->get_m_pluginOn() ? "true" : "false");
      m_setting->SaveConfig();
    }
    break;
  default:
    if (Config::MENU_KEYBIND_UP <= m_menuSelection && m_menuSelection <= Config::MENU_KEYBIND_MULTIPLY) {
      if (bLeft) {
        memset(keyname, 0, sizeof(keyname));
        m_setting->SaveConfig((Config::MENUITEM)m_menuSelection, keyname);
      }
      if (bSelect) {
        m_settingtime = ::GetTickCount32();
        ClearLastEvent();
        m_listenSetting = true;
      }
    }
    break;
  }
}