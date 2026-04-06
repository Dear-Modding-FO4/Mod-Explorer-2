#pragma once

namespace ME
{
	class GraphicsObject
	{
		GraphicsObject(const GraphicsObject&) = delete;
		GraphicsObject(GraphicsObject&&) = delete;
		GraphicsObject& operator=(const GraphicsObject&) = delete;
		GraphicsObject& operator=(GraphicsObject&&) = delete;
	public:
		GraphicsObject() = default;
		virtual ~GraphicsObject() = default;

		virtual void Execute() const noexcept = 0;
	};
}