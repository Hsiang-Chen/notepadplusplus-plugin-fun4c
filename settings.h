/* $ @settings.h
 *
 * Copyright(C)2016 Sean Chen (³ÂÏé)
 * This program may be freely redistributed under the terms of the GNU General Public License.
 * */
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <vector>
#include <windows.h>

using namespace std;

////
#define GT_STR(a00,k00,s00,f00) GetPrivateProfileString(a00,k00,TEXT(""),s00,sizeof(s00),f00)
#define WT_STR(a00,k00,s00,f00) WritePrivateProfileString(a00,k00,s00,f00)
#define MAX_LEN MAX_PATH

struct String {
public:
	String(TCHAR* text) {
		UINT n = (lstrlen(text) / 4 + 1) * 4;
		if ((this->text = new TCHAR[n]) != NULL) {
			lstrcpyn(this->text, text, n);
		}
	};
	TCHAR* text;
};

struct REGEXCONF {
public:
	REGEXCONF(TCHAR* type, TCHAR* text) {
		UINT n = (lstrlen(type) / 4 + 1) * 4;
		if ((this->type = new TCHAR[n]) != NULL) {
			lstrcpyn(this->type, type, n);
		}
		n = (lstrlen(text) / 4 + 1) * 4;
		if ((this->text = new TCHAR[n]) != NULL) {
			lstrcpyn(this->text, text, n);
		}
	};
	TCHAR* type;
	TCHAR* text;
};

class fun4c_settings {
	public:
		fun4c_settings();
		~fun4c_settings();
		void load();
		void save();
		void npp(HWND hWnd);
		vector<REGEXCONF*> regex() {
			return regexes;
		};
		vector<String*> uistr() {
			return uistres;
		};
	private:
		// ÅäÖÃÎÄ¼þÂ·¾¶ /
		TCHAR filename[MAX_PATH];
		////
		HWND hNpp;
		vector<String*> uistres;
		vector<REGEXCONF*> regexes;
};

#endif

// end.