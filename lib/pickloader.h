#ifndef INCLUDED_PICKLOADER_H_
#define INCLUDED_PICKLOADER_H_

#include "faces.h"
#include "multidrawer.h"
#include "stl_io.h"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>

struct MeshArrays
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

class PickLoader
{
private:
    seth_tl::stl_data info;
    std::vector<MultiDrawer> selectedIndices;
    seth_tl::Point model_center;
    float scaleFactor;
    std::vector<MeshArrays> mesh_arrays;
    void loadToAssimp( const std::string& stl_file_content );
    void loadNode( aiNode* node, const aiScene* scene );
    void loadMesh( aiMesh* mesh, const aiScene* scene );

public:
    PickLoader( const std::string& stl_file_content );
    PickLoader() = delete;
    PickLoader( const PickLoader& rhs ) = delete;
    PickLoader& operator=( const PickLoader& rhs ) = delete;
    PickLoader( const PickLoader&& rhs ) = delete;
    PickLoader& operator=( const PickLoader&& rhs ) = delete;
    ~PickLoader() = default;

    seth_tl::Point getModelCenter() const;
    float getScaleFactor() const;
    const MultiDrawer calcCurrentFaces( int vertexId, float tolerance );
    bool on;
    unsigned int groupId;
    val getVertices() const;
    val getIndices() const;
    int getNumTriangles() const;
    void clearSelection();
};

#endif // INCLUDED_PICKLOADER_H_