#pragma once

#include <ME/Core.h>
#include <ME/Graphics/GraphicsObject.h>

namespace ME
{
	class RendererBackground final : public GraphicsObject
	{
        RendererBackground(const RendererBackground&) = delete;
        RendererBackground(RendererBackground&&) = delete;
        RendererBackground& operator=(const RendererBackground&) = delete;
        RendererBackground& operator=(RendererBackground&&) = delete;
    public:
        RendererBackground();
        ~RendererBackground() override = default;

        void Execute() const noexcept override;
	};
}

