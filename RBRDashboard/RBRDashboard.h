#pragma once

#include <map>
#include "RBR/RBR.h"
#include "RBR/D3D9Helpers.h"
#include "RBR/D3D9Font/D3DFont.h"
#include "Utils/INIUtil.h"
#include "config.h"
#include "RBRVRDash.h"
#include "openvr.h"
#include "SimpleMath.h"

class RBRDashboard : public IPlugin {
private:
  IRBRGame* m_pGame; 
  RBRVRDash* m_Vr;
  unsigned int m_tickTime;
  Config::CarSetting* m_curCarSetting;
  float m_scalex, m_scaley;
  Config::Setting *m_setting;
  std::map<int, Config::CarSetting*> m_carSettings;
  ID3D11Device* m_pID3D11Device;
	ID3D11DeviceContext* m_pID3D11DeviceContext;

  // 2d game overlay needed
  PIMAGE_TEXTURE m_metatex;
  PIMAGE_TEXTURE m_dashtex;
  CD3DFont* m_timeFont;
  CD3DFont* m_speedFont;
  CD3DFont* m_distanceFont;
  CD3DFont* m_engineFont;

  // vr game overlay needed
  ID3D11Texture2D* m_pD3D11TextureMeta, * m_pD3D11TextureDash;
  ID3D11ShaderResourceView* m_pD3D11ShaderResourceView;
  DX11::SpriteBatch* m_spriteBatch;
  DX11::SpriteFont* m_timeSpriteFont;
  DX11::SpriteFont* m_speedSpriteFont;
  DX11::SpriteFont* m_distanceSpriteFont;
  DX11::SpriteFont* m_engineSpriteFont;

public:
  RBRDashboard(IRBRGame* pGame);

  virtual ~RBRDashboard(void);

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

  HRESULT CustomRBRDirectXStartSceneBeforeStart(void* objPointer);
  
  HRESULT CustomRBRDirectXEndSceneDriving(void* objPointer);

  HRESULT CustomRBRDirectXEndSceneBackMenu(void* objPointer);

public:
  void EnableDashboard(Config::CarSetting* car);

  void DisableDashboard(void);

  void EnableVROverlay(Config::CarSetting* car);

  void DisableVROverlay(void);

private:
  void LoadINI(void);

  void InitDashboard(void);

  void InitVrSystem(void);

  void DrawDashboard(void);

  void DrawVROverlay(void);

  bool IsHMDAvailable(void);

  vr::HmdMatrix34_t MatrixToHmdMatrix34(DirectX::SimpleMath::Matrix& m);
};
