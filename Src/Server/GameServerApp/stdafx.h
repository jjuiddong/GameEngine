// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include "Common/Common.h"
#include "Network/Network.h"
#include "wxMemMonitorLib/wxMemMonitor.h"

// ���̸��� ª��..
namespace dbg = common::dbg;
namespace clog = common::log;


class CSubServerGroup;
class CRemoteSubServer;

typedef common::ReferencePtr<CSubServerGroup> SubServerGroupPtr;
typedef common::ReferencePtr<CRemoteSubServer> RemoteSubServerPtr;
