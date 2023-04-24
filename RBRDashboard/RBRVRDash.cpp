//====== Copyright Valve Corporation, All rights reserved. =======


#include "RBRVRDash.h"
#include "Utils/LogUtil.h"

using namespace vr;

RBRVRDash::RBRVRDash()
	: m_eLastHmdError(vr::VRInitError_None)
	, m_eCompositorError(vr::VRInitError_None)
	, m_eOverlayError(vr::VRInitError_None)
	, m_ulOverlayHandle(vr::k_ulOverlayHandleInvalid)
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
RBRVRDash::~RBRVRDash()
{
}

bool RBRVRDash::Init()
{
	bool bSuccess = true;

	// Loading the OpenVR Runtime
	bSuccess = ConnectToVRRuntime();

	bSuccess = bSuccess && vr::VRCompositor() != NULL;

	if (vr::VROverlay())
	{
		vr::VROverlayError overlayError = vr::VROverlay()->CreateOverlay("RBR VR Dashboard", "RBR VR Dashboard", &m_ulOverlayHandle);
		bSuccess = bSuccess && overlayError == vr::VROverlayError_None;
	}

	if (bSuccess)
	{
		vr::VROverlay()->SetOverlayWidthInMeters(m_ulOverlayHandle, 1.5f);
	}
	return true;
}

void RBRVRDash::Shutdown()
{
	DisconnectFromVRRuntime();
}

bool RBRVRDash::ConnectToVRRuntime()
{
	m_eLastHmdError = vr::VRInitError_None;
	vr::IVRSystem* pVRSystem = vr::VR_Init(&m_eLastHmdError, vr::VRApplication_Overlay);

	return true;
}


void RBRVRDash::DisconnectFromVRRuntime()
{
	vr::VR_Shutdown();
}


bool RBRVRDash::IsHMDAvailable()
{
	return vr::VRSystem() != NULL;
}

vr::HmdError RBRVRDash::GetLastHmdError()
{
	return m_eLastHmdError;
}

void RBRVRDash::HandleVrEvent()
{
	if (!vr::VRSystem())
		return;

	vr::VREvent_t vrEvent;
	while (vr::VROverlay()->PollNextOverlayEvent(m_ulOverlayHandle, &vrEvent, sizeof(vrEvent)))
	{
		switch (vrEvent.eventType)
		{
		case vr::VREvent_Quit:
			Shutdown();
			break;
		}
	}
}

void RBRVRDash::SubmitOverlay(const Config::CarSetting* car)
{
	if (!vr::VRSystem())
		return;

	vr::Texture_t tex;
	tex.handle = car->m_pD3D11TextureDash;
	tex.eType = vr::TextureType_DirectX;
	tex.eColorSpace = vr::ColorSpace_Auto;
	vr::VROverlay()->SetOverlayTexture(m_ulOverlayHandle, &tex);
}