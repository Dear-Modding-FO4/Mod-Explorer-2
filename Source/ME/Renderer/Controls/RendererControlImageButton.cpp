#include <ME/Renderer/Controls/RendererControlImageButton.h>
#include <ME/Graphics/GraphicsShaders.h>
#include <imgui_internal.h>

void ME::Controls::RendererControlImageButton::DrawButtonStage(uint8_t a_state) const noexcept
{
	auto drawList = ImGui::GetWindowDrawList();
	if (images[a_state])
	{
		auto imageSize = std::min(size.x, size.y);
		drawList->AddImage((ImTextureID)(intptr_t)(images[a_state]->GetResourceView())->Get(), position, position + imageSize);

		auto caption = GetCaption();
		if (caption.length())
		{
			auto textSize = ImGui::CalcTextSize(caption.c_str());
			drawList->AddText(position + ImVec2{ imageSize + 4.f, (size.y - textSize.y) * .5f },
				enabled ? ImColor{ colorText } : ImColor{ colorDisabledText }, caption.c_str());
		}
	}
	else if (caption.length())
	{
		auto textSize = ImGui::CalcTextSize(caption.c_str());
		drawList->AddText(position + ImVec2{ 0.f, (size.y - textSize.y) * .5f },
			enabled ? ImColor{ colorText } : ImColor{ colorDisabledText }, caption.c_str());
	}
}

void ME::Controls::RendererControlImageButton::DoDraw() const noexcept
{
	ImGui::BeginDisabled(!enabled);

	ImRect bb{ position.x, position.y, position.x + size.x, position.y + size.y };
	ImGui::ItemSize(bb);
	if (ImGui::ItemAdd(bb, 0))
	{
		if (enabled)
		{
			if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
			{
				DrawButtonStage(1);
				DoHovered();
			}
			else
				goto __default;

			if (ImGui::IsItemClicked())
			{
				DrawButtonStage(2);
				DoClicked();
			}
		}
		else
		{
		__default:
			DrawButtonStage(0);
		}
	}

	ImGui::EndDisabled();
}

ME::Controls::RendererControlImageButton::RendererControlImageButton(RendererControlBase* a_parent, const char* a_class, const char* a_caption) :
	RendererControlBase(a_parent, a_class, a_caption)
{}

bool ME::Controls::RendererControlImageButton::SetImageDefaultByResId(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext,
	const char* a_type, uint32_t a_resourceID, bool a_wic) noexcept
{
	images[0] = std::make_unique<ME::GraphicsImage>(a_device, a_deviceContext);
	if (images[0])
	{
		auto result = images[0]->LoadFromResource(a_type, a_resourceID, a_wic);
		if (!result) images[0].reset();
		return result;
	}
	return false;
}

bool ME::Controls::RendererControlImageButton::SetImageHoverByResId(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext,
	const char* a_type, uint32_t a_resourceID, bool a_wic) noexcept
{
	images[1] = std::make_unique<ME::GraphicsImage>(a_device, a_deviceContext);
	if (images[1])
	{
		auto result = images[1]->LoadFromResource(a_type, a_resourceID, a_wic);
		if (!result) images[1].reset();
		return result;
	}
	return false;
}

bool ME::Controls::RendererControlImageButton::SetImagePressedByResId(REX::W32::ID3D11Device* a_device, REX::W32::ID3D11DeviceContext* a_deviceContext,
	const char* a_type, uint32_t a_resourceID, bool a_wic) noexcept
{
	images[2] = std::make_unique<ME::GraphicsImage>(a_device, a_deviceContext);
	if (images[2]) 
	{
		auto result = images[2]->LoadFromResource(a_type, a_resourceID, a_wic);
		if (!result) images[2].reset();
		return result;
	}
	return false;
}