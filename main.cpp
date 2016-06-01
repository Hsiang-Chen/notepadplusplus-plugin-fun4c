/* $ @main.c
 *
 * Copyright(C)2016 Hsiang Chen (³ÂÏé)
 * This program may be freely redistributed under the terms of the GNU General Public License.
 * */
 
#include <windows.h>

#include "PluginInterface.h"
#include "fundlg.h"
#include "resource.h"
#include "Scintilla.h"

#define PLUGIN_NAME "Fun4c"

NppData nppData;

UINT newSC = MAIN_VIEW;

HINSTANCE hInstance;
HICON hIcon;

const INT funcs_count = 3;
FuncItem funcs[funcs_count];

FuncsPluginDialog fundlg;
fun4c_settings settings;

////
void show_funDlg();
void show_aboutDlg();

BOOL WINAPI DllEntryPoint(HINSTANCE hInstDll, DWORD dwReason, LPVOID res)
{
	hInstance = hInstDll;
	
	switch (dwReason) {
		case DLL_PROCESS_ATTACH:
			{	
				funcs[0]._pFunc = show_funDlg;
				funcs[1]._pFunc = NULL;
				funcs[2]._pFunc = show_aboutDlg;
				
				lstrcpyn(funcs[0]._itemName, TEXT("C Functions..."), sizeof(funcs[0]._itemName));
				lstrcpyn(funcs[2]._itemName, TEXT("About..."), sizeof(funcs[0]._itemName));
				
				hIcon = LoadIcon(hInstDll, MAKEINTRESOURCE(IDI_FUN4C));
			}
		break;
		case DLL_PROCESS_DETACH:
			{
				// ±£´æÉèÖÃ /
				settings.save();
				fundlg.destroy();
			}
		break;
		case DLL_THREAD_ATTACH:
		break;
		case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID res)
{
	return DllEntryPoint(hModule, dwReason, res);
}

extern "C" __declspec(dllexport) void setInfo(NppData dat)
{
	nppData = dat;
	
	// ¼ÓÔØÉèÖÃ /
	settings.npp(nppData._nppHandle);
	settings.load();
}

extern "C" __declspec(dllexport) const TCHAR* getName()
{
	return PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem* getFuncsArray(int* num)
{
	*num = funcs_count;
	return funcs;
}

extern "C" __declspec(dllexport) void beNotified(SCNotification* notify)
{
	if (notify->nmhdr.code == NPPN_FILEOPENED) {
		fundlg.addListView((HWND)notify->nmhdr.hwndFrom, notify->nmhdr.idFrom);
	} else if (notify->nmhdr.code == NPPN_FILEBEFORECLOSE) {
		fundlg.delListView((HWND)notify->nmhdr.hwndFrom, notify->nmhdr.idFrom);
	} else if (notify->nmhdr.code == NPPN_BUFFERACTIVATED) {
		fundlg.addListView((HWND)notify->nmhdr.hwndFrom, notify->nmhdr.idFrom);
	}
}

extern "C" __declspec(dllexport) LRESULT messageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

#ifdef UNICODE

extern "C" __declspec(dllexport) BOOL isUnicode()
{
	return TRUE;
}

#endif

////

void show_funDlg()
{	
	::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&newSC);
	
	if (! fundlg.isCreated()) {
		tTbData data = {0};
	
		fundlg.init(hInstance, nppData._nppHandle);
		fundlg.setSettings(&settings);
		fundlg.nppview(newSC);
		fundlg.nppScintilla(nppData._scintillaMainHandle, nppData._scintillaSecondHandle);
		fundlg.create(&data);
		
		data.pszName = /* PLUGIN_NAME */ TEXT("Functions...");
		data.dlgID = 0;
		data.uMask = DWS_DF_CONT_LEFT;
		data.hIconTab = hIcon;
		data.pszModuleName = fundlg.getPluginFileName();
		
		::SendMessage(nppData._nppHandle, NPPM_DMMREGASDCKDLG, 0, (LPARAM)&data);
	} else {
		fundlg.display(true);
	}
}

void show_aboutDlg()
{
	::ShellAbout(nppData._nppHandle, TEXT("About...#fun4c (Notepad++ Plugin)"), TEXT("Copyright (C) 2016 Hsiang Chen ( ³ÂÏé )"), hIcon);
}

// end.
