#pragma once

#include <ME/Core.h>
#include <ME/Graphics/GraphicsObject.h>
#include <memory>

namespace ME
{
    class RendererWarningMessage final :
        public GraphicsObject
    {
        std::string shortMessage;
        std::string message;

        RendererWarningMessage(const RendererWarningMessage&) = delete;
        RendererWarningMessage(RendererWarningMessage&&) = delete;
        RendererWarningMessage& operator=(const RendererWarningMessage&) = delete;
        RendererWarningMessage& operator=(RendererWarningMessage&&) = delete;
    public:
        RendererWarningMessage();
        ~RendererWarningMessage() override = default;

        void SetMessage(const std::string_view& a_shortMessage, const std::string_view& a_message) noexcept;
        void Execute() const noexcept override;
    };
}