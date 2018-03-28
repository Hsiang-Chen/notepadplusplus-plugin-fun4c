/* $ @settings.cpp
 *
 * Copyright(C)2016 Sean Chen (³ÂÏé)
 * This program may be freely redistributed under the terms of the GNU General Public License.
 * */

#include <vector>
#include <windows.h>
#include "Shlwapi.h"

#include "settings.h"
#include "Notepad_plus_msgs.h"

////
fun4c_settings::fun4c_settings() : hNpp(0)
{
	memset(filename, 0, sizeof(filename));
	
	uistres.clear();
	regexes.clear();
}

fun4c_settings::~fun4c_settings()
{
	for (size_t i = 0; i < regexes.size(); i++) {
		if (regexes[i] != NULL) {
			if (regexes[i]->type != NULL) {
				delete[] regexes[i]->type;
			}
			if (regexes[i]->text != NULL) {
				delete[] regexes[i]->text;
			}
			delete regexes[i];
		}
	}
	
	for (size_t i = 0; i < uistres.size(); i++) {
		if (uistres[i] != NULL) {
			if (uistres[i]->text != NULL) {
				delete[] uistres[i]->text;
			}
			delete uistres[i];
		}
	}
	
	regexes.clear();
	uistres.clear();
}

void fun4c_settings::load()
{
	if (::PathFileExists(filename)) {
		TCHAR buf[MAX_PATH] = {0}, suf[MAX_PATH] = {0};
		
		if (GT_STR(TEXT("string"), TEXT("general.go"), buf, filename) > 0) {
			uistres.push_back(new String(buf));
		}
		
		if (GT_STR(TEXT("string"), TEXT("listview.line"), buf, filename) > 0) {
			uistres.push_back(new String(buf));
		}
		
		if (GT_STR(TEXT("string"), TEXT("listview.function"), buf, filename) > 0) {
			uistres.push_back(new String(buf));
		}
		
		if (GT_STR(TEXT("string"), TEXT("listview.type"), buf, filename) > 0) {
			uistres.push_back(new String(buf));
		}
		
		// regexes<TCHAR*, TCHAR*>
		for (size_t i = 0; ; i++) {
			wsprintf(buf, TEXT("Fn.%d"), i);
			if (GT_STR(TEXT("regex"), buf /* Fn.0 */, suf, filename) > 0) {
				REGEXCONF* conf = new REGEXCONF(buf, suf);
				if (conf != NULL) {
					regexes.push_back(conf);
				}
			} else {
				break;
			}
		}
		
		for (size_t i = 0; ; i++) {
			wsprintf(buf, TEXT("Var.%d"), i);
			if (GT_STR(TEXT("regex"), buf /* Var.0 */ , suf, filename) > 0) {
				REGEXCONF* conf = new REGEXCONF(buf, suf);
				if (conf != NULL) {
					regexes.push_back(conf);
				}
			} else {
				break;
			}
		}
	}
	
	if (! uistres.size()){
		uistres.push_back(new String(TEXT("Go")));
		uistres.push_back(new String(TEXT("Line")));
		uistres.push_back(new String(TEXT("Elements")));
		uistres.push_back(new String(TEXT("Type")));
	}
	
	if (! regexes.size()) {
		regexes.push_back(new REGEXCONF(TEXT("Fn.0"),  TEXT("^[a-zA-Z_]\\w+::[a-zA-Z_]\\w+.*\\)$")));
		regexes.push_back(new REGEXCONF(TEXT("Fn.1"),  TEXT("^[a-zA-Z_]\\w+[\\t\\s]+[a-zA-Z_]\\w+.*\\)$")));
		regexes.push_back(new REGEXCONF(TEXT("Var.0"), TEXT("^[a-zA-Z_]\\w+[\\t\\s]+[a-zA-Z_]\\w+.*;$")));
	}
}

void fun4c_settings::save()
{
	TCHAR buf[MAX_PATH];
	
	if (uistres.size() > 3) {
		WT_STR(TEXT("string"), TEXT("general.go"), uistres[0]->text, filename);
		WT_STR(TEXT("string"), TEXT("listview.line"), uistres[1]->text, filename);
		WT_STR(TEXT("string"), TEXT("listview.function"), uistres[2]->text, filename);
		WT_STR(TEXT("string"), TEXT("listview.type"), uistres[3]->text, filename);
	}
	
	// regexes<TCHAR*>
	for (size_t i = 0; i < regexes.size(); i++) {
		wsprintf(buf, TEXT("%s"), regexes[i]->type/* , i */);
		WT_STR(TEXT("regex"), buf, regexes[i]->text, filename);
	}
}

void fun4c_settings::npp(HWND hWnd)
{
	if (::IsWindow(hNpp = hWnd)) {
		::SendMessage(hNpp, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)filename);
		::PathRemoveBackslash(filename);
		if (! ::PathFileExists(filename)) {
			::CreateDirectory(filename, NULL);
		}
		::lstrcat(filename, TEXT("\\fun4c.ini"));
	}
}

// end.