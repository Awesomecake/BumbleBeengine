#pragma once
#include "Mesh.h"
#include <memory>

struct MeshComponent
{
	std::shared_ptr<Mesh> mesh;
};