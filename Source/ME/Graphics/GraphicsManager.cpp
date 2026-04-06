#include <ME/Graphics/GraphicsManager.h>
#include <ME/Graphics/GraphicsObject.h>

size_t ME::GraphicsManager::Add(const std::shared_ptr<GraphicsObject> a_object) noexcept
{
	std::lock_guard locker(lock);

    if (list.max_size() == list.size())
		return FailIndex;

    auto it = std::find_if(list.begin(), list.end(), [&a_object](auto elem) { return a_object == elem; });

    if (it != list.end()) 
		return FailIndex;

    auto result = list.size();
	try
	{
		REX::INFO("Added");

		list.emplace_back(a_object);
		return result;
	}
	catch (...)
	{
		return FailIndex;
	}
}

bool ME::GraphicsManager::Remove(::size_t a_idx) noexcept
{
	std::lock_guard locker(lock);

	if (Count() <= a_idx)
		return false;

	auto it = list.begin();
	std::advance(it, a_idx);
	list.erase(it);

	return true;
}

void ME::GraphicsManager::Cleanup() noexcept
{
	std::lock_guard locker(lock);
	list.clear();
}

size_t ME::GraphicsManager::Count() noexcept
{
	std::lock_guard locker(lock);
	return list.size();
}

void ME::GraphicsManager::Execute() noexcept
{
	std::lock_guard locker(lock);
	for (auto& elem : list)
		elem->Execute();
}
