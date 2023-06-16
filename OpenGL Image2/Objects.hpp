#pragma once
#include <memory>
#include <vector>
#include <optional>
#include <variant>

#include "Materials.hpp"
#include "Ray.hpp"
#include "Shapes.hpp"


using HitResult = std::pair<std::optional<HitRecord>, std::optional<Scattering>>;
class Hitableobject
{
public:
	Hitableobject(Shape shape, const size_t materialIndex) : m_shape{ shape }, m_materialIndex{ materialIndex } {};
	virtual ~Hitableobject() = default;

	size_t MaterialIndex() const { return m_materialIndex; };
	float FastHit(const Ray& r, float tMin, float tMax) const;
	HitRecord ComputeHit(const Ray& r, float t) const;
	std::optional<HitRecord> Hit(const Ray& r, float tMin, float tMax) const;

private:
	Shape m_shape;
	size_t m_materialIndex{ std::numeric_limits<size_t>::max() };
};



class Objects
{
public:
	Objects() = default;
	~Objects() { ClearObjects(); };

	void ClearObjects() { m_objects.clear(); };
	void Add(const Shape& shape, const Material& material);

	[[nodiscard]] HitResult Hit(const Ray& r, float tMin, float tMax) const;
private:

	std::vector<Hitableobject> m_objects;
	std::vector<Material> m_materials;
};
