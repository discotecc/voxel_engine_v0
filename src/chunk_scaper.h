#pragma once
#include "player.h"
#include "renderer.h"

// denotes 3 nested zones in chunk space.... visible zone, meshing zone,
// generation zone

// visible zone -> render distance -> chunks between player and render distance
// are rendered on screen

// meshing zone -> meshing distance -> chunks between player and meshing
// distance are meshed and uploaded to GPU, available to be rendered

// generation zone -> generation distance -> chunks between player and
// generation distance will be created, terrained, and made available for
// meshing, if not already present

// meshing zone is the boundary between what is and what isn't on the GPU at any
// given frame...

// meshing distance and render distance can be equal or near, generation
// distance should be pretty far away

class Chunk_Scaper {
public:
  Renderer &renderer_;

  std::unordered_map<glm::ivec3, Chunk> visible_chunks_;
  int render_distance_;
  int meshing_distance_;
  int generation_distance_;

  Chunk_Scaper(Renderer &renderer, int render_distance);
  void update_visible_chunks();
  void add_visible_chunks();
  void remove_invisible_chunks();
  void generate_new_chunks();
  void mesh_new_chunks();
  void unmesh_old_chunks();

  bool is_chunk_visible(glm::ivec3 chunk_pos);
  bool should_be_meshed(glm::ivec3 chunk_pos);
  bool should_be_generated(glm::ivec3 chunk_pos);
};

Chunk_Scaper::Chunk_Scaper(Renderer &renderer, int render_distance)
    : renderer_(renderer), render_distance_(render_distance) {};

void Chunk_Scaper::update_visible_chunks() {
  remove_invisible_chunks();
  add_visible_chunks();
}

void Chunk_Scaper::add_visible_chunks() {
  for (int x = -render_distance_; x <= render_distance_; x++) {
    for (int z = -render_distance_; z <= render_distance_; z++) {
      glm::ivec3 current_chunk_pos = glm::ivec3(x, 0, z);
      if (is_chunk_visible(current_chunk_pos)) {
        if (!renderer_.world_.chunks_.count(current_chunk_pos)) {
          // case chunk hasnt aleady been generated
          renderer_.world_.generate_terrained_chunk(current_chunk_pos);
        }
        Chunk *current_chunk = renderer_.world_.get_chunk(current_chunk_pos);
        visible_chunks_.try_emplace(current_chunk_pos, current_chunk);
      }
    }
  }
};

// iterates through set of visible chunks and checks their positions against
// render distance to determine removal
void Chunk_Scaper::remove_invisible_chunks() {
  for (auto it = visible_chunks_.begin(); it != visible_chunks_.end();) {
    auto &chunk = it->second;
    if (!is_chunk_visible(chunk.chunk_pos_)) {
      it = visible_chunks_.erase(it);
    } else {
      ++it;
    }
  }
};

// iterates through chunks within generation distance ensuring they have been
// generated onto cpu with terrain
void Chunk_Scaper::generate_new_chunks() {
  glm::ivec3 central_chunk = renderer_.player_->get_inhabited_chunk();
  for (int x = -generation_distance_; x <= generation_distance_; x++) {
    for (int z = -generation_distance_; z <= generation_distance_; z++) {
      renderer_.world_.generate_terrained_chunk(
          glm::ivec3(x + central_chunk.x, 0, z + central_chunk.z));
    }
  }
}

// iterates through chunks within meshing distance and adds new vertices to
// global mesh if not already present
void Chunk_Scaper::mesh_new_chunks() {
  glm::ivec3 central_chunk = renderer_.player_->get_inhabited_chunk();
  for (int x = -meshing_distance_; x <= meshing_distance_; x++) {
    for (int z = -meshing_distance_; z <= meshing_distance_; z++) {
    }
  }
}

void Chunk_Scaper::unmesh_old_chunks() {}

bool Chunk_Scaper::is_chunk_visible(glm::ivec3 chunk_pos) {
  glm::ivec3 central_chunk = renderer_.player_->get_inhabited_chunk();
  if (chunk_pos.x >= central_chunk.x - render_distance_ &&
      chunk_pos.x <= central_chunk.x + render_distance_ &&
      chunk_pos.z >= central_chunk.z - render_distance_ &&
      chunk_pos.z <= central_chunk.z + render_distance_) {
    return true;
  } else
    return false;
}

bool Chunk_Scaper::should_be_meshed(glm::ivec3 chunk_pos) {
  glm::ivec3 central_chunk = renderer_.player_->get_inhabited_chunk();
  if (chunk_pos.x >= central_chunk.x - meshing_distance_ &&
      chunk_pos.x <= central_chunk.x + meshing_distance_ &&
      chunk_pos.z >= central_chunk.z - meshing_distance_ &&
      chunk_pos.z <= central_chunk.z + meshing_distance_) {
    return true;
  } else
    return false;
}

bool Chunk_Scaper::should_be_generated(glm::ivec3 chunk_pos) {
  glm::ivec3 central_chunk = renderer_.player_->get_inhabited_chunk();
  if (chunk_pos.x >= central_chunk.x - generation_distance_ &&
      chunk_pos.x <= central_chunk.x + generation_distance_ &&
      chunk_pos.z >= central_chunk.z - generation_distance_ &&
      chunk_pos.z <= central_chunk.z + generation_distance_) {
    return true;
  } else
    return false;
}