#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <set>
#include <cmath>

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
        return std::tie(x, z) < std::tie(other.x, other.z);
    }
};

bool isValidPosition(const TreePosition& newPos, const std::set<TreePosition>& trees, float minDistance) {
    for (const auto& tree : trees) {
        float distance = std::sqrt((newPos.x - tree.x) * (newPos.x - tree.x) +
            (newPos.z - tree.z) * (newPos.z - tree.z));
        if (distance < minDistance) {
            return false;
        }
    }
    return true;
}
void generateForestFile(const std::string& filename, int treeCount, int gridSize, float minDistance, int exclusionRange) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }

    file << "TreePositions\n";
    std::set<TreePosition> trees;
    srand(static_cast<unsigned>(time(0)));

    while (trees.size() < treeCount) {
        int x = rand() % (gridSize + 1) - gridSize / 2;
        int z = rand() % (gridSize + 1) - gridSize / 2;

    
        if (abs(x) <= exclusionRange && abs(z) <= exclusionRange) {
            continue;  
        }

        TreePosition newTree = { x, z };

        
        if (trees.find(newTree) == trees.end() && isValidPosition(newTree, trees, minDistance)) {
            trees.insert(newTree);
            file << x << " 0 " << z << "\n";
        }
    }

    file.close();
  
}

