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

tRBRDirectXEndScene Func_OrigRBRDirectXStartScene = nullptr;
tRBRDirectXEndScene Func_OrigRBRDirectXEndScene = nullptr;

HRESULT __fastcall CustomRBRDirectXStartScene(void* objPointer) {
  // We need code at the beginning of this function that is not a call to another function.
  // If we just added "Countdown::DrawCountdown();" then hooking EndScene from another 
  // plugin will fail. This is because of some technicality of how the hooking works.

  ((RBRJoykey*)g_pRBRPlugin)->CustomRBRDirectXStartSceneJoykey();

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
  std::map<int, SDL_Joystick*>::iterator iter = m_joys.begin();
  while (iter != m_joys.end()) {
    SDL_JoystickClose(iter->second);
    iter++;
  }
  SDL_Quit();
  SAFE_DELETE(m_setting);
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
  m_menuSelection = 0;

  if (SDL_Init(SDL_INIT_JOYSTICK) != 0) {
    LogUtil::ToFile("Failed initialize sdl with error: %s" + std::string(SDL_GetError()));
    return;
  }

  SDL_JoystickEventState(SDL_ENABLE);

  // Open all connected joysticks
  for (int i = 0; i < SDL_NumJoysticks(); i++) {
    m_joys[i] = SDL_JoystickOpen(i);
  }
}

void RBRJoykey::CustomRBRDirectXStartSceneJoykey(void) {
  if (!m_setting->get_m_pluginOn())
    return;

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
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

  if (m_setting->get_m_keyUp() == keyname) {
    SendKeyInput(VK_UP, 0);
  }
  else if (m_setting->get_m_keyDown() == keyname) {
    SendKeyInput(VK_DOWN, 0);
  }
  else if (m_setting->get_m_keyLeft() == keyname) {
    SendKeyInput(VK_LEFT, 0);
  }
  else if (m_setting->get_m_keyRight() == keyname) {
    SendKeyInput(VK_RIGHT, 0);
  }
  else if (m_setting->get_m_keyEsc() == keyname) {
    SendKeyInput(VK_ESCAPE, 0);
  }
  else if (m_setting->get_m_keyEnter() == keyname) {
    SendKeyInput(VK_RETURN, 0);
  }
  else if (m_setting->get_m_keySpace() == keyname) {
    SendKeyInput(VK_SPACE, 0);
  }
  else if (m_setting->get_m_keyNum6() == keyname) {
    SendKeyInput(VK_NUMPAD6, 0);
  }
  else if (m_setting->get_m_keyNum4() == keyname) {
    SendKeyInput(VK_NUMPAD4, 0);
  }
  else if (m_setting->get_m_keyNum2() == keyname) {
    SendKeyInput(VK_NUMPAD2, 0);
  }
  else if (m_setting->get_m_keyNum8() == keyname) {
    SendKeyInput(VK_NUMPAD8, 0);
  }
  else if (m_setting->get_m_keyNumadd() == keyname) {
    SendKeyInput(VK_ADD, 0);
  }
  else if (m_setting->get_m_keyNumsub() == keyname) {
    SendKeyInput(VK_SUBTRACT, 0);
  }
  else if (m_setting->get_m_keyNum0() == keyname) {
    SendKeyInput(VK_NUMPAD0, 0);
  }
}

void RBRJoykey::JoystickButtonRelease(SDL_Event &event) {
  char keyname[64] = { 0 };
  snprintf(keyname, sizeof(keyname), "%s # %d", SDL_JoystickName(SDL_JoystickFromInstanceID(event.jbutton.which)), event.jbutton.button);

  if (m_setting->get_m_keyUp() == keyname) {
    SendKeyInput(VK_UP, KEYEVENTF_KEYUP);
  }
  else if (m_setting->get_m_keyDown() == keyname) {
    SendKeyInput(VK_DOWN, KEYEVENTF_KEYUP);
  }
  else if (m_setting->get_m_keyLeft() == keyname) {
    SendKeyInput(VK_LEFT, KEYEVENTF_KEYUP);
  }
  else if (m_setting->get_m_keyRight() == keyname) {
    SendKeyInput(VK_RIGHT, KEYEVENTF_KEYUP);
  }
  else if (m_setting->get_m_keyEsc() == keyname) {
    SendKeyInput(VK_ESCAPE, KEYEVENTF_KEYUP);
  }
  else if (m_setting->get_m_keyEnter() == keyname) {
    SendKeyInput(VK_RETURN, KEYEVENTF_KEYUP);
  }
  else if (m_setting->get_m_keySpace() == keyname) {
    SendKeyInput(VK_SPACE, KEYEVENTF_KEYUP);
  }
  else if (m_setting->get_m_keyNum6() == keyname) {
    SendKeyInput(VK_NUMPAD6, KEYEVENTF_KEYUP);
  }
  else if (m_setting->get_m_keyNum4() == keyname) {
    SendKeyInput(VK_NUMPAD4, KEYEVENTF_KEYUP);
  }
  else if (m_setting->get_m_keyNum2() == keyname) {
    SendKeyInput(VK_NUMPAD2, KEYEVENTF_KEYUP);
  }
  else if (m_setting->get_m_keyNum8() == keyname) {
    SendKeyInput(VK_NUMPAD8, KEYEVENTF_KEYUP);
  }
  else if (m_setting->get_m_keyNumadd() == keyname) {
    SendKeyInput(VK_ADD, KEYEVENTF_KEYUP);
  }
  else if (m_setting->get_m_keyNumsub() == keyname) {
    SendKeyInput(VK_SUBTRACT, KEYEVENTF_KEYUP);
  }
  else if (m_setting->get_m_keyNum0() == keyname) {
    SendKeyInput(VK_NUMPAD0, KEYEVENTF_KEYUP);
  }
}

void RBRJoykey::DrawFrontEndPage(void) {
  float lineposx = 65.0f;
  float lineposy = 49.0f;
  float lineheight = 21.0f;

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
  m_pGame->DrawSelection(0.0f, 68.0f + (static_cast<float>(m_menuSelection) * lineheight), 180.0f);

  lineposy += lineheight;
  m_pGame->SetMenuColor(IRBRGame::MENU_TEXT);
  m_pGame->WriteText(lineposx, lineposy, "Plugin Status");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_pluginOn() ? "ON" : "OFF");

  lineposy += lineheight;
  m_pGame->SetMenuColor(IRBRGame::MENU_HEADING);
  m_pGame->WriteText(lineposx, lineposy, "KEY");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, "INPUT");

  if (m_setting->get_m_pluginOn()) {
    m_pGame->SetMenuColor(IRBRGame::MENU_TEXT);
  }
  else
  {
    m_pGame->SetColor(0x20, 0x20, 0x20, 180);
  }

  lineposy += lineheight;
  m_pGame->WriteText(lineposx, lineposy, "Up");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_keyUp().c_str());

  lineposy += lineheight;
  m_pGame->WriteText(lineposx, lineposy, "Down");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_keyDown().c_str());

  lineposy += lineheight;
  m_pGame->WriteText(lineposx, lineposy, "Left");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_keyLeft().c_str());

  lineposy += lineheight;
  m_pGame->WriteText(lineposx, lineposy, "Right");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_keyRight().c_str());

  lineposy += lineheight;
  m_pGame->WriteText(lineposx, lineposy, "ESC");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_keyEsc().c_str());

  lineposy += lineheight;
  m_pGame->WriteText(lineposx, lineposy, "Enter");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_keyEnter().c_str());

  lineposy += lineheight;
  m_pGame->WriteText(lineposx, lineposy, "Space");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_keySpace().c_str());

  lineposy += lineheight;
  m_pGame->WriteText(lineposx, lineposy, "Num4");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_keyNum4().c_str());

  lineposy += lineheight;
  m_pGame->WriteText(lineposx, lineposy, "Num6");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_keyNum6().c_str());

  lineposy += lineheight;
  m_pGame->WriteText(lineposx, lineposy, "Num8");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_keyNum8().c_str());

  lineposy += lineheight;
  m_pGame->WriteText(lineposx, lineposy, "Num2");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_keyNum2().c_str());

  lineposy += lineheight;
  m_pGame->WriteText(lineposx, lineposy, "Numadd");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_keyNumadd().c_str());

  lineposy += lineheight;
  m_pGame->WriteText(lineposx, lineposy, "Numsub");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_keyNumsub().c_str());

  lineposy += lineheight;
  m_pGame->WriteText(lineposx, lineposy, "Num0");
  m_pGame->WriteText(lineposx + 160.0f, lineposy, m_setting->get_m_keyNum0().c_str());

  // Draw footer readme
  m_pGame->SetMenuColor(IRBRGame::MENU_HEADING);
  m_pGame->SetFont(IRBRGame::FONT_SMALL);
  m_pGame->WriteText(360.0f, 149.0f, "Use 'Enter' to set select Input.");
  m_pGame->WriteText(360.0f, 149.0f + lineheight, "Use 'Left' to clear select setting.");
  m_pGame->WriteText(360.0f, 149.0f + lineheight * 2, "Use 'Right' to change select setting.");
  m_pGame->WriteText(360.0f, 149.0f + lineheight * 3, "Designed by Lw_Ziye.");
  m_pGame->WriteText(360.0f, 149.0f + lineheight * 4, "https:://github.com/geekerlw/RBRPlugins");
}

void RBRJoykey::HandleFrontEndEvents(char txtKeyboard, bool bUp, bool bDown, bool bLeft, bool bRight, bool bSelect) {
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
    if (bRight) {
      m_setting->m_pluginOn = !m_setting->get_m_pluginOn();
      m_setting->SaveConfig((Config::MENUITEM)m_menuSelection, m_setting->get_m_pluginOn() ? "true" : "false");
    }
    break;
  case Config::MENU_KEYBIND_UP:
  case Config::MENU_KEYBIND_DOWN:
  case Config::MENU_KEYBIND_LEFT:
  case Config::MENU_KEYBIND_RIGHT:
  case Config::MENU_KEYBIND_ESC:
  case Config::MENU_KEYBIND_ENTER:
  case Config::MENU_KEYBIND_SPACE:
  case Config::MENU_KEYBIND_NUM4:
  case Config::MENU_KEYBIND_NUM6:
  case Config::MENU_KEYBIND_NUM8:
  case Config::MENU_KEYBIND_NUM2:
  case Config::MENU_KEYBIND_NUMADD:
  case Config::MENU_KEYBIND_NUMSUB:
  case Config::MENU_KEYBIND_NUM0:
    if (bLeft) {
      memset(keyname, 0, sizeof(keyname));
      m_setting->SaveConfig((Config::MENUITEM)m_menuSelection, keyname);
    }
    if (bSelect) {
      Uint64 startTime = SDL_GetTicks64();
      SDL_Event event;
      memset(&event, 0, sizeof(SDL_Event));
      while (SDL_GetTicks64() - startTime < MAX_INPUT_WAITTING_TIME) {
        if (SDL_PollEvent(&event) && event.type == SDL_JOYBUTTONDOWN) {
          snprintf(keyname, sizeof(keyname), "%s # %d", SDL_JoystickName(SDL_JoystickFromInstanceID(event.jbutton.which)), event.jbutton.button);
          m_setting->SaveConfig((Config::MENUITEM)m_menuSelection, keyname);
          break;
        }
      }
    }
    break;
  default:
    break;
  }
  m_setting->SaveConfig();
}