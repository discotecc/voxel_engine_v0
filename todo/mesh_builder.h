#pragma once

// mesh_builder.h
// Builds a flat array of vertex data for one chunk using simple face-culling:
// for every solid block, emit only the faces whose neighbour is air.
//
// Vertex layout (6 floats per vertex):
//   [x, y, z,  nx, ny, nz]
//   position in *world* space (chunk_pos * CHUNK_SIZE + local), normal for lighting.
//
// Each quad = 2 triangles = 6 vertices (no index buffer, keeps things simple).

#include <vector>
#include <glm/glm.hpp>
#include "chunk.h"

// The 6 axis-aligned face directions.
// Each entry: { normal, 4 corner offsets from block origin (BL,BR,TR,TL) }
struct FaceTemplate {
    glm::ivec3 normal;
    glm::vec3  corners[4];   // local offsets, one unit cube at origin
};

static constexpr FaceTemplate FACES[6] = {
    // +X  (right)
    { { 1, 0, 0}, { {1,0,1},{1,0,0},{1,1,0},{1,1,1} } },
    // -X  (left)
    { {-1, 0, 0}, { {0,0,0},{0,0,1},{0,1,1},{0,1,0} } },
    // +Y  (top)
    { { 0, 1, 0}, { {0,1,1},{1,1,1},{1,1,0},{0,1,0} } },
    // -Y  (bottom)
    { { 0,-1, 0}, { {0,0,0},{1,0,0},{1,0,1},{0,0,1} } },
    // +Z  (front)
    { { 0, 0, 1}, { {0,0,1},{1,0,1},{1,1,1},{0,1,1} } },
    // -Z  (back)
    { { 0, 0,-1}, { {1,0,0},{0,0,0},{0,1,0},{1,1,0} } },
};

// Returns true when the block at (nx,ny,nz) inside *chunk* is solid.
// Blocks outside the chunk boundary are treated as air (0) — cross-chunk
// culling is a future optimisation; for now we just show those border faces.
inline bool is_solid_local(const Chunk& chunk, int nx, int ny, int nz) {
    if (!chunk.is_contained(nx, ny, nz)) return false;
    return chunk.get_block(nx, ny, nz) != 0;
}

// Build vertex data for `chunk`.  Returns a flat float array ready for GL.
// Each vertex: x y z  nx ny nz  (6 floats).
inline std::vector<float> build_chunk_mesh(const Chunk& chunk) {
    std::vector<float> verts;
    verts.reserve(CHUNK_VOLUME * 3);   // rough upper bound, will shrink-to-fit

    // World-space origin of this chunk
    const glm::vec3 origin(
        chunk.pos_.x * CHUNK_SIZE,
        chunk.pos_.y * CHUNK_SIZE,
        chunk.pos_.z * CHUNK_SIZE
    );

    for (int y = 0; y < CHUNK_SIZE; ++y)
    for (int z = 0; z < CHUNK_SIZE; ++z)
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        if (chunk.get_block(x, y, z) == 0) continue;  // skip air

        // For each of the 6 faces, emit a quad only if the neighbour is air.
        for (const FaceTemplate& face : FACES) {
            int nx = x + face.normal.x;
            int ny = y + face.normal.y;
            int nz = z + face.normal.z;

            if (is_solid_local(chunk, nx, ny, nz)) continue;  // face hidden

            // Quad corners in world space (2 triangles: 0,1,2 and 0,2,3)
            glm::vec3 ws[4];
            for (int i = 0; i < 4; ++i) {
                ws[i] = origin + glm::vec3(x, y, z) + face.corners[i];
            }

            const glm::vec3 n(face.normal);

            // Triangle 1: corners 0, 1, 2
            for (int i : {0, 1, 2}) {
                verts.insert(verts.end(), { ws[i].x, ws[i].y, ws[i].z,
                                            n.x,     n.y,     n.z });
            }
            // Triangle 2: corners 0, 2, 3
            for (int i : {0, 2, 3}) {
                verts.insert(verts.end(), { ws[i].x, ws[i].y, ws[i].z,
                                            n.x,     n.y,     n.z });
            }
        }
    }

    verts.shrink_to_fit();
    return verts;
}
