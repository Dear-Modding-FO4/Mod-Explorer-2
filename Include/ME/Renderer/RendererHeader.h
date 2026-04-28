#pragma once

#include <ME/Core.h>
#include <ME/Graphics/GraphicsObject.h>
#include <ME/Renderer/Controls/RendererControlImageButton.h>
#include <ME/Renderer/Controls/RendererControlAnimLogoSpinner.h>
#include <memory>

namespace ME
{
    class RendererHeader final : public GraphicsObject
    {
        std::shared_ptr<Controls::RendererControlFrame> frame;
        std::unique_ptr<Controls::RendererControlAnimLogoSpinner> logo;
        std::unique_ptr<Controls::RendererControlImageButton> quitBtn;

        RendererHeader(const RendererHeader&) = delete;
        RendererHeader(RendererHeader&&) = delete;
        RendererHeader& operator=(const RendererHeader&) = delete;
        RendererHeader& operator=(RendererHeader&&) = delete;
    public:
        RendererHeader(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext);
        ~RendererHeader() override = default;

        void Execute() const noexcept override;
    };
}
