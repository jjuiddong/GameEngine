//------------------------------------------------------------------------
// Name:    NetworkUtility.h
// Author:  jjuiddong
// Date:    2/28/2013
// 
// network ���̺귯������ ���Ǵ� ���� ��ƿ��Ƽ��
//------------------------------------------------------------------------
#pragma once

namespace network
{
	struct SFd_Set : fd_set
	{
		netid netid_array[ FD_SETSIZE];		// fd_array[] ���Ͽ� �ش��ϴ� netid ��
	};
	
}
