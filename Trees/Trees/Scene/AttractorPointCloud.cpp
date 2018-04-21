#include "Globals.h"
#include "AttractorPointCloud.h"

void AttractorPointCloud::GeneratePointsInUnitCube(unsigned int numPoints) {
    #ifdef ENABLE_DEBUG_OUTPUT
    auto start = std::chrono::system_clock::now();
    #endif
    for (unsigned int i = 0; i < numPoints; ++i) {
        const glm::vec3 p = glm::vec3(dis(rng), dis(rng), dis(rng));
        minPoint.x = std::min(minPoint.x, p.x);
        minPoint.y = std::min(minPoint.y, p.y);
        minPoint.z = std::min(minPoint.z, p.z);
        maxPoint.x = std::max(maxPoint.x, p.x);
        maxPoint.y = std::max(maxPoint.y, p.y);
        maxPoint.z = std::max(maxPoint.z, p.z);
        points.emplace_back(AttractorPoint(p));
    }
    #ifdef ENABLE_DEBUG_OUTPUT
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << "Elapsed time for Attractor Point Cloud Generation: " << elapsed_seconds.count() << "s\n";
    std::cout << "Number of Attractor Points Generated: " << points.size() << "\n\n";
    #endif
    create();
}

void AttractorPointCloud::GeneratePoints(unsigned int numPoints) {
    #ifdef ENABLE_DEBUG_OUTPUT
    auto start = std::chrono::system_clock::now();
    #endif
    boundingMesh.LoadFromFile("OBJs/helixRot.obj");
    for (unsigned int i = 0; i < numPoints; ++i) {
        const glm::vec3 p = glm::vec3(dis(rng) * 2.0f, dis(rng) * 2.0f, dis(rng) * 4.0f); // these scales are hard coded for the helix mesh

        // Intersect with mesh
        if (boundingMesh.Contains(p)) {
            minPoint.x = std::min(minPoint.x, p.x);
            minPoint.y = std::min(minPoint.y, p.y);
            minPoint.z = std::min(minPoint.z, p.z);
            maxPoint.x = std::max(maxPoint.x, p.x);
            maxPoint.y = std::max(maxPoint.y, p.y);
            maxPoint.z = std::max(maxPoint.z, p.z);
            points.emplace_back(AttractorPoint(p));
        }
    }
    #ifdef ENABLE_DEBUG_OUTPUT
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << "Elapsed time for Attractor Point Cloud Generation: " << elapsed_seconds.count() << "s\n";
    std::cout << "Number of Attractor Points Generated: " << points.size() << "\n\n";
    #endif
    create();
}

// Generate points 
void AttractorPointCloud::GeneratePoints(unsigned int numPoints, const std::vector<glm::vec3>& sketchPoints, const float brushRadius) {
    #ifdef ENABLE_DEBUG_OUTPUT
    auto start = std::chrono::system_clock::now();
    #endif
    boundingMesh.LoadFromFile("OBJs/helixRot.obj");
    for (unsigned int i = 0; i < numPoints; ++i) {
        const glm::vec3 p = glm::vec3(dis(rng) * 2.0f, dis(rng) * 2.0f, dis(rng) * 4.0f); // these scales are hard coded for the helix mesh

        // Check if the point lies within at least one of the brush locations, assuming spheres for now (TODO: more brush types / cylinders?)
        bool contained = false;

        for (unsigned int sp = 0; sp < (unsigned int)sketchPoints.size(); ++sp) {
            if ((glm::length2(p - sketchPoints[sp]) - brushRadius * brushRadius) < 0) { // if the point is inside the sphere
                contained = true;
                break;
            }
        }

        if (contained) {
            minPoint.x = std::min(minPoint.x, p.x);
            minPoint.y = std::min(minPoint.y, p.y);
            minPoint.z = std::min(minPoint.z, p.z);
            maxPoint.x = std::max(maxPoint.x, p.x);
            maxPoint.y = std::max(maxPoint.y, p.y);
            maxPoint.z = std::max(maxPoint.z, p.z);
            points.emplace_back(AttractorPoint(p));
        }
    }
    #ifdef ENABLE_DEBUG_OUTPUT
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << "Elapsed time for Attractor Point Cloud Generation: " << elapsed_seconds.count() << "s\n";
    std::cout << "Number of Attractor Points Generated: " << points.size() << "\n\n";
    #endif
    create();
}

void AttractorPointCloud::create() {
    // Indices
    genBufIdx();

    // Create an indices vector
    std::vector<unsigned int> indices = std::vector<unsigned int>();
    for (int i = 0; i < points.size(); ++i) {
        indices.emplace_back(i);
    }

    count = (int)indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Positions
    genBufPos();
    glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(), points.data(), GL_STATIC_DRAW);
}
