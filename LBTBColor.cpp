#include "pch.h"
#include "LBTBColor.h"

#ifdef _DEBUG
HWND hNotepadLog = NULL;
VOID WriteDebug(LPCWSTR logData);
#endif

#pragma comment(lib, "comctl32.lib")

LRESULT CALLBACK LBWinSubProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK LBTBSubProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

BOOL setTextboxColorsInternal(HWND hTextbox, BOOL useTextColor, COLORREF textColor, BOOL useBGColor, COLORREF bgColor);

LBTBC_API BOOL APIENTRY SetTextboxColorsRGB(HWND hTextbox, DWORD textRed, DWORD textGreen, DWORD textBlue,
	DWORD bgRed, DWORD bgGreen, DWORD bgBlue)
{
	return SetTextboxColors(hTextbox, RGB(textRed, textGreen, textBlue), RGB(bgRed, bgGreen, bgBlue));
}

LBTBC_API BOOL APIENTRY SetTextboxTextColor(HWND hTextbox, COLORREF text)
{
	return setTextboxColorsInternal(hTextbox, TRUE, text, FALSE, 0);
}

LBTBC_API BOOL APIENTRY SetTextboxBGColor(HWND hTextbox, COLORREF bg)
{
	return setTextboxColorsInternal(hTextbox, FALSE, 0, TRUE, bg);
}

LBTBC_API BOOL APIENTRY SetTextboxTextColorRGB(HWND hTextbox, DWORD textRed, DWORD textGreen, DWORD textBlue)
{
	return setTextboxColorsInternal(hTextbox, TRUE, RGB(textRed, textGreen, textBlue), FALSE, 0);
}

LBTBC_API BOOL APIENTRY SetTextboxBGColorRGB(HWND hTextbox, DWORD bgRed, DWORD bgGreen, DWORD bgBlue)
{
	return setTextboxColorsInternal(hTextbox, FALSE, 0, TRUE, RGB(bgRed, bgGreen, bgBlue));
}


BOOL subclassParent(HWND hTextbox)
{
	if (!hTextbox) return FALSE;

	HWND hParent = GetParent(hTextbox);

	// Check if we've already subclassed the window, and set the subclass otherwise
	DWORD* numTextboxes = 0;

	// LB's top-level windows are ANSI-only, so we hard-code ANSI-version function calls
	// when interacting with it.
	WNDPROC prevWndProc = (WNDPROC)GetPropA(hParent, "LBTBC_PrevWndProc");
	if (prevWndProc == NULL)
	{
		// We keep track of the number of textboxes we're working with on each window
		// If we're no longer tracking any textboxes, we'll remove the subclass
		numTextboxes = new DWORD;
		*numTextboxes = 0;

		// If we fail to set the property that keeps track of the textboxes,
		// we abort the process.
		if (!SetPropA(hParent, "LBTBC_TBCount", numTextboxes))
		{
			delete numTextboxes;
			return FALSE;
		}

		// Because LB's main windows are ANSI-only windows, we cannot use the SetWindowSubclass()
		// helper functions to subclass it, because they are Unicode-only.
		//
		// Learned that one the hard way.
		prevWndProc = (WNDPROC)SetWindowLongPtrA(hParent, GWLP_WNDPROC, (LONG_PTR)LBWinSubProc);

		// Same as above, if we fail to store the previous window procedure(or fail to subclass),
		// we cannot properly maintain the subclass.  We undo the work we've done so far, 
		// and return failure.
		if (!prevWndProc || !SetPropA(hParent, "LBTBC_PrevWndProc", prevWndProc))
		{
			if (prevWndProc) SetWindowLongPtrA(hParent, GWLP_WNDPROC, (LONG_PTR)prevWndProc);

			RemovePropA(hParent, "LBTBC_TBCount");
			delete numTextboxes;

			return FALSE;
		}
	}

	return TRUE;
}

BOOL setTextboxColorsInternal(HWND hTextbox, BOOL useTextColor, COLORREF textColor, BOOL useBGColor, COLORREF bgColor)
{
	if (!hTextbox) return FALSE;

	if (!subclassParent(hTextbox)) return FALSE;

	HWND hParent = GetParent(hTextbox);
	DWORD refData = 0;

	DWORD* numTextboxes = (DWORD*)GetPropA(hParent, "LBTBC_TBCount");

	// If we can't properly keep track of the number of textboxes we're managing,
	// we abort.
	if (!numTextboxes)
	{
		return FALSE;
	}

	// Check if we've already subclassed the textbox, and set the subclass otherwise
	// (We set a subclass on the textbox as well, so we know when it's being destroyed to remove
	// the properties we set to keep track of the colors before destruction)
	//
	// We CAN use the SetWindowSubclass() helper functions here, because textboxes are
	// provided by Windows, and DO support Unicode, even if LB doesn't use it.
	if (!GetWindowSubclass(hTextbox, LBTBSubProc, 0, &refData))
	{
		if (!SetWindowSubclass(hTextbox, LBTBSubProc, 0, 0))
		{
			return FALSE;
		}

		// We haven't subclassed this textbox, which means it's a new one.
		// Add it to the count.
		(*numTextboxes)++;
	}

	// Increment the colors so we know the difference between pure black
	// and the property not being set. (We'll decrement in the message handler
	// before setting the colors.)
	if (useTextColor)
	{
		if (!SetProp(hTextbox, L"LBTBC_TextColor", (HANDLE)(textColor + 1)))
		{
			return FALSE;
		}
	}

	if (useBGColor)
	{
		if (!SetProp(hTextbox, L"LBTBC_BGColor", (HANDLE)(bgColor + 1)))
		{
			return FALSE;
		}
	}

	RedrawWindow(hParent, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	return TRUE;
}

LBTBC_API BOOL APIENTRY SetTextboxColors(HWND hTextbox, COLORREF textColor, COLORREF bgColor)
{
	return setTextboxColorsInternal(hTextbox, TRUE, textColor, TRUE, bgColor);
}

LBTBC_API BOOL APIENTRY ResetTextboxColors(HWND hTextbox)
{
	if (!hTextbox) return FALSE;

	RemoveProp(hTextbox, L"LBTBC_BGColor");
	RemoveProp(hTextbox, L"LBTBC_TextColor");

	RemoveWindowSubclass(hTextbox, LBTBSubProc, 0);

	HWND hParent = GetParent(hTextbox);
	DWORD* numTextboxes = NULL;
	
	numTextboxes = (DWORD*)GetPropA(hParent, "LBTBC_TBCount");

	// No longer changing any textbox on the window, so let's remove
	// the subclass and free the memory for numTextboxes
	if (-- * numTextboxes == 0)
	{
		// Check if we're currently the top-level subclass on the window.
		// If we are, it's safe to unsubclass.  If not, we have to leave ourselves in place,
		// or we will break other subclassing that might be happening.
		//
		// We only have to do this with the main LB window because we're not using the newer
		// subclassing helper functions.  RemoveWindowSubclass() takes care of this check
		// for you, so it's not needed for the textbox subclasses.
		WNDPROC currentWndProc = (WNDPROC)GetWindowLongPtrA(hParent, GWLP_WNDPROC);
		if (currentWndProc == LBWinSubProc)
		{
			WNDPROC prevWndProc = (WNDPROC)GetPropA(hParent, "LBTBC_PrevWndProc");
			SetWindowLongPtrA(hParent, GWLP_WNDPROC, (LONG_PTR)prevWndProc);
			delete numTextboxes;
		}

	}

	RedrawWindow(hParent, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	return TRUE;
}

LRESULT CALLBACK LBWinSubProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC prevWndProc = (WNDPROC)GetPropA(hWnd, "LBTBC_PrevWndProc");

	if (uMsg == WM_CTLCOLOREDIT)
	{
		HWND hTextbox = (HWND)lParam;
		HDC hDC = (HDC)wParam;
		COLORREF textColor = 0;
		COLORREF bgColor = 0;

		textColor = (COLORREF)GetProp(hTextbox, L"LBTBC_TextColor");
		bgColor = (COLORREF)GetProp(hTextbox, L"LBTBC_BGColor");

		if (textColor != NULL || bgColor != NULL)
		{
			HBRUSH hBrush = NULL;

			if (textColor)
			{
				textColor--;
				SetTextColor(hDC, textColor);
			}

			if (bgColor)
			{
				bgColor--;
				SetBkColor(hDC, bgColor);

				hBrush = GetStockBrush(DC_BRUSH);
				SelectBrush(hDC, hBrush);
			}

			return (LRESULT)hBrush;
		}
	}
	else if (uMsg == WM_NCDESTROY)
	{
		// In theory, we shouldn't reach this, since the textboxes should remove themselves
		// during their destruction, resulting in this subclass being removed once the last
		// textbox is removed.  However, just in case, we'll remove ourselves here and free
		// the counter memory.
		SetWindowLongPtrA(hWnd, GWLP_WNDPROC, (LONG_PTR)prevWndProc);

		DWORD* numTextboxes = (DWORD*)GetPropA(hWnd, "LBTBC_TBCount");
		if (numTextboxes) delete numTextboxes;
	}

	return CallWindowProcA(prevWndProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK LBTBSubProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	// Clear up the properties and subclass when destroying the textboxes
	if (uMsg == WM_NCDESTROY)
	{
		ResetTextboxColors(hWnd);
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

#ifdef _DEBUG
VOID WriteDebug(LPCWSTR logData)
{
	if (!hNotepadLog)
	{
		HWND hNotepad = FindWindow(NULL, L"Untitled - Notepad");
		hNotepadLog = GetWindow(hNotepad, GW_CHILD);
	}

	SendMessage(hNotepadLog, EM_REPLACESEL, 0, (LPARAM)logData);
}
#endif