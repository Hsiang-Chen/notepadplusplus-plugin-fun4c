/* $ @fundlg.h
 *
 * Copyright(C)2016 Sean Chen (³ÂÏé)
 * This program may be freely redistributed under the terms of the GNU General Public License.
 * */
 
#ifndef _FUNDLG_H_
#define _FUNDLG_H_

#include <windows.h>
#include <commctrl.h>
#include <vector>

#include "DockingDlgInterface.h"
#include "Scintilla.h"
#include "settings.h"
#include "resource.h"

using namespace std;

class FuncsPluginDialog : public DockingDlgInterface
{
	public:
		FuncsPluginDialog() : \
		DockingDlgInterface(IDD_FUN4C), \
		pListViewOldWndProc(0), \
		pSciMsg(0), pSciWndData(0), \
		hScintillaM(0), hScintillaS(0), \
		settings(0), nCurView(MAIN_VIEW), \
		hListView(0), hComboBox(0), hButton(0) {};
		
		~FuncsPluginDialog() {
			freeInsertedItems();
		};
		
		virtual void display(bool toShow = true) const {
			DockingDlgInterface::display(toShow);
		};
		
		void setSettings(fun4c_settings* sets) {
			settings = sets;
		};
		
		void nppview(UINT sc) {
			nCurView = sc;
		};
		
		void nppScintilla(HWND m, HWND s) {
			hScintillaM = m;
			hScintillaS = s;
		};
		
		void addListView(HWND hWnd, uptr_t id);
		void delListView(HWND hWnd, uptr_t id);
		
	protected:
	
		UINT FuncsPluginDialog::DecStringToInt(TCHAR* str);
		
		virtual sptr_t CallScintilla(unsigned int iMessage, uptr_t wParam = 0, sptr_t lParam = 0) {
			UINT sc = MAIN_VIEW;
			::SendMessage(_hParent, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&sc);
			if (sc != nCurView) {
				nCurView = sc;
				pSciMsg = NULL;
				pSciWndData = NULL;
			}
			if (pSciMsg == NULL) {
				pSciMsg = (SciFnDirect)::SendMessage(nCurView == MAIN_VIEW ? hScintillaM : hScintillaS, SCI_GETDIRECTFUNCTION, 0, 0);
			}
			if (pSciWndData == NULL) {
				pSciWndData = (sptr_t)::SendMessage(nCurView == MAIN_VIEW ? hScintillaM : hScintillaS, SCI_GETDIRECTPOINTER, 0, 0);
			}
			return pSciMsg(pSciWndData, iMessage, wParam, lParam);
		};
		
		virtual void freeInsertedItems();
		virtual BOOL isInserted(TCHAR* item);
		virtual void updateDockingDlg();
		virtual BOOL CALLBACK run_listviewDlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual BOOL CALLBACK run_dlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		
		static BOOL CALLBACK ListViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			if (uMsg == WM_CREATE) {
				SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
				return FALSE;
			} else {
				FuncsPluginDialog* dlg = reinterpret_cast<FuncsPluginDialog*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
				if (dlg != NULL) return dlg->run_listviewDlgProc(uMsg, wParam, lParam);
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		};

	private:
		fun4c_settings* settings;
		UINT nCurView;
		HWND hScintillaM;
		HWND hScintillaS;
		////
		HWND hListView;
		HWND hComboBox;
		HWND hButton;
		////
		SciFnDirect pSciMsg;
		sptr_t pSciWndData;
		////
		vector<TCHAR*> items;
		////
		LONG_PTR pListViewOldWndProc;
};

#endif

// end.
 