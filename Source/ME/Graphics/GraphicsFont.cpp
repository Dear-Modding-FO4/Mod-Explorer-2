#include <ME/Graphics/GraphicsFont.h>
#include <ShlObj_core.h>
#include <Windows.h>
#include <filesystem>
#include <stdlib.h>

ME::Graphics::GraphicsFont::~GraphicsFont()
{
	Release();
}

bool ME::Graphics::GraphicsFont::OpenFromFile(const std::string& a_filename, float a_size) noexcept
{
	Release();

	char path[MAX_PATH]{};

	std::filesystem::path filename = a_filename;
	if (filename.is_relative())
	{
		SHGetFolderPath(nullptr, CSIDL_FONTS, nullptr, SHGFP_TYPE_CURRENT, path);
		strcat(path, "/");
		strcat(path, a_filename.c_str());
	}
	else
		strcpy_s(path, a_filename.c_str());

	if (!std::filesystem::exists(path) || std::filesystem::is_directory(filename))
		return false;

	static constexpr ImWchar ranges[] =
	{
		0x0020, 0xFFFD, // ALL
		0,
	};

	ImFontConfig font_config;
	font_config.OversampleH = 1; //or 2 is the same
	font_config.OversampleV = 1;
	font_config.PixelSnapH = true;

	font = ImGui::GetIO().Fonts->AddFontFromFileTTF(path, a_size, &font_config, ranges);
	return font != nullptr;
}

bool ME::Graphics::GraphicsFont::OpenFromStream(const F4SE::IBufferStream& a_stream, float a_size) noexcept
{
	Release();

	if (!a_stream.GetLength() || !a_stream.GetBuffer())
		return false;

	static constexpr ImWchar ranges[] =
	{
		0x0020, 0xFFFD, // ALL
		0,
	};

	ImFontConfig font_config;
	font_config.OversampleH = 1; //or 2 is the same
	font_config.OversampleV = 1;
	font_config.PixelSnapH = true;

	font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(a_stream.GetBuffer(), (int32_t)a_stream.GetLength(), a_size, &font_config, ranges);
	return font != nullptr;
}

void ME::Graphics::GraphicsFont::Push() noexcept
{
	if (font)
		ImGui::PushFont(font);
}

void ME::Graphics::GraphicsFont::Pop() noexcept
{
	if (font)
		ImGui::PopFont();
}

void ME::Graphics::GraphicsFont::Release() noexcept
{
	if (font)
	{
		Pop();

		ImGui::GetIO().Fonts->RemoveFont(font);
		font = nullptr;
	}
}