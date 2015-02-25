//
//  BlurPass.h
//  vrphysics
//
//  Created by Robin Wu on 2/24/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#ifndef __vrphysics__BlurPass__
#define __vrphysics__BlurPass__

#include <stdio.h>
#include "ScreenPass.h"

class BlurPass : public ScreenPass
{
public:
    BlurPass(osg::Camera *mainCamera);
    virtual ~BlurPass();
    
protected:
    void configureStateSet();
};

#endif /* defined(__vrphysics__BlurPass__) */
