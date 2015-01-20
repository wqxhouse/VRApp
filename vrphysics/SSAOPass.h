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
#include <osg/Camera>
#include <osg/TextureRectangle>

class SSAOPass: public ScreenPass
{
public:
    SSAOPass(osg::Camera *mainCamera, osg::TextureRectangle *positionTex, osg::TextureRectangle *normalTex, osg::Texture2D *randomJitterTex);
    virtual ~SSAOPass();
    
    inline osg::TextureRectangle *getSSAOOutTexture()
    {
        return getOutputTexture(_out_ssao_tex_id);
    }
    
    // override
    virtual int addOutTexture();
    inline float getOcculderBias()
    {
        return _occulderBias;
    }
    
    inline float getSamplingRadius()
    {
        return _samplingRadius;
    }
    
    inline float getConstantAttenuation()
    {
        return _constantAttenuation;
    }
    
    inline float getLinearAttenuation()
    {
        return _linearAttenuation;
    }
    
protected:
    // overriden
    virtual void setupCamera();
    void configRTTCamera();
    virtual void configureStateSet();
    
private:
    void setSSAOParameters(float occluderBias, float samplingRadius, float constantAttenuation, float linearAttenuation);
    int _in_position_tex;
    int _in_normal_tex;
    //int _in_random_tex;
    int _out_ssao_tex_id;
    int _ssao_shader_id;
    
    // ssao params
    float _occulderBias;
    float _samplingRadius;
    float _constantAttenuation;
    float _linearAttenuation;
    
    float _farPlaneDist;
    osg::ref_ptr<osg::Group> _screenQuad;
    osg::ref_ptr<osg::Texture2D> _randomTexture2D;
};

class SSAOStateCallback : public osg::StateSet::Callback
{
public:
    SSAOStateCallback(osg::Camera *mainCamera, SSAOPass *ssaoPass)
    {
        _mainCamera = mainCamera;
        _ssaoPass = ssaoPass;
    }
    
    ~SSAOStateCallback() {}
    virtual void operator()(osg::StateSet *ss, osg::NodeVisitor *nv);
    
private:
    osg::ref_ptr<osg::Camera> _mainCamera;
    SSAOPass *_ssaoPass;
};

#endif /* defined(__vrphysics__SSAOPass__) */
