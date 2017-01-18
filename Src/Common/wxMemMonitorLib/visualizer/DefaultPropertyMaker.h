/**
Name:   DefaultPropertyMaker.h
Author:  jjuiddong
Date:    1/30/2013


2014-06-14
	- 최적화 (함수 인자 레퍼런스로 바꿈)
	- 프로퍼티를 만든 후, 생성한 SVisDispDesc 구조체를 리턴한다.

*/
#pragma once


#include "VisualizerGlobal.h"


class wxPGProperty;
namespace visualizer
{
	using namespace memmonitor;


	// Make Property
	SVisDispDesc MakeProperty_DefaultForm( const SVisDispDesc &parentDispdesc, 
		const std::string &symbolName, const bool IsApplyVisualizer, const int depth );

	SVisDispDesc MakeProperty_DefaultForm( const SVisDispDesc &parentDispdesc, 
		const SSymbolInfo &symbol, const bool IsApplyVisualizer, const int depth );

	bool	MakePropertyChild_DefaultForm(const SVisDispDesc &parentDispdesc, 
		const SSymbolInfo &symbol, const bool IsApplyVisualizer, const int depth );

	SVisDispDesc MakeProperty_Dummy( const SVisDispDesc &parentDispdesc, const string &str );

}
