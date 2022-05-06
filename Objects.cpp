#include <numeric>
#include "Objects.h"
#include "Shapes.h"

template <class... Ts> struct overload : Ts... { using Ts::operator()...; };
template <class... Ts> overload(Ts...)->overload<Ts...>;

float Hitableobject::fast_hit(Ray& r, float tMin, float tMax)
{
	return std::visit(
		overload{
			[&](auto& shape) { return shape.fast_hit(r, tMin, tMax); },
		},
		m_shape);
}

Hit Hitableobject::compute_hit(Ray& r, float t)
{
	std::optional<Hit> empty_result{};

	return std::visit(
		overload{
			[&](auto& shape) { return shape.compute_hit(r, t); },
		},
		m_shape);
}

std::optional<Hit> Hitableobject::hit(Ray& r, float tMin, float tMax)
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

	if (auto matIt = std::find(begin(m_materials), end(m_materials), material); matIt == end(m_materials))
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

HitResult Objects::hit(Ray& r, float tMin, float tMax)
{
	HitResult emptyResult{};

	float closest = tMax;
	Hitableobject* closestObject = nullptr;

	for (auto& object : m_objects)
	{
		if (float tmp = object.fast_hit(r, tMin, closest); tmp < std::numeric_limits<float>::max())
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