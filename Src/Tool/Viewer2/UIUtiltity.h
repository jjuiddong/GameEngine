// UI ���� �������� ����ϴ� �Լ����� ���� ��ƿ��Ƽ �ڵ��.

#pragma once



void ExpandAll(CTreeCtrl &treeCtrl);

HTREEITEM FindTree( CTreeCtrl &treeCtrl, const wstring &text );

void MoveChildCtrlWindow(CWnd &parent, CWnd &wndCtrl, int cx, int cy);

