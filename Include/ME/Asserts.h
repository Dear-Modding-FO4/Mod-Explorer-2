#pragma once

namespace ME
{
	void AssertMsg(const char* SourceFile, int SourceLine, const char* Function, const char* FormattedMessage, ...) noexcept;
}

#define ME_Assert(Cond)									if(!(Cond)) ME::AssertMsg(__FILE__, __LINE__, __FUNCTION__, #Cond)
#define ME_AssertWithFormattedMessage(Cond, Msg, ...)	if(!(Cond)) ME::AssertMsg(__FILE__, __LINE__, __FUNCTION__, "%s\n\n" Msg, #Cond, ##__VA_ARGS__)
#define ME_AssertWithMessage(Cond, Msg)					ME_AssertWithFormattedMessage(Cond, Msg)