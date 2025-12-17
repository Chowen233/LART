#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

//#include "hittable.h"
//#include "hittable_list.h"

#include <fstream>
#include <sstream>
#include <vector>

//class obj_loader {
//  public:
//	std::shared_ptr<hittable_list> parseOBJ(const std::string filePath, shared_ptr<material> mat);
//};

inline std::shared_ptr<hittable_list> parseOBJ(const std::string filePath, shared_ptr<material> mat) {
	std::ifstream fileStream(filePath);
	if (!fileStream.is_open()) {
		std::cerr << "Failed to open OBJ file: " << filePath << std::endl;
		return std::make_shared<hittable_list>();
	}

	std::string line;
	std::vector<point3> verts;
	auto faces = std::make_shared<hittable_list>();

    while (std::getline(fileStream, line)) {
        if (line.empty()) continue;

        std::istringstream ls(line);
        std::string token;
        ls >> token;
        if (token == "v") {
            double x, y, z;
            ls >> x >> y >> z;
            verts.emplace_back(x, y, z);
        }
        else if (token == "f") {
            std::vector<int> indices;
            std::string vertToken;
            while (ls >> vertToken) {
                int vIndex = 0;
                size_t pos = vertToken.find('/');
                if (pos == std::string::npos) {
                    vIndex = std::stoi(vertToken);
                }
                else {
                    std::string vStr = vertToken.substr(0, pos);
                    vIndex = std::stoi(vStr);
                }

                int actualIdx;
                if (vIndex < 0) {
                    actualIdx = static_cast<int>(verts.size()) + vIndex; // vIndex is negative
                }
                else {
                    actualIdx = vIndex - 1; // OBJ is 1-based
                }

                indices.push_back(actualIdx);
            }

            // 如果是三角面直接加入；多边形用扇形三角剖分 (0, i-1, i) 方式
            if (indices.size() >= 3) {
                for (size_t i = 1; i + 1 < indices.size(); ++i) {
                    const vec3& a = verts[indices[0]];
                    const vec3& b = verts[indices[i]];
                    const vec3& c = verts[indices[i + 1]];
                    faces->add(std::make_shared<triangle>(a * 1600, b * 1600, c * 1600, mat));
                }
            }
        }
    }
	return faces;
}

#endif