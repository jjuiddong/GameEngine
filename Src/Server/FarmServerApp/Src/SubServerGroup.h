/**
Name:   SubServerGroup.h
Author:  jjuiddong
Date:    4/7/2013

서브서버 그룹
*/
#pragma once

class CSubServerGroup : public network::CGroup
{
public:
	void SetSvrType(const std::string &svrType);
	void SetOuterPortBase(int port);
	void SetInnerPortBase(int port);
	const std::string& GetSvrType() const;
	int GetOuterPortBase() const;
	int GetInnerPortBase() const;

	bool GetServerInfoCorrespondClient( const std::string &clientSvrType, const std::string &serverSvrType, 
		network::ISessionAccess &userAccess, OUT std::vector<network::SHostInfo> &v);

	bool GetCorrespondClientInfo( network::ISessionAccess &userAccess, OUT std::vector<std::string> &svrTypes );

	int GetToBindOuterPort(network::ISessionAccess &userAccess);
	int GetToBindInnerPort(network::ISessionAccess &userAccess);

private:
	std::string m_SvrType;
	int	m_OuterPortBase;
	int	m_InnerPortBase;
};


inline void CSubServerGroup::SetSvrType(const std::string &svrType) { m_SvrType = svrType; }
inline void CSubServerGroup::SetOuterPortBase(int port) { m_OuterPortBase = port; }
inline void CSubServerGroup::SetInnerPortBase(int port) { m_InnerPortBase = port; }
inline const std::string& CSubServerGroup::GetSvrType()  const { return m_SvrType; }
inline int CSubServerGroup::GetOuterPortBase() const { return m_OuterPortBase; }
inline int CSubServerGroup::GetInnerPortBase() const { return m_InnerPortBase; }


class CSubServerGroupFactory : public network::IGroupFactory
{
	virtual network::CGroup* New() { return new CSubServerGroup(); }
	virtual network::IGroupFactory* Clone() { return new CSubServerGroupFactory(); }
};

