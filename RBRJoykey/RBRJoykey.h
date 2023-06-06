#pragma once

#include <map>
#include "RBR/RBR.h"
#include "Utils/INIUtil.h"
#include "config.h"

class RBRJoykey : public IPlugin {
private:
  IRBRGame* m_pGame;
  float m_scalex, m_scaley;
  Config::Setting *m_setting;

public:
  RBRJoykey(IRBRGame* pGame);

  virtual ~RBRJoykey(void);

  virtual const char* GetName(void);

  virtual void DrawResultsUI(void) {
    // Do nothing
  }

  virtual void DrawFrontEndPage(void) {
    // Do nothing
  }

  virtual void HandleFrontEndEvents(char txtKeyboard, bool bUp, bool bDown, bool bLeft, bool bRight, bool bSelect) {
    // Do nothing
  }

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

private:
  void LoadINI(void);

  void InitJoykey(void);
};
