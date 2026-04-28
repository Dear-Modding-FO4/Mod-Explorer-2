#include <ME/Renderer/RendererWarningMessage.h>
#include <ME/Graphics/GraphicsFont.h>
#include <ME/Shop.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace ME
{
	bool g_RendererWarningMessageShowing = false;
	RendererWarningMessage* g_RendererWarningMessage = nullptr;
	constexpr static auto ImWarningMessageClassPopupModal = "RendererWarningMessage##pme2RendererWarningMessage";
	extern std::shared_ptr<Graphics::GraphicsFont> CaptionMessageFont;
	extern std::shared_ptr<Graphics::GraphicsFont> DefaultMessageFont;
	extern std::shared_ptr<Graphics::GraphicsFont> DefaultTextMessageFont;

	bool Renderer_CheckResult(bool a_condition, const std::string_view& a_shortMessage, const std::string_view& a_message) noexcept
	{
		if (!a_condition && g_RendererWarningMessage && !g_RendererWarningMessageShowing)
		{
			g_RendererWarningMessage->SetMessage(a_shortMessage, a_message);
			g_RendererWarningMessageShowing = true;
			ImGui::OpenPopup(ImWarningMessageClassPopupModal);
		}

		return a_condition;
	}
}

ME::RendererWarningMessage::RendererWarningMessage()
{
	assert(!g_RendererWarningMessage);
	if (g_RendererWarningMessage)
		REX::CRITICAL("g_RendererWarningMessage already is initialized"sv);
	else
		g_RendererWarningMessage = this;
}

void ME::RendererWarningMessage::SetMessage(const std::string_view& a_shortMessage, const std::string_view& a_message) noexcept
{
	shortMessage = a_shortMessage;
	message = a_message;
}

void ME::RendererWarningMessage::Execute() const noexcept
{
	if (g_RendererWarningMessageShowing)
	{
		auto vWndSize = ImVec2(400.f, 408.f);
		ImGui::SetNextWindowSize(vWndSize);
		if (ImGui::BeginPopupModal(ImWarningMessageClassPopupModal, &g_RendererWarningMessageShowing,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			auto drawList = ImGui::GetWindowDrawList();

			auto pos = Graphics::Vec2{ ImGui::GetWindowPos() };
			auto size = Graphics::Vec2{ ImGui::GetWindowSize() };
			Graphics::Vec2 curPos = pos;

			////////////////////////////////
			// Sizes 

			auto capSize = Graphics::Vec2{ size.x, 96.f };
			auto shortMsgSize = Graphics::Vec2{ size.x, 50.f };
			auto msgSize = Graphics::Vec2{ size.x, 200.f };
			auto btnSize = Graphics::Vec2{ size.x, 50.f };
			
			constexpr static auto COLOR_BACK = IM_COL32(237, 55, 55, 32);
			constexpr static auto COLOR_BRDR = IM_COL32(237, 55, 55, 48);
			constexpr static auto COLOR_TEXT = IM_COL32(237, 55, 55, 255);

			////////////////////////////////
			// Caption 

			// Push the font onto the stack
			if (CaptionMessageFont)
				CaptionMessageFont->Push();

			constexpr static auto CAPTION = "WARNING";
			auto textSize = ImGui::CalcTextSize(CAPTION);
			auto textPos = curPos + ((capSize - textSize) * .5f);	textPos.y += 4.f;
			drawList->PushClipRect(curPos, { curPos.x + capSize.x, curPos.y + capSize.y }, false);
			drawList->AddRectFilled(pos, pos + size, COLOR_BACK);
			drawList->AddRect(pos, pos + size, COLOR_BRDR);
			drawList->AddText(textPos, COLOR_TEXT, CAPTION);
			drawList->PopClipRect();
			curPos.y += capSize.y + 4.f;

			// Pop the pushed font 
			if (CaptionMessageFont)
				CaptionMessageFont->Pop();

			////////////////////////////////
			// Short message 

			// Push the font onto the stack
			if (DefaultMessageFont)
				DefaultMessageFont->Push();

			textSize = ImGui::CalcTextSize(shortMessage.c_str());
			drawList->PushClipRect(curPos, { curPos.x + shortMsgSize.x, curPos.y + shortMsgSize.y }, false);
			drawList->AddRectFilled(pos, pos + size, COLOR_BACK);
			drawList->AddRect(pos, pos + size, COLOR_BRDR);
			drawList->AddText(curPos + ((shortMsgSize - textSize) * .5f), COLOR_TEXT, shortMessage.c_str());
			drawList->PopClipRect();
			curPos.y += shortMsgSize.y + 4.f;

			// Pop the pushed font 
			if (DefaultMessageFont)
				DefaultMessageFont->Pop();

			////////////////////////////////
			// Message 

			// Push the font onto the stack
			if (DefaultTextMessageFont)
				DefaultTextMessageFont->Push();

			drawList->PushClipRect(curPos, { curPos.x + msgSize.x, curPos.y + msgSize.y }, false);
			drawList->AddRectFilled(pos, pos + size, COLOR_BACK);
			drawList->AddRect(pos, pos + size, COLOR_BRDR);
			drawList->PopClipRect();

			auto posIm = Graphics::Vec2{ curPos.x + 8.f - pos.x, curPos.y + 8.f - pos.y };
			auto sizeIm = Graphics::Vec2{ msgSize.x - 16.f, msgSize.y - 16.f };
			ImGui::SetCursorPos(posIm); posIm += pos;
			ImGui::PushClipRect(posIm, posIm + sizeIm, false);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.f , 0.f , 0.f , 0.f });
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ ImColor{ COLOR_TEXT } });
			ImGui::PushStyleColor(ImGuiCol_InputTextCursor, ImVec4{ ImColor{ COLOR_TEXT } });
			ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, ImVec4{ ImColor{ IM_COL32(237, 55, 55, 24) } });
			ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, { 0.f , 0.f , 0.f , 0.f });
			ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4{ ImColor{ IM_COL32(237, 55, 55, 24) } });
			ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, ImVec4{ ImColor{ IM_COL32(237, 55, 55, 32) } });
			ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, ImVec4{ ImColor{ IM_COL32(237, 55, 55, 40) } });
			ImGui::InputTextMultiline("message##pme2WarningMessageInputText", const_cast<char*>(message.c_str()), message.length(), 
				sizeIm, ImGuiInputTextFlags_WordWrap | ImGuiInputTextFlags_ReadOnly);
			ImGui::PopStyleColor(8);
			ImGui::PopClipRect();
			
			curPos.y += msgSize.y + 4.f;

			// Pop the pushed font 
			if (DefaultTextMessageFont)
				DefaultTextMessageFont->Pop();

			////////////////////////////////
			// Button 

			// Push the font onto the stack
			if (DefaultMessageFont)
				DefaultMessageFont->Push();

			drawList->PushClipRect(curPos, { curPos.x + btnSize.x, curPos.y + btnSize.y }, false);
			drawList->AddRectFilled(pos, pos + size, COLOR_BACK);
			drawList->AddRect(pos, pos + size, COLOR_BRDR);

			constexpr static auto ABORT = "ABORT";
			textSize = ImGui::CalcTextSize(ABORT);
			ImRect bb{ curPos.x + 1.f, curPos.y, curPos.x + btnSize.x - 1.f, curPos.y + btnSize.y - 1.f };
			ImGui::ItemSize(bb);
			if (ImGui::ItemAdd(bb, 0))
			{
				if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
				{
					drawList->AddRectFilledMultiColor(bb.Min, bb.Max, IM_COL32(237, 55, 55, 8), IM_COL32(237, 55, 55, 8),
						IM_COL32(237, 55, 55, 24), IM_COL32(237, 55, 55, 24));
					drawList->AddText(curPos + ((btnSize - textSize) * .5f), COLOR_TEXT, ABORT);
				}
				else
				{
					drawList->AddRectFilledMultiColor(bb.Min, bb.Max, IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 0),
						IM_COL32(237, 55, 55, 16), IM_COL32(237, 55, 55, 16));
					drawList->AddText(curPos + ((btnSize - textSize) * .5f), COLOR_TEXT, ABORT);
				}
				
				if (ImGui::IsItemClicked())
				{
					// Closes the modal and shop
					ImGui::CloseCurrentPopup();
					g_RendererWarningMessageShowing = false;
					Shop::GetSingleton()->Close();
				}
			}

			drawList->PopClipRect();

			// Pop the pushed font 
			if (DefaultMessageFont)
				DefaultMessageFont->Pop();

			////////////////////////////////

			// End
			ImGui::EndPopup();
		}
	}
}