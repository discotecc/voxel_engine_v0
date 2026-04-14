//converts chunk data to mesh (vertices, indices, etc)
#pragma once

#include "world.h"
#include "block_type.h"

class Mesher {
public:
    Mesher(World& world);
    ~Mesher();

    void get_tex_coords(Block_Type type, int face, float& u_min, float& v_min, float& u_max, float& v_max);
    void add_face(std::vector<float>& vertices, float x, float y, float z, int face_index, Block_Type block);
    void generate_mesh();
    void clear_mesh();
    void init();

    unsigned int get_VAO();
    unsigned int get_VBO();
    std::size_t get_vertex_count();

private:
    World& world_;

    std::vector<float> mesh_vertices_;

    unsigned int VAO_;
    unsigned int VBO_;
    
    std::size_t vertex_count_;

};

Mesher::Mesher(World& world) : world_(world), VAO_(0), VBO_(0), vertex_count_(0) {
   
}

Mesher::~Mesher() {
    clear_mesh();
}

void Mesher::init() {
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
}

void Mesher::get_tex_coords(Block_Type type, int face, float& u_min, float& v_min, float& u_max, float& v_max) {
    int tile_x, tile_y;
    switch(type) {
        case GRASS:
            if (face == 4) {  // Top face
                tile_x = 29; tile_y = 3;  // Grass top
            } else if (face == 5) {  // Bottom face
                tile_x = 28; tile_y = 0;  // Dirt
            } else {  // Side faces
                   tile_x = 28; tile_y = 1;  // Grass side
            }
            break;
        case DIRT:
            tile_x = 28; tile_y = 0;
            break;
        case STONE:
            tile_x = 17; tile_y = 0;
            break;
        default:
            tile_x = 0; tile_y = 0;
    }
        
        u_max = (tile_x * 16.0f) / 1024.0f;
        u_min= ((tile_x + 1) * 16.0f) / 1024.0f;
        v_max = (tile_y * 16.0f) / 512.0f;
        v_min = ((tile_y + 1) * 16.0f) / 512.0f;
    }

void Mesher::add_face(std::vector<float>& vertices, float x, float y, float z, int face_index, Block_Type block) {
        float u_min, v_min, u_max, v_max;
        get_tex_coords(block, face_index, u_min, v_min, u_max, v_max);

        switch(face_index) {
            case 0: // Back face (-Z)
                vertices.insert(vertices.end(), {
                    // Position                    Normal              TexCoord
                    x-0.5f, y-0.5f, z-0.5f,   0.0f, 0.0f, -1.0f,   u_max, v_min,
                    x+0.5f, y-0.5f, z-0.5f,   0.0f, 0.0f, -1.0f,   u_min, v_min,
                    x+0.5f, y+0.5f, z-0.5f,   0.0f, 0.0f, -1.0f,   u_min, v_max,
                    x+0.5f, y+0.5f, z-0.5f,   0.0f, 0.0f, -1.0f,   u_min, v_max,
                    x-0.5f, y+0.5f, z-0.5f,   0.0f, 0.0f, -1.0f,   u_max, v_max,
                    x-0.5f, y-0.5f, z-0.5f,   0.0f, 0.0f, -1.0f,   u_max, v_min
                });
                break;
            case 1: // Front face (+Z)
                vertices.insert(vertices.end(), {
                    x-0.5f, y-0.5f, z+0.5f,   0.0f, 0.0f, 1.0f,    u_min, v_min,
                    x+0.5f, y-0.5f, z+0.5f,   0.0f, 0.0f, 1.0f,    u_max, v_min,
                    x+0.5f, y+0.5f, z+0.5f,   0.0f, 0.0f, 1.0f,    u_max, v_max,
                    x+0.5f, y+0.5f, z+0.5f,   0.0f, 0.0f, 1.0f,    u_max, v_max,
                    x-0.5f, y+0.5f, z+0.5f,   0.0f, 0.0f, 1.0f,    u_min, v_max,
                    x-0.5f, y-0.5f, z+0.5f,   0.0f, 0.0f, 1.0f,    u_min, v_min
                });
                break;
            case 2: // Left face (-X)
                vertices.insert(vertices.end(), {
                    x-0.5f, y+0.5f, z+0.5f,   -1.0f, 0.0f, 0.0f,   u_max, v_max,
                    x-0.5f, y+0.5f, z-0.5f,   -1.0f, 0.0f, 0.0f,   u_min, v_max,
                    x-0.5f, y-0.5f, z-0.5f,   -1.0f, 0.0f, 0.0f,   u_min, v_min,
                    x-0.5f, y-0.5f, z-0.5f,   -1.0f, 0.0f, 0.0f,   u_min, v_min,
                    x-0.5f, y-0.5f, z+0.5f,   -1.0f, 0.0f, 0.0f,   u_max, v_min,
                    x-0.5f, y+0.5f, z+0.5f,   -1.0f, 0.0f, 0.0f,   u_max, v_max,
                });
                break;
            case 3: // Right face (+X)
                vertices.insert(vertices.end(), {
                    x+0.5f, y+0.5f, z+0.5f,   1.0f, 0.0f, 0.0f,    u_min, v_max,
                    x+0.5f, y+0.5f, z-0.5f,   1.0f, 0.0f, 0.0f,    u_max, v_max,
                    x+0.5f, y-0.5f, z-0.5f,   1.0f, 0.0f, 0.0f,    u_max, v_min,
                    x+0.5f, y-0.5f, z-0.5f,   1.0f, 0.0f, 0.0f,    u_max, v_min,
                    x+0.5f, y-0.5f, z+0.5f,   1.0f, 0.0f, 0.0f,    u_min, v_min,
                    x+0.5f, y+0.5f, z+0.5f,   1.0f, 0.0f, 0.0f,    u_min, v_max
                });
                break;
            case 4: // Top face (+Y)
                vertices.insert(vertices.end(), {
                    x-0.5f, y+0.5f, z-0.5f,   0.0f, 1.0f, 0.0f,    u_min, v_max,
                    x+0.5f, y+0.5f, z-0.5f,   0.0f, 1.0f, 0.0f,    u_max, v_max,
                    x+0.5f, y+0.5f, z+0.5f,   0.0f, 1.0f, 0.0f,    u_max, v_min,
                    x+0.5f, y+0.5f, z+0.5f,   0.0f, 1.0f, 0.0f,    u_max, v_min,
                    x-0.5f, y+0.5f, z+0.5f,   0.0f, 1.0f, 0.0f,    u_min, v_min,
                    x-0.5f, y+0.5f, z-0.5f,   0.0f, 1.0f, 0.0f,    u_min, v_max
                });
                break;
            case 5: // Bottom face (-Y)
                vertices.insert(vertices.end(), {
                    x-0.5f, y-0.5f, z-0.5f,   0.0f, -1.0f, 0.0f,   u_min, v_max,
                    x+0.5f, y-0.5f, z-0.5f,   0.0f, -1.0f, 0.0f,   u_max, v_max,
                    x+0.5f, y-0.5f, z+0.5f,   0.0f, -1.0f, 0.0f,   u_max, v_min,
                    x+0.5f, y-0.5f, z+0.5f,   0.0f, -1.0f, 0.0f,   u_max, v_min,
                    x-0.5f, y-0.5f, z+0.5f,   0.0f, -1.0f, 0.0f,   u_min, v_min,
                    x-0.5f, y-0.5f, z-0.5f,   0.0f, -1.0f, 0.0f,   u_min, v_max
                });
                break;
        }
    }

void Mesher::generate_mesh() {
        
        float world_x, world_y, world_z;

        for (auto& [position, chunk] : world_.chunks_) {
            world_x = position.x * CHUNK_SIZE;
            world_y = position.y * CHUNK_SIZE;
            world_z = position.z * CHUNK_SIZE;
            for (int x = 0; x < CHUNK_SIZE; x++) {
                for (int y = 0; y < CHUNK_SIZE; y++) {
                    for (int z = 0; z < CHUNK_SIZE; z++) {
                        Block_Type block = static_cast<Block_Type>(chunk.blocks_[chunk.index(x,y,z)]);

                        if (block == AIR) continue;

                        float wx = world_x + x;
                        float wy = world_y + y;
                        float wz = world_z + z;

                        // Check each face and add vertices if visible
                        // Face 0: Back (-Z)
                        if (chunk.should_render_face(x, y, z, 0, 0, -1)) {
                            add_face(mesh_vertices_, wx, wy, wz, 0, block);
                        }
                        // Face 1: Front (+Z)
                        if (chunk.should_render_face(x, y, z, 0, 0, 1)) {
                            add_face(mesh_vertices_, wx, wy, wz, 1, block);
                        }
                        // Face 2: Left (-X)
                        if (chunk.should_render_face(x, y, z, -1, 0, 0)) {
                            add_face(mesh_vertices_, wx, wy, wz, 2, block);
                        }
                        // Face 3: Right (+X)
                        if (chunk.should_render_face(x, y, z, 1, 0, 0)) {
                            add_face(mesh_vertices_, wx, wy, wz, 3, block);
                        }
                        // Face 4: Top (+Y)
                        if (chunk.should_render_face(x, y, z, 0, 1, 0)) {
                            add_face(mesh_vertices_, wx, wy, wz, 4, block);
                        }
                        // Face 5: Bottom (-Y)
                        if (chunk.should_render_face(x, y, z, 0, -1, 0)) {
                            add_face(mesh_vertices_, wx, wy, wz, 5, block);
                        }
                    }
                }
            }   
        }

        vertex_count_ = mesh_vertices_.size() / 8; //5 floats per vertex (3pos + 2tex)

        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, mesh_vertices_.size() * sizeof(float), mesh_vertices_.data(), GL_STATIC_DRAW);
        //pos attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        //norm attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        //texture coord attribute
        glVertexAttribPointer(2,2,GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

void Mesher::clear_mesh() {
    if (VAO_ != 0) {
        glDeleteVertexArrays(1, &VAO_);
    }
    if (VBO_ != 0) {
        glDeleteBuffers(1, &VBO_);
    }
}

std::size_t Mesher::get_vertex_count() {
    return vertex_count_;
}


unsigned int Mesher::get_VAO() {
    return VAO_;
}

unsigned int Mesher::get_VBO() {
    return VBO_;
}