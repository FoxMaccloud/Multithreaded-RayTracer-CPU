#pragma once
#include <memory>
#include <vector>
#include <optional>
#include <variant>

#include "Materials.h"
#include "Ray.h"
#include "Shapes.h"


using HitResult = std::pair<std::optional<HitRecord>, std::optional<Scattering>>;
class Hitableobject
{
public:
	Hitableobject(Shape shape, const size_t materialIndex) : m_shape{ shape }, m_materialIndex{ materialIndex } {};
	virtual ~Hitableobject() = default;

	[[nodiscard]] size_t material_index() const { return m_materialIndex; };
	[[nodiscard]] float fast_hit(const Ray& r, float tMin, float tMax) const;
	[[nodiscard]] HitRecord compute_hit(const Ray& r, float t) const;
	[[nodiscard]] std::optional<HitRecord> hit(const Ray& r, float tMin, float tMax) const;

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

	[[nodiscard]] HitResult hit(const Ray& r, float tMin, float tMax) const;
private:

	std::vector<Hitableobject> m_objects;
	std::vector<Material> m_materials;
};
