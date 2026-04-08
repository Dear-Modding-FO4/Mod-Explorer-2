#pragma once

#include <ME/Core.h>
#include <REX/REX/Singleton.h>
#include <RE/T/TESForm.h>
#include <unordered_dense/unordered_dense.h>
#include <concurrent_vector.h>
#include <vector>
#include <string>
#include <mutex>

namespace ME
{
	class DataSystem :
		public REX::Singleton<DataSystem>
	{
	public:
		constexpr static std::array<RE::ENUM_FORM_ID, 8> AllowFormType =
		{
			RE::ENUM_FORM_ID::kARMO,
			RE::ENUM_FORM_ID::kBOOK,
			RE::ENUM_FORM_ID::kMISC,
			RE::ENUM_FORM_ID::kWEAP,
			RE::ENUM_FORM_ID::kAMMO,
			RE::ENUM_FORM_ID::kKEYM,
			RE::ENUM_FORM_ID::kALCH,
			RE::ENUM_FORM_ID::kNOTE,
		};

		class Plugin
		{
		public:
			struct Item
			{
				float weight{ .0f };
				uint32_t price{ 0 };
				const RE::TESForm* form{ nullptr };
			};

			Plugin(const RE::TESFile* a_file);
			virtual ~Plugin() = default;

			friend class DataSystem;

			[[nodiscard]] virtual inline bool IsLight() const noexcept { return isLight; }
			[[nodiscard]] virtual inline uint16_t GetIndex() const noexcept { return index; }
			[[nodiscard]] virtual inline const RE::TESFile* GetFile() const noexcept { return file; }
			[[nodiscard]] virtual std::string GetFilename() const noexcept;

			virtual bool Add(const RE::TESForm* a_form) noexcept;
			[[nodiscard]] virtual size_t Count(RE::ENUM_FORM_ID a_type) const noexcept;
			[[nodiscard]] virtual size_t CountAll() const noexcept;
			[[nodiscard]] virtual bool Empty() const noexcept;
			virtual void Clear() noexcept;
			virtual size_t GetData(RE::ENUM_FORM_ID a_type, std::vector<Item>& a_receive, bool a_clear = true) noexcept;
			virtual size_t GetAllData(std::vector<Item>& a_receive, bool a_clear = true) noexcept;
		private:
			Plugin(const Plugin&) = delete;
			Plugin(Plugin&&) = delete;
			Plugin& operator=(const Plugin&) = delete;
			Plugin& operator=(Plugin&&) = delete;
		private:
			concurrency::concurrent_vector<Item> datas[AllowFormType.size()];
			const RE::TESFile* file{ nullptr };
			bool isLight{ false };
			uint16_t index{ 0 };
		};

		DataSystem() = default;
		virtual ~DataSystem() = default;

		virtual bool ReadData() noexcept;
		virtual void Cleanup() noexcept;
		virtual void Dump() noexcept;

	private:
		DataSystem(const DataSystem&) = delete;
		DataSystem(DataSystem&&) = delete;
		DataSystem& operator=(const DataSystem&) = delete;
		DataSystem& operator=(DataSystem&&) = delete;

		bool ReadDataByType(RE::ENUM_FORM_ID a_type) noexcept;
		void ReadForms(const RE::TESForm** a_array, size_t a_count) noexcept;
		bool IsValidForm(const RE::TESForm* a_form) const noexcept;
		void AddForm(const RE::TESForm* a_form) noexcept;
	private:
		std::recursive_mutex lock;
		std::vector<std::shared_ptr<Plugin>> plugins[2];
	};
}

