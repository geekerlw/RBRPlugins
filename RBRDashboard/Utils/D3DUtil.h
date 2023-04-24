#pragma once

#include <string>
#include <Windows.h>
#include <d3d9.h>
#include <d3d11.h>

namespace D3DUtil {
  D3D11_BOX D3D9ToD3D11Rect(const RECT& rect);
  D3D11_BOX D3D9ToD3D11Rect(const RECT* rect);
  RECT D3D11ToD3D9Rect(const D3D11_BOX& box);
  RECT D3D11ToD3D9Rect(const D3D11_BOX* box);
}