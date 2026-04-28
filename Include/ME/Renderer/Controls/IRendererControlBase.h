#pragma once

#include <ME/Core.h>
#include <ME/Graphics/GraphicsFont.h>
#include <string>

namespace ME
{
	namespace Controls
	{
		constexpr static Graphics::Vec4 BasementColor{ 0.647f, 0.882f, 1.f, 1.f };

		class IRendererControlBase
		{
		protected:
			Graphics::Vec2 position{ .0f, .0f };
			Graphics::Vec2 size{ 40.f, 40.f };
			std::string className{ "Base" };
			std::string caption{ "Base" };
			std::string hint{};
			IRendererControlBase* parent{ nullptr };
			Graphics::Vec4 colorbg{ 1.f, 1.f, 1.f, 1.f };
			bool round{ false };
			float roundSize;
			bool border{ true };
			float borderSize{ 1.0f };
			Graphics::Vec4 colorBorder{ 0.f, 0.f, 0.f, 1.f };
			Graphics::Vec4 colorText{ 0.f, 0.f, 0.f, 1.f };
			Graphics::Vec4 colorDisabledText{ .4f, .4f, .4f, 1.f };
			std::shared_ptr<Graphics::GraphicsFont> font{};
			bool enabled{ true };
			bool transparentBg{ false };
		public:
			IRendererControlBase(IRendererControlBase* a_parent, const char* a_class, const char* a_caption);
			virtual ~IRendererControlBase() = default;

			friend class IRendererControlChild;

			[[nodiscard]] virtual inline Graphics::Vec2 GetPosition() const noexcept { return position; }
			[[nodiscard]] virtual inline Graphics::Vec2 GetSize() const noexcept { return size; }
			[[nodiscard]] virtual inline std::string GetClassName() const noexcept { return className; }
			[[nodiscard]] virtual inline std::string GetCaption() const noexcept { return caption; }
			virtual inline void SetCaption(const char* a_text) noexcept { caption = a_text ? a_text : ""; }
			virtual inline void SetCaption(const std::string& a_text) noexcept { caption = a_text; }
			[[nodiscard]] virtual inline IRendererControlBase* GetParent() const noexcept { return parent; }
			virtual inline void SetPosition(const Graphics::Vec2& a_pos) noexcept { position = a_pos; }
			virtual inline void SetSize(const Graphics::Vec2& a_size) noexcept { size = a_size; }
			[[nodiscard]] virtual inline Graphics::Vec4 GetBackgroundColorV4() const noexcept { return colorbg; }
			[[nodiscard]] virtual inline ImColor GetBackgroundColor() const noexcept { return ImColor{ colorbg }; }
			virtual inline void SetBackgroundColor(const ImColor& a_color) noexcept { colorbg = ImGui::ColorConvertU32ToFloat4(a_color); }
			virtual inline void SetBackgroundColorV4(const Graphics::Vec4& a_color) noexcept { colorbg = a_color; }
			[[nodiscard]] virtual inline bool HasBorderRound() const noexcept { return round; }
			virtual inline void SetBorderRound(bool a_value) noexcept { round = a_value; }
			[[nodiscard]] virtual inline float GetBorderRoundSize() const noexcept { return roundSize; }
			virtual inline void SetBorderRoundSize(float a_size) noexcept { roundSize = a_size; }
			[[nodiscard]] virtual inline bool HasBorder() const noexcept { return round; }
			virtual inline void SetBorder(bool a_value) noexcept { border = a_value; }
			[[nodiscard]] virtual inline float GetBorderSize() const noexcept { return borderSize; }
			virtual inline void SetBorderSize(float a_size) noexcept { borderSize = a_size; }
			[[nodiscard]] virtual inline Graphics::Vec4 GetBorderColorV4() const noexcept { return colorBorder; }
			[[nodiscard]] virtual inline ImColor GetBorderColor() const noexcept { return ImColor{ colorBorder }; }
			virtual inline void SetBorderColor(const ImColor& a_color) noexcept { colorBorder = ImGui::ColorConvertU32ToFloat4(a_color); }
			virtual inline void SetBorderColorV4(const Graphics::Vec4& a_color) noexcept { colorBorder = a_color; }
			[[nodiscard]] virtual inline Graphics::Vec4 GetTextColorV4() const noexcept { return colorText; }
			[[nodiscard]] virtual inline ImColor GetTextColor() const noexcept { return ImColor{ colorText }; }
			virtual inline void SetTextColor(const ImColor& a_color) noexcept { colorText = ImGui::ColorConvertU32ToFloat4(a_color); }
			virtual inline void SetTextColorV4(const Graphics::Vec4& a_color) noexcept { colorText = a_color; }
			[[nodiscard]] virtual inline Graphics::Vec4 GetDisabledTextColorV4() const noexcept { return colorDisabledText; }
			[[nodiscard]] virtual inline ImColor GetDisabledTextColor() const noexcept { return ImColor{ colorDisabledText }; }
			virtual inline void SetDisabledTextColor(const ImColor& a_color) noexcept { colorDisabledText = ImGui::ColorConvertU32ToFloat4(a_color); }
			virtual inline void SetDisabledTextColorV4(const Graphics::Vec4& a_color) noexcept { colorDisabledText = a_color; }
			[[nodiscard]] virtual std::shared_ptr<Graphics::GraphicsFont> GetFont() const noexcept { return font; }
			virtual void SetFont(const std::shared_ptr<Graphics::GraphicsFont>& a_font) noexcept { font = a_font; }
			[[nodiscard]] virtual inline bool HasEnabled() const noexcept { return enabled; }
			virtual inline void SetEnabled(bool a_value) noexcept { enabled = a_value; }
			[[nodiscard]] virtual inline bool HasTransparentBackground() const noexcept { return transparentBg; }
			virtual inline void SetTransparentBackground(bool a_value) noexcept { transparentBg = a_value; }
			[[nodiscard]] virtual inline std::string GetHint() const noexcept { return hint; }
			virtual inline void SetHint(const char* a_text) noexcept { hint = a_text ? a_text : ""; }

			virtual void Draw() const noexcept = 0;
		};
	}
}