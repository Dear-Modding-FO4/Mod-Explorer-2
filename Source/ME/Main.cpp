#include <ME/Main.h>

namespace ME
{
	static bool PreloadInitUnsafe(const F4SE::PreLoadInterface* a_preloadf4se) noexcept
	{
		return true;
	}

	static bool InitUnsafe(const F4SE::LoadInterface* a_loadf4se) noexcept
	{
		return true;
	}

	bool PreloadInit(const F4SE::PreLoadInterface* a_preloadf4se)
	{
		__try
		{
			return PreloadInitUnsafe(a_preloadf4se);
		}
		__except (1)
		{
			return false;
		}
	}

	bool Init(const F4SE::LoadInterface* a_loadf4se)
	{
		__try
		{
			return InitUnsafe(a_loadf4se);
		}
		__except (1)
		{
			return false;
		}
	}
}