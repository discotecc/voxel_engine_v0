#pragma once

#include "world.h"

struct MeshStats {
    size_t total_vertices = 0;
    size_t total_faces = 0;
    size_t total_blocks_processed = 0;
    size_t chunks_processed = 0;
    size_t air_blocks_skipped = 0;
};

class Mesher {
public:
    Mesher(World& world);
    ~Mesher();

    void get_tex_coords(Block_Type type, int face, float& u_min, float& v_min, float& u_max, float& v_max);
    void add_face(std::vector<float>& vertices, float x, float y, float z, int face_index, Block_Type block);
    void generate_mesh();
    void generate_full_mesh();
    bool should_mesh_face(const glm::ivec3& chunk_pos, const glm::ivec3& face_pos, const glm::ivec3& direction);    //takes neighboring chunks into account when face is at chunk edge
    void upload_mesh();
    void clear_mesh();
    bool is_dirty();
    void mark_clean();
    void init();

    unsigned int get_VAO();
    unsigned int get_VBO();
    std::size_t get_vertex_count();
    MeshStats get_mesh_stats();

    

private:
    World& world_;

    std::vector<float> mesh_vertices_;

    unsigned int VAO_;
    unsigned int VBO_;
    
    std::size_t vertex_count_;

    bool is_dirty_;

    MeshStats stats_;
};

Mesher::Mesher(World& world) : world_(world), VAO_(0), VBO_(0), vertex_count_(0), is_dirty_(false) {
   
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
        if (face == 4) { //case top face
            u_min = (tile_x * 16.0f) / 1024.0f;
            u_max = ((tile_x + 1) * 16.0f) / 1024.0f;
            v_min = (tile_y * 16.0f) / 512.0f;
            v_max = ((tile_y + 1) * 16.0f) / 512.0f;
        }
        else if (face == 5) { //case bottom face
            u_min = (tile_x * 16.0f) / 1024.0f;
            u_max = ((tile_x + 1) * 16.0f) / 1024.0f;
            v_min = (tile_y * 16.0f) / 512.0f;
            v_max = ((tile_y + 1) * 16.0f) / 512.0f;
        }
        else { //case side face
            u_min = (tile_x * 16.0f) / 1024.0f;
            u_max= ((tile_x + 1) * 16.0f) / 1024.0f;
            v_max = (tile_y * 16.0f) / 512.0f;
            v_min = ((tile_y + 1) * 16.0f) / 512.0f;
        }

    
    }

void Mesher::add_face(std::vector<float>& vertices, float x, float y, float z, int face_index, Block_Type block) {
    float u_min, v_min, u_max, v_max;
    get_tex_coords(block, face_index, u_min, v_min, u_max, v_max);

    switch(face_index) {
        case 0: // Back face (-Z)
            vertices.insert(vertices.end(), {
                // Position                    Normal              TexCoord
                x-0.5f, y-0.5f, z-0.5f,   0.0f, 0.0f, -1.0f,   u_max, v_min, 
                x-0.5f, y+0.5f, z-0.5f,   0.0f, 0.0f, -1.0f,   u_max, v_max,
                x+0.5f, y+0.5f, z-0.5f,   0.0f, 0.0f, -1.0f,   u_min, v_max,
                x+0.5f, y+0.5f, z-0.5f,   0.0f, 0.0f, -1.0f,   u_min, v_max,
                x+0.5f, y-0.5f, z-0.5f,   0.0f, 0.0f, -1.0f,   u_min, v_min,
                x-0.5f, y-0.5f, z-0.5f,   0.0f, 0.0f, -1.0f,   u_max, v_min
            });
            stats_.total_vertices += 48; //48 vertices per face....
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
            stats_.total_vertices += 48;
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
            stats_.total_vertices += 48;
            break;
        case 3: // Right face (+X)
            vertices.insert(vertices.end(), {
                x+0.5f, y+0.5f, z+0.5f,   1.0f, 0.0f, 0.0f,    u_min, v_max, 
                x+0.5f, y-0.5f, z+0.5f,   1.0f, 0.0f, 0.0f,    u_min, v_min,
                x+0.5f, y-0.5f, z-0.5f,   1.0f, 0.0f, 0.0f,    u_max, v_min,
                x+0.5f, y-0.5f, z-0.5f,   1.0f, 0.0f, 0.0f,    u_max, v_min,
                x+0.5f, y+0.5f, z-0.5f,   1.0f, 0.0f, 0.0f,    u_max, v_max,
                x+0.5f, y+0.5f, z+0.5f,   1.0f, 0.0f, 0.0f,    u_min, v_max
            });
            stats_.total_vertices += 48;
            break;
        case 4: // Top face (+Y) 
            vertices.insert(vertices.end(), {
                x-0.5f, y+0.5f, z-0.5f,   0.0f, 1.0f, 0.0f,    u_min, v_max,  
                x-0.5f, y+0.5f, z+0.5f,   0.0f, 1.0f, 0.0f,    u_min, v_min,
                x+0.5f, y+0.5f, z+0.5f,   0.0f, 1.0f, 0.0f,    u_max, v_min,
                x+0.5f, y+0.5f, z+0.5f,   0.0f, 1.0f, 0.0f,    u_max, v_min,
                x+0.5f, y+0.5f, z-0.5f,   0.0f, 1.0f, 0.0f,    u_max, v_max,
                x-0.5f, y+0.5f, z-0.5f,   0.0f, 1.0f, 0.0f,    u_min, v_max
            });
            stats_.total_vertices += 48;
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
            stats_.total_vertices += 48;
            break;
    }
}

void Mesher::generate_mesh() {
	
	    mesh_vertices_.clear();
        stats_ = MeshStats{};

        for (auto& [chunk_pos, chunk] : world_.chunks_) {
            //origin of current chunk in integer world space... 
            glm::ivec3 chunk_origin(chunk_pos.x * CHUNK_SIZE, chunk_pos.y * CHUNK_SIZE, chunk_pos.z * CHUNK_SIZE);
            
            float world_x = static_cast<float>(chunk_pos.x * CHUNK_SIZE);
            float world_y = static_cast<float>(chunk_pos.y * CHUNK_SIZE);
            float world_z = static_cast<float>(chunk_pos.z * CHUNK_SIZE);

            for (int x = 0; x < CHUNK_SIZE; x++) {
                for (int y = 0; y < CHUNK_SIZE; y++) {
                    for (int z = 0; z < CHUNK_SIZE; z++) {
                        Block_Type block = static_cast<Block_Type>(chunk.blocks_[index(glm::ivec3(x,y,z))]);
                        if (block == AIR) {
                            stats_.air_blocks_skipped++;
                            continue;
                        };

                        glm::ivec3 local_pos(x,y,z);
                        glm::ivec3 world_pos(chunk_origin.x + local_pos.x, chunk_origin.y + local_pos.y, chunk_origin.z + local_pos.z);
                        float wx = world_x + static_cast<float>(x);
                        float wy = world_y + static_cast<float>(y);
                        float wz = world_z + static_cast<float>(z);

                        // Check each face and add vertices if visible... condition for visibility is whether surface is exposed to air
                        // Face 0: Back (-Z)
                        if (should_mesh_face(chunk_pos, local_pos,glm::ivec3(0, 0, -1))) {
                            add_face(mesh_vertices_, wx, wy, wz, 0, block);
                            stats_.total_faces++;
                        }
                        // Face 1: Front (+Z)
                        if (should_mesh_face(chunk_pos, local_pos,glm::ivec3(0, 0, 1))) {
                            add_face(mesh_vertices_, wx, wy, wz, 1, block);
                            stats_.total_faces++;
                        }
                        // Face 2: Left (-X)
                        if (should_mesh_face(chunk_pos, local_pos,glm::ivec3(-1, 0, 0))) {
                            add_face(mesh_vertices_, wx, wy, wz, 2, block);
                            stats_.total_faces++;
                        }
                        // Face 3: Right (+X)
                        if (should_mesh_face(chunk_pos, local_pos,glm::ivec3(1, 0, 0))) {
                            add_face(mesh_vertices_, wx, wy, wz, 3, block);
                            stats_.total_faces++;
                        }
                        // Face 4: Top (+Y)
                        if (should_mesh_face(chunk_pos, local_pos,glm::ivec3( 0, 1, 0))) {
                            add_face(mesh_vertices_, wx, wy, wz, 4, block);
                            stats_.total_faces++;
                        }
                        // Face 5: Bottom (-Y)
                        if (should_mesh_face(chunk_pos, local_pos,glm::ivec3(0, -1, 0))) {
                            add_face(mesh_vertices_, wx, wy, wz, 5, block);
                            stats_.total_faces++;
                        }
                        stats_.total_blocks_processed++;
                    }
                }
            } 
            stats_.chunks_processed++;  
        }

	upload_mesh();
    }

void Mesher::generate_full_mesh() {
       
	mesh_vertices_.clear();

        float world_x, world_y, world_z;

        for (auto& [position, chunk] : world_.chunks_) {
            world_x = position.x * CHUNK_SIZE;
            world_y = position.y * CHUNK_SIZE;
            world_z = position.z * CHUNK_SIZE;
            for (int x = 0; x < CHUNK_SIZE; x++) {
                for (int y = 0; y < CHUNK_SIZE; y++) {
                    for (int z = 0; z < CHUNK_SIZE; z++) {
                        Block_Type block = static_cast<Block_Type>(chunk.blocks_[index(glm::ivec3(x,y,z))]);

                        if (block == AIR) continue;

                        float wx = world_x + x;
                        float wy = world_y + y;
                        float wz = world_z + z;

                        // Face 0: Back (-Z)
                        add_face(mesh_vertices_, wx, wy, wz, 0, block);
                        // Face 1: Front (+Z)
                        add_face(mesh_vertices_, wx, wy, wz, 1, block);
                        // Face 2: Left (-X)
                        add_face(mesh_vertices_, wx, wy, wz, 2, block);
                        // Face 3: Right (+X)
                        add_face(mesh_vertices_, wx, wy, wz, 3, block);
                        // Face 4: Top (+Y)
                        add_face(mesh_vertices_, wx, wy, wz, 4, block);
                        // Face 5: Bottom (-Y)
                        add_face(mesh_vertices_, wx, wy, wz, 5, block);
                        
                    }
                }
            }   
        }

	upload_mesh();
}

bool Mesher::should_mesh_face(const glm::ivec3& chunk_pos, const glm::ivec3& local_pos, const glm::ivec3& direction) {
    glm::ivec3 neighbor_local = local_pos + direction;
    if (neighbor_local.x >= 0 && neighbor_local.x < CHUNK_SIZE && neighbor_local.y >= 0 && neighbor_local.y < CHUNK_SIZE && neighbor_local.z >= 0 && neighbor_local.z < CHUNK_SIZE) {
        //case neighboring block is in same chunk
        uint16_t neighbor_block = world_.chunks_[chunk_pos].blocks_[index(neighbor_local)];
        return neighbor_block == AIR;
    }
    //else neighboring block is in a different chunk
    glm::ivec3 neighbor_chunk_pos = chunk_pos + direction; //direction is a unit vector so is context agnostic in discrete spaces
    auto it = world_.chunks_.find(neighbor_chunk_pos);
    if (it == world_.chunks_.end()) {
        return true; //neighboring chunk doesn't exist
    }
    glm::ivec3 converted_local((neighbor_local.x + CHUNK_SIZE) % CHUNK_SIZE, (neighbor_local.y + CHUNK_SIZE) % CHUNK_SIZE, (neighbor_local.z + CHUNK_SIZE) % CHUNK_SIZE); //neighbor_local is relative to current chunk; conversion to neighbor chunk's block space
    uint16_t neighbor_block = it->second.blocks_[index(converted_local)];
    return neighbor_block == AIR;
}

void Mesher::upload_mesh() {
        vertex_count_ = mesh_vertices_.size() / 8; //5 floats per vertex (3pos + 2tex)

        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, mesh_vertices_.size() * sizeof(float), mesh_vertices_.data(), GL_STATIC_DRAW);
        //pos attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        //norm attribute
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        //glEnableVertexAttribArray(1);
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

MeshStats Mesher::get_mesh_stats() {
    return stats_;
}

unsigned int Mesher::get_VAO() {
    return VAO_;
}

unsigned int Mesher::get_VBO() {
    return VBO_;
}

bool Mesher::is_dirty() {
    return is_dirty_;
}

void Mesher::mark_clean() {
    is_dirty_ = false;
}