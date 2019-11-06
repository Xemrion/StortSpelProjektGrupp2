#pragma once

#include "defs.hpp"
#include "TileMap.hpp"
#include "../graphics.h"

Vector<UPtr<GameObject>> instantiateTilesAsModels( Graphics &graphics, TileMap const &map ) noexcept;