#pragma once

#include <ME/Core.h>
#include <ME/Graphics/GraphicsObject.h>
#include <memory>

namespace ME
{
    class RendererCursor final : public GraphicsObject
    {
        std::shared_ptr<GraphicsImage> image;

        RendererCursor(const RendererCursor&) = delete;
        RendererCursor(RendererCursor&&) = delete;
        RendererCursor& operator=(const RendererCursor&) = delete;
        RendererCursor& operator=(RendererCursor&&) = delete;
    public:
        RendererCursor(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
        ~RendererCursor() override = default;

        void Execute() const noexcept override;
    };
}

