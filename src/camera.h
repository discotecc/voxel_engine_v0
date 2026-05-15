#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Defines directions for camera movement abstracted away from window-specific
// input methods so can be reused in many contexts
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

// const float SPEED = 8.5f;
const float SPEED = 20.0f;
const float SENSITIVITY = 0.7f;
const float ZOOM = 45.0f;

class Camera {
public:
  // constructor takes in position, yaw, pitch
  Camera(glm::vec3 pos, float yaw, float pitch);

  glm::mat4 get_view_matrix() const;
  glm::mat4 get_projection_matrix(float width, float height) const;

  void toggle_lock();
  void process_mouse_scroll(float yoffset);
  void update() { update_camera_vectors(); };

  glm::vec3 get_pos();
  glm::vec3 get_front();
  glm::vec3 get_up();
  glm::vec3 get_right();
  glm::vec3 get_world_up();

  float get_yaw();
  float get_pitch();

  float get_movement_speed();
  float get_mouse_sensitivity();
  float get_zoom();
  bool is_locked();

  void set_pos(glm::vec3 pos);
  void set_front(glm::vec3 front);
  void set_yaw(float yaw);
  void set_pitch(float pitch);
  void set_movement_speed(float movement_speed);
  void set_mouse_sensitivity(float mouse_sensitivity);
  void set_zoom(float zoom);

private:
  glm::vec3 pos_;
  glm::vec3 front_;
  glm::vec3 up_;
  glm::vec3 right_;
  glm::vec3 world_up_;
  // eueler angles
  float yaw_;
  float pitch_;
  // camera options
  float movement_speed_;
  float mouse_sensitivity_;
  float zoom_;
  bool is_locked_;

  void update_camera_vectors();
};

// constructor takes in position, yaw, pitch
Camera::Camera(glm::vec3 pos, float yaw, float pitch) {
  spdlog::info("entered camera constructor...");
  pos_ = pos;
  yaw_ = yaw;
  pitch_ = pitch;
  up_ = glm::vec3(0.0f, 1.0f, 0.0f);
  world_up_ = glm::vec3(0.0f, 1.0f, 0.0f);
  front_ = glm::vec3(0.0f, 0.0f, -1.0f);
  movement_speed_ = SPEED;
  mouse_sensitivity_ = SENSITIVITY;
  zoom_ = ZOOM;
  is_locked_ = false;
  update_camera_vectors();
  spdlog::info("leaving camera constructor...");
}

glm::mat4 Camera::get_view_matrix() const {
  return glm::lookAt(pos_, pos_ + front_, up_);
}

glm::mat4 Camera::get_projection_matrix(float width, float height) const {
  return glm::perspective(glm::radians(zoom_), width / height, 0.1f, 10000.0f);
}

void Camera::update_camera_vectors() {

  // calculate the new front vector
  glm::vec3 front;
  front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
  front.y = sin(glm::radians(pitch_));
  front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
  front_ = glm::normalize(front);
  // recalculate right and up vectors
  right_ = glm::normalize(glm::cross(front, world_up_));
  up_ = glm::normalize(glm::cross(right_, front));
}

void Camera::process_mouse_scroll(float yoffset) {
  zoom_ -= (float)yoffset;

  if (zoom_ < 1.0f)
    zoom_ = 1.0f;
  if (zoom_ > 45.0f)
    zoom_ = 45.0f;
}

void Camera::toggle_lock() {
  is_locked_ = !is_locked_;
  if (is_locked_)
    std::cout << "Locked Camera" << std::endl;
  else
    std::cout << "Unlocked Camera" << std::endl;
}

glm::vec3 Camera::get_pos() { return pos_; };
glm::vec3 Camera::get_front() { return front_; };
glm::vec3 Camera::get_up() { return up_; };
glm::vec3 Camera::get_right() { return right_; };
glm::vec3 Camera::get_world_up() { return world_up_; };
float Camera::get_yaw() { return yaw_; };
float Camera::get_pitch() { return pitch_; };
float Camera::get_movement_speed() { return movement_speed_; };
float Camera::get_mouse_sensitivity() { return mouse_sensitivity_; };
float Camera::get_zoom() { return zoom_; };
bool Camera::is_locked() { return is_locked_; };

void Camera::set_pos(glm::vec3 pos) { pos_ = pos; };
void Camera::set_front(glm::vec3 front) { front_ = front; };
void Camera::set_yaw(float yaw) { yaw_ = yaw; };
void Camera::set_pitch(float pitch) { pitch_ = pitch; };
void Camera::set_movement_speed(float movement_speed) {
  movement_speed_ = movement_speed;
};
void Camera::set_mouse_sensitivity(float mouse_sensitivity) {
  mouse_sensitivity_ = mouse_sensitivity;
};
void Camera::set_zoom(float zoom) { zoom_ = zoom; };

#endif