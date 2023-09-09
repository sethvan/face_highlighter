#include "pickloader.h"
#include "faces.h"
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <stdexcept>

PickLoader::PickLoader( const std::string& stl_file_content )
{
    printf( "PickLoader constructor began!\n" );
    loadToAssimp( stl_file_content );
    printf( "Loaded selected file to assimp!\n" );
    populateTriangleVec();
    printf( "Populated triangles vector!\n" );
    std::cout << "Size of vector: " << triangles.size() << '\n';
    seth_tl::getAdjacentFaces( triangles );
    printf( "Got adjacent faces!\n" );
    model_center = seth_tl::getCenter( triangles );
    scaleFactor = seth_tl::getScaleFactor( triangles );
    selectedIndices.push_back( {} ); // Do not want to use an index 0
    on = true;
    groupId = 0;
    printf( "PickLoader constructor finished!\n" );
}

std::array<float, 3> PickLoader::getModelCenter() const
{
    return model_center;
}

float PickLoader::getScaleFactor() const
{
    return scaleFactor;
}

const MultiDrawer PickLoader::calcCurrentFaces( int vertexId, float tolerance )
{
    auto it = std::find( mesh_arrays.indices.begin(), mesh_arrays.indices.end(), vertexId );
    int primId = 0;

    if ( it != mesh_arrays.indices.end() )
    {
        auto vertexIdFoundAtIndex = std::distance( mesh_arrays.indices.begin(), it );
        primId = vertexIdFoundAtIndex / 3;
    }

    if ( !triangles[ primId ].groupId )
    {
        std::vector<unsigned int> selectedFaces;
        seth_tl::getSelectionFaces( triangles, selectedFaces, primId, ++groupId, tolerance );
        selectedIndices.push_back( std::move( seth_tl::getSelectionIndices( selectedFaces ) ) );
        on = true;
    }
    else if ( !on )
    {
        selectedIndices[ triangles[ primId ].groupId ].enabled = !selectedIndices[ triangles[ primId ].groupId ].enabled;
        on = true;
    }

    MultiDrawer currentFaces;
    std::for_each( std::next( selectedIndices.begin(), 1 ), selectedIndices.end(),
        [ & ]( const auto& md )
        {
            if ( md.enabled )
            {
                currentFaces.startIndices.insert( currentFaces.startIndices.end(), md.startIndices.begin(), md.startIndices.end() );
                currentFaces.counts.insert( currentFaces.counts.end(), md.counts.begin(), md.counts.end() );
            }
        } );

    return currentFaces;
}

val PickLoader::getVertices() const
{
    return val( typed_memory_view( mesh_arrays.vertices.size(), mesh_arrays.vertices.data() ) );
}

val PickLoader::getIndices() const
{
    return val( typed_memory_view( mesh_arrays.indices.size(), mesh_arrays.indices.data() ) );
}

int PickLoader::getNumTriangles() const
{
    return mesh_arrays.indices.size();
}

void PickLoader::loadToAssimp( const std::string& file_content )
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFileFromMemory(
        file_content.data(), file_content.length(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals, "stl" );

    if ( !scene )
    {
        printf( "Model failed to load: %s", importer.GetErrorString() );
        return;
    }

    loadNode( scene->mRootNode, scene );
}

void PickLoader::loadNode( aiNode* node, const aiScene* scene )
{
    for ( size_t i = 0; i < node->mNumMeshes; i++ )
    {
        loadMesh( scene->mMeshes[ node->mMeshes[ i ] ], scene );
    }

    for ( size_t i = 0; i < node->mNumChildren; i++ )
    {
        loadNode( node->mChildren[ i ], scene );
    }
}

void PickLoader::loadMesh( aiMesh* mesh, const aiScene* scene )
{

    for ( size_t i = 0; i < mesh->mNumVertices; i++ )
    {
        mesh_arrays.vertices.insert(
            mesh_arrays.vertices.end(), { mesh->mVertices[ i ].x, mesh->mVertices[ i ].y, mesh->mVertices[ i ].z } );

        if ( mesh->mTextureCoords[ 0 ] )
        {
            mesh_arrays.vertices.insert(
                mesh_arrays.vertices.end(), { mesh->mTextureCoords[ 0 ][ i ].x, mesh->mTextureCoords[ 0 ][ i ].y } );
        }
        else
        {
            mesh_arrays.vertices.insert( mesh_arrays.vertices.end(), { 0.0f, 0.0f } );
        }
        mesh_arrays.vertices.insert(
            mesh_arrays.vertices.end(), { -mesh->mNormals[ i ].x, -mesh->mNormals[ i ].y, -mesh->mNormals[ i ].z } );
    }

    for ( size_t i = 0; i < mesh->mNumFaces; i++ )
    {
        aiFace face = mesh->mFaces[ i ];
        for ( size_t j = 0; j < face.mNumIndices; j++ )
        {
            mesh_arrays.indices.push_back( face.mIndices[ j ] );
        }
    }
}

void PickLoader::clearSelection()
{
    selectedIndices.clear();
    selectedIndices.push_back( {} );
    groupId = 0;
    std::for_each( triangles.begin(), triangles.end(), []( auto& triangle ) { triangle.groupId = 0; } );
}

void PickLoader::populateTriangleVec()
{
    size_t stride = 8;
    for ( size_t i = 0; i < mesh_arrays.indices.size(); i += 3 )
    {
        auto v1Base = mesh_arrays.indices[ i ] * stride;
        auto v1xIndex = v1Base;
        auto v1yIndex = v1Base + 1;
        auto v1zIndex = v1Base + 2;
        auto v2Base = mesh_arrays.indices[ i + 1 ] * stride;
        auto v2xIndex = v2Base;
        auto v2yIndex = v2Base + 1;
        auto v2zIndex = v2Base + 2;
        auto v3Base = mesh_arrays.indices[ i + 2 ] * stride;
        auto v3xIndex = v3Base;
        auto v3yIndex = v3Base + 1;
        auto v3zIndex = v3Base + 2;
        triangles.emplace_back(
            seth_tl::Point( mesh_arrays.vertices[ v1xIndex ], mesh_arrays.vertices[ v1yIndex ], mesh_arrays.vertices[ v1zIndex ] ),
            seth_tl::Point( mesh_arrays.vertices[ v2xIndex ], mesh_arrays.vertices[ v2yIndex ], mesh_arrays.vertices[ v2zIndex ] ),
            seth_tl::Point( mesh_arrays.vertices[ v3xIndex ], mesh_arrays.vertices[ v3yIndex ], mesh_arrays.vertices[ v3zIndex ] ) );
    }
}

using namespace emscripten;

EMSCRIPTEN_BINDINGS( PickLoader )
{
    class_<PickLoader>( "PickLoader" )
        .constructor<const std::string&>()
        .property( "model_center", &PickLoader::getModelCenter )
        .property( "scaleFactor", &PickLoader::getScaleFactor )
        .function( "calcCurrentFaces", &PickLoader::calcCurrentFaces )
        .function( "clearSelection", &PickLoader::clearSelection )
        .property( "on", &PickLoader::on )
        .property( "vertices", &PickLoader::getVertices )
        .property( "indices", &PickLoader::getIndices )
        .property( "numTriangles", &PickLoader::getNumTriangles );
}