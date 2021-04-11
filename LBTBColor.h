#pragma once

#define LBTBC_API EXTERN_C __declspec(dllexport)

LBTBC_API BOOL APIENTRY SetTextboxColorsRGB(HWND hTextbox, DWORD textRed, DWORD textGreen, DWORD textBlue,
	DWORD bgRed, DWORD bgGreen, DWORD bgBlue);

LBTBC_API BOOL APIENTRY SetTextboxColors(HWND hTextbox, COLORREF text, COLORREF bg);

LBTBC_API BOOL APIENTRY ResetTextboxColors(HWND hTextbox);

LBTBC_API BOOL APIENTRY SetTextboxTextColor(HWND hTextbox, COLORREF text);

LBTBC_API BOOL APIENTRY SetTextboxBGColor(HWND hTextbox, COLORREF bg);

LBTBC_API BOOL APIENTRY SetTextboxTextColorRGB(HWND hTextbox, DWORD textRed, DWORD textGreen, DWORD textBlue);

LBTBC_API BOOL APIENTRY SetTextboxBGColorRGB(HWND hTextbox, DWORD bgRed, DWORD bgGreen, DWORD bgBlue);

