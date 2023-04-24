
#include "D3DUtil.h"
#include <Windows.h>
#include <stdexcept>

namespace D3DUtil {
  D3D11_BOX D3D9ToD3D11Rect(const RECT& rect) {
    D3D11_BOX box;
    box.left = rect.left;
    box.top = rect.top;
    box.front = 0;
    box.right = rect.right;
    box.bottom = rect.bottom;
    box.back = 1;
    return box;
  }

  D3D11_BOX D3D9ToD3D11Rect(const RECT* rect) {
    D3D11_BOX box;
    box.left = rect->left;
    box.top = rect->top;
    box.front = 0;
    box.right = rect->right;
    box.bottom = rect->bottom;
    box.back = 1;
    return box;
  }

  RECT D3D11ToD3D9Rect(const D3D11_BOX& box) {
    RECT rect;
    rect.left = box.left;
    rect.top = box.top;
    rect.right = box.right;
    rect.bottom = box.bottom;
    return rect;
  }

  RECT D3D11ToD3D9Rect(const D3D11_BOX* box) {
    RECT rect;
    rect.left = box->left;
    rect.top = box->top;
    rect.right = box->right;
    rect.bottom = box->bottom;
    return rect;
  }
}