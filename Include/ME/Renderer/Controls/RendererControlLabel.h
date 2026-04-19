#pragma once

#include <ME/Renderer/Controls/RendererControlBase.h>

namespace ME
{
	namespace Controls
	{
		class RendererControlLabel final :
			public RendererControlBase
		{
		protected:
			void DoDraw() const noexcept override;
		public:
			RendererControlLabel(RendererControlBase* a_parent, const char* a_class, const char* a_caption);
			~RendererControlLabel() override = default;
		};
	}
}