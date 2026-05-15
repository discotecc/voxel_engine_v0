#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <random>
#include <vector>

#include "block_type.h"

constexpr int CHUNK_WIDTH = 16;
constexpr int CHUNK_HEIGHT = 300;
constexpr int CHUNK_VOLUME = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT;

// given w, h, v as chunk_width, chunk_height, chunk_volume respectively...
// index = z + wy + whx => one to one mapping from {0 <= (x,y,z) < v : Z} to
// [0,v-1] requires iterating through dimensions in reverse alphabetical order
// (for x {for y {for z {...}}})
inline constexpr size_t index(const glm::ivec3 &pos) noexcept {
  return static_cast<size_t>(pos.z) + static_cast<size_t>(pos.y) * CHUNK_WIDTH +
         static_cast<size_t>(pos.x) * CHUNK_WIDTH * CHUNK_HEIGHT;
}

class Chunk {
public:
  std::vector<uint16_t> blocks_ = std::vector<uint16_t>(CHUNK_VOLUME, 0);
  glm::ivec3 chunk_pos_;
  bool is_terrained_;

  Chunk(glm::ivec3 chunk_pos) {
    chunk_pos_ = chunk_pos;
    is_terrained_ = false;
  }

  Chunk() : chunk_pos_(0, 0, 0) {}

  inline uint16_t get_block(const glm::ivec3 &local_pos) const {
    if (is_contained(local_pos)) {
      return blocks_[index(local_pos)];
    }
    return 0;
  }

  inline void set_block(const glm::ivec3 &local_pos, Block_Type block) {
    if (is_contained(local_pos)) {
      blocks_[index(local_pos)] = block;
    }
  }

  void mark_terrained() { is_terrained_ = true; }
  void mark_unterrained() { is_terrained_ = false; }

  inline void remove_block() {}

  inline bool is_contained(const glm::ivec3 &local_pos) const {
    if (local_pos.x < 0 || local_pos.x >= CHUNK_WIDTH || local_pos.y < 0 ||
        local_pos.y >= CHUNK_HEIGHT || local_pos.z < 0 ||
        local_pos.z >= CHUNK_WIDTH) {
      return false;
    }
    return true;
  }

  inline void fill_dirt() { blocks_.assign(CHUNK_VOLUME, 2); }

  inline void fill_air() { blocks_.assign(CHUNK_VOLUME, 0); }

  inline void fill_grass() { blocks_.assign(CHUNK_VOLUME, 1); }

  size_t measure_memory() const {
    size_t total = sizeof(Chunk);
    total += blocks_.capacity() * sizeof(uint16_t);
    return total;
  }

private:
};
