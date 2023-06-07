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

#define MAX_SUPPORT_JOYS (8)

extern IPlugin* g_pRBRPlugin;

tRBRDirectXEndScene Func_OrigRBRDirectXStartScene = nullptr;
tRBRDirectXEndScene Func_OrigRBRDirectXEndScene = nullptr;

HRESULT __fastcall CustomRBRDirectXStartScene(void* objPointer) {
  // We need code at the beginning of this function that is not a call to another function.
  // If we just added "Countdown::DrawCountdown();" then hooking EndScene from another 
  // plugin will fail. This is because of some technicality of how the hooking works.

  return ::Func_OrigRBRDirectXStartScene(objPointer);
}

HRESULT __fastcall CustomRBRDirectXEndScene(void* objPointer) {
  // We need code at the beginning of this function that is not a call to another function.
  // If we just added "Countdown::DrawCountdown();" then hooking EndScene from another 
  // plugin will fail. This is because of some technicality of how the hooking works.

  return ::Func_OrigRBRDirectXEndScene(objPointer);
}

RBRJoykey::RBRJoykey(IRBRGame* pGame) : m_pGame(pGame), m_thread(nullptr) {
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
  if (m_thread) {
    //TODO: maybe need to push an event like SDL_QUIT here.
    SDL_WaitThread(m_thread, NULL);
  }
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


static int JoykeyThread(void *arg) {
  RBRJoykey *pthis = static_cast<RBRJoykey*>(arg);
  SDL_Joystick *joysticks[MAX_SUPPORT_JOYS] = { 0 };

  if (SDL_Init(SDL_INIT_JOYSTICK) != 0) {
    LogUtil::ToFile("Failed initialize sdl with error: %s" + std::string(SDL_GetError()));
    return -1;
  }

  SDL_JoystickEventState(SDL_ENABLE);

  // Open all connected joysticks
  int num_joysticks = SDL_NumJoysticks();
  for (int i = 0; i < num_joysticks && i < MAX_SUPPORT_JOYS; i++) {
      joysticks[i] = SDL_JoystickOpen(i);
  }

  // Set the joystick event handler
  SDL_Event event;
  while (SDL_WaitEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        break;
      case SDL_JOYBUTTONDOWN:
        pthis->JoystickButtonPressed(event);
        break;
      case SDL_JOYBUTTONUP:
        pthis->JoystickButtonRelease(event);
        break;
      default:
        break;
    }
  }

  // Close all connected joysticks
  for (int i = 0; i < num_joysticks && i < MAX_SUPPORT_JOYS; i++) {
      SDL_JoystickClose(joysticks[i]);
  }

  SDL_Quit();
  return 0;
}

void RBRJoykey::InitJoykey(void) {
  POINT pa, pb;
  RBRAPI_MapRBRPointToScreenPoint(0, 0, &pa);
  RBRAPI_MapRBRPointToScreenPoint(640, 480, &pb);
  m_scalex = std::abs(pb.x - pa.x) / 1920.0f;
  m_scaley = std::abs(pb.y - pa.y) / 1440.0f;

  m_thread = SDL_CreateThread(JoykeyThread, "JoyKeythread", this);
}

void RBRJoykey::JoystickButtonPressed(SDL_Event &event) {
  char log[128] = { 0 };
  int joystick_id = event.jbutton.which;
  int instance_id = SDL_JoystickGetDeviceInstanceID(joystick_id);
  int button = event.jbutton.button;
  snprintf(log, sizeof(log), "Joystick %d (instance %d, name: %s) button %d down\n", joystick_id, instance_id, SDL_JoystickName(SDL_JoystickFromInstanceID(joystick_id)), button);
  LogUtil::ToFile(log);
}

void RBRJoykey::JoystickButtonRelease(SDL_Event &event) {
  char log[128] = { 0 };
  int joystick_id = event.jbutton.which;
  int instance_id = SDL_JoystickGetDeviceInstanceID(joystick_id);
  int button = event.jbutton.button;
  snprintf(log, sizeof(log), "Joystick %d (instance %d, name: %s) button %d up\n", joystick_id, instance_id, SDL_JoystickName(SDL_JoystickFromInstanceID(joystick_id)), button);
  LogUtil::ToFile(log);
}