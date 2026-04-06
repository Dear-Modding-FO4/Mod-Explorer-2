#pragma once

#include <ME/Core.h>
#include <REX/REX/Singleton.h>
#include <RE/B/BSGraphics.h>

namespace ME
{
	class GraphicsSystem :
		public REX::Singleton<GraphicsSystem>
	{
	public:
		enum class Ratio : uint8_t
		{
			kRatio_4x3 = 0,
			kRatio_16x9,
			kRatio_21x9,
		};

		GraphicsSystem() = default;
		virtual ~GraphicsSystem() = default;

		virtual bool Initialize() noexcept;
		virtual bool InitializeContinue() noexcept;
		virtual void Release() noexcept;

		virtual RE::BSGraphics::RendererData* GetRendererData() const noexcept { return rendererData; };
		virtual float GetRatio() const noexcept { return ratio; };
		virtual Ratio GetRatioEnum() const noexcept { return ratioEnum; };
	private:
		GraphicsSystem(const GraphicsSystem&) = delete;
		GraphicsSystem(GraphicsSystem&&) = delete;
		GraphicsSystem& operator=(const GraphicsSystem&) = delete;
		GraphicsSystem& operator=(GraphicsSystem&&) = delete;

		static void* GameSubInRenderWndSwap(void* _this) noexcept;
		static void GameRendererUI(uint32_t a_unk) noexcept;
	private:
		decltype(&GameSubInRenderWndSwap) subInRenderWndSwap{ nullptr };
		decltype(&GameRendererUI) rendererUI{ nullptr };
		decltype(&GameRendererUI) rendererUIRestore{ nullptr };

		RE::BSGraphics::RendererData* rendererData{ nullptr };
		bool horzDisplay{ false };
		float ratio{ .0f };
		Ratio ratioEnum{ Ratio::kRatio_4x3 };
	};
}