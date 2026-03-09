#pragma once

#include <F4SE/F4SE.h>

namespace ME
{
	bool PreloadInit(const F4SE::PreLoadInterface* a_preloadf4se);
	bool Init(const F4SE::LoadInterface* a_loadf4se);
}