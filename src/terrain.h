#pragma once

#include <vector>

#include <FastNoiseLite.h>

class Terrain { 
	public:
		FastNoiseLite noise_;

		Terrain(int chunk_size) {
			noise_.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
			noise_data_.resize(chunk_size * chunk_size);
			chunk_size_ = chunk_size;
			std::cout << "noise_data size.... " << noise_data_.size() << std::endl;
		}

		void generate_terrain();
		std::vector<float> get_normalized_terrain();
		std::vector<float> get_terrain();
	private:
		std::vector<float> noise_data_;
		int chunk_size_;
					
};

void Terrain::generate_terrain() {
	int index = 0;
	for (int x = 0; x < chunk_size_; x++) {
		for (int y = 0; y < chunk_size_; y++) {
			noise_data_[index++] = noise_.GetNoise((float)x, (float)y);
		}
	}
};

std::vector<float> Terrain::get_normalized_terrain() {
	std::vector<float> nt(noise_data_.size());
	int index = 0;
	float data = 0;
	for (int x = 0; x < noise_data_.size(); x++) {
		data = noise_data_[x];
		data = data + 1;
		data = data / 2;
		data = data * chunk_size_;	//general norm equation... (x - min(x))/(max(x)-min(x))
		nt[x] = data;
	}
	return nt;
};

std::vector<float> Terrain::get_terrain() {
	return 	noise_data_;
}
