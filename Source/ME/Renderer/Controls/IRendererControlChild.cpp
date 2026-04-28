#include <ME/Renderer/Controls/IRendererControlBase.h>
#include <ME/Renderer/Controls/IRendererControlChild.h>

void ME::Controls::IRendererControlChild::Attach(IRendererControlBase* a_control) noexcept
{
	std::lock_guard locker(lock);
	auto it = std::find(childs.begin(), childs.end(), a_control);
	if (it == childs.end())
	{
		childs.emplace_back(a_control);
		a_control->parent = (IRendererControlBase*)this;
	}
}

void ME::Controls::IRendererControlChild::Deattach(IRendererControlBase* a_control) noexcept
{
	std::lock_guard locker(lock);
	auto it = std::find(childs.begin(), childs.end(), a_control);
	if (it != childs.end())
	{
		(*it)->parent = nullptr;
		childs.erase(it);
	}
}