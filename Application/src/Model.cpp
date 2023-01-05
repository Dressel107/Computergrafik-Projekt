//
//  Model.cpp
//  ogl4
//
//  Created by Philipp Lensing on 21.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Model.h"
#include "phongshader.h"
#include <list>
#include <sstream>

Model::Model() : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0)
{
    
}
Model::Model(const char* ModelFile, bool FitSize) : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0)
{
    bool ret = load(ModelFile, FitSize);
    if(!ret)
        throw std::exception();
}
Model::~Model()
{
    delete[] pMaterials;
    delete[] pMeshes;
    deleteNodes(&RootNode);
}

void Model::deleteNodes(Node* pNode)
{
    if(!pNode)
        return;
    for(unsigned int i=0; i<pNode->ChildCount; ++i)
        deleteNodes(&(pNode->Children[i]));
    if(pNode->ChildCount>0)
        delete [] pNode->Children;
    if(pNode->MeshCount>0)
        delete [] pNode->Meshes;
}

bool Model::load(const char* ModelFile, bool FitSize)
{
    const aiScene* pScene = aiImportFile( ModelFile,aiProcessPreset_TargetRealtime_Fast | aiProcess_TransformUVCoords );
    
    if(pScene==NULL || pScene->mNumMeshes<=0)
        return false;
    
    Filepath = ModelFile;
    Path = Filepath;
    size_t pos = Filepath.rfind('/');
    if(pos == std::string::npos)
        pos = Filepath.rfind('\\');
    if(pos !=std::string::npos)
        Path.resize(pos+1);
    
    loadMeshes(pScene, FitSize);
    loadMaterials(pScene);
    loadNodes(pScene);
    
    return true;
}

void Model::loadMeshes(const aiScene* pScene, bool FitSize)
{
    // Boundingbox für Modell berechnen -> calcBoundingBox()
    calcBoundingBox(pScene, this->BoundingBox);

    const float FIT_SIZE_SCALING = 4;
    float scaling = 1;
    float bbDiaLength = (this->BoundingBox.Max + this->BoundingBox.Min).length();

    // Mesh auf feste Größe skalieren
    if (FitSize)
    {
        scaling = FIT_SIZE_SCALING / bbDiaLength;
    }

    // Meshes aus aiScene in Model::pMeshes und MeshCount laden
    this->MeshCount = pScene->mNumMeshes;
    this->pMeshes = new Mesh[this->MeshCount];

    //Für jedes Mesh in pScene ein  VB und IB erstellen
    for (int meshIdx = 0; meshIdx < this->MeshCount; meshIdx++) 
    {
        Mesh* currentMesh = &pMeshes[meshIdx];
        aiMesh* currentAiMesh = pScene->mMeshes[meshIdx];
        currentMesh->MaterialIdx = currentAiMesh->mMaterialIndex;

        currentMesh->VB.begin();
        for (int verticesIdx = 0; verticesIdx < currentAiMesh->mNumVertices; verticesIdx++) 
        {
            //Normale hinzufügen
            currentMesh->VB.addNormal(Vector(currentAiMesh->mNormals[verticesIdx].x, currentAiMesh->mNormals[verticesIdx].y, currentAiMesh->mNormals[verticesIdx].z));
            
            //Texturcoord hinzügen
            unsigned int numOfUVChannels = currentAiMesh->GetNumUVChannels();
            switch (numOfUVChannels)
            {
                case 1:
                    currentMesh->VB.addTexcoord0(currentAiMesh->mTextureCoords[0][verticesIdx].x, -currentAiMesh->mTextureCoords[0][verticesIdx].y);
                    break;
                case 2:
                    currentMesh->VB.addTexcoord1(currentAiMesh->mTextureCoords[1][verticesIdx].x, -currentAiMesh->mTextureCoords[1][verticesIdx].y);
                case 3:
                    currentMesh->VB.addTexcoord2(currentAiMesh->mTextureCoords[2][verticesIdx].x, -currentAiMesh->mTextureCoords[2][verticesIdx].y);
                case 4:
                    currentMesh->VB.addTexcoord3(currentAiMesh->mTextureCoords[3][verticesIdx].x, -currentAiMesh->mTextureCoords[3][verticesIdx].y);
                default:
                    break;
            }

            // Position hinzufügen
            currentMesh->VB.addVertex(Vector(currentAiMesh->mVertices[verticesIdx].x, currentAiMesh->mVertices[verticesIdx].y, currentAiMesh->mVertices[verticesIdx].z) * 1);
        }

        currentMesh->VB.end();
        currentMesh->IB.begin();
        for (int i = 0; i < currentAiMesh->mNumFaces; i++)
        {
            for (int j = 0; j < currentAiMesh->mFaces[i].mNumIndices; j++)
            {
                currentMesh->IB.addIndex(currentAiMesh->mFaces[i].mIndices[j]);
            }
        }
        currentMesh->IB.end();
    } 
}

// Doku/Web: Assimp - Material System
void Model::loadMaterials(const aiScene* pScene)
{
    MaterialCount = pScene->mNumMaterials;
    pMaterials = new Material[MaterialCount];

    // Alle Materialien durchlaufen
    for (int i = 0; i < MaterialCount; i++)
    {
        aiMaterial* aiMaterial = pScene->mMaterials[i];
        Material* material = &pMaterials[i];

        // AmbColor
        aiColor3D ambColor(0, 0, 0);
        aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambColor);

        // DiffColor
        aiColor3D diffColor(0, 0, 0);
        aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffColor);

        // SpecColor
        aiColor3D specColor(0, 0, 0);
        aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specColor);

        // SpecExp
        float specExp = 0.0f;
        aiMaterial->Get(AI_MATKEY_SHININESS, specExp);

        // DiffTex
        aiString texture;
        aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture);
        
        // Werte zuweisen
        material->AmbColor = Color(ambColor.r, ambColor.g, ambColor.b);
        material->DiffColor = Color(diffColor.r, diffColor.g, diffColor.b);
        material->SpecColor = Color(specColor.r, specColor.g, specColor.b);
        material->SpecExp = specExp;

        std::stringstream ss;
        ss << Path << texture.data;
        material->DiffTex = Texture::LoadShared(ss.str().c_str());
    }
}
/*
* https://en.wikibooks.org/wiki/OpenGL_Programming/Bounding_box
*/
void Model::calcBoundingBox(const aiScene* pScene, AABB& Box)
{
    // Defaultwerte setzen
    Box.Min.X = std::numeric_limits<float>::max();
    Box.Min.Y = std::numeric_limits<float>::max();
    Box.Min.Z = std::numeric_limits<float>::max();
    Box.Max.Z = std::numeric_limits<float>::min();
    Box.Max.Z = std::numeric_limits<float>::min();
    Box.Max.Z = std::numeric_limits<float>::min();

    // Den kleinsten und größten Punkt suchen
    for (int i = 0; i < pScene->mNumMeshes; i++) 
    {
        for (int j = 0; j < pScene->mMeshes[i]->mNumVertices; j++) 
        {
            if (pScene->mMeshes[i]->mVertices[j].x < Box.Min.X)
            {
                Box.Min.X = pScene->mMeshes[i]->mVertices[j].x;
            }
            if (pScene->mMeshes[i]->mVertices[j].y < Box.Min.Y)
            {
                Box.Min.Y = pScene->mMeshes[i]->mVertices[j].y;
            }
            if (pScene->mMeshes[i]->mVertices[j].z < Box.Min.Z) 
            {
                Box.Min.Z = pScene->mMeshes[i]->mVertices[j].z;
            }
            if (pScene->mMeshes[i]->mVertices[j].x > Box.Max.X) 
            {
                Box.Max.X = pScene->mMeshes[i]->mVertices[j].x;
            }
            if (pScene->mMeshes[i]->mVertices[j].y > Box.Max.Y) 
            {
                Box.Max.Y = pScene->mMeshes[i]->mVertices[j].y;
            }
            if (pScene->mMeshes[i]->mVertices[j].z > Box.Max.Z) {
                Box.Max.Z = pScene->mMeshes[i]->mVertices[j].z;
            }
        }
    }
}

void Model::loadNodes(const aiScene* pScene)
{
    deleteNodes(&RootNode);
    copyNodesRecursive(pScene->mRootNode, &RootNode);
}

void Model::copyNodesRecursive(const aiNode* paiNode, Node* pNode)
{
    pNode->Name = paiNode->mName.C_Str();
    pNode->Trans = convert(paiNode->mTransformation);
    
    if(paiNode->mNumMeshes > 0)
    {
        pNode->MeshCount = paiNode->mNumMeshes;
        pNode->Meshes = new int[pNode->MeshCount];
        for(unsigned int i=0; i<pNode->MeshCount; ++i)
            pNode->Meshes[i] = (int)paiNode->mMeshes[i];
    }
    
    if(paiNode->mNumChildren <=0)
        return;
    
    pNode->ChildCount = paiNode->mNumChildren;
    pNode->Children = new Node[pNode->ChildCount];
    for(unsigned int i=0; i<paiNode->mNumChildren; ++i)
    {
        copyNodesRecursive(paiNode->mChildren[i], &(pNode->Children[i]));
        pNode->Children[i].Parent = pNode;
    }
}

void Model::applyMaterial( unsigned int index)
{
    if(index>=MaterialCount)
        return;
    
    PhongShader* pPhong = dynamic_cast<PhongShader*>(shader());
    if(!pPhong) {
        std::cout << "Model::applyMaterial(): WARNING Invalid shader-type. Please apply PhongShader for rendering models.\n";
        return;
    }
    
    Material* pMat = &pMaterials[index];
    pPhong->ambientColor(pMat->AmbColor);
    pPhong->diffuseColor(pMat->DiffColor);
    pPhong->specularExp(pMat->SpecExp);
    pPhong->specularColor(pMat->SpecColor);
    pPhong->diffuseTexture(pMat->DiffTex);
}

void Model::draw(const BaseCamera& Cam)
{
    if(!pShader) {
        std::cout << "BaseModel::draw() no shader found" << std::endl;
        return;
    }
    pShader->modelTransform(transform());
    
    std::list<Node*> DrawNodes;
    DrawNodes.push_back(&RootNode);
    
    while(!DrawNodes.empty())
    {
        Node* pNode = DrawNodes.front();
        Matrix GlobalTransform;
        
        if(pNode->Parent != NULL)
            pNode->GlobalTrans = pNode->Parent->GlobalTrans * pNode->Trans;
        else
            pNode->GlobalTrans = transform() * pNode->Trans;
        
        pShader->modelTransform(pNode->GlobalTrans);
    
        for(unsigned int i = 0; i<pNode->MeshCount; ++i )
        {
            Mesh& mesh = pMeshes[pNode->Meshes[i]];
            mesh.VB.activate();
            mesh.IB.activate();
            applyMaterial(mesh.MaterialIdx);
            pShader->activate(Cam);
            glDrawElements(GL_TRIANGLES, mesh.IB.indexCount(), mesh.IB.indexFormat(), 0);
            mesh.IB.deactivate();
            mesh.VB.deactivate();
        }
        for(unsigned int i = 0; i<pNode->ChildCount; ++i )
            DrawNodes.push_back(&(pNode->Children[i]));
        
        DrawNodes.pop_front();
    }
}

Matrix Model::convert(const aiMatrix4x4& m)
{
    return Matrix(m.a1, m.a2, m.a3, m.a4,
                  m.b1, m.b2, m.b3, m.b4,
                  m.c1, m.c2, m.c3, m.c4,
                  m.d1, m.d2, m.d3, m.d4);
}



