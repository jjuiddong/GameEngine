//------------------------------------------------------------------------
// Name:    Marshalling.h
// Author:  jjuiddong
// Date:    3/6/2013
// 
// Packet Marshalling
//------------------------------------------------------------------------
#pragma once

namespace network
{
	namespace marshalling
	{
		// <<
		//CPacket& operator<<(CPacket& packet, const bool &rhs);
		//CPacket& operator<<(CPacket& packet, const long &rhs);
		//CPacket& operator<<(CPacket& packet, const int &rhs);
		//CPacket& operator<<(CPacket& packet, const unsigned int &rhs);
		//CPacket& operator<<(CPacket& packet, const char &rhs);
		//CPacket& operator<<(CPacket& packet, const unsigned char &rhs);
		//CPacket& operator<<(CPacket& packet, const float &rhs);
		//CPacket& operator<<(CPacket& packet, const double &rhs);
		//CPacket& operator<<(CPacket& packet, const short &rhs);
		//CPacket& operator<<(CPacket& packet, const unsigned short &rhs);
		CPacket& operator<<(CPacket& packet, const std::string &rhs);
		CPacket& operator<<(CPacket& packet, const _variant_t &rhs);
		CPacket& operator<<(CPacket& packet, const P2P_STATE &rhs);

		template<class T>
		CPacket& operator<<(CPacket& packet, const std::vector<T> &v)
		{
			AppendSequence( packet, v );
			return packet;
		}

		template<class T>
		CPacket& operator<<(CPacket& packet, const std::list<T> &v)
		{
			AppendSequence( packet, v );
			return packet;
		}


		// >>
		//CPacket& operator>>( CPacket& packet, bool &rhs);
		//CPacket& operator>>( CPacket& packet, long &rhs);
		//CPacket& operator>>( CPacket& packet, int &rhs);
		//CPacket& operator>>( CPacket& packet, unsigned int &rhs);
		//CPacket& operator>>( CPacket& packet, char &rhs);
		//CPacket& operator>>( CPacket& packet, unsigned char &rhs);
		//CPacket& operator>>( CPacket& packet, float &rhs);
		//CPacket& operator>>( CPacket& packet, double &rhs);
		//CPacket& operator>>( CPacket& packet, short &rhs);
		//CPacket& operator>>( CPacket& packet, unsigned short &rhs);
		CPacket& operator>>(CPacket& packet, std::string &rhs);
		CPacket& operator>>(CPacket& packet, P2P_STATE &rhs);
		CPacket& operator>>(CPacket& packet, _variant_t &varType);

		template<class T>
		CPacket& operator>>(CPacket& packet, std::vector<T> &v)
		{
			GetSequence(packet, v);
			return packet;
		}

		template<class T>
		CPacket& operator>>(CPacket& packet, std::list<T> &v)
		{
			int size;
			packet >> size;
			for (int i=0; i < size; ++i)
			{
				T t;
				packet >> t;
				v.push_back(t);
			}
			return packet;
		}


		//--------------------------------------------------------------------------
		template<class T>
		void AppendEnum(CPacket& packet, const T val)
		{
			packet.Append((int)val);
		}

		template<class Seq>
		CPacket& AppendSequence(CPacket& packet, const Seq &v)
		{
			packet << (int)v.size();
			BOOST_FOREACH(const Seq::value_type &t, v) {
				packet << t;
			}
			return packet;
		}

		template<class T>
		void GetDataEnum(CPacket& packet, T& val)
		{
			int tmp;
			packet.GetData(tmp);
			val = (T)tmp;
		}

		template<class Seq>
		void GetSequence(CPacket& packet, Seq& val)
		{
			int size;
			packet >> size;
			if (size < 0)
				return; // todo: log, error!! 
			if (size > 10000)
				return; // todo: too many size data in packet, log, error!!	

			val.reserve(size);
			for (int i=0; i < size; ++i)
			{
				Seq::value_type t;
				packet >> t;
				val.push_back(t);
			}
		}

	}
}
