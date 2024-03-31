#pragma once

#include "Block.hpp"
#include "../Level/Dimension.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include "../Level/Biome.hpp"
#include "../Level/Level.hpp"

class BlockSource {
public:

    BUILD_ACCESS(this, mcDimension*, dimension, GET_OFFSET("BlockSource::dimension"));

    Block *getBlock(const Vec3<int> &);

    Biome *getBiome(const Vec3<int> &);
};