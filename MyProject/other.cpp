#include "other.h"

bool running = true;
bool aoeTriggered = false;   
float heroProjectileSpeed = 100.f;
int aoeNumber = 5;
float score = 0.f;
bool saveTriggered = false;
int level;

bool fileExists(std::string filename) {
	std::ifstream infile(filename, std::ios::binary);
	if (infile.is_open()) return true;
	return false;
}

int getLevel() {
	do {
		std::cout << "Input 1 to enter fixed map		Input 2 to enter loop map" << std::endl;
		std::cin >> level;
	} while (level != 2 && level != 1);
	return level;
}

void vecNormalize(Vector& vec) {
	float length = std::sqrtf(std::pow(vec.x, 2) + std::pow(vec.y, 2));
	if (length != 0) {
		vec.x /= length;
		vec.y /= length;
	}
}


void renderImg(GamesEngineeringBase::Window& canvas, GamesEngineeringBase::Image& sprite, Vector& pos, Camera& cm) {
	for (unsigned int i = 0; i < sprite.height; i++) {
		int cmY = pos.y + i - cm.Position.y;
		if (cmY < canvas.getHeight() && cmY >= 0) {
			for (unsigned int n = 0; n < sprite.width; n++)
			{
				int cmX = pos.x + n - cm.Position.x;
				if (cmX < canvas.getWidth() && cmX >= 0) {
					if (sprite.alphaAt(n, i) > 200)
						canvas.draw(cmX, cmY, sprite.at(n, i));
				}
			}
		}
	}
}


bool checkImageCollision(GamesEngineeringBase::Image& img1, unsigned int x1, unsigned int y1, GamesEngineeringBase::Image& img2, unsigned int x2, unsigned int y2)
{
	unsigned int xOverlapStart = max(x1, x2);
	unsigned int yOverlapStart = max(y1, y2);
	unsigned int xOverlapEnd = min(x1 + img1.width, x2 + img2.width);
	unsigned int yOverlapEnd = min(y1 + img1.height, y2 + img2.height);

	if (xOverlapStart >= xOverlapEnd || yOverlapStart >= yOverlapEnd) return false;

	for (unsigned int y = yOverlapStart; y < yOverlapEnd; ++y)
	{
		for (unsigned int x = xOverlapStart; x < xOverlapEnd; ++x)
		{
			if (img1.alphaAt(x - x1, y - y1) > 0 && img2.alphaAt(x - x2, y - y2) > 0)
				return true;
		}
	}

	return false;
}