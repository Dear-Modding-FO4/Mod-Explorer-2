#include <F4SE/Impl/PCH.h>
#include <detours/Detours.h>
#include <ME/Util.h>

ME::Util::ScopeLock::ScopeLock(uintptr_t a_target, uintptr_t a_size) noexcept :
	_unlocked(false), _old(0), _target(a_target), _size(a_size)
{
	_unlocked = REX::W32::VirtualProtect(reinterpret_cast<void*>(a_target), (size_t)a_size,
		REX::W32::PAGE_EXECUTE_READWRITE, (uint32_t*)&_old);
}

ME::Util::ScopeLock::ScopeLock(void* a_target, uintptr_t a_size) noexcept :
	_unlocked(false), _old(0), _target((uintptr_t)a_target), _size(a_size)
{
	_unlocked = REX::W32::VirtualProtect(a_target, (size_t)a_size, REX::W32::PAGE_EXECUTE_READWRITE, (uint32_t*)&_old);
}

ME::Util::ScopeLock::~ScopeLock() noexcept
{
	if (_unlocked)
	{
		// Ignore if this fails, the memory was copied either way
		REX::W32::VirtualProtect(reinterpret_cast<void*>(_target), (size_t)_size, _old, (uint32_t*)&_old);
		REX::W32::FlushInstructionCache(REX::W32::GetCurrentProcess(), (void*)_target, _size);
		_unlocked = false;
	}
}

std::string ME::Util::GetRuntimePath() noexcept
{
	static char	appPath[4096] = { 0 };

	if (appPath[0])
		return appPath;

	REX::W32::GetModuleFileNameA(REX::W32::GetModuleHandleA(nullptr), appPath, sizeof(appPath));
	return appPath;
}

std::string ME::Util::GetRuntimeDirectory() noexcept
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

void ME::Util::Write(uintptr_t a_target, const std::initializer_list<uint8_t>& a_data) noexcept
{
	if (!a_target || !a_data.size()) return;
	REL::Write(a_target, (const void*)a_data.begin(), a_data.size());
}

void ME::Util::WriteNop(uintptr_t a_target, size_t a_size) noexcept
{
	if (!a_target || !a_size) return;
	std::fill_n(reinterpret_cast<std::uint8_t*>(a_target), a_size, REL::NOP);
}

void ME::Util::WriteSafe(uintptr_t a_target, const std::initializer_list<uint8_t>& a_data) noexcept
{
	if (!a_target || !a_data.size()) return;

	ScopeLock Lock(a_target, a_data.size());
	if (Lock.HasUnlocked())
		REL::Write(a_target, (const void*)a_data.begin(), a_data.size());
}

void ME::Util::WriteSafeNop(uintptr_t a_target, size_t a_size) noexcept
{
	if (!a_target || !a_size) return;

	ScopeLock Lock(a_target, a_size);
	if (Lock.HasUnlocked())
		std::fill_n(reinterpret_cast<std::uint8_t*>(a_target), a_size, REL::NOP);
}

bool ME::Util::IsRuntimeOG() noexcept
{
	return REL::Module::IsRuntimeOG();
}

bool ME::Util::IsRuntimeNG() noexcept
{
	return REL::Module::IsRuntimeNG();
}

bool ME::Util::IsRuntimeAE() noexcept
{
	return REL::Module::IsRuntimeAE();
}

uintptr_t ME::Util::DetourJump(uintptr_t a_target, uintptr_t a_function) noexcept
{
	return Detours::X64::DetourFunction(a_target, a_function, Detours::X64Option::USE_REL32_JUMP);
}

uintptr_t ME::Util::DetourCall(uintptr_t a_target, uintptr_t a_function) noexcept
{
	return Detours::X64::DetourFunction(a_target, a_function, Detours::X64Option::USE_REL32_CALL);
}

bool ME::Util::DetourRemove(uintptr_t a_target) noexcept
{
	return Detours::X64::DetourRemove(a_target);
}

uintptr_t ME::Util::DetourVTable(uintptr_t a_target, uintptr_t a_function, uint32_t a_index) noexcept
{
	return Detours::X64::DetourVTable(a_target, a_function, a_index);
}

uintptr_t ME::Util::DetourIAT(const char* a_importModule, const char* a_functionName, uintptr_t a_function) noexcept
{
	return Detours::IATHook(REL::Module::GetSingleton()->base(), a_importModule, a_functionName, a_function);
}

uintptr_t ME::Util::DetourIAT(uintptr_t a_targetModule, const char* a_importModule, const char* a_functionName, uintptr_t a_function) noexcept
{
	return Detours::IATHook(a_targetModule, a_importModule, a_functionName, a_function);
}

uintptr_t ME::Util::DetourIATDelayed(const char* a_importModule, const char* a_functionName, uintptr_t a_function) noexcept
{
	return Detours::IATDelayedHook(REL::Module::GetSingleton()->base(), a_importModule, a_functionName, a_function);
}

uintptr_t ME::Util::DetourIATDelayed(uintptr_t a_targetModule, const char* a_importModule, const char* a_functionName, uintptr_t a_function) noexcept
{
	return Detours::IATDelayedHook(a_targetModule, a_importModule, a_functionName, a_function);
}

const char* ME::Util::GetSaveFolderName() noexcept
{
	return "Fallout4";
}
