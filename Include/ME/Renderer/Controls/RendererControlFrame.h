#pragma once

#include <ME/Renderer/Controls/IRendererControlBase.h>
#include <ME/Renderer/Controls/IRendererControlChild.h>
#include <vector>
#include <mutex>

namespace ME
{
	namespace Controls
	{
		class RendererControlFrame final :
			public IRendererControlBase,
			public IRendererControlChild
		{
		public:
			RendererControlFrame(RendererControlBase* a_parent, const char* a_class, const char* a_caption);
			~RendererControlFrame() override = default;

			void Draw() const noexcept override;
		};
	}
}