#pragma once

#include "openvr.h"
#include "config.h"

class RBRVRDash
{
public:
	RBRVRDash(void);
	virtual ~RBRVRDash(void);

	bool Init(void);
	void Shutdown(void);
	void ShowOverlay(void);
	void HideOverlay(void);

	vr::HmdError GetLastHmdError(void);

public:
	void HandleVrEvent(void);
	void SubmitOverlay(ID3D11Texture2D *pTexture);
	void UpdatePose(vr::HmdMatrix34_t* transform);

private:
	bool ConnectToVRRuntime(void);
	void DisconnectFromVRRuntime(void);

	vr::HmdError m_eLastHmdError;

private:
	vr::HmdError m_eCompositorError;
	vr::HmdError m_eOverlayError;
	vr::VROverlayHandle_t m_ulOverlayHandle;
};