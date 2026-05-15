#pragma once

#include <vector>

#include <FastNoiseLite.h>

#include "biome_type.h"
#include "chunk.h"

class Terrain {
public:
  FastNoiseLite noise_;

  Terrain() {
    noise_.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise_data_.resize(CHUNK_WIDTH * CHUNK_WIDTH);
    // std::cout << "noise_data size.... " << noise_data_.size() << std::endl;
  }

  void generate_terrain();
  void generate_biome_terrain(Biome_Type biome,
                              int size); // biome size in square chunks
  std::vector<float> get_normalized_terrain();
  std::vector<float> get_terrain();

  int get_column_height(int x, int z);

private:
  std::vector<float> noise_data_;
};

void Terrain::generate_terrain() {
  int index = 0;
  for (int x = 0; x < CHUNK_WIDTH; x++) {
    for (int z = 0; z < CHUNK_WIDTH; z++) {
      noise_data_[index++] = noise_.GetNoise((float)x, (float)z);
    }
  }
};

// for now produces a mountainous noise pattern contiguous across a square set
// of chunks
void Terrain::generate_biome_terrain(Biome_Type biome, int size) {

  // per column height map for every chunk of the biome
  noise_data_.resize(CHUNK_WIDTH * CHUNK_WIDTH * size);
};

std::vector<float> Terrain::get_normalized_terrain() {
  std::vector<float> nt(noise_data_.size());
  int index = 0;
  float data = 0;
  for (int x = 0; x < noise_data_.size(); x++) {
    data = noise_data_[x];
    data = data + 1;
    data = data / 2;
    data = data *
           CHUNK_WIDTH; // general norm equation... (x - min(x))/(max(x)-min(x))
    nt[x] = data;
  }
  return nt;
};

int Terrain::get_column_height(int x, int z) {
  float data = noise_.GetNoise((float)x, (float)z);
  data = data + 1;
  data = data / 2;
  data = data * CHUNK_HEIGHT;
  int column_height = static_cast<int>(data);
  return column_height;
}

std::vector<float> Terrain::get_terrain() { return noise_data_; }
