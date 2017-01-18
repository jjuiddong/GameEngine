//------------------------------------------------------------------------
// Name:    NetworkUtility.h
// Author:  jjuiddong
// Date:    2/28/2013
// 
// network 라이브러리에서 사용되는 각종 유틸리티들
//------------------------------------------------------------------------
#pragma once

namespace network
{
	struct SFd_Set : fd_set
	{
		netid netid_array[ FD_SETSIZE];		// fd_array[] 소켓에 해당하는 netid 값
	};
	
}
