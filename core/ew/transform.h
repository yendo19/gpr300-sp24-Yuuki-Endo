#pragma once
#include "ewMath/ewMath.h"
#include "ewMath/transformations.h"
#include "ewMath/quaternion.h"

namespace ew {
	struct Transform {
		ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 0.0f);
		ew::Quaternion rotation = ew::Quaternion::Identity;
		ew::Vec3 scale = ew::Vec3(1.0f, 1.0f, 1.0f);

		ew::Mat4 ModelMatrix() const {
			return ew::Translate(position) 
				* ew::ToRotationMatrix(rotation)
				* ew::Scale(scale);
		}
	};
}
