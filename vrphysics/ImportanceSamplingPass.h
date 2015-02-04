//
//  ImportanceSamplingPass.h
//  vrphysics
//
//  Created by Robin Wu on 1/24/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#ifndef __vrphysics__ImportanceSamplingPass__
#define __vrphysics__ImportanceSamplingPass__

#include <stdio.h>
#include <osg/Texture2D>

#include "ScreenPass.h"

class ImportanceSamplingPass : public ScreenPass
{
public:
    ImportanceSamplingPass(osg::Camera *mainCamera, osg::TextureRectangle *fluxTex, int rsmWidth, int rsmHeight);
    virtual ~ImportanceSamplingPass();
    
    // overriding
    virtual int addOutTexture();
    
protected:
    virtual void configureStateSet();
    void configRTTCamera();
    void configMipMapCamera();
    
private:
    osg::ref_ptr<osg::Texture2D> createMipMapTexture();
    int _in_flux_tex_id;

    int _out_importance_sample_tex_id;
    
    int _rsmWidth;
    int _rsmHeight;
    
    int _splatsSize;
    
    osg::ref_ptr<osg::Texture2D> _tapTexture;
    osg::ref_ptr<osg::Camera> _mipMapCamera;
    
    osg::ref_ptr<osg::Group> _screenQuad;
};

#endif /* defined(__vrphysics__ImportanceSamplingPass__) */
