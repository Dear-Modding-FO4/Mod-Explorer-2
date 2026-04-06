#pragma once

#include <ME/HotkeySystem.h>
#include <mutex>

namespace ME
{
	class Shop :
		public REX::Singleton<Shop>
	{
	public:
		Shop() = default;
		virtual ~Shop() = default;

		virtual bool Initialize(const F4SE::LoadInterface* a_f4se);
		virtual void Open() noexcept;
		virtual void Close() noexcept;

		virtual inline bool TryLock() noexcept { return lock.try_lock(); }
		virtual inline void Lock() noexcept { lock.lock(); }
		virtual inline void Unlock() noexcept { lock.unlock(); }

		virtual inline bool HasInitialize() const noexcept { return hasInitialize; }
		virtual inline bool HasInitializeGameHooks() const noexcept { return hasInitializeGameHooks; }
		virtual inline bool HasInitializeGraphicsAndData() const noexcept { return hasInitializeGraphicsAndData; }
		virtual inline bool HasOpened() const noexcept { return hasOpened; }
	private:
		Shop(const Shop&) = delete;
		Shop(Shop&&) = delete;
		Shop& operator=(const Shop&) = delete;
		Shop& operator=(Shop&&) = delete;

		void InitializeGameHooks() noexcept;
		void InitializeGraphicsAndData() noexcept;

		static void F4SEMessageListener(F4SE::MessagingInterface::Message* a_msg) noexcept;
		static void GameKeyboardListener(uint8_t a_vkCode, bool a_up) noexcept;
	private:
		bool hasInitialize{ false };
		bool hasInitializeGameHooks{ false };
		bool hasInitializeGraphicsAndData{ false };
		bool hasOpened{ false };

		HotkeySystem::Hotkey hotkey{};
		std::recursive_mutex lock{};
	};
}