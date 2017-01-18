// UI 에서 공통으로 사용하는 함수들은 모은 유틸리티 코드다.

#pragma once



void ExpandAll(CTreeCtrl &treeCtrl);

HTREEITEM FindTree( CTreeCtrl &treeCtrl, const wstring &text );

void MoveChildCtrlWindow(CWnd &parent, CWnd &wndCtrl, int cx, int cy);

