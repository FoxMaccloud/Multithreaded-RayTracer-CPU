#pragma once
#include <memory>
#include <vector>
#include <optional>
#include <variant>

#include "Materials.h"
#include "Ray.h"
#include "Shapes.h"


using HitResult = std::pair<std::optional<Hit>, std::optional<Scattering>>;
class Hitableobject
{
public:
	Hitableobject(Shape shape, size_t materialIndex) : m_shape{ shape }, m_materialIndex{ materialIndex } {}
	virtual ~Hitableobject() = default;

	size_t material_index() { return m_materialIndex; };
	float fast_hit(Ray& r, float tMin, float tMax);
	Hit compute_hit(Ray& r, float t);
	std::optional<Hit> hit(Ray& r, float tMin, float tMax);

private:
	Shape m_shape;
	size_t m_materialIndex{ std::numeric_limits<size_t>::max() };
};



class Objects
{
public:
	Objects() = default;
	~Objects() { clear_objects(); };

	void clear_objects() { m_objects.clear(); };
	void add(const Shape& shape, const Material& material);

	HitResult hit(Ray& r, float tMin, float tMax);
private:

	std::vector<Hitableobject> m_objects;
	std::vector<Material> m_materials;
};
