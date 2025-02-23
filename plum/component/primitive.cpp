#include <plum/component/primitive.hpp>

namespace Component {

    Primitive::Primitive() : Mesh(SceneObjectType::Primitive) {}

    void Primitive::DrawQuad() {
        static std::unique_ptr<Vao> vao;
        if (!vao) {
            const std::vector<float> quadVertices = {    // Remember CCW
                // Positions    // UV
                -1.0f, -1.0f,   0.0f, 0.0f,
                 1.0f, -1.0f,   1.0f, 0.0f,
                -1.0f,  1.0f,   0.0f, 1.0f,

                 1.0f,  1.0f,   1.0f, 1.0f,
                -1.0f,  1.0f,   0.0f, 1.0f,
                 1.0f, -1.0f,   1.0f, 0.0f
            };
            const VertexArray va(quadVertices, VertexAttrFlags::Position2 | VertexAttrFlags::Uv);
            const std::shared_ptr<Vbo> vbo = std::make_shared<Vbo>(va);
            vao = std::make_unique<Vao>(vbo);
        }
        vao->Draw();
    }

    Cube::Cube(const int nrows, const int ncols) : nRows(nrows), nCols(ncols) {
        const std::vector<float> cubedata = {
            // Positions           // Normals           // UV                    // Tangents
            -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,        0.0f,       0.0f,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f*ncols, 1.0f*nrows,  1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f*ncols,       0.0f,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f*ncols, 1.0f*nrows,  1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,        0.0f,       0.0f,  1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,        0.0f, 1.0f*nrows,  1.0f, 0.0f, 0.0f,
        
            -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,         0.0f,       0.0f,  1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   1.0f*ncols,       0.0f,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   1.0f*ncols, 1.0f*nrows,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   1.0f*ncols, 1.0f*nrows,  1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,         0.0f, 1.0f*nrows,  1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,         0.0f,       0.0f,  1.0f, 0.0f, 0.0f,
        
            -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f*ncols,       0.0f,  0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f*ncols, 1.0f*nrows,  0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,        0.0f, 1.0f*nrows,  0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,        0.0f, 1.0f*nrows,  0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,        0.0f,       0.0f,  0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f*ncols,       0.0f,  0.0f, 1.0f, 0.0f,
        
             0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f*ncols,       0.0f,  0.0f, 1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,        0.0f, 1.0f*nrows,  0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  1.0f*ncols, 1.0f*nrows,  0.0f, 1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,        0.0f, 1.0f*nrows,  0.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f*ncols,       0.0f,  0.0f, 1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,        0.0f,       0.0f,  0.0f, 1.0f, 0.0f,
        
            -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,        0.0f, 1.0f*nrows,  1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  1.0f*ncols, 1.0f*nrows,  1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  1.0f*ncols,       0.0f,  1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  1.0f*ncols,       0.0f,  1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,        0.0f,       0.0f,  1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,        0.0f, 1.0f*nrows,  1.0f, 0.0f, 0.0f,
        
            -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,        0.0f, 1.0f*nrows,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  1.0f*ncols,       0.0f,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  1.0f*ncols, 1.0f*nrows,  1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,        0.0f,       0.0f,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  1.0f*ncols,       0.0f,  1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,        0.0f, 1.0f*nrows,  1.0f, 0.0f, 0.0f
        };

        VertexArray va(cubedata, VertexAttrFlags::All ^ VertexAttrFlags::Bitangent);
        auto vbo = std::make_shared<Vbo>(va);
        vao = std::make_shared<Vao>(vbo);
    }
    
    Sphere::Sphere(const int nstacks, const int nsectors) : nStacks(nstacks), nSectors(nsectors) {
        VertexArray::UncollatedVertices verts;
        std::vector<unsigned int> indices;
        
        // Generate vertices
        for (int i = 0; i <= nstacks; i++) {
            
            float stack_angle = glm::pi<float>() * (-0.5 + float(i) / nstacks);    // bottom up
            float y = glm::sin(stack_angle);
            std::vector<glm::vec3> stack_positions;
            
            for (int j = 0; j <= nsectors; j++) {
                
                // Positions, normals, texture coords
                float sector_angle = glm::two_pi<float>() * (float(j) / nsectors); // clockwise when looking in -y direction
                float x = glm::cos(stack_angle) * glm::cos(sector_angle);
                float z = glm::cos(stack_angle) * glm::sin(sector_angle);
                verts.positions.insert(verts.positions.end(), {x,y,z});
                verts.normals.insert(verts.normals.end(), {x,y,z});
                verts.uvs.insert(verts.uvs.end(), {float(j)/nsectors, float(i)/nstacks});

                stack_positions.push_back(glm::vec3(x,y,z));
                
                // Tangents, bitangents for top and bottom edge cases
                if (i == 0 || i == nstacks) {
                    float fake_stack_angle = stack_angle + 0.01;
                    float fake_x = glm::cos(fake_stack_angle) * glm::cos(fake_stack_angle);
                    float fake_z = glm::cos(fake_stack_angle) * glm::sin(fake_stack_angle);
                    glm::vec3 bitangent = glm::normalize( glm::vec3(fake_x,0,fake_z) );
                    verts.bitangents.push_back(bitangent.x);
                    verts.bitangents.push_back(bitangent.y);
                    verts.bitangents.push_back(bitangent.z);
                    glm::vec3 tangent;
                    if (i == 0) {
                        tangent = glm::normalize( glm::cross(glm::vec3(0,1,0), bitangent) );
                        verts.tangents.push_back(tangent.x);
                        verts.tangents.push_back(tangent.y);
                        verts.tangents.push_back(tangent.z);
                    } else if (i == nstacks) {
                        tangent = glm::normalize( glm::cross(glm::vec3(0,-1,0), bitangent) );
                    } 
                    verts.tangents.push_back(tangent.x);
                    verts.tangents.push_back(tangent.y);
                    verts.tangents.push_back(tangent.z);
                }

            }
            
            // Work backwards to compute non-edgecase tangents and bitangents
            glm::vec3 lastpos = stack_positions.front();
            for (auto currpos = stack_positions.rbegin(); currpos != stack_positions.rend(); currpos++) {
                if (i == 0 || i == nstacks) {
                    continue;
                } else {
                    glm::vec3 bitangent = glm::normalize( glm::cross(*currpos, lastpos) );
                    verts.bitangents.push_back(bitangent.x);
                    verts.bitangents.push_back(bitangent.y);
                    verts.bitangents.push_back(bitangent.z);
                    glm::vec3 tangent = glm::normalize( glm::cross(bitangent, *currpos) );
                    verts.tangents.push_back(tangent.x);
                    verts.tangents.push_back(tangent.y);
                    verts.tangents.push_back(tangent.z);
                    lastpos = *currpos;
                }
            }
        }
        
        // Generate indices
        for (int i = 0; i < nstacks; i++) {
            int ind1 = i * (nsectors + 1);        // first index in current stack
            int ind2 = (i + 1) * (nsectors + 1);  // first index in stack immediately above
            for (int j = 0; j < nsectors; j++, ind1++, ind2++) {
                // Triangle 1 (CCW)
                if (i != nsectors - 1) {
                    indices.push_back(ind1);
                    indices.push_back(ind2);
                    indices.push_back(ind2 + 1);
                }
                // Triangle 2 (CCW)
                if (i != 0) {
                    indices.push_back(ind1);
                    indices.push_back(ind2 + 1);
                    indices.push_back(ind1 + 1);
                }
            }
        }
        
        auto vbo = std::make_shared<Component::Vbo>(verts);
        auto ebo = std::make_shared<Component::Ebo>(indices);
        vao = std::make_shared<Component::Vao>(vbo, ebo);
    }
    
    Rectangle::Rectangle(const int nrows, const int ncols) : nRows(nrows), nCols(ncols) {
        const std::vector<float> rectdata = {    // Remember CCW (x-z vs x-y)
            // Positions         // Normals         // UV                        // Tangents
            -1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,          0.0f,         0.0f,  1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,          0.0f, 1.0f * nrows,  1.0f, 0.0f, 0.0f,
             1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f * ncols,         0.0f,  1.0f, 0.0f, 0.0f,
             1.0f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f * ncols, 1.0f * nrows,  1.0f, 0.0f, 0.0f,
             1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f * ncols,         0.0f,  1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,          0.0f, 1.0f * nrows,  1.0f, 0.0f, 0.0f
        };

        VertexArray va(rectdata, VertexAttrFlags::All ^ VertexAttrFlags::Bitangent);
        auto vbo = std::make_shared<Vbo>(va);
        vao = std::make_shared<Vao>(vbo);
    }

}