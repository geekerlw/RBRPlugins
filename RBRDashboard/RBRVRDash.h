#pragma once

#include "openvr.h"
#include "config.h"
#include "SimpleMath.h"

class RBRVRDash
{
public:
	RBRVRDash(void);
	virtual ~RBRVRDash(void);

	bool Init(void);
	void Shutdown(void);
	void ShowOverlay(void);
	void HideOverlay(void);

	bool IsHMDAvailable(void);
	vr::HmdError GetLastHmdError(void);

public:
	void HandleVrEvent(void);
	void SubmitOverlay(const Config::CarSetting *car);
	void UpdatePose(Config::CarSetting *car);

private:
	bool ConnectToVRRuntime(void);
	void DisconnectFromVRRuntime(void);
	vr::HmdMatrix34_t MatrixToHmdMatrix34(DirectX::SimpleMath::Matrix& m);

	vr::HmdError m_eLastHmdError;

private:
	vr::HmdError m_eCompositorError;
	vr::HmdError m_eOverlayError;
	vr::VROverlayHandle_t m_ulOverlayHandle;
};