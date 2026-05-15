#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "camera.h"
#include "chunk.h"
#include "world.h"

#include <spdlog/spdlog.h>

class Player {
public:
  Camera *camera_;

  Player();
  ~Player();

  void process_keyboard(Camera_Movement direction);
  void process_mouse_movement(float xoffset, float yoffset,
                              GLboolean constrainPitch);

  void set_player_pos_f(glm::vec3 global_pos_f);
  void set_player_pos_i(glm::ivec3 global_pos_i);
  void set_player_pos_l(glm::ivec3 chunk_pos, glm::ivec3 block_pos);

  glm::ivec3 get_player_chunk_pos(); // get position in chunk space of the chunk
                                     // currently inhabited by the player
  glm::ivec3 get_player_pos_global_i(); // get global coords of the nearest
                                        // block to the player position
  glm::vec3 get_player_pos_global_f();  // get actual player coordinates

private:
  // player position
  glm::vec3 pos_global_f_;     // raw floats
  glm::ivec3 pos_global_i_;    // integer floor of pos_global_f, origin of block
                               // pos_global_f sits within
  glm::ivec3 inhabited_chunk_; // what chunk is the player currently in

  // player orientation
  float yaw_;
  float pitch_;
  float movement_speed_;
};

// quick todo later set up this constructor to optionally set movementspeed but
// default to 20.0f
Player::Player() {
  spdlog::info("entered player constructor....");
  yaw_ = -90.0f;
  pitch_ = 0.0f;
  movement_speed_ = 20.0f;
  // camera_ = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), yaw_, pitch_);
  camera_ = new Camera(glm::vec3(88.0f, 252.0f, 88.0f), yaw_, pitch_);
  pos_global_f_ = camera_->get_pos();
  pos_global_i_ = global_f_to_global_i_pos(pos_global_f_);
  spdlog::info("leaving player constructor...");
};

Player::~Player() { delete camera_; };

void Player::process_mouse_movement(float xoffset, float yoffset,
                                    GLboolean constrainPitch = true) {
  spdlog::info("entered process_mouse_movement...");
  if (camera_->is_locked()) {
    spdlog::info("Camera is locked! leaving process_mouse_movement...");
    return;
  }

  xoffset *= camera_->get_mouse_sensitivity();
  yoffset *= camera_->get_mouse_sensitivity();

  yaw_ += xoffset;
  pitch_ += yoffset;

  if (constrainPitch) {
    if (pitch_ > 89.0f) {
      pitch_ = 89.0f;
      camera_->set_pitch(pitch_);
    }
    if (pitch_ < -89.0f) {
      pitch_ = -89.0f;
      camera_->set_pitch(pitch_);
    }
  }
  camera_->set_pitch(pitch_);
  camera_->set_yaw(yaw_);
  camera_->update();
  spdlog::info("leaving process mouse movement...");
}

void Player::process_keyboard(Camera_Movement direction) {
  float velocity = movement_speed_ * 0.02;
  if (direction == FORWARD)
    set_player_pos_f(pos_global_f_ + camera_->get_front() * velocity);
  if (direction == BACKWARD)
    set_player_pos_f(pos_global_f_ - camera_->get_front() * velocity);
  if (direction == LEFT)
    set_player_pos_f(pos_global_f_ - camera_->get_right() * velocity);
  if (direction == RIGHT)
    set_player_pos_f(pos_global_f_ + camera_->get_right() * velocity);
}

void Player::set_player_pos_f(glm::vec3 pos_global_f) {
  pos_global_f_ = pos_global_f;
  pos_global_i_ = global_f_to_global_i_pos(pos_global_f);
  camera_->set_pos(pos_global_f);
  inhabited_chunk_ = global_to_chunk_pos(pos_global_f);
  spdlog::info("Player::set_player_pos_f... player moved to {}x {}y {}z...",
               (float)pos_global_f_.x, (float)pos_global_f_.y,
               (float)pos_global_f_.z);
};

void Player::set_player_pos_i(glm::ivec3 pos_global_i) {
  pos_global_i_ = pos_global_i;
  pos_global_f_ = pos_global_i;
  camera_->set_pos(pos_global_i);
};

void Player::set_player_pos_l(glm::ivec3 chunk_pos, glm::ivec3 block_pos) {
  pos_global_i_ = local_to_global_pos(chunk_pos, block_pos);
  pos_global_f_ = pos_global_i_;
  camera_->set_pos(pos_global_f_);
  inhabited_chunk_ = global_to_chunk_pos(pos_global_f_);
};

glm::ivec3 Player::get_player_chunk_pos() {
  return global_to_chunk_pos(pos_global_f_);
};

glm::ivec3 Player::get_player_pos_global_i() { return pos_global_i_; };

glm::vec3 Player::get_player_pos_global_f() { return pos_global_f_; };