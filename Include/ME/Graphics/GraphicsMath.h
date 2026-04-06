#pragma once

#include <imgui.h>
#include <cmath>

namespace ME
{
	namespace Graphics
	{
		struct Vec2 : public ImVec2
		{
			constexpr Vec2() : ImVec2() {}
			constexpr Vec2(float a_x, float a_y) : ImVec2(a_x, a_y) {}
			constexpr Vec2(const ImVec2& a_rhs) : ImVec2(a_rhs.x, a_rhs.y) {}
			constexpr Vec2(const Vec2& a_rhs) : ImVec2(a_rhs.x, a_rhs.y) {}

			inline Vec2& operator=(const Vec2& a_rhs) { x = a_rhs.x; y = a_rhs.y; return *this; }

			inline bool operator==(const Vec2& a_rhs) const noexcept
			{
				return (fabs(x - a_rhs.x) <= 0.001) && (fabs(y - a_rhs.y) <= 0.001);
			}

			inline bool operator!=(const Vec2& a_rhs) const noexcept
			{
				return (fabs(x - a_rhs.x) > 0.001) || (fabs(y - a_rhs.y) > 0.001);
			}

			inline Vec2 operator+(const Vec2& a_rhs) const noexcept
			{
				return Vec2{ x + a_rhs.x, y + a_rhs.y };
			}

			inline Vec2 operator-(const Vec2& a_rhs) const noexcept
			{
				return Vec2{ x - a_rhs.x, y - a_rhs.y };
			}

			inline Vec2 operator+(float a_value) const noexcept
			{
				return Vec2{ x + a_value, y + a_value };
			}

			inline Vec2 operator-(float a_value) const noexcept
			{
				return Vec2{ x - a_value, y - a_value };
			}

			inline Vec2& operator+=(const Vec2& a_rhs) noexcept
			{
				x += a_rhs.x;
				y += a_rhs.y;
				return *this;
			}

			inline Vec2& operator-=(const Vec2& a_rhs) noexcept
			{
				x -= a_rhs.x;
				y -= a_rhs.y;
				return *this;
			}

			inline Vec2& operator+=(float a_value) noexcept
			{
				x += a_value;
				y += a_value;
				return *this;
			}

			inline Vec2& operator-=(float a_value) noexcept
			{
				x -= a_value;
				y -= a_value;
				return *this;
			}

			inline Vec2 operator*(float a_value) const noexcept
			{
				return Vec2{ x * a_value, y * a_value };
			}

			inline Vec2 operator/(float a_value) const noexcept
			{
				return Vec2{ x / a_value, y / a_value };
			}

			inline Vec2& operator*=(float a_value) noexcept
			{
				x *= a_value;
				y *= a_value;
				return *this;
			}

			inline Vec2& operator/=(float a_value) noexcept
			{
				x /= a_value;
				y /= a_value;
				return *this;
			}

			float& operator[] (size_t a_idx) { IM_ASSERT(a_idx == 0 || a_idx == 1); return ((float*)this)[a_idx]; }
			float  operator[] (size_t a_idx) const { IM_ASSERT(a_idx == 0 || a_idx == 1); return ((const float*)this)[a_idx]; }
		};

		struct Vec4 : public ImVec4
		{
			constexpr Vec4() : ImVec4() {}
			constexpr Vec4(float a_x, float a_y, float a_z, float a_w) : ImVec4(a_x, a_y, a_z, a_w) {}
			constexpr Vec4(const ImVec4& a_rhs) : ImVec4(a_rhs.x, a_rhs.y, a_rhs.z, a_rhs.w) {}
			constexpr Vec4(const Vec4& a_rhs) : ImVec4(a_rhs.x, a_rhs.y, a_rhs.z, a_rhs.w) {}
			constexpr Vec4(const Vec2& a_rhs1, const Vec2& a_rhs2) : ImVec4(a_rhs1.x, a_rhs1.y, a_rhs2.x, a_rhs2.y) {}

			inline Vec4& operator=(const Vec4& a_rhs) noexcept { x = a_rhs.x; y = a_rhs.y; z = a_rhs.z; w = a_rhs.w; return *this; }

			inline bool operator==(const Vec4& a_rhs) const noexcept
			{ 
				return
					(fabs(x - a_rhs.x) <= 0.001) &&
					(fabs(y - a_rhs.y) <= 0.001) &&
					(fabs(z - a_rhs.z) <= 0.001) &&
					(fabs(w - a_rhs.w) <= 0.001);
			}

			inline bool operator!=(const Vec4& a_rhs) const noexcept
			{
				return
					(fabs(x - a_rhs.x) > 0.001) ||
					(fabs(y - a_rhs.y) > 0.001) ||
					(fabs(z - a_rhs.z) > 0.001) ||
					(fabs(w - a_rhs.w) > 0.001);
			}

			inline Vec4 operator+(const Vec4& a_rhs) const noexcept
			{
				return Vec4{ x + a_rhs.x, y + a_rhs.y, z + a_rhs.z, w + a_rhs.w };
			}

			inline Vec4 operator-(const Vec4& a_rhs) const noexcept
			{
				return Vec4{ x - a_rhs.x, y - a_rhs.y, z + a_rhs.z, w + a_rhs.w };
			}

			inline Vec4 operator+(float a_value) const noexcept
			{
				return Vec4{ x + a_value, y + a_value, z + a_value, w + a_value };
			}

			inline Vec4 operator-(float a_value) const noexcept
			{
				return Vec4{ x - a_value, y - a_value, z - a_value, w - a_value };
			}

			inline Vec4& operator+=(const Vec4& a_rhs) noexcept
			{
				x += a_rhs.x;
				y += a_rhs.y;
				z += a_rhs.z;
				w += a_rhs.w;
				return *this;
			}

			inline Vec4& operator-=(const Vec4& a_rhs) noexcept
			{
				x -= a_rhs.x;
				y -= a_rhs.y;
				z -= a_rhs.z;
				w -= a_rhs.w;
				return *this;
			}

			inline Vec4& operator+=(float a_value) noexcept
			{
				x += a_value;
				y += a_value;
				z += a_value;
				w += a_value;
				return *this;
			}

			inline Vec4& operator-=(float a_value) noexcept
			{
				x -= a_value;
				y -= a_value;
				z -= a_value;
				w -= a_value;
				return *this;
			}

			inline Vec4 operator*(float a_value) const noexcept
			{
				return Vec4{ x * a_value, y * a_value, z * a_value, w * a_value };
			}

			inline Vec4 operator/(float a_value) const noexcept
			{
				return Vec4{ x / a_value, y / a_value, z / a_value, w / a_value };
			}

			inline Vec4& operator*=(float a_value) noexcept
			{
				x *= a_value;
				y *= a_value;
				z *= a_value;
				w *= a_value;
				return *this;
			}

			inline Vec4& operator/=(float a_value) noexcept
			{
				x /= a_value;
				y /= a_value;
				z /= a_value;
				w /= a_value;
				return *this;
			}

			float& operator[] (size_t a_idx) { IM_ASSERT(a_idx == 0 || a_idx == 1); return ((float*)this)[a_idx]; }
			float  operator[] (size_t a_idx) const { IM_ASSERT(a_idx == 0 || a_idx == 1); return ((const float*)this)[a_idx]; }
		};
	}
}