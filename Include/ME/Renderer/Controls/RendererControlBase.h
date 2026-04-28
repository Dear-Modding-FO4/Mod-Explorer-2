#pragma once

#include <ME/Renderer/Controls/IRendererControlBase.h>

namespace ME
{
	namespace Controls
	{
		class RendererControlBase :
			public IRendererControlBase
		{
		protected:
			virtual void DoDraw() const noexcept = 0;
		public:
			RendererControlBase(RendererControlBase* a_parent, const char* a_class, const char* a_caption);
			virtual ~RendererControlBase() = default;

			void Draw() const noexcept override;
		};
	}
}