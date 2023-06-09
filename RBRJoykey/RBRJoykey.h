#pragma once

#include <map>
#include "RBR/RBR.h"
#include "Utils/INIUtil.h"
#include "config.h"
#include "SDL.h"

class RBRJoykey : public IPlugin {
private:
  IRBRGame* m_pGame;
  std::map<int, SDL_Joystick*> m_joys;
  int m_menuSelection;
  float m_scalex, m_scaley;
  Config::Setting *m_setting;

public:
  RBRJoykey(IRBRGame* pGame);

  virtual ~RBRJoykey(void);

  virtual const char* GetName(void);

  virtual void DrawResultsUI(void) {
    // Do nothing
  }

  virtual void DrawFrontEndPage(void);

  virtual void HandleFrontEndEvents(char txtKeyboard, bool bUp, bool bDown, bool bLeft, bool bRight, bool bSelect);

  virtual void TickFrontEndPage(float fTimeDelta) {
    // Do nothing
  }

  // Is called when the player timer starts (after GO! or in case of a false start)
  virtual void StageStarted(int iMap, const char* ptxtPlayerName, bool bWasFalseStart) {
    // Do nothing
  }

  // Is called when player finishes stage (fFinishTime is 0.0f if player failed the stage)
  virtual void HandleResults(
    float fCheckPoint1, float fCheckPoint2, float fFinishTime, const char* ptxtPlayerName
  ) {
    // Do nothing
  }

  // Is called when a player passed a checkpoint 
  virtual void CheckPoint(float fCheckPointTime, int iCheckPointID, const char* ptxtPlayerName) {
    // Do nothing
  }

public:
  void CustomRBRDirectXStartSceneJoykey(void);

  void JoystickButtonPressed(SDL_Event &event);

  void JoystickButtonRelease(SDL_Event &event);

private:
  void LoadINI(void);

  void InitJoykey(void);

  void SendKeyInput(WORD key, DWORD flags);
};
