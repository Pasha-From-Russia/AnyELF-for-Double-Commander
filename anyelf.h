#ifndef _ANYELF__H
#define _ANYELF__H

#include "common.h"
#include <string>

#define lc_copy            1
#define lc_newparams       2
#define lc_selectall       3
#define lc_setpercent      4

#define lcp_wraptext       1
#define lcp_fittowindow    2
#define lcp_ansi           4
#define lcp_ascii          8
#define lcp_variable      12
#define lcp_forceshow     16
#define lcp_fitlargeronly 32
#define lcp_center        64

#define lcs_findfirst      1
#define lcs_matchcase      2
#define lcs_wholewords     4
#define lcs_backwards      8

#define itm_percent   0xFFFE
#define itm_fontstyle 0xFFFD
#define itm_wrap      0xFFFC
#define itm_fit       0xFFFB
#define itm_next      0xFFFA
#define itm_center    0xFFF9

#define LISTPLUGIN_OK      0
#define LISTPLUGIN_ERROR   1

#ifdef __cplusplus
extern "C" {
#endif

HWND DCPCALL ListLoad(HWND ParentWin,char* FileToLoad,int ShowFlags);
void DCPCALL ListCloseWindow(HWND ListWin);
void DCPCALL ListGetDetectString(char* DetectString,int maxlen);
int DCPCALL ListSearchText(HWND ListWin,char* SearchString,int SearchParameter);
int DCPCALL ListSendCommand(HWND ListWin,int Command,int Parameter);

#ifdef __cplusplus
}
#endif

std::string elfdump(std::string fileToLoad);
bool isElfFile(const char *fileToLoad);

#endif // _ANYELF__H
