#include <ME/Renderer/RendererConfirmMessage.h>
#include <ME/Graphics/GraphicsFont.h>
#include <ME/Shop.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace ME
{
	bool* g_RendererConfirmMessageResult = nullptr;
	bool g_RendererConfirmMessageShowing = false;
	RendererConfirmMessage* g_RendererConfirmMessage = nullptr;
	constexpr static auto ImWarningMessageClassPopupModal = "RendererConfirmMessage##pme2RendererConfirmMessage";
	extern std::shared_ptr<Graphics::GraphicsFont> DefaultMessageFont;
	extern std::shared_ptr<Graphics::GraphicsFont> DefaultTextMessageFont;
	extern std::shared_ptr<Graphics::GraphicsFont> UserCaptionMessageFont;
	extern std::shared_ptr<Graphics::GraphicsFont> UserDefaultTextMessageFont;

	void Renderer_ConfirmMessage(bool& a_result, const std::string_view& a_caption, const std::string_view& a_message) noexcept
	{
		if (g_RendererConfirmMessage && !g_RendererConfirmMessageShowing)
		{
			g_RendererConfirmMessage->SetMessage(a_caption, a_message);
			g_RendererConfirmMessageShowing = true;
			g_RendererConfirmMessageResult = &a_result;
			ImGui::OpenPopup(ImWarningMessageClassPopupModal);
		}
	}
}

ME::RendererConfirmMessage::RendererConfirmMessage()
{
	assert(!g_RendererConfirmMessage);
	if (g_RendererConfirmMessage)
		REX::CRITICAL("g_RendererConfirmMessage already is initialized"sv);
	else
		g_RendererConfirmMessage = this;
}

void ME::RendererConfirmMessage::SetMessage(const std::string_view& a_caption, const std::string_view& a_message) noexcept
{
	caption = a_caption;
	message = a_message;
}

void ME::RendererConfirmMessage::Execute() const noexcept
{
	if (g_RendererConfirmMessageShowing)
	{
		auto vWndSize = ImVec2(400.f, 208.f);
		ImGui::SetNextWindowSize(vWndSize);
		if (ImGui::BeginPopupModal(ImWarningMessageClassPopupModal, &g_RendererConfirmMessageShowing,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			auto drawList = ImGui::GetWindowDrawList();

			auto pos = Graphics::Vec2{ ImGui::GetWindowPos() };
			auto size = Graphics::Vec2{ ImGui::GetWindowSize() };
			Graphics::Vec2 curPos = pos;

			////////////////////////////////
			// Sizes 

			auto shortMsgSize = Graphics::Vec2{ size.x, 50.f };
			auto msgSize = Graphics::Vec2{ size.x, 100.f };
			auto btnSize = Graphics::Vec2{ size.x, 50.f };
			
			constexpr static auto COLOR_BACK = IM_COL32(37, 194, 189, 32);
			constexpr static auto COLOR_BRDR = IM_COL32(37, 194, 189, 48);
			constexpr static auto COLOR_BCK2 = IM_COL32(37, 194, 189, 24);
			constexpr static auto COLOR_TEXT = IM_COL32(37, 194, 189, 255);

			////////////////////////////////
			// Caption 

			// Push the font onto the stack
			if (UserCaptionMessageFont)
				UserCaptionMessageFont->Push();
			else
				DefaultMessageFont->Push();

			auto textSize = ImGui::CalcTextSize(caption.c_str());
			drawList->PushClipRect(curPos, { curPos.x + shortMsgSize.x, curPos.y + shortMsgSize.y }, false);
			drawList->AddRectFilled(pos, pos + size, COLOR_BACK);
			drawList->AddRect(pos, pos + size, COLOR_BRDR);
			drawList->AddText(curPos + ((shortMsgSize - textSize) * .5f), COLOR_TEXT, caption.c_str());
			drawList->PopClipRect();
			curPos.y += shortMsgSize.y + 4.f;

			// Pop the pushed font 
			if (UserCaptionMessageFont)
				UserCaptionMessageFont->Pop();
			else
				DefaultMessageFont->Pop();

			////////////////////////////////
			// Message 

			// Push the font onto the stack
			if (UserDefaultTextMessageFont)
				UserDefaultTextMessageFont->Push();
			else
				DefaultTextMessageFont->Push();

			drawList->PushClipRect(curPos, { curPos.x + msgSize.x, curPos.y + msgSize.y }, false);
			drawList->AddRectFilled(pos, pos + size, COLOR_BCK2);
			drawList->AddRect(pos, pos + size, COLOR_BRDR);
			drawList->PopClipRect();

			auto posIm = Graphics::Vec2{ curPos.x + 8.f - pos.x, curPos.y + 8.f - pos.y };
			auto sizeIm = Graphics::Vec2{ msgSize.x - 16.f, msgSize.y - 16.f };
			ImGui::SetCursorPos(posIm); posIm += pos;
			ImGui::PushClipRect(posIm, posIm + sizeIm, false);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.f , 0.f , 0.f , 0.f });
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ ImColor{ COLOR_TEXT } });
			ImGui::PushStyleColor(ImGuiCol_InputTextCursor, ImVec4{ ImColor{ COLOR_TEXT } });
			ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, ImVec4{ ImColor{ IM_COL32(37, 194, 189, 24) } });
			ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, { 0.f , 0.f , 0.f , 0.f });
			ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4{ ImColor{ IM_COL32(37, 194, 189, 24) } });
			ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, ImVec4{ ImColor{ IM_COL32(37, 194, 189, 32) } });
			ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, ImVec4{ ImColor{ IM_COL32(37, 194, 189, 40) } });
			ImGui::InputTextMultiline("message##pme2ConfirmMessageInputText", const_cast<char*>(message.c_str()), message.length(), 
				sizeIm, ImGuiInputTextFlags_WordWrap | ImGuiInputTextFlags_ReadOnly);
			ImGui::PopStyleColor(8);
			ImGui::PopClipRect();
			
			curPos.y += msgSize.y + 4.f;

			// Pop the pushed font 
			if (UserDefaultTextMessageFont)
				UserDefaultTextMessageFont->Pop();
			else
				DefaultTextMessageFont->Pop();

			////////////////////////////////
			// Button 

			// Push the font onto the stack
			if (UserCaptionMessageFont)
				UserCaptionMessageFont->Push();
			else
				DefaultMessageFont->Push();

			drawList->PushClipRect(curPos, { curPos.x + btnSize.x, curPos.y + btnSize.y }, false);
			drawList->AddRectFilled(pos, pos + size, COLOR_BACK);
			drawList->AddRect(pos, pos + size, COLOR_BRDR);
			btnSize.x /= 2.f;

			constexpr static auto CANCEL = "Cancel";
			textSize = ImGui::CalcTextSize(CANCEL);
			ImRect bb{ curPos.x + 1.f, curPos.y, curPos.x + btnSize.x - 1.f, curPos.y + btnSize.y - 1.f };
			ImGui::ItemSize(bb);
			if (ImGui::ItemAdd(bb, 0))
			{
				if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
				{
					drawList->AddRectFilledMultiColor(bb.Min, bb.Max, IM_COL32(37, 194, 189, 16), IM_COL32(37, 194, 189, 16),
						IM_COL32(37, 194, 189, 32), IM_COL32(37, 194, 189, 32));
					drawList->AddText(curPos + ((btnSize - textSize) * .5f), COLOR_TEXT, CANCEL);
				}
				else
				{
					drawList->AddRectFilledMultiColor(bb.Min, bb.Max, IM_COL32(37, 194, 189, 8), IM_COL32(37, 194, 189, 8),
						IM_COL32(37, 194, 189, 24), IM_COL32(37, 194, 189, 24));
					drawList->AddText(curPos + ((btnSize - textSize) * .5f), COLOR_TEXT, CANCEL);
				}
				
				if (ImGui::IsItemClicked())
				{
					if (g_RendererConfirmMessageResult)
					{
						*g_RendererConfirmMessageResult = false;
						g_RendererConfirmMessageResult = nullptr;
					}

					// Closes the modal and shop
					ImGui::CloseCurrentPopup();
					g_RendererConfirmMessageShowing = false;
				}
			}

			curPos.x += btnSize.x;

			constexpr static auto OK = "Ok";
			textSize = ImGui::CalcTextSize(OK);
			bb = { curPos.x + 1.f, curPos.y, curPos.x + btnSize.x - 1.f, curPos.y + btnSize.y - 1.f };
			ImGui::ItemSize(bb);
			if (ImGui::ItemAdd(bb, 0))
			{
				if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
				{
					drawList->AddRectFilledMultiColor(bb.Min, bb.Max, IM_COL32(37, 194, 189, 16), IM_COL32(37, 194, 189, 16),
						IM_COL32(37, 194, 189, 32), IM_COL32(37, 194, 189, 32));
					drawList->AddText(curPos + ((btnSize - textSize) * .5f), COLOR_TEXT, OK);
				}
				else
				{
					drawList->AddRectFilledMultiColor(bb.Min, bb.Max, IM_COL32(37, 194, 189, 8), IM_COL32(37, 194, 189, 8),
						IM_COL32(37, 194, 189, 24), IM_COL32(37, 194, 189, 24));
					drawList->AddText(curPos + ((btnSize - textSize) * .5f), COLOR_TEXT, OK);
				}

				if (ImGui::IsItemClicked())
				{
					if (g_RendererConfirmMessageResult)
					{
						*g_RendererConfirmMessageResult = true;
						g_RendererConfirmMessageResult = nullptr;
					}

					// Closes the modal and shop
					ImGui::CloseCurrentPopup();
					g_RendererConfirmMessageShowing = false;
				}
			}

			drawList->PopClipRect();

			// Pop the pushed font 
			if (UserCaptionMessageFont)
				UserCaptionMessageFont->Pop();
			else
				DefaultMessageFont->Pop();

			////////////////////////////////

			// End
			ImGui::EndPopup();
		}
	}
}