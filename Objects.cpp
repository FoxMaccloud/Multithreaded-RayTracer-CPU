#include <numeric>
#include "Objects.h"

template <class... Ts> struct overload : Ts... { using Ts::operator()...; };
template <class... Ts> overload(Ts...)->overload<Ts...>;

float Hitableobject::fast_hit(const Ray& r, float tMin, float tMax) const
{
	return std::visit(
		overload{
			[&](auto& shape) { return shape.fast_hit(r, tMin, tMax); },
		},
		m_shape);
}

HitRecord Hitableobject::compute_hit(const Ray& r, float t) const 
{
	std::optional<HitRecord> empty_result{};

	return std::visit(
		overload{
			[&](auto& shape) { return shape.compute_hit(r, t); },
		},
		m_shape);
}

std::optional<HitRecord> Hitableobject::hit(const Ray& r, float tMin, float tMax) const
{
	return std::visit(
		overload{
			[&](auto& shape) { return shape.hit(r, tMin, tMax); },
		},
		m_shape);
}

void Objects::add(const Shape& shape, const Material& material)
{
	size_t matIndex = std::numeric_limits<size_t>::max();

	if (const auto matIt = std::find(begin(m_materials), end(m_materials), material); matIt == end(m_materials))
	{
		m_materials.push_back(material);
		matIndex = m_materials.size() - 1;
	}
	else
	{
		matIndex = std::distance(begin(m_materials), matIt);
	}
	m_objects.emplace_back(shape, matIndex);
}

HitResult Objects::hit(const Ray& r, float tMin, float tMax) const
{
	HitResult emptyResult{};

	float closest = tMax;
	const Hitableobject* closestObject = nullptr;

	for (const auto& object : m_objects)
	{
		if (const float tmp = object.fast_hit(r, tMin, closest); tmp < std::numeric_limits<float>::max())
		{
			closestObject = &object;
			closest = tmp;
		}
	}

	if (!closestObject)
		return emptyResult;
	else
	{
		auto hitR = closestObject->compute_hit(r, closest);
		return { hitR, std::visit(
						  overload{
							  [&](auto& material) { return material.scatter(r, hitR); },
						  },
						  m_materials[closestObject->material_index()]) };
	}
}