//====== Copyright Valve Corporation, All rights reserved. =======


#include "RBRVRDash.h"
#include "Utils/LogUtil.h"

#pragma comment(lib,"d3d11.lib")

using namespace vr;

RBRVRDash::RBRVRDash()
	: m_eLastHmdError(vr::VRInitError_None)
	, m_eCompositorError(vr::VRInitError_None)
	, m_eOverlayError(vr::VRInitError_None)
	, m_ulOverlayHandle(vr::k_ulOverlayHandleInvalid)
	, m_dashtex(nullptr)
{
	HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
		D3D11_SDK_VERSION, &m_pID3D11Device, nullptr, &m_pID3D11DeviceContext);
	if (SUCCEEDED(hr)) {
		LogUtil::ToFile("Success create D3D11 Device and Context");
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
RBRVRDash::~RBRVRDash()
{
	SAFE_DELETE(m_pID3D11DeviceContext);
	SAFE_DELETE(m_pID3D11Device);
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

	// Get D3D9 surface from texture
	IDirect3DTexture9* d3d9Texture = car->m_dashtex->pTexture;
	IDirect3DSurface9* d3d9Surface = nullptr;
	HRESULT hr = d3d9Texture->GetSurfaceLevel(0, &d3d9Surface);

	// Create D3D11 texture from D3D9 surface
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = 800;
	desc.Height = 636;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

	IDXGIResource* dxgiResource = nullptr;
	hr = d3d9Surface->QueryInterface(__uuidof(IDXGIResource), (void**)&dxgiResource);

	if (FAILED(hr)) {
		LogUtil::ToFile("steven: failed 33");
	}

	HANDLE sharedHandle = nullptr;
	hr = dxgiResource->GetSharedHandle(&sharedHandle);

	ID3D11Texture2D* d3d11Texture = nullptr;
	hr = m_pID3D11Device->OpenSharedResource(sharedHandle, __uuidof(ID3D11Texture2D),
		(LPVOID*)&d3d11Texture);

	vr::Texture_t tex;
	tex.handle = d3d11Texture;
	tex.eType = vr::TextureType_DirectX;
	tex.eColorSpace = vr::ColorSpace_Auto;
	vr::VROverlay()->SetOverlayTexture(m_ulOverlayHandle, &tex);

	d3d9Surface->Release();
	dxgiResource->Release();
}