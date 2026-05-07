//manages all loaded chunks + hash for fast lookup by position, loading/unloading/ dirt propogation to neighbors
#pragma once

#include <unordered_map>
#include <iostream>
#include <iomanip>

#include <glm/glm.hpp>


#include "chunk.h"

// Provide std::hash for glm::ivec3 so we can use it as key in unordered_map
namespace std {
    template<>
    struct hash<glm::ivec3> {
        size_t operator()(const glm::ivec3& v) const noexcept {
            // Simple but good enough hash for 3D integer coordinates
            size_t h1 = std::hash<int>()(v.x);
            size_t h2 = std::hash<int>()(v.y);
            size_t h3 = std::hash<int>()(v.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2);   // or any other decent mixing
        }
    };
}

constexpr int WORLD_SIZE = 20;
constexpr int WORLD_VOLUME = WORLD_SIZE * WORLD_SIZE * WORLD_SIZE;

class World {
public:
    std::unordered_map<glm::ivec3, Chunk> chunks_;

    //default constructor, starts with no chunks loaded
    World() = default;

    //returns pointer to chunk if found, nullptr otherwise
    Chunk* get_chunk(glm::ivec3 pos) {
        auto it = chunks_.find(pos);
        if (it != chunks_.end()) {
            return &it->second;
        }
        return nullptr; //failed to load chunk
    }

    //tries to insert air chunks as position in world space
    Chunk& get_or_create_chunk(const glm::ivec3& pos) {
        auto [it,inserted] = chunks_.try_emplace(pos,pos);
        if (inserted) {
            it->second.fill_air();
        }
        return it->second;
    }

    void set_block(const glm::ivec3& pos, Block_Type type) {
        glm::ivec3 chunk_pos = world_to_chunk_pos(pos);
        glm::ivec3 local_pos  = world_to_local_pos(pos);
        Chunk& chunk = get_or_create_chunk(chunk_pos);
        chunk.set_block(local_pos, type);
    }

    void create_dirt_chunk(const glm::ivec3& pos) {;
        Chunk& chunk = get_or_create_chunk(pos);
        chunk.fill_dirt();
    }

    void create_air_chunk(const glm::ivec3& pos) {
        Chunk& chunk = get_or_create_chunk(pos);
        chunk.fill_air();
    }

    void create_grass_chunk(const glm::ivec3& pos) {
        Chunk& chunk = get_or_create_chunk(pos);
        chunk.fill_grass();
    }

    void print_all_loaded_chunks() const {
        std::cout << "=== Loaded chunks: " << chunks_.size() << " ===\n\n";

        for (const auto& [pos, chunk] : chunks_) {
            std::cout << "Chunk (" << pos.x << ", " << pos.y << ", " << pos.z 
                      << ")  |  solid blocks: " << count_solid_blocks(chunk) 
                      << "/" << CHUNK_VOLUME 
                      << "  |  dirty: " << std::boolalpha << chunk.is_dirty << "\n";
        }
        std::cout << "\n";
    }

    inline size_t measure_memory() const {
        size_t total = sizeof(chunks_);

        for (const auto& [position, chunk] : chunks_) {
            total += chunk.measure_memory();
        }
        return total;
    }


private:

    inline constexpr glm::ivec3 world_to_chunk_pos(const glm::ivec3& pos) const {
        return glm::ivec3(
            (int)std::floor((float)pos.x / CHUNK_SIZE),
            (int)std::floor((float)pos.y / CHUNK_SIZE),
            (int)std::floor((float)pos.z / CHUNK_SIZE)
        );
    }

    inline constexpr glm::ivec3 world_to_local_pos(const glm::ivec3& pos) const {
        auto mod = [](int a, int b) { return ((a % b) + b) % b; };
        return glm::ivec3(
            mod(pos.x, CHUNK_SIZE),
            mod(pos.y, CHUNK_SIZE),
            mod(pos.z, CHUNK_SIZE));
    }

    inline int count_solid_blocks(const Chunk& chunk) const {
        int count = 0;
        for (uint16_t b : chunk.blocks_) {
            if (b != 0) ++count;
        }
        return count;
    }
};
