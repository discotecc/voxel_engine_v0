#pragma once
#include "player.h"
#include "world.h"

class Chunk_Scaper {
public:
  World &world_;

  Chunk_Scaper(World &world);
};

Chunk_Scaper::Chunk_Scaper(World &world) : world_(world) {};