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

    bool is_dirty = true; //has the chunk changed contents since building its current mesh

    /*
    Chunk(glm::ivec3 pos) : chunk_position(pos), VAO(0), VBO(0), vertex_count(0) {
        std::memset(blocks, 0, sizeof(blocks));
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }
    */
    
    Chunk(glm::ivec3 chunk_pos) {
         chunk_pos_ = chunk_pos;
    }

    Chunk() : chunk_pos_(0,0,0) {}

    //given local pos, returns block type at coords or air if coords out of bounds
    inline uint16_t get_block(const glm::ivec3& local_pos) const {
        if (is_contained(local_pos)) {
            return blocks_[index(local_pos)];
        }
        return 0;
    }

    inline void set_block(const glm::ivec3& local_pos, Block_Type block) {
        if (is_contained(local_pos)) {
            blocks_[index(local_pos)] = block;
            is_dirty = true;
        }
    }

    inline bool is_contained(const glm::ivec3& local_pos) const {
        if (local_pos.x < 0 || local_pos.x >= CHUNK_SIZE || local_pos.y < 0 || local_pos.y >= CHUNK_SIZE || local_pos.z < 0 || local_pos.z >= CHUNK_SIZE) {
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

    inline void fill_grass() {
        blocks_.assign(CHUNK_VOLUME, 1);
    }
    
    size_t measure_memory() const {
        size_t total = sizeof(Chunk);
        total += blocks_.capacity() * sizeof(uint16_t);
        //todo: measure mesh data once meshing is implemented
        return total;
    }

    /*
    bool should_render_face_world(World& world, const glm::ivec3& chunk_pos, int x, int y, int z, int dx, int dy, int dz) {
        int nx = x + dx;
        int ny = y + dy;
        int nz = z + dz;   
        
        //case that face is not at edge of chunk
        if (nx >= 0 && nx < CHUNK_SIZE && ny >= 0 && ny < CHUNK_SIZE && nz >= 0 && nz < CHUNK_SIZE) {
            return should_render_face(x,y,z,dx,dy,dz);
        }

        //case face is at edge of chunk, check if opaque surfaces exist in neighboring chunk
        glm::ivec3 neighbor_pos = chunk_pos + glm::ivec3(dx,dy,dz);
        const Chunk* neighbor = world.get_chunk(neighbor_pos);
        if (!neighbor) return true;

        //if neighbor exists, get pertinent block coords within the chunk's block space
        int nnx = (nx + CHUNK_SIZE) % CHUNK_SIZE;
        int nny = (ny + CHUNK_SIZE) % CHUNK_SIZE;
        int nnz = (nz + CHUNK_SIZE) % CHUNK_SIZE;
        return neighbor->blocks_[index(glm::ivec3(nnx,nny,nnz))] == AIR;
    }

    bool should_render_face(int x, int y, int z, int dx, int dy, int dz) {
        int nx = x + dx;
        int ny = y + dy;
        int nz = z + dz;
        
        if (nx < 0 || nx >= CHUNK_SIZE || 
            ny < 0 || ny >= CHUNK_SIZE || 
            nz < 0 || nz >= CHUNK_SIZE) {
            return true;
        }
        
        return blocks_[index(glm::ivec3(nx, ny, nz))] == AIR;
    }
        */


private:

};