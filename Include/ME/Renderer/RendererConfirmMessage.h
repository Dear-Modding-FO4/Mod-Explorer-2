#pragma once

#include <ME/Core.h>
#include <ME/Graphics/GraphicsObject.h>
#include <memory>

namespace ME
{
    class RendererConfirmMessage final :
        public GraphicsObject
    {
        std::string caption;
        std::string message;

        RendererConfirmMessage(const RendererConfirmMessage&) = delete;
        RendererConfirmMessage(RendererConfirmMessage&&) = delete;
        RendererConfirmMessage& operator=(const RendererConfirmMessage&) = delete;
        RendererConfirmMessage& operator=(RendererConfirmMessage&&) = delete;
    public:
        RendererConfirmMessage();
        ~RendererConfirmMessage() override = default;

        void SetMessage(const std::string_view& a_caption, const std::string_view& a_message) noexcept;
        void Execute() const noexcept override;
    };
}