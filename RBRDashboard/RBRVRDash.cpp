//====== Copyright Valve Corporation, All rights reserved. =======


#include "RBRVRDash.h"
#include "Utils/LogUtil.h"

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



/*
{
#include <string>
#include <DirectXMath.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

using namespace DirectX;

// Create a texture to draw the text onto
ID3D11Texture2D* pTexture = nullptr;
D3D11_TEXTURE2D_DESC desc = {};
desc.Width = 512;
desc.Height = 512;
desc.MipLevels = 1;
desc.ArraySize = 1;
desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
desc.SampleDesc.Count = 1;
desc.Usage = D3D11_USAGE_DEFAULT;
desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
device->CreateTexture2D(&desc, nullptr, &pTexture);

// Create a render target view for the texture
ID3D11RenderTargetView* pRenderTargetView = nullptr;
device->CreateRenderTargetView(pTexture, nullptr, &pRenderTargetView);

// Create a shader resource view for the texture (so we can use it as a texture later)
ID3D11ShaderResourceView* pShaderResourceView = nullptr;
device->CreateShaderResourceView(pTexture, nullptr, &pShaderResourceView);

// Initialize the SpriteBatch object
std::unique_ptr<SpriteBatch> m_pSpriteBatch;
m_pSpriteBatch = std::make_unique<SpriteBatch>(context);

// Initialize the SpriteFont object
std::unique_ptr<SpriteFont> m_pFont;
m_pFont = std::make_unique<SpriteFont>(device, L"Arial.spritefont");

// Set up the text to display
std::wstring textToDisplay = L"Hello, world!";
XMFLOAT2 position = XMFLOAT2(100.0f, 100.0f);
XMVECTOR color = Colors::White;

// Set the render target to the texture we created
context->OMSetRenderTargets(1, &pRenderTargetView, nullptr);

// Clear the render target
float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
context->ClearRenderTargetView(pRenderTargetView, clearColor);

// Draw the text using SpriteFont and SpriteBatch
m_pSpriteBatch->Begin();
m_pFont->DrawString(m_pSpriteBatch.get(), textToDisplay.c_str(), position, color);
m_pSpriteBatch->End();

// Reset the render target
ID3D11RenderTargetView* pNullRTV = nullptr;
context->OMSetRenderTargets(1, &pNullRTV, nullptr);

// Use the texture as a shader resource (if needed)
context->PSSetShaderResources(0, 1, &pShaderResourceView);

// Release all resources when you're done with them
delete m_pFont;
m_pFont = nullptr;

delete m_pSpriteBatch;
m_pSpriteBatch = nullptr;

pShaderResourceView->Release();
pShaderResourceView = nullptr;

pRenderTargetView->Release();
pRenderTargetView = nullptr;

pTexture->Release();
pTexture = nullptr;
}
*/