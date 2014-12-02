//
//  Material.cpp
//  vrphysics
//
//  Created by Robin Wu on 12/1/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "Material.h"


Material::Material(osg::Node *owner, osg::Texture2D *albedo/* = NULL */ )
: _hasTexture(false), _owner(owner)
{
    if(albedo != NULL)
    {
        _albedoTex = albedo;
        _hasTexture = true;
    }
}
