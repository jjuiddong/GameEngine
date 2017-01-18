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


	struct STimer
	{
		int id;
		int interval;
		int beginT;
		bool repeat;

		STimer() {}
		STimer(int _id):id(_id) {}
		STimer(int _id, int _interval, int _beginT, bool _repeat) : id(_id), interval(_interval), beginT(_beginT), repeat(_repeat) {}
		bool operator==(const STimer&rhs) { return id == rhs.id; }
	};


	// Error Message
	static std::string g_LastError;
	inline std::string GetLastError() { return g_LastError; }
	inline void SetLastError(const std::string &err) { g_LastError = err; }

}
