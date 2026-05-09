#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <random>

#include "block_type.h"

constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

//globally defined coordinate flattener for indexing into the vectors, same for all flat data structures. keep in mind that chunk map takes ivecs
inline constexpr size_t index(const glm::ivec3& pos)  noexcept {
    return static_cast<size_t>(pos.x) + static_cast<size_t>(pos.z) * CHUNK_SIZE + static_cast<size_t>(pos.y) * CHUNK_SIZE * CHUNK_SIZE;
}

class Chunk {
public:
    std::vector<uint16_t> blocks_ = std::vector<uint16_t>(CHUNK_VOLUME, 0);
    glm::ivec3 chunk_pos_;

    
    Chunk(glm::ivec3 chunk_pos) {
         chunk_pos_ = chunk_pos;
    }

    Chunk() : chunk_pos_(0,0,0) {}

    inline uint16_t get_block(const glm::ivec3& local_pos) const {
        if (is_contained(local_pos)) {
            return blocks_[index(local_pos)];
        }
        return 0;
    }

    inline void set_block(const glm::ivec3& local_pos, Block_Type block) {
        if (is_contained(local_pos)) {
            blocks_[index(local_pos)] = block;
        }
    }

    inline bool is_contained(const glm::ivec3& local_pos) const {
        if (local_pos.x < 0 || local_pos.x >= CHUNK_SIZE || local_pos.y < 0 || local_pos.y >= CHUNK_SIZE || local_pos.z < 0 || local_pos.z >= CHUNK_SIZE) {
        }
    }

    inline void fill_dirt() {
        blocks_.assign(CHUNK_VOLUME, 2);
    }

    inline void fill_air() {
        blocks_.assign(CHUNK_VOLUME, 0);
    }

    inline void fill_grass() {
        blocks_.assign(CHUNK_VOLUME, 1);
    }
    
    size_t measure_memory() const {
        size_t total = sizeof(Chunk);
        total += blocks_.capacity() * sizeof(uint16_t);
        return total;
    }

private:

};