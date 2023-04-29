//====== Copyright Valve Corporation, All rights reserved. =======


#include "RBRVRDash.h"
#include "Utils/LogUtil.h"
#include "SimpleMath.h"

using namespace vr;
using namespace DirectX::SimpleMath;

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
		vr::HmdMatrix34_t transform = {
			1.0f, 0.0f, 0.0f, 0,
			0.0f, 1.0f, 0.0f, 0,
			0.0f, 0.0f, 1.0f, -1.28f,
		};
		vr::VROverlay()->SetOverlayTransformAbsolute(m_ulOverlayHandle, vr::TrackingUniverseSeated, &transform);
		vr::VROverlay()->ShowOverlay(m_ulOverlayHandle);
	}
	return true;
}

void RBRVRDash::Shutdown()
{
	DisconnectFromVRRuntime();
}

void RBRVRDash::ShowOverlay()
{
	vr::VROverlay()->ShowOverlay(m_ulOverlayHandle);
	return;
}

void RBRVRDash::HideOverlay()
{
	vr::VROverlay()->HideOverlay(m_ulOverlayHandle);
	return;
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

void RBRVRDash::SubmitOverlay(Config::CarSetting* car)
{
	if (!vr::VRSystem())
		return;

	vr::Texture_t tex;
	tex.handle = car->m_pD3D11TextureDash;
	tex.eType = vr::TextureType_DirectX;
	tex.eColorSpace = vr::ColorSpace_Auto;
	vr::VROverlay()->SetOverlayTexture(m_ulOverlayHandle, &tex);
}

vr::HmdMatrix34_t RBRVRDash::MatrixToHmdMatrix34(Matrix &m)
{
	HmdMatrix34_t pose;
	pose.m[0][0] = m._11;
	pose.m[0][1] = m._12;
	pose.m[0][2] = m._13;
	pose.m[0][3] = m._14;
	pose.m[1][0] = m._21;
	pose.m[1][1] = m._22;
	pose.m[1][2] = m._23;
	pose.m[1][3] = m._24;
	pose.m[2][0] = m._31;
	pose.m[2][1] = m._32;
	pose.m[2][2] = m._33;
	pose.m[2][3] = m._34;

	return pose;
}

void RBRVRDash::UpdatePose(Config::CarSetting* car)
{	
	if (!vr::VRSystem())
		return;

	Matrix matrix = Matrix::CreateTranslation(0, 0, 0);
	matrix *= Matrix::CreateTranslation(car->get_m_vrPositionX(), car->get_m_vrPositionY(), car->get_m_vrPositionZ());
	matrix *= Matrix::CreateRotationX(car->get_m_vrRotationX());
	matrix *= Matrix::CreateRotationY(car->get_m_vrRotationY());
	matrix *= Matrix::CreateRotationZ(car->get_m_vrRotationZ());
	matrix *= Matrix::CreateScale(car->get_m_vrScale());
	matrix = matrix.Transpose();

	vr::HmdMatrix34_t transform = MatrixToHmdMatrix34(matrix);
	vr::VROverlay()->SetOverlayTransformAbsolute(m_ulOverlayHandle, vr::TrackingUniverseSeated, &transform);
}