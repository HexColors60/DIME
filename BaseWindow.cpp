//
//
// Derived from Microsoft Sample IME by Jeremy '13,7,17
//
//
//#define DEBUG_PRINT

#include "Private.h"
#include "Globals.h"
#include "BaseWindow.h"


//+---------------------------------------------------------------------------
//
// CBaseWindow::ctor
//
//----------------------------------------------------------------------------

CBaseWindow::CBaseWindow()
{
    _wndHandle = nullptr;
    _pParentWnd = nullptr;
    _pUIWnd = nullptr;

    _pTimerUIObj = nullptr;
    _pUIObjCapture = nullptr;

    _enableVirtualWnd = TRUE;
    _visibleVirtualWnd = TRUE;
    _RectOfVirtualWnd.left = 0;
    _RectOfVirtualWnd.top = 0;
    _RectOfVirtualWnd.right = 0;
    _RectOfVirtualWnd.bottom = 0;
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::dtor
//
//----------------------------------------------------------------------------

CBaseWindow::~CBaseWindow()
{
    _SetThis(_wndHandle, nullptr);
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::_InitWindowClass
//
//----------------------------------------------------------------------------

/* static */
BOOL CBaseWindow::_InitWindowClass(_In_ LPCWSTR lpwszClassName, _Out_ ATOM *patom)
{
    WNDCLASS wc;

    wc.style         = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_IME;
    wc.lpfnWndProc   = CBaseWindow::_WindowProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = Global::dllInstanceHandle;
    wc.hIcon         = nullptr;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wc.lpszMenuName  = nullptr;
    wc.lpszClassName = lpwszClassName;

    *patom = RegisterClass(&wc);

    return (*patom != 0);
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::_UninitClass
//
//----------------------------------------------------------------------------

/* static */
void CBaseWindow::_UninitWindowClass(ATOM atom)
{
    if (atom != 0)
    {
        UnregisterClass((LPCTSTR)atom, Global::dllInstanceHandle);
    }
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::_Create
//
//----------------------------------------------------------------------------

BOOL CBaseWindow::_Create(ATOM atom, DWORD dwExStyle, DWORD dwStyle, _In_opt_ CBaseWindow *pParentWnd, int wndWidth, int wndHeight, _In_opt_ HWND parentWndHandle)
{
    _pParentWnd = pParentWnd;

    if (atom != 0)
    {
        // create real window

        _wndHandle = CreateWindowEx(dwExStyle,
            (LPCTSTR)atom,
            NULL,
            dwStyle,
            0, 0,
            wndWidth, wndHeight,
            _pParentWnd ? _pParentWnd->_GetWnd() : parentWndHandle,    // parentWndHandle
            NULL,
            Global::dllInstanceHandle,
            this);   // lpParam

        if (!_wndHandle)
        {
            return FALSE;
        }
    }

    return TRUE;
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::_Destroy
//
//----------------------------------------------------------------------------

void CBaseWindow::_Destroy()
{
    if (_wndHandle != nullptr)
    {
        DestroyWindow(_wndHandle);
        _wndHandle = nullptr;
    }
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::_Move
//
//----------------------------------------------------------------------------

void CBaseWindow::_Move(int x, int y)
{
    if (_wndHandle != nullptr)
    {
		// do not alter zorder to  to avoid bounce back on child windows (like office share->email)
        SetWindowPos(_wndHandle, NULL, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE |SWP_NOZORDER);
    }
    else
    {
        int dx = x - _RectOfVirtualWnd.left;
        int dy = y - _RectOfVirtualWnd.top;

        _RectOfVirtualWnd.left += dx;
        _RectOfVirtualWnd.top += dy;
        _RectOfVirtualWnd.right += dx;
        _RectOfVirtualWnd.bottom += dy;
    }
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::_Resize
//
//----------------------------------------------------------------------------

void CBaseWindow::_Resize(int x, int y, int cx, int cy)
{
    if (_wndHandle != nullptr)
    {
        MoveWindow(_wndHandle, x, y, cx, cy, TRUE);
    }
    else
    {
        _RectOfVirtualWnd.left = x;
        _RectOfVirtualWnd.top = y;
        _RectOfVirtualWnd.right = x + cx;
        _RectOfVirtualWnd.bottom = y + cy;
    }
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::_Show
//
//----------------------------------------------------------------------------

void CBaseWindow::_Show(BOOL isShowWnd)
{
    if (_wndHandle != nullptr)
    {
        if (isShowWnd)
        {
			SetWindowPos(_wndHandle,  NULL, 0, 0, 0, 0, 
				SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_SHOWWINDOW |SWP_NOZORDER); 
        }
        else
        {
           SetWindowPos(_wndHandle,  NULL, 0, 0, 0, 0, 
				SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_HIDEWINDOW |SWP_NOZORDER); 
        }
    }
    else
    {
        _visibleVirtualWnd = isShowWnd;
    }
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::_IsWindowVisible
//
//----------------------------------------------------------------------------

BOOL CBaseWindow::_IsWindowVisible()
{
    if (_wndHandle != nullptr)
    {
        return IsWindowVisible(_wndHandle);
    }
    else
    {
        return _visibleVirtualWnd;
    }
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::_Enable
//
//----------------------------------------------------------------------------

void CBaseWindow::_Enable(BOOL enableWindowReceiveInput)
{
    if (_wndHandle != nullptr)
    {
        EnableWindow(_wndHandle, enableWindowReceiveInput);
    }
    else
    {
        _enableVirtualWnd = enableWindowReceiveInput;
    }
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::_IsEnabled
//
//----------------------------------------------------------------------------

BOOL CBaseWindow::_IsEnabled()
{
    if (_wndHandle != nullptr)
    {
        return IsWindowEnabled(_wndHandle);
    }
    else
    {
        return _enableVirtualWnd;
    }
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::_InvalidateRect
//
//----------------------------------------------------------------------------

void CBaseWindow::_InvalidateRect()
{
    if (_wndHandle != nullptr)
    {
        InvalidateRect(_wndHandle, NULL, TRUE);
    }
    else
    {
        CBaseWindow *pobj = _pParentWnd;
        while (pobj != nullptr)
        {
            if (pobj->_wndHandle)
            {
                InvalidateRect(pobj->_wndHandle, &_RectOfVirtualWnd, TRUE);
                break;
            }
            pobj = pobj->_pParentWnd;
        }
    }
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::_GetWindowRect
//
//----------------------------------------------------------------------------

BOOL CBaseWindow::_GetWindowRect(_Inout_ LPRECT lpRect)
{
    if (_wndHandle != nullptr)
    {
        return GetWindowRect(_wndHandle, lpRect);
    }
    else
    {
        *lpRect = _RectOfVirtualWnd;
        return TRUE;
    }
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::_GetClientRect
//
//----------------------------------------------------------------------------

BOOL CBaseWindow::_GetClientRect(_Inout_ LPRECT lpRect)
{
    if (_wndHandle != nullptr)
    {
        return GetClientRect(_wndHandle, lpRect);
    }
    else
    {
        *lpRect = _RectOfVirtualWnd;
        return TRUE;
    }
}

//+---------------------------------------------------------------------------
//
// _GetWindowExtent
//
//----------------------------------------------------------------------------

HRESULT CBaseWindow::_GetWindowExtent(_In_ const RECT *prcTextExtent, _In_opt_ RECT *prcCandidateExtent, _Inout_ POINT *pptCandidate)
{
	debugPrint(L"CBaseWindow::_GetWindowExtent, prcTextExtent top = %d, bottom = %d, right = %d, left = %d",
		prcTextExtent->top, prcTextExtent->bottom, prcTextExtent->right, prcTextExtent->left);
	debugPrint(L"CBaseWindow::_GetWindowExtent, prcCandidateExtent top = %d, bottom = %d, right = %d, left = %d", 
		prcCandidateExtent->top, prcCandidateExtent->bottom, prcCandidateExtent->right, prcCandidateExtent->left);
    
	if(prcTextExtent == nullptr) return E_INVALIDARG;

	RECT rcWorkArea = {0, 0, 0, 0};

    // Get work area
    GetWorkAreaFromPoint(*(LPPOINT)&prcTextExtent->left, &rcWorkArea);

    // Calc candidate window extent
    if (prcCandidateExtent)
    {
        CalcFitPointAroundTextExtent(prcTextExtent, &rcWorkArea, prcCandidateExtent, pptCandidate);
    }

    return S_OK;
}

//+---------------------------------------------------------------------------
//
// CalcFitPointAroundTextExtent
//
//----------------------------------------------------------------------------

void CBaseWindow::CalcFitPointAroundTextExtent(_In_ const RECT *prcTextExtent, _In_ const RECT *prcWorkArea, _In_ const RECT *prcWindow, _Out_ POINT *ppt)
{
	debugPrint(L"CBaseWindow::CalcFitPointAroundTextExtent, prcWorkArea top = %d, bottom = %d, right = %d, left = %d",
		prcWorkArea->top, prcWorkArea->bottom, prcWorkArea->right, prcWorkArea->left);
	
	if(prcTextExtent == nullptr || prcWindow == nullptr || ppt == nullptr) return;
    RECT rcTargetWindow[2];
    DWORD dwFlags[2];

    // set rcTargetWindow[0] which rectangle attached on bottom side of text extent
    rcTargetWindow[0] = *prcWindow;
    OffsetRect(&rcTargetWindow[0], prcTextExtent->left, prcTextExtent->bottom);

	debugPrint(L"CBaseWindow::CalcFitPointAroundTextExtent, attach top side: top = %d, bottom = %d, right = %d, left = %d",
		rcTargetWindow[0].top, rcTargetWindow[0].bottom, rcTargetWindow[0].right, rcTargetWindow[0].left);
	

    // set rcTargetWindow[1] which rectangle attached on top side of text extent
    rcTargetWindow[1] = *prcWindow;
    OffsetRect(&rcTargetWindow[1], prcTextExtent->left, prcTextExtent->top - (prcWindow->bottom - prcWindow->top));

	debugPrint(L"CBaseWindow::CalcFitPointAroundTextExtent, attach bottom side: top = %d, bottom = %d, right = %d, left = %d",
		rcTargetWindow[1].top, rcTargetWindow[1].bottom, rcTargetWindow[1].right, rcTargetWindow[1].left);
	

    //
    // check target rectangle fit in workarea
    //
    for (DWORD i = 0; i < ARRAYSIZE(rcTargetWindow); i++)
    {
        dwFlags[i] = RectInRect(prcWorkArea, &rcTargetWindow[i]);
		
		debugPrint(L"CBaseWindow::CalcFitPointAroundTextExtent, dwFlags[%d] = %x",i, dwFlags[i]);

        // generally, rcTargetWindow[0] or rcTargetWindow[1] have neither of these flags
        // but in the case where window is just complete way off the screen e.g. user
        // starts input, then drag window way off screen, both of these will be true
        if ((dwFlags[i] & RECT_OVER_TOP) || (dwFlags[i] & RECT_OVER_BOTTOM))
        {
            continue;
        }

        if (dwFlags[i] == RECT_INSIDE)
        {
            *ppt = *(POINT*)&rcTargetWindow[i].left;
            return;
        }
        else if ((dwFlags[i] & RECT_OVER_LEFT) != 0)
        {
            ppt->x = 0;
            ppt->y = rcTargetWindow[i].top;
            return;
        }
        else if ((dwFlags[i] & RECT_OVER_RIGHT) != 0)
        {
            ppt->x = prcWorkArea->right - (prcWindow->right - prcWindow->left);
            ppt->y = rcTargetWindow[i].top;
            return;
        }
    }

    // if both rcTargetWindow have RECT_OVER_TOP or RECT_OVER_BOTTOM,
    // then "dock" the window to top or bottom of working area.
    if ((dwFlags[0] & RECT_OVER_TOP) != 0)
    {
        ppt->y = 0;
    }
    else 
    {
        ppt->y = prcWorkArea->bottom - (prcWindow->bottom - prcWindow->top);
    }

    // dock to left/right edge if RECT_OVER_LEFT or RECT_OVER_RIGHT.
    // else just stay where we are
    if ((dwFlags[0] & RECT_OVER_LEFT) != 0)
    {
        ppt->x = 0;
    }
    else if ((dwFlags[0] & RECT_OVER_RIGHT) != 0)
    {
        ppt->x = prcWorkArea->right - (prcWindow->right - prcWindow->left);
    }
    else
    {
        ppt->x = prcTextExtent->left;
    }

    return;
}

//+---------------------------------------------------------------------------
//
// RectInRect
//
//----------------------------------------------------------------------------

DWORD CBaseWindow::RectInRect(_In_ const RECT *prcLimit, _In_ const RECT *prcTarget)
{
	if(prcLimit == nullptr || prcTarget == nullptr) return 0;
    DWORD dwFlags = 0;
    // Check if prcTarget is entirely inside prcLimit
    if (prcLimit->left <= prcTarget->left && prcTarget->right  <= prcLimit->right &&
        prcLimit->top  <= prcTarget->top  && prcTarget->bottom <= prcLimit->bottom)
    {
        return RECT_INSIDE;
    }

    // Check horizontal range.  Target can be
    // - wider than the limit (assert here since it should never happen)
    // - entirely outside the limit (RECT_OVERLEFT or RECT_OVERRIGHT)
    // - partially inside the limit (RECT_OVERLEFT or RECT_OVERRIGHT)
    if (prcTarget->left < prcLimit->left && prcTarget->right > prcLimit->right)
    {
        assert(FALSE);
        dwFlags |= RECT_TOO_WIDE;
    }
    else if (prcTarget->left < prcLimit->left) 
    {
        dwFlags |= RECT_OVER_LEFT;
    }
    else if (prcTarget->right > prcLimit->right)
    {
        dwFlags |= RECT_OVER_RIGHT;
    }

    // Check vertical range.  Target can be
    // - taller than the limit (assert here since it should never happen)
    // - entirely outside the limit (RECT_OVERTOP or RECT_OVERBOTTOM)
    // - partially inside the limit (RECT_OVERTOP or RECT_OVERBOTTOM)
    if (prcTarget->top < prcLimit->top && prcTarget->bottom > prcLimit->bottom)
    {
        assert(FALSE);
        dwFlags |= RECT_TOO_TALL;
    }
    else if (prcTarget->top < prcLimit->top)
    {
        dwFlags |= RECT_OVER_TOP;
    }
    else if (prcTarget->bottom > prcLimit->bottom)
    {
        dwFlags |= RECT_OVER_BOTTOM;
    }

    return dwFlags;
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::_NotifyCommand
//
//----------------------------------------------------------------------------

LRESULT CBaseWindow::_NotifyCommand(UINT uMsg, DWORD dwSB, int nPos)
{
    CBaseWindow* pUIWnd = _GetUIWnd();
    if (pUIWnd && pUIWnd->_GetWnd())
    {
        WPARAM wParam = MAKEWPARAM(dwSB, nPos);
        if (pUIWnd->_GetWnd())
        {
            return SendMessage(pUIWnd->_GetWnd(), uMsg, wParam, (LPARAM)0);
        }
    }
    return 0;
}

//+---------------------------------------------------------------------------
//
// _SetCaptureObject
//
//----------------------------------------------------------------------------

void CBaseWindow::_SetCaptureObject(_In_opt_ CBaseWindow *pUIObj)
{
    CBaseWindow* pUIWnd = _GetTopmostUIWnd();
    if (nullptr == pUIWnd)
    {
        return;
    }

    pUIWnd->_pUIObjCapture = pUIObj;
    if (pUIObj != nullptr)
    { 
        SetCapture(pUIWnd->_GetWnd());
    }
    else
    {
        ReleaseCapture();
    }
}

//+---------------------------------------------------------------------------
//
// _SetTimerObject
//
//----------------------------------------------------------------------------

void CBaseWindow::_SetTimerObject(_In_opt_ CBaseWindow *pUIObj, UINT uElapse, _In_opt_ UINT_PTR timerID)
{
    CBaseWindow* pUIWnd = _GetTopmostUIWnd();
    if (nullptr == pUIWnd)
    {
        return;
    }

  
    if (pUIObj != nullptr)
    {
		pUIWnd->_pTimerUIObj = pUIObj;
        SetTimer(pUIWnd->_GetWnd(), timerID, uElapse,  NULL);
    }
    else
    {
		if(timerID == DEFAULT_TIMER_ID) pUIWnd->_pTimerUIObj = nullptr;
        KillTimer(pUIWnd->_GetWnd(), timerID);
    }
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::_WindowProc
//
// CBaseWindow window proc.
//----------------------------------------------------------------------------

/* static */
LRESULT CALLBACK CBaseWindow::_WindowProc(_In_ HWND wndHandle, UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (uMsg == WM_CREATE && lParam)
    {
        _SetThis(wndHandle, ((CREATESTRUCT*)lParam)->lpCreateParams);
    }

    CBaseWindow* pv = _GetThis(wndHandle);
    if (pv == nullptr)
    {
        return DefWindowProc(wndHandle, uMsg, wParam, lParam);
    }

    if (uMsg == WM_TIMER)
    {
        switch (wParam)
        {
        case DEFAULT_TIMER_ID:
            if (pv->_GetTimerObject())
            {
                pv->_GetTimerObject()->_OnTimer();
            }
            break;
		default:
			if (pv->_GetTimerObject())
            {
				pv->_GetTimerObject()->_OnTimerID((UINT_PTR) wParam);
            }
        }
        return 0;
    }
    else
    {
        return pv->_WindowProcCallback(wndHandle, uMsg, wParam, lParam);
    }
}

//+---------------------------------------------------------------------------
//
// CBaseWindow::GetWorkAreaFromPoint
//
//----------------------------------------------------------------------------

void CBaseWindow::GetWorkAreaFromPoint(_In_ const POINT& ptPoint, _Out_ LPRECT lprcWorkArea)
{
	debugPrint(L"CBaseWindow::GetWorkAreaFromPoint, point = x = %d, y = %d", ptPoint.x, ptPoint.y);

	
    if (lprcWorkArea == nullptr)
    {
        return;
    }

    lprcWorkArea->left = 0;
    lprcWorkArea->top = 0;
    lprcWorkArea->right = 0;
    lprcWorkArea->bottom = 0;

	
    HMONITOR hMonitor = MonitorFromPoint(ptPoint, MONITOR_DEFAULTTONEAREST);
    if (hMonitor)
    {
        MONITORINFO MonitorInfo = {0};

        MonitorInfo.cbSize = sizeof(MONITORINFO);
        if (GetMonitorInfo(hMonitor, &MonitorInfo))
        {
			debugPrint(L"CBaseWindow::GetWorkAreaFromPoint from GetMonitorInfo rcmonitor top = %d, bottom = %d, right = %d, left = %d",
				MonitorInfo.rcMonitor.top, MonitorInfo.rcMonitor.bottom, MonitorInfo.rcMonitor.right, MonitorInfo.rcMonitor.left);
            *lprcWorkArea = MonitorInfo.rcWork;

			if(! (ptPoint.x < lprcWorkArea->right && ptPoint.x > lprcWorkArea->left))  // in store app snapeed view. rcworkarea is the desktop view, but not current app
			{
				if(ptPoint.x > lprcWorkArea->right)
					lprcWorkArea->right = MonitorInfo.rcMonitor.right;
				if(ptPoint.x < lprcWorkArea->left)
					lprcWorkArea->left = MonitorInfo.rcMonitor.left;
			}

			debugPrint(L"CBaseWindow::GetWorkAreaFromPoint from GetMonitorInfo rect top = %d, bottom = %d, right = %d, left = %d",
					lprcWorkArea->top, lprcWorkArea->bottom, lprcWorkArea->right, lprcWorkArea->left);
	
            return;
        }
    }
	

    SystemParametersInfo(SPI_GETWORKAREA, 0, lprcWorkArea, 0);
	debugPrint(L"CBaseWindow::GetWorkAreaFromPoint from SystemParametersInfo rect top = %d, bottom = %d, right = %d, left = %d",
					lprcWorkArea->top, lprcWorkArea->bottom, lprcWorkArea->right, lprcWorkArea->left);
	
    return;
}

BOOL CBaseWindow::_IsTimer()
{
    CBaseWindow* pobj = _GetTopmostUIWnd();
    if (pobj)
    {
        return (pobj->_pTimerUIObj != nullptr);
    }
    return FALSE;
}

BOOL CBaseWindow::_IsCapture()
{
    CBaseWindow* pobj = _GetTopmostUIWnd();
    if (pobj)
    {
        return (pobj->_pUIObjCapture != nullptr);
    }
    return FALSE;
}

CBaseWindow* CBaseWindow::_GetTopmostUIWnd()
{
    CBaseWindow* pobj = this;

    while (pobj->_pParentWnd)
    {
        pobj = pobj->_pParentWnd;
    }

    return pobj->_pUIWnd;
}