//
//  Material.h
//  vrphysics
//
//  Created by Robin Wu on 12/1/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__Material__
#define __vrphysics__Material__

#include <stdio.h>
#include <osg/Texture2D>
#include <osg/Node>

class Material
{
public:
    Material(osg::Node *owner, osg::Texture2D *albedo = NULL);
    ~Material() {}
    
    inline bool hasTexture()
    {
        return _hasTexture;
    }
    
    inline osg::ref_ptr<osg::Node> getOwner()
    {
        return _owner;
    }
    
    inline osg::ref_ptr<osg::Texture2D> getAlbedoTexture()
    {
        if(!_hasTexture)
        {
            return NULL;
        }
        else
        {
            return _albedoTex;
        }
    }
    
private:
    osg::ref_ptr<osg::Node> _owner;
    bool _hasTexture;
    osg::ref_ptr<osg::Texture2D> _albedoTex;
    
    // TODO: in future add bump mapping, normal mapping etc.
};

#endif /* defined(__vrphysics__Material__) */
