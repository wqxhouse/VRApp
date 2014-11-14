//
//  SSAOPass.h
//  vrphysics
//
//  Created by Robin Wu on 11/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__SSAOPass__
#define __vrphysics__SSAOPass__

#include <stdio.h>
#include "ScreenPass.h"

class GeometryPass : public ScreenPass
{
public:
    virtual void setShader(std::string filename);
    
protected:
    virtual osg::ref_ptr<osg::Group> createTexturedQuad();
    virtual void createOutputTextures();
    virtual void setupCamera();
};

#endif /* defined(__vrphysics__SSAOPass__) */
