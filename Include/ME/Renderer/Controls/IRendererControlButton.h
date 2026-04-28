#pragma once

#include <ME/Core.h>
#include <vector>
#include <mutex>

namespace ME
{
	namespace Controls
	{
		class IRendererControlButton
		{
		protected:
			void DoHovered() const noexcept;
			void DoClicked() const noexcept;
		public:
			IRendererControlButton() = default;
			virtual ~IRendererControlButton() = default;

			std::function<void(IRendererControlBase*)> OnHovered{};
			std::function<void(IRendererControlBase*)> OnClicked{};
		};
	}
}