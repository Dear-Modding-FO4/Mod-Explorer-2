#pragma once

#include <ME/Core.h>
#include <REX/REX/Singleton.h>
#include <cassert>
#include <vector>
#include <mutex>

namespace ME
{
	class GraphicsManager :
		public REX::Singleton<GraphicsManager>
	{
		std::vector<std::shared_ptr<GraphicsObject>> list{};
		std::recursive_mutex lock{};

		GraphicsManager(const GraphicsManager&) = delete;
		GraphicsManager(GraphicsManager&&) = delete;
		GraphicsManager& operator=(const GraphicsManager&) = delete;
		GraphicsManager& operator=(GraphicsManager&&) = delete;
	public:
		constexpr static auto FailIndex = std::numeric_limits<size_t>::max();

		GraphicsManager() = default;
		virtual ~GraphicsManager() = default;

		std::shared_ptr<GraphicsObject> operator[](size_t a_idx) const noexcept { return GetAt(a_idx); }

		virtual size_t Add(const std::shared_ptr<GraphicsObject> a_object) noexcept;
		virtual bool Remove(size_t a_idx) noexcept;
		virtual void Cleanup() noexcept;
		virtual size_t Count() noexcept;
		virtual std::shared_ptr<GraphicsObject> GetAt(size_t a_idx) const noexcept { assert(Count() > a_idx); return list[a_idx]; }

		virtual void Execute() noexcept;
	};
}