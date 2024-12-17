#include <fstream>
#include <vector>
#include <sstream>

std::vector<Vec3> loadTreePositions(const std::string& filename) {
    std::vector<Vec3> positions;
    std::ifstream file(filename);
    if (!file.is_open()) {
        return positions;
    }
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        float x, y, z;
        ss >> x >> y >> z;
        positions.emplace_back(x, y, z);
    }
    file.close();
    return positions;
}
struct TreePosition {
    int x, z;
    bool operator<(const TreePosition& other) const {
        if (x < other.x) return true;
        if (x > other.x) return false;
        return z < other.z;
    }
};
