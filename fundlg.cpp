/* $ @fundlg.c
 *
 * Copyright(C)2016 Hsiang Chen (陈祥)
 * This program may be freely redistributed under the terms of the GNU General Public License.
 * */

#include <windows.h>
#include <commctrl.h>

#include "fundlg.h"
#include "Notepad_plus_msgs.h"
#include "Scintilla.h"
#include "resource.h"

#define INS_ITEM(h00,i00,s00,t00,p00) { \
	LV_ITEM lvi = {0}; \
	lvi.mask = LVIF_TEXT; \
	lvi.iItem = i00; \
	lvi.pszText = s00; \
	ListView_InsertItem(h00, &lvi); \
	lvi.iSubItem++; \
	lvi.pszText = t00; \
	ListView_SetItem(h00, &lvi); \
	lvi.iSubItem++; \
	lvi.pszText = p00; \
	ListView_SetItem(h00, &lvi); \
}

void FuncsPluginDialog::updateDockingDlg()
{
	RECT rect = {0}, rect2 = {0};
	size_t num = 0;
	
	if (::GetClientRect(_hSelf, &rect)) {
		rect.left += 2;
		rect.top += 2;
		rect.right -= 4;
		rect.bottom -= 4;
		::MoveWindow(hComboBox, rect.left, rect.top, rect.right - 38, 24, TRUE);
		::GetWindowRect(hComboBox, &rect2);
		rect2.bottom -= rect2.top;
		::MoveWindow(hButton, rect.right - 34, rect.top, 34, rect2.bottom, TRUE);
		::MoveWindow(hListView, rect.left, rect.top + rect2.bottom + 2, rect.right, rect.bottom - rect2.bottom - 2, TRUE);
		::GetWindowRect(hListView, &rect2);
		ListView_SetColumnWidth(hListView, 0, (rect2.right - rect2.left) * 0.22);
		ListView_SetColumnWidth(hListView, 1, (rect2.right - rect2.left) * 0.12);
		ListView_SetColumnWidth(hListView, 2, (rect2.right - rect2.left) * 0.54);
	}
	
	////
	if ((num = settings->regex().size()) > 0 && ::IsWindow(hListView)) {
		LONG len = (LONG)CallScintilla(SCI_GETTEXTLENGTH);
		struct Sci_TextToFind scittf = {0};
		TCHAR* buf = new TCHAR[MAX_PATH];
		UINT ln = 0;
		
		scittf.chrg.cpMin = 0;
		scittf.chrg.cpMax = len;
		
		if (buf != NULL && len > 0) {
			freeInsertedItems();
			ListView_DeleteAllItems(hListView);
			for (size_t i = 0; i < num; i++) {
				scittf.lpstrText = settings->regex()[i]->text;
				while (1)
				if (CallScintilla(SCI_FINDTEXT, SCFIND_REGEXP | SCFIND_MATCHCASE | SCFIND_POSIX, (sptr_t)&scittf) != -1) {
					// 如果找到 /
					if (scittf.chrgText.cpMax - scittf.chrgText.cpMin > MAX_PATH) {
						delete[] buf;
						buf = new TCHAR[((scittf.chrg.cpMax - scittf.chrg.cpMin) / 4 + 1) * 4];
					}
					if (buf != NULL) {
						struct Sci_TextRange scitr = {0};
						
						scitr.chrg.cpMin = scittf.chrgText.cpMin;
						scitr.chrg.cpMax = scittf.chrgText.cpMax;
						scitr.lpstrText = buf;
						if (CallScintilla(SCI_GETTEXTRANGE, 0, (sptr_t)&scitr) > 0) {
							TCHAR  suf[2][MAX_PATH] = {0};
							
							lstrcpyn(suf[0], settings->regex()[i]->type, sizeof(suf));
							wsprintf(suf[1], TEXT("%d"), (UINT)CallScintilla(SCI_LINEFROMPOSITION, scittf.chrgText.cpMin) + 1);
							if (! isInserted(suf[1])) {
								INS_ITEM(hListView, ln++, suf[1], suf[0], buf);
							}
						}
					}
					scittf.chrg.cpMin = scittf.chrgText.cpMax;
				} else break;
			}
		}
		
		if (buf != NULL) {
			delete[] buf;
		}
	}
}

void FuncsPluginDialog::freeInsertedItems()
{
	for (size_t i = 0; i < items.size(); i++) 
		if (items[i] != NULL)
			delete[] items[i];
	
	items.clear();
}

BOOL FuncsPluginDialog::isInserted(TCHAR* item)
{
	for (size_t i = 0; i < items.size(); i++)
		if (! lstrcmp(items[i], item))
			return TRUE;
	
	TCHAR* str = new TCHAR[MAX_PATH];
	if (str != NULL) {
		lstrcpyn(str, item, MAX_PATH);
		items.push_back(str);
	}
	
	return FALSE;
}

void FuncsPluginDialog::addListView(HWND hWnd, uptr_t id)
{
	if (::IsWindow(hComboBox) && ::IsWindow(hWnd)) {
		UINT len = ::SendMessage(hWnd, NPPM_GETFULLPATHFROMBUFFERID, id, NULL);
		
		if (len > 0 && len != -1) {
			TCHAR* ptr = new TCHAR[(len / 4 + 1) * 4];
			
			if (ptr != NULL) {
				::SendMessage(hWnd, NPPM_GETFULLPATHFROMBUFFERID, id, (LPARAM)ptr);
				if (::SendMessage(hComboBox, CB_FINDSTRING, 0, (LPARAM)ptr) == CB_ERR) {
					if (::PathFileExists(ptr)) {
						::SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)ptr);
					}
				}
				::SendMessage(hComboBox, CB_SELECTSTRING, 0, (LPARAM)ptr);
				delete[] ptr;
			}
		}
		
		updateDockingDlg();
	}
}

void FuncsPluginDialog::delListView(HWND hWnd, uptr_t id)
{
	if (::IsWindow(hComboBox) && ::IsWindow(hWnd)) {
		UINT len = ::SendMessage(hWnd, NPPM_GETFULLPATHFROMBUFFERID, id, NULL);
		
		if (len > 0 && len != -1) {
			TCHAR* ptr = new TCHAR[(len / 4 + 1) * 4]; UINT ix = CB_ERR;
			
			if (ptr != NULL) {
				::SendMessage(hWnd, NPPM_GETFULLPATHFROMBUFFERID, id, (LPARAM)ptr);
				if ((ix = ::SendMessage(hComboBox, CB_FINDSTRING, 0, (LPARAM)ptr)) != CB_ERR) {
					::SendMessage(hComboBox, CB_DELETESTRING, ix, 0);
					::SendMessage(hComboBox, CB_SETCURSEL, 0, 0);
				}
				delete[] ptr;
			}
		}
	}
}

UINT FuncsPluginDialog::DecStringToInt(TCHAR* str)
{
	char buf[0xff + 1] = {0};
	UINT len, i, val = 0;
	
	if (str == NULL) return 0;
	
	len = lstrlen(str);
	
#ifdef UNICODE
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, str, len, buf, sizeof(buf), NULL, NULL);
	// len = lstrlenA(buf); // 字符个数与Unicode字符个数一样的 /
#else
	lstrcpynA(buf, str, sizeof(buf));
#endif
	
	for (i = 0; i < len; i++) {
		if (buf[i] >= '0' && buf[i] <= '9') {
			val = val * 10 + buf[i] - '0';
		}
	}
	
	return val;
}

BOOL CALLBACK FuncsPluginDialog::run_listviewDlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		case WM_LBUTTONDBLCLK:
			{	TCHAR buf[MAX_PATH];
				UINT num = ListView_GetItemCount(hListView);
				for (size_t i = 0; i < num; i++) {
					if (ListView_GetItemState(hListView, i, LVIS_SELECTED) & LVIS_SELECTED) {
						ListView_GetItemText(hListView, i, 0, buf, sizeof(buf));
						UINT line = DecStringToInt(buf);
						CallScintilla(SCI_GOTOLINE, line - 1);
						::SetFocus(nCurView == MAIN_VIEW ? hScintillaM : hScintillaS);
					}
				}
			}
		break;
	}
	
	if (pListViewOldWndProc != NULL) return CallWindowProc((WNDPROC)pListViewOldWndProc, hListView, uMsg, wParam, lParam);
	else return DefWindowProc(hListView, uMsg, wParam, lParam);
}

BOOL CALLBACK FuncsPluginDialog::run_dlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		case WM_INITDIALOG:
			{
				hComboBox = ::GetDlgItem(_hSelf, IDC_COMBOBOX);
				hButton = ::GetDlgItem(_hSelf, IDOK);
				hListView = ::GetDlgItem(_hSelf, IDC_LISTVIEW);
			}
			if (::IsWindow(hListView) && ::IsWindow(hButton) && ::IsWindow(hComboBox)) {
				LV_COLUMN lvc = {0};
				
				ListView_DeleteAllItems(hListView);
				
				SetWindowLongPtr(hListView, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
				pListViewOldWndProc = SetWindowLongPtr(hListView, GWLP_WNDPROC, (LONG_PTR)ListViewProc);
				
				if (settings->uistr().size() > 2) {
					lvc.mask = LVCF_TEXT | LVCF_FMT;
					lvc.fmt = LVCFMT_CENTER;
					lvc.pszText = settings->uistr()[2]->text;
					ListView_InsertColumn(hListView, 0, &lvc);
					lvc.pszText = settings->uistr()[3]->text;
					ListView_InsertColumn(hListView, 0, &lvc);
					lvc.pszText = settings->uistr()[1]->text;
					ListView_InsertColumn(hListView, 0, &lvc);
					::SetWindowText(hButton, settings->uistr()[0]->text);
				}
				
				::SendMessage(hListView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
				::SendMessage(hComboBox, CB_RESETCONTENT, 0, 0);
			}
			updateDockingDlg();
			return TRUE;
		break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
					//INT n = (INT)::SendMessage(_hParent, NPPM_GETCURRENTLINE, 0, 0);
					//sptr_t a = CallScintilla(SCI_MARKERGET, n, 0);
					//::SetDlgItemInt(_hSelf, IDC_COMBOBOX, a, FALSE);
					//::SetDlgItemInt(_hSelf, 1000, n, FALSE);
					UINT ix = ::SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
					if (ix != CB_ERR) {
						TCHAR buf[MAX_PATH] = {0};
						if (::SendMessage(hComboBox, CB_GETLBTEXT, ix, (LPARAM)buf) != CB_ERR) {
							::SendMessage(_hParent, NPPM_DOOPEN, 0, (LPARAM)buf);
						}
					}
				break;
			}
		break;
		case WM_SIZE:
			updateDockingDlg();
			return TRUE;
		break;
	}
	
	return DockingDlgInterface::run_dlgProc(uMsg, wParam, lParam);
}