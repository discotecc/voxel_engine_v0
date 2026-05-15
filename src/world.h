#pragma once

#include <iomanip>
#include <iostream>
#include <unordered_map>

#include <glm/glm.hpp>

#include "chunk.h"
#include "terrain.h"

namespace std {
template <> struct hash<glm::ivec3> {
  size_t operator()(const glm::ivec3 &v) const noexcept {
    // Simple but good enough hash for 3D integer coordinates
    size_t h1 = std::hash<int>()(v.x);
    size_t h2 = std::hash<int>()(v.y);
    size_t h3 = std::hash<int>()(v.z);
    return h1 ^ (h2 << 1) ^ (h3 << 2); // or any other decent mixing
  }
};
} // namespace std

// some global helper functions for converting coords...
constexpr glm::ivec3 global_to_chunk_pos(const glm::vec3 &pos) {
  return glm::ivec3((int)std::floor((float)pos.x / CHUNK_WIDTH),
                    (int)std::floor((float)pos.y / CHUNK_WIDTH),
                    (int)std::floor((float)pos.z / CHUNK_WIDTH));
}

constexpr glm::ivec3 global_to_local_pos(const glm::vec3 &pos) {
  auto mod = [](int a, int b) { return ((a % b) + b) % b; };
  return glm::ivec3(mod(pos.x, CHUNK_WIDTH), mod(pos.y, CHUNK_HEIGHT),
                    mod(pos.z, CHUNK_WIDTH));
}

// takes the position of a chunk and a block within that chunk; returns
constexpr glm::vec3 local_to_global_pos(const glm::ivec3 &chunk_pos,
                                        const glm::ivec3 &local_pos) {
  return glm::vec3(chunk_pos.x * CHUNK_WIDTH + local_pos.x,
                   chunk_pos.y * CHUNK_HEIGHT + local_pos.y,
                   chunk_pos.z * CHUNK_WIDTH + local_pos.z);
};

constexpr glm::ivec3 global_f_to_global_i_pos(glm::vec3 global_pos_f) {
  return glm::ivec3(std::floor(global_pos_f.x), std::floor(global_pos_f.y),
                    std::floor(global_pos_f.z));
};

constexpr int WORLD_SIZE = 20;
constexpr int WORLD_AREA = WORLD_SIZE * WORLD_SIZE;

class World {
public:
  std::unordered_map<glm::ivec3, Chunk> chunks_;
  Terrain terrain;

  World() = default;

  Chunk *get_chunk(glm::ivec3 pos) {
    auto it = chunks_.find(pos);
    if (it != chunks_.end()) {
      return &it->second;
    }
    return nullptr;
  }

  // tries to insert air chunks as position in world space
  Chunk &get_or_create_chunk(const glm::ivec3 &pos) {
    auto [it, inserted] = chunks_.try_emplace(pos, pos);
    if (inserted) {
      it->second.fill_air();
    }
    return it->second;
  }

  void set_block(const glm::ivec3 &pos, Block_Type type) {
    glm::ivec3 chunk_pos = global_to_chunk_pos(pos);
    glm::ivec3 local_pos = global_to_local_pos(pos);
    Chunk &chunk = get_or_create_chunk(chunk_pos);
    chunk.set_block(local_pos, type);
  }

  void create_terrained_chunk(const glm::ivec3 &pos,
                              std::vector<float> terrain_data) {
    Chunk &chunk = get_or_create_chunk(pos);
    auto terrain_it = terrain_data.begin();
    int terrain_val;
    for (int x = 0; x < CHUNK_WIDTH; x++) {
      for (int z = 0; z < CHUNK_WIDTH; z++) {
        terrain_val = static_cast<int>(*terrain_it);
        for (int y = 0; y < terrain_val; y++) {
          chunk.set_block(glm::ivec3(x, y, z), Block_Type::STONE);
        }
        terrain_it++;
      }
    }
  }

  void generate_biome(const glm::ivec3 biome_start_pos, Biome_Type biome,
                      int size) {
    terrain = Terrain();
    // terrain.noise_.SetFrequency(0.017);
    terrain.noise_.SetFrequency(0.03);
    int column_height = 0;
    // iterate through every chunk of the biome
    for (int bx = 0; bx < size; bx++) {
      int current_chunk_x = biome_start_pos.x + bx;
      for (int bz = 0; bz < size; bz++) {
        int current_chunk_z = biome_start_pos.z + bz;
        Chunk &current_chunk = get_or_create_chunk(
            glm::ivec3(current_chunk_x, 0, current_chunk_z));
        for (int x = 0; x < CHUNK_WIDTH; x++) {
          for (int z = 0; z < CHUNK_WIDTH; z++) {
            glm::ivec3 current_global_pos = local_to_global_pos(
                glm::ivec3(current_chunk_x, 0, current_chunk_z),
                glm::ivec3(x, 0, z));
            column_height = terrain.get_column_height(current_global_pos.x,
                                                      current_global_pos.z);
            for (int y = 0; y < column_height; y++) {
              current_chunk.set_block(glm::ivec3(x, y, z), STONE);
            }
          }
        }
      }
    }
  }

  void handle_player_chunk_change(glm::ivec3 player_pos) {}

  void update_loaded_chunks(glm::ivec3 center_chunk_pos, int render_radius) {}

  /*
  //biome begins as pos X,Z in the 2D chunk space (will be a corner chunk of the
  biome) void generate_biome(const glm::ivec3& pos, Biome_Type biome, int size)
  {

      int column_height = 0;
      terrain = Terrain();
      terrain.noise_.SetFrequency(0.017);
      int x_multiplicand, z_multiplicand = 0;
      for (int bx = 0; bx < size; bx++) {
          for (int bz = 0; bz < size; bz++) {
              glm::ivec3 current_chunk_pos((bx+1) + pos.x,0,(bz+1) + pos.z);
              Chunk& chunk = get_or_create_chunk(current_chunk_pos);
              for (int z = 0; z < CHUNK_WIDTH; z++) {
                  for (int x = 0; x < CHUNK_WIDTH; x++) {
                      column_height = terrain.get_column_height(
  (x_multiplicand*chunks_.size()) + x, (z_multiplicand*chunks_.size() + z));
                      spdlog::info("column_height val... {}", column_height);
                      for (int y = 0; y < column_height; y++) {
                          chunk.set_block(glm::ivec3(x,y,z), GRASS);
                      }
                  }
              }
              z_multiplicand++;
          }
          x_multiplicand++;
      };
  }
*/
  void create_dirt_chunk(const glm::ivec3 &pos) {
    Chunk &chunk = get_or_create_chunk(pos);
    chunk.fill_dirt();
  }

  void create_air_chunk(const glm::ivec3 &pos) {
    Chunk &chunk = get_or_create_chunk(pos);
    chunk.fill_air();
  }

  void create_grass_chunk(const glm::ivec3 &pos) {
    Chunk &chunk = get_or_create_chunk(pos);
    chunk.fill_grass();
  }

  void print_all_loaded_chunks() const {
    std::cout << "=== Loaded chunks: " << chunks_.size() << " ===\n\n";
    for (const auto &[pos, chunk] : chunks_) {
      std::cout << "Chunk (" << pos.x << ", " << pos.y << ", " << pos.z
                << ")  |  solid blocks: " << count_solid_blocks(chunk) << "/"
                << CHUNK_VOLUME << "  |  dirty: " << std::boolalpha
                << "property was removed... :()" << "\n";
    }
    std::cout << "\n";
  }

  inline size_t measure_memory() const {
    size_t total = sizeof(chunks_);
    for (const auto &[position, chunk] : chunks_) {
      total += chunk.measure_memory();
    }
    return total;
  }

private:
  inline int count_solid_blocks(const Chunk &chunk) const {
    int count = 0;
    for (uint16_t b : chunk.blocks_) {
      if (b != 0)
        ++count;
    }
    return count;
  }
};
