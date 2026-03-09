#include <ME/Util.h>
#include <ME/Asserts.h>

#include <detours/Detours.h>

namespace ME
{
	std::string GetRuntimePath() noexcept
	{
		static char	appPath[4096] = { 0 };

		if (appPath[0])
			return appPath;

		ME_Assert(REX::W32::GetModuleFileNameA(REX::W32::GetModuleHandleA(nullptr), appPath, sizeof(appPath)));
		return appPath;
	}

	std::string GetRuntimeDirectory() noexcept
	{
		std::string runtimeDirectory;

		if (runtimeDirectory.empty())
		{
			std::string	runtimePath = GetRuntimePath();
			// truncate at last slash
			std::string::size_type	lastSlash = runtimePath.rfind('\\');
			if (lastSlash != std::string::npos)
				// if we don't find a slash something is VERY WRONG
				runtimeDirectory = runtimePath.substr(0, lastSlash + 1);
		}

		return runtimeDirectory;
	}

	ScopeLock::ScopeLock(uintptr_t a_target, uintptr_t a_size) noexcept :
		_unlocked(false), _old(0), _target(a_target), _size(a_size)
	{
		_unlocked = REX::W32::VirtualProtect(reinterpret_cast<void*>(a_target), (size_t)a_size,
			REX::W32::PAGE_EXECUTE_READWRITE, (uint32_t*)&_old);
	}

	ScopeLock::ScopeLock(void* a_target, uintptr_t a_size) noexcept :
		_unlocked(false), _old(0), _target((uintptr_t)a_target), _size(a_size)
	{
		_unlocked = REX::W32::VirtualProtect(a_target, (size_t)a_size, REX::W32::PAGE_EXECUTE_READWRITE, (uint32_t*)&_old);
	}

	ScopeLock::~ScopeLock() noexcept
	{
		if (_unlocked)
		{
			// Ignore if this fails, the memory was copied either way
			REX::W32::VirtualProtect(reinterpret_cast<void*>(_target), (size_t)_size, _old, (uint32_t*)&_old);
			REX::W32::FlushInstructionCache(REX::W32::GetCurrentProcess(), (void*)_target, _size);
			_unlocked = false;
		}
	}

	void Write(uintptr_t a_target, const std::initializer_list<uint8_t>& a_data) noexcept
	{
		if (!a_target || !a_data.size()) return;
		REL::Write(a_target, (const void*)a_data.begin(), a_data.size());
	}

	void WriteNop(uintptr_t a_target, size_t a_size) noexcept
	{
		if (!a_target || !a_size) return;
		std::fill_n(reinterpret_cast<std::uint8_t*>(a_target), a_size, REL::NOP);
	}

	void WriteSafe(uintptr_t a_target, const std::initializer_list<uint8_t>& a_data) noexcept
	{
		if (!a_target || !a_data.size()) return;

		ScopeLock Lock(a_target, a_data.size());
		if (Lock.HasUnlocked())
			REL::Write(a_target, (const void*)a_data.begin(), a_data.size());
	}

	void WriteSafeNop(uintptr_t a_target, size_t a_size) noexcept
	{
		if (!a_target || !a_size) return;
		ScopeLock Lock(a_target, a_size);
		if (Lock.HasUnlocked())
			std::fill_n(reinterpret_cast<std::uint8_t*>(a_target), a_size, REL::NOP);
	}

	bool IsRuntimeOG() noexcept
	{
		return REL::Module::IsRuntimeOG();
	}

	bool IsRuntimeNG() noexcept
	{
		return REL::Module::IsRuntimeNG();
	}

	bool IsRuntimeAE() noexcept
	{
		return REL::Module::IsRuntimeAE();
	}

	uintptr_t DetourJump(uintptr_t a_target, uintptr_t a_function) noexcept
	{
		return Detours::X64::DetourFunction(a_target, a_function, Detours::X64Option::USE_REL32_JUMP);
	}

	uintptr_t DetourCall(uintptr_t a_target, uintptr_t a_function) noexcept
	{
		return Detours::X64::DetourFunction(a_target, a_function, Detours::X64Option::USE_REL32_CALL);
	}

	uintptr_t DetourVTable(uintptr_t a_target, uintptr_t a_function, uint32_t a_index) noexcept
	{
		return Detours::X64::DetourVTable(a_target, a_function, a_index);
	}

	uintptr_t DetourIAT(const char* a_importModule, const char* a_functionName, uintptr_t a_function) noexcept
	{
		return Detours::IATHook(REL::Module::GetSingleton()->base(), a_importModule, a_functionName, a_function);
	}

	uintptr_t DetourIAT(uintptr_t a_targetModule, const char* a_importModule, const char* a_functionName, uintptr_t a_function) noexcept
	{
		return Detours::IATHook(a_targetModule, a_importModule, a_functionName, a_function);
	}

	uintptr_t DetourIATDelayed(const char* a_importModule, const char* a_functionName, uintptr_t a_function) noexcept
	{
		return Detours::IATDelayedHook(REL::Module::GetSingleton()->base(), a_importModule, a_functionName, a_function);
	}

	uintptr_t DetourIATDelayed(uintptr_t a_targetModule, const char* a_importModule, const char* a_functionName, uintptr_t a_function) noexcept
	{
		return Detours::IATDelayedHook(a_targetModule, a_importModule, a_functionName, a_function);
	}
}