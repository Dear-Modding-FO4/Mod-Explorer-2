#pragma once

#include <string>
#include <F4SE/Impl/PCH.h>

namespace ME
{
	std::string GetRuntimePath() noexcept;
	std::string GetRuntimeDirectory() noexcept;

	using namespace std::literals;
}