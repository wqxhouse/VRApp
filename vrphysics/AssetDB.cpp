//
//  GeometryGroup.cpp
//  vrphysics
//
//  Created by Robin Wu on 12/1/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "AssetDB.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <osg/LightSource>

AssetDB::AssetDB()
{
    _geomRoot = new osg::Group;
}

AssetDB::~AssetDB()
{
    for(std::map<std::string, std::pair<osg::Node *, Material *> >::iterator it = _geomNodeAndMaterials.begin();
        it != _geomNodeAndMaterials.end(); it++)
    {
        std::pair<osg::Node *, Material *> pair = it->second;
        Material *mat = pair.second;
        delete mat;
    }
}

void AssetDB::addGeometryWithFile(const std::string &fileURL)
{
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(fileURL);
    if(loadedModel == NULL)
    {
        fprintf(stderr, "load model %s failed", fileURL.c_str());
        return;
    }
    if(loadedModel->getName() == "")
    {
        loadedModel->setName("loaded: " + fileURL);
    }
    
    // transform to center
    osg::Vec3 loadedModelCenter = loadedModel->getBound().center();
    osg::ref_ptr<osg::PositionAttitudeTransform> modelToCenter(new osg::PositionAttitudeTransform);
    modelToCenter->setPosition(-loadedModelCenter);
    modelToCenter->addChild(loadedModel);
    modelToCenter->setName("loaded: " + fileURL + " toCenter");
    
    // link to geom root
    _geomRoot->addChild(modelToCenter);
    processNodes(modelToCenter);
}

// string helpers
// code of Evan Teran from http://stackoverflow.com/questions/236129/split-a-string-in-c
std::vector<std::string> & AssetDB::split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> AssetDB::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

osg::ref_ptr<osg::Texture2D> AssetDB::extractTexture2DFromNode(osg::Transform *mt)
{
    osg::Geometry *geom = (osg::Geometry *)mt->getChild(0)->asGeode()->getDrawable(0)->asGeometry();
    osg::StateSet *ss = geom->getOrCreateStateSet();
    osg::StateSet::TextureAttributeList *list;
    list = &ss->getTextureAttributeList();
    if(list->size() == 0)
    {
        return NULL;
    }
    osg::Texture2D *tex = NULL;
    for(int i = 0; i < list->size(); i++)
    {
        tex = dynamic_cast<osg::Texture2D *>(ss->getTextureAttribute(i, osg::StateAttribute::TEXTURE));
        if(tex != NULL) // get the first tex. Currently mutli-tex not supported
        {
            return tex;
        }
    }
    return NULL;
}

osg::LightSource *AssetDB::extractLight(osg::Transform *mt)
{
    osg::LightSource *ls = dynamic_cast<osg::LightSource *>(mt->getChild(0));
    if(ls) return ls;
    else return NULL;
}

void AssetDB::processCurrentNode(osg::Node *node)
{
    std::string name = node->getName();
    printf("%s\n", name.c_str());
    std::vector<std::string> splitArr = split(name, '_');
    if(splitArr.size() <= 1)
    {
        return;
    }
    const std::string &objRealName = splitArr[splitArr.size() - 1];
    const std::string &objType = splitArr[0];
    const std::string &subType = splitArr[1];
    
    osg::Transform *t = node->asTransform();
    
    // TODO: later extract position and other information for physics
    if(objType == "g")
    {
        Material *material = NULL;
        
        if(subType == "t")
        {
            if(!t)
            {
                fprintf(stderr, "%s node is not a transform node\n", node->getName().c_str());
                return;
            }
            osg::Texture2D *tex = extractTexture2DFromNode(t);
            material = new Material(node, tex);
        }
        else if(subType == "nt")
        {
            material = new Material(node);
        }
        if(_geomNodeAndMaterials.find(objRealName) != _geomNodeAndMaterials.end())
        {
            // copy node so that it default node settings are not modified by other modules
            _geomNodeAndMaterials.insert(std::make_pair(objRealName, std::make_pair(node, material)));
        }
        else
        {
            std::string newName = objRealName + "_new";
            
            // copy node so that it default node settings are not modified by other modules
            _geomNodeAndMaterials.insert(std::make_pair(objRealName, std::make_pair(node, material)));
        }
    }
    else if(objType == "l")
    {
        if(!t)
        {
            fprintf(stderr, "%s node is not a transform node\n", node->getName().c_str());
            return;
        }
        osg::LightSource *ls = extractLight(t);
        if(ls == NULL)
        {
            return;
        }
        osg::Light *light = ls->getLight();
        osg::MatrixTransform *mt = t->asMatrixTransform();
        osg::Vec3 pos = mt->getMatrix().getTrans();
        osg::Vec4f diffuse = light->getDiffuse();
        osg::Vec4f ambient = light->getAmbient();
        osg::Vec4f specular = light->getSpecular();
        
        if(subType == "p")
        {
            PointLight *pt = new PointLight();
            pt->setPosition(osg::Vec3(pos.x(), pos.y(), pos.z()));
            pt->setAmbient(ambient.x(), ambient.y(), ambient.z());
            pt->setDiffuse(diffuse.x(), diffuse.y(), diffuse.z());
            pt->setSpecular(specular.x(), specular.y(), specular.z());
            pt->setAttenuation(light->getConstantAttenuation(), light->getLinearAttenuation(), light->getQuadraticAttenuation());
            pt->setIntensity(1.0f);
            
            if(_pointLights.find(objRealName) != _pointLights.end())
            {
                _pointLights.insert(std::make_pair(objRealName, pt));
            }
            else
            {
                std::string newName = objRealName + "_new";
                _pointLights.insert(std::make_pair(newName, pt));
            }
        }
        else if(subType == "d")
        {
            DirectionalLight *dl = new DirectionalLight();
            dl->setPosition(osg::Vec3(pos.x(), pos.y(), pos.z()));
            osg::Vec3 lookAt = osg::Vec3(0, 0, -1) * mt->getMatrix();
            dl->setLookAt(lookAt);
            dl->setAmbient(ambient.x(), ambient.y(), ambient.z());
            dl->setDiffuse(diffuse.x(), diffuse.y(), diffuse.z());
            dl->setSpecular(specular.x(), specular.y(), specular.z());
            dl->genGeomTransform(0.25);
            dl->orbitAxis = osg::Vec3(0, 0, 1);
            
            if(_directionalLights.find(objRealName) != _directionalLights.end())
            {
                _directionalLights.insert(std::make_pair(objRealName, dl));
            }
            else
            {
                std::string newName = objRealName + "_new";
                _directionalLights.insert(std::make_pair(objRealName, dl));
            }
        }
        else if(subType == "s")
        {
            
        }
    }
}

// TODO: add camera
void AssetDB::processNodes(osg::Node *node)
{
    // DFS
    osg::Group* currGroup;
    osg::Node *currNode = node;
    
    // check to see if we have a valid (non-NULL) node.
    // if we do have a null node, return NULL.
    if (!currNode)
    {
        return;
    }
    
    currGroup = currNode->asGroup(); // returns NULL if not a group.
    if ( currGroup )
    {
        processCurrentNode(currNode);
        
        // traverse
        for (unsigned int i = 0 ; i < currGroup->getNumChildren(); i ++)
        {
            processNodes(currGroup->getChild(i));
        }
        return; // We have checked each child node - no match found.
    }
    else
    {
        return; // leaf node, no match
    }
}

std::vector<DirectionalLight *> AssetDB::getDirectionalLights()
{
    std::vector<DirectionalLight *> dirLights(_directionalLights.size(), NULL);
    int i = 0;
    for(std::map<std::string, DirectionalLight *>::iterator it = _directionalLights.begin();
        it != _directionalLights.end(); it++)
    {
        DirectionalLight *light = it->second;
        dirLights[i++] = light;
    }
    return dirLights;
}

std::vector<PointLight *> AssetDB::getPointLights()
{
    std::vector<PointLight *> pointLights(_pointLights.size(), NULL);
    int i = 0;
    for(std::map<std::string, PointLight *>::iterator it = _pointLights.begin();
        it != _pointLights.end(); it++)
    {
        PointLight *light = it->second;
        pointLights[i++] = light;
    }
    return pointLights;
}

std::vector<std::pair<osg::Node *, Material *> > AssetDB::getGeometryNodesAndMaterials()
{
    std::vector<std::pair<osg::Node *, Material *> > arr(_geomNodeAndMaterials.size());
    int i = 0;
    for(std::map<std::string, std::pair<osg::Node *, Material *> >::iterator it = _geomNodeAndMaterials.begin();
        it != _geomNodeAndMaterials.end(); it++)
    {
        std::pair<osg::Node *, Material *> pair = it->second;
        arr[i++] = pair;
    }
    return arr;
}

void AssetDB::printAllNames()
{
    printf("Geometries: \n");
    for(std::map<std::string, std::pair<osg::Node *, Material *> >::iterator it = _geomNodeAndMaterials.begin();
        it != _geomNodeAndMaterials.end(); it++)
    {
        printf("%s\n", it->first.c_str());
    }
    
    printf("PointLights: \n");
    for(std::map<std::string, PointLight *>::iterator it = _pointLights.begin();
        it != _pointLights.end(); it++)
    {
        printf("%s\n", it->first.c_str());
    }
    
    printf("DirectionalLights: \n");
    for(std::map<std::string, DirectionalLight *>::iterator it = _directionalLights.begin();
        it != _directionalLights.end(); it++)
    {
        printf("%s\n", it->first.c_str());
    }
}

osg::ref_ptr<osg::Group> AssetDB::getGeomRoot()
{
    return _geomRoot;
}

// Assets adapter
Assets::Assets(AssetDB *assetDB)
: _assetDB(assetDB)
{
}

void Assets::addGeometryWithFile(const std::string &fileURL)
{
    _assetDB->addGeometryWithFile(fileURL);
}


