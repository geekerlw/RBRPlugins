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

	bool IsHMDAvailable(void);
	vr::HmdError GetLastHmdError(void);

public:
	void HandleVrEvent(void);
	void SubmitOverlay(const Config::CarSetting *car);

private:
	bool ConnectToVRRuntime(void);
	void DisconnectFromVRRuntime(void);

	vr::HmdError m_eLastHmdError;

private:
	vr::HmdError m_eCompositorError;
	vr::HmdError m_eOverlayError;
	vr::VROverlayHandle_t m_ulOverlayHandle;
};