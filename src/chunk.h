#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <random>

#include "block_type.h"

constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

class Chunk {
public:
    std::vector<uint16_t> blocks_ = std::vector<uint16_t>(CHUNK_VOLUME, 0);
    glm::ivec3 pos_;

    /*
        is_dirty - this flag is set per chunk so that when meshes are updated,
        only the chunks in the game world with is_dirt == true will be dealt with,
        saving a lot of time not having to check the chunks that we know haven't changed
        and thus don't need anything done to them. Otherwise we would have to iterate through
        all of those chunk's unchanged contents for nothing. Might want to flag adjacent chunks if meshing alg isnt chunk-local
    */
    bool is_dirty = true; //flag for if changes to chunk contents have been made since the last visual mesh was built


    /*
    Chunk(glm::ivec3 pos) : chunk_position(pos), VAO(0), VBO(0), vertex_count(0) {
        std::memset(blocks, 0, sizeof(blocks));
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }
    */
    
    Chunk(glm::ivec3 location) {
         pos_ = location;
    }

    //given (x,y,z):chunk returns block type at coords or air if coords out of bounds
    inline uint16_t get_block(int x, int y, int z) const {
        if (is_contained(x,y,z)) {
            return blocks_[index(x,y,z)];
        }
        return 0;
    }

    inline void set_block(int x, int y, int z, Block_Type block) {
        if (is_contained(x,y,z)) {
            blocks_[index(x,y,z)] = block;
            is_dirty = true;
        }
    }

    inline bool is_contained(int x, int y, int z) const {
        if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
            return false;
        }
        return true;
    }

    inline void fill_dirt() {
        blocks_.assign(CHUNK_VOLUME, 2);
    }

    inline void fill_air() {
        blocks_.assign(CHUNK_VOLUME, 0);
    }

    size_t measure_memory() const {
        size_t total = sizeof(Chunk);
        total += blocks_.capacity() * sizeof(uint16_t);
        //todo: measure mesh data once meshing is implemented
        return total;
    }

    bool should_render_face(int x, int y, int z, int dx, int dy, int dz) const {
        int nx = x + dx;
        int ny = y + dy;
        int nz = z + dz;
        
        if (nx < 0 || nx >= CHUNK_SIZE || 
            ny < 0 || ny >= CHUNK_SIZE || 
            nz < 0 || nz >= CHUNK_SIZE) {
            return true;
        }
        
        return blocks_[index(nx, ny, nz)] == AIR;
    }

    inline constexpr size_t index(int x, int y, int z) const noexcept {
        return static_cast<size_t>(x) + static_cast<size_t>(z) * CHUNK_SIZE + static_cast<size_t>(y) * CHUNK_SIZE * CHUNK_SIZE;
    }


private:

};