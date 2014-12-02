//
//  GeometryGroup.h
//  vrphysics
//
//  Created by Robin Wu on 12/1/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__GeometryGroup__
#define __vrphysics__GeometryGroup__

#include <stdio.h>
#include <vector>
#include <map>
#include <osg/Node>
#include "Material.h"
#include "PointLight.h"
#include "DirectionalLight.h"

class AssetDB
{
public:
    AssetDB();
    ~AssetDB();
    void printAllNames();
    std::vector<DirectionalLight *> getDirectionalLights();
    std::vector<PointLight *> getPointLights();
    std::vector<std::pair<osg::Node *, Material *> > getGeometryNodesAndMaterials();
    
    osg::ref_ptr<osg::Group> getGeomRoot();
    void addGeometryWithFile(const std::string &fileURL);
    
private:
    void processNodes(osg::Node *node);
    void processCurrentNode(osg::Node *node);
    
    // string helpers
    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
    std::vector<std::string> split(const std::string &s, char delim);
    
    osg::Node *findNodeWithName(const char *name, osg::Node *node);
    osg::Texture2D *extractTexture2DFromNode(osg::Transform *mt);
    osg::LightSource *extractLight(osg::Transform *mt);
    
    std::map<std::string, std::pair<osg::Node *, Material *> > _geomNodeAndMaterials;
    
    std::map<std::string, PointLight *> _pointLights;
    std::map<std::string, DirectionalLight *> _directionalLights;
    
    osg::ref_ptr<osg::Group> _geomRoot;
};

#endif /* defined(__vrphysics__GeometryGroup__) */
