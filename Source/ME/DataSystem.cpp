#include <ME/DataSystem.h>
#include <RE/RTTI.h>
#include <RE/IDs_RTTI.h>
#include <RE/B/BGSKeywordForm.h>
#include <RE/T/TESFile.h>
#include <RE/T/TESFullName.h>
#include <RE/T/TESDataHandler.h>
#include <RE/T/TESWeightForm.h>
#include <RE/T/TESValueForm.h>
#include <RE/T/TESObjectWEAP.h>
#include <RE/T/TESObjectARMO.h>
#include <Windows.h>
#include <thread>

#undef ERROR
#undef MAX_PATH
#undef MEM_RELEASE

ME::DataSystem::Plugin::Plugin(const RE::TESFile* a_file) :
	file(a_file)
{
	assert(file);

	isLight = file->IsLight();
	index = isLight ? file->smallFileCompileIndex : file->compileIndex;
}

std::string ME::DataSystem::Plugin::GetFilename() const noexcept
{
	if (!file) return "";

	std::string fn = file->filename;
	auto it = fn.find_last_of("\\/");

	return (it == std::string::npos) ? fn : fn.substr(it + 1);
}

bool ME::DataSystem::Plugin::Add(const RE::TESForm* a_form) noexcept
{
	if (!a_form) return false;

	auto type = a_form->formType.get();
	auto it = std::find(AllowFormType.begin(), AllowFormType.end(), type);
	if (it != AllowFormType.end())
	{
		Item item{ .0f, 0, a_form };
		switch (type)
		{
		case RE::ENUM_FORM_ID::kARMO:
		{
			auto& data = ((RE::TESObjectARMO*)a_form)->armorData;
			item.weight = data.weight;
			item.price = data.value;
			break;
		}
		case RE::ENUM_FORM_ID::kBOOK:
		{
			auto weight = (RE::TESWeightForm*)RE::RTDynamicCast((void*)a_form, 0, (void*)RE::RTTI::TESObjectBOOK.address(),
				(void*)RE::RTTI::TESWeightForm.address(), 0);
			auto value = (RE::TESValueForm*)RE::RTDynamicCast((void*)a_form, 0, (void*)RE::RTTI::TESObjectBOOK.address(),
				(void*)RE::RTTI::TESValueForm.address(), 0);
			if (weight) item.weight = weight->weight;
			if (value) item.price = value->value;
			break;
		}
		case RE::ENUM_FORM_ID::kMISC:
		{
			auto weight = (RE::TESWeightForm*)RE::RTDynamicCast((void*)a_form, 0, (void*)RE::RTTI::TESObjectMISC.address(),
				(void*)RE::RTTI::TESWeightForm.address(), 0);
			auto value = (RE::TESValueForm*)RE::RTDynamicCast((void*)a_form, 0, (void*)RE::RTTI::TESObjectMISC.address(),
				(void*)RE::RTTI::TESValueForm.address(), 0);
			if (weight) item.weight = weight->weight;
			if (value) item.price = value->value;
			break;
		}
		case RE::ENUM_FORM_ID::kWEAP:
		{
			auto& data = ((RE::TESObjectWEAP*)a_form)->weaponData;
			item.weight = data.weight;
			item.price = data.value;
			break;
		}
		case RE::ENUM_FORM_ID::kAMMO:
		{
			auto weight = (RE::TESWeightForm*)RE::RTDynamicCast((void*)a_form, 0, (void*)RE::RTTI::TESAmmo.address(),
				(void*)RE::RTTI::TESWeightForm.address(), 0);
			auto value = (RE::TESValueForm*)RE::RTDynamicCast((void*)a_form, 0, (void*)RE::RTTI::TESAmmo.address(),
				(void*)RE::RTTI::TESValueForm.address(), 0);
			if (weight) item.weight = weight->weight;
			if (value) item.price = value->value;
			break;
		}
		case RE::ENUM_FORM_ID::kKEYM:
		{
			auto weight = (RE::TESWeightForm*)RE::RTDynamicCast((void*)a_form, 0, (void*)RE::RTTI::TESKey.address(),
				(void*)RE::RTTI::TESWeightForm.address(), 0);
			auto value = (RE::TESValueForm*)RE::RTDynamicCast((void*)a_form, 0, (void*)RE::RTTI::TESKey.address(),
				(void*)RE::RTTI::TESValueForm.address(), 0);
			if (weight) item.weight = weight->weight;
			if (value) item.price = value->value;
			break;
		}
		case RE::ENUM_FORM_ID::kALCH:
		{
			auto weight = (RE::TESWeightForm*)RE::RTDynamicCast((void*)a_form, 0, (void*)RE::RTTI::AlchemyItem.address(),
				(void*)RE::RTTI::TESWeightForm.address(), 0);
			auto value = (RE::TESValueForm*)RE::RTDynamicCast((void*)a_form, 0, (void*)RE::RTTI::AlchemyItem.address(),
				(void*)RE::RTTI::TESValueForm.address(), 0);
			if (weight) item.weight = weight->weight;
			if (value) item.price = value->value;
			break;
		}
		case RE::ENUM_FORM_ID::kNOTE:
		{
			auto weight = (RE::TESWeightForm*)RE::RTDynamicCast((void*)a_form, 0, (void*)RE::RTTI::BGSNote.address(),
				(void*)RE::RTTI::TESWeightForm.address(), 0);
			auto value = (RE::TESValueForm*)RE::RTDynamicCast((void*)a_form, 0, (void*)RE::RTTI::BGSNote.address(),
				(void*)RE::RTTI::TESValueForm.address(), 0);
			if (weight) item.weight = weight->weight;
			if (value) item.price = value->value;
			break;
		}
		default:
			return false;
		}

		datas[std::distance(AllowFormType.begin(), it)].push_back(item);
		return true;
	}

	return false;
}

size_t ME::DataSystem::Plugin::Count(RE::ENUM_FORM_ID a_type) const noexcept
{
	auto it = std::find(AllowFormType.begin(), AllowFormType.end(), a_type);
	if (it != AllowFormType.end())
		return datas[std::distance(AllowFormType.begin(), it)].size();
	return 0;
}

size_t ME::DataSystem::Plugin::CountAll() const noexcept
{
	size_t ret = 0;
	for (auto& items : datas)
		ret += items.size();
	return ret;
}

bool ME::DataSystem::Plugin::Empty() const noexcept
{
	for (auto& items : datas)
		if (!items.empty())
			return false;
	return true;
}

void ME::DataSystem::Plugin::Clear() noexcept
{
	for (auto& items : datas)
		items.clear();
}

size_t ME::DataSystem::Plugin::GetData(RE::ENUM_FORM_ID a_type, std::vector<Item>& a_receive, bool a_clear) noexcept
{
	if (a_clear)
		a_receive.clear();

	auto it = std::find(AllowFormType.begin(), AllowFormType.end(), a_type);
	if (it != AllowFormType.end())
	{
		auto idx = std::distance(AllowFormType.begin(), it);
		a_receive.reserve(a_clear ? datas[idx].size() : (a_receive.size() + datas[idx].size()));
		std::copy(datas[idx].begin(), datas[idx].end(), std::back_inserter(a_receive));
	}

	return a_receive.size();
}

size_t ME::DataSystem::Plugin::GetAllData(std::vector<Item>& a_receive, bool a_clear) noexcept
{
	if (a_clear)
	{
		a_receive.clear();
		a_receive.reserve(CountAll());
	}
	else
		a_receive.reserve(a_receive.size() + CountAll());

	for (auto& items : datas)
		std::copy(items.begin(), items.end(), std::back_inserter(a_receive));

	return a_receive.size();
}

bool ME::DataSystem::ReadData() noexcept
{
	std::lock_guard locker(lock);

	plugins[0].clear();
	plugins[1].clear();

	auto handler = RE::TESDataHandler::GetSingleton();
	if (!handler)
	{
		REX::ERROR("TESDataHandler singleton does not exist"sv);
		return false;
	}

	auto& collection = handler->compiledFileCollection;
	plugins[0].reserve(collection.files.size());
	plugins[1].reserve(collection.smallFiles.size());

	for (auto file : collection.files)
		plugins[0].push_back(std::make_shared<Plugin>(file));

	for (auto file : collection.smallFiles)
		plugins[1].push_back(std::make_shared<Plugin>(file));

	for (auto type : AllowFormType)
		if (!ReadDataByType(type))
			REX::ERROR("Error reading the \"0x{:X}\" type forms"sv, std::to_underlying(type));

	return true;
}

void ME::DataSystem::Cleanup() noexcept
{
	std::lock_guard locker(lock);

	plugins[0].clear();
	plugins[1].clear();
}

void ME::DataSystem::Dump() noexcept
{
	REX::INFO("Default plugins:"sv);
	for (auto& plugin : plugins[0])
		REX::INFO("\tFilename: {} index 0x{:02X} count form {}"sv, plugin->GetFilename(), plugin->GetIndex(), plugin->CountAll());
	REX::INFO("Light plugins:"sv);
	for (auto& plugin : plugins[1])
		REX::INFO("\tFilename: {} index 0x{:03X} count form {}"sv, plugin->GetFilename(), plugin->GetIndex(), plugin->CountAll());
}

bool ME::DataSystem::ReadDataByType(RE::ENUM_FORM_ID a_type) noexcept
{
	__try
	{
		auto handler = RE::TESDataHandler::GetSingleton();
		auto& arr = handler->formArrays[std::to_underlying(a_type)];
		ReadForms((const RE::TESForm**)arr.data(), arr.size());
		return true;
	}
	__except (1)
	{
		return false;
	}
}

void ME::DataSystem::ReadForms(const RE::TESForm** a_array, size_t a_count) noexcept
{
	if (!a_array || !a_count)
		return;

	if (a_count > 200)
	{
		std::array<HANDLE, 4> hDone{};

		hDone[0] = CreateEventA(nullptr, true, false, nullptr);
		hDone[1] = CreateEventA(nullptr, true, false, nullptr);
		hDone[2] = CreateEventA(nullptr, true, false, nullptr);
		hDone[3] = CreateEventA(nullptr, true, false, nullptr);

		auto part = a_count >> 2;

		std::thread task01([&](HANDLE a_eventDone, size_t a_itBeg, size_t a_itEnd) {

			for (auto itCur = a_itBeg; itCur < a_itEnd; itCur++)
				AddForm(a_array[itCur]);
			SetEvent(a_eventDone);
			}, hDone[0], 0, part);

		std::thread task02([&](HANDLE a_eventDone, size_t a_itBeg, size_t a_itEnd) {

			for (auto itCur = a_itBeg; itCur < a_itEnd; itCur++)
				AddForm(a_array[itCur]);
			SetEvent(a_eventDone);

			}, hDone[1], part, part << 1);

		std::thread task03([&](HANDLE a_eventDone, size_t a_itBeg, size_t a_itEnd) {

			for (auto itCur = a_itBeg; itCur < a_itEnd; itCur++)
				AddForm(a_array[itCur]);
			SetEvent(a_eventDone);

			}, hDone[2], part << 1, part * 3);

		std::thread task04([&](HANDLE a_eventDone, size_t a_itBeg, size_t a_itEnd) {

			for (auto itCur = a_itBeg; itCur < a_itEnd; itCur++)
				AddForm(a_array[itCur]);
			SetEvent(a_eventDone);

			}, hDone[3], part * 3, a_count);

		SetThreadPriority(task01.native_handle(), THREAD_PRIORITY_ABOVE_NORMAL);
		SetThreadPriority(task02.native_handle(), THREAD_PRIORITY_ABOVE_NORMAL);
		SetThreadPriority(task03.native_handle(), THREAD_PRIORITY_ABOVE_NORMAL);
		SetThreadPriority(task04.native_handle(), THREAD_PRIORITY_ABOVE_NORMAL);

		task01.detach();
		task02.detach();
		task03.detach();
		task04.detach();

		WaitForMultipleObjects(4, hDone.data(), true, INFINITE);

		CloseHandle(hDone[3]);
		CloseHandle(hDone[2]);
		CloseHandle(hDone[1]);
		CloseHandle(hDone[0]);
	}
	else
	{
		for (size_t idx = 0; idx < a_count; idx++)
			AddForm(a_array[idx]);
	}
}

bool ME::DataSystem::IsValidForm(const RE::TESForm* a_form) const noexcept
{
	if (!a_form)
		return false;

	if (!a_form->IsBoundObject() || !a_form->GetPlayable(nullptr))
		return false;

	auto name = (RE::TESFullName*)RE::RTDynamicCast((void*)a_form, 0, (void*)RE::RTTI::TESForm.address(),
		(void*)RE::RTTI::TESFullName.address(), 0);

	if (!name || !strlen(name->fullName.c_str()) || (name->fullName.c_str()[0] == '<'))
		return false;

	auto pKeyword = (RE::BGSKeywordForm*)RE::RTDynamicCast((void*)a_form, 0, (void*)RE::RTTI::TESForm.address(),
		(void*)RE::RTTI::BGSKeywordForm.address(), 0);
	if (pKeyword)
		for (uint32_t j = 0; j < pKeyword->numKeywords; j++)
			// FeaturedItem [KYWD:001B3FAC], quest item
			if ((pKeyword->keywords[j]->formID == 0x1B3FAC) ||
				// VendorItemKey [KYWD:00022CE3], this a key to buy or enter a house.
				((a_form->formType.get() == RE::ENUM_FORM_ID::kKEYM) && (pKeyword->keywords[j]->formID == 0x22CE3)))
				return false;
	return true;
}

void ME::DataSystem::AddForm(const RE::TESForm* a_form) noexcept
{
	if (IsValidForm(a_form))
	{
		auto idx = a_form->formID >> 24;
		if (idx == 0xFE)
		{
			idx = (a_form->formID >> 12) & 0xFFF;
			auto it = std::find_if(plugins[1].begin(), plugins[1].end(), [&idx](const std::shared_ptr<Plugin>& elem) { return elem->GetIndex() == idx; });
			if (it == plugins[1].end())
				REX::ERROR("Form 0x{:08X} no found"sv, a_form->formID);
			else
				it->get()->Add(a_form);
		}
		else
		{
			auto it = std::find_if(plugins[0].begin(), plugins[0].end(), [&idx](const std::shared_ptr<Plugin>& elem) { return elem->GetIndex() == idx; });
			if (it == plugins[0].end())
				REX::ERROR("Form 0x{:08X} no found"sv, a_form->formID);
			else
				it->get()->Add(a_form);
		}
	}
}
