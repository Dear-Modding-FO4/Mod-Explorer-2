#pragma once

#include <ME/Core.h>
#include <vector>
#include <mutex>

namespace ME
{
	namespace Controls
	{
		class IRendererControlChild
		{
		protected:
			std::recursive_mutex lock{};
			std::vector<IRendererControlBase*> childs{};
		public:
			IRendererControlChild() = default;
			virtual ~IRendererControlChild() = default;

			virtual void Attach(IRendererControlBase* a_control) noexcept;
			virtual void Deattach(IRendererControlBase* a_control) noexcept;
		};
	}
}