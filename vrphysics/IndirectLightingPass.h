//
//  IndirectLightingPass.h
//  vrphysics
//
//  Created by Robin Wu on 2/6/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#ifndef __vrphysics__IndirectLightingPass__
#define __vrphysics__IndirectLightingPass__

#include <stdio.h>
#include <map>
#include "ScreenPass.h"

class DirectionalLight;
class IndirectLightingCallback : public osg::StateSet::Callback
{
public:
    IndirectLightingCallback(osg::Camera *mainCamera, DirectionalLight *mainLight);
    virtual ~IndirectLightingCallback() {};
    virtual void operator()(osg::StateSet* ss, osg::NodeVisitor* nv);
    
private:
    osg::ref_ptr<osg::Camera> _mainCamera;
    DirectionalLight *_mainLight;
    osg::Vec3 getCameraPosition();
};

class VPLGroup;
class ImportanceSamplingPass;
class IndirectLightingPass : public ScreenPass
{
public:
    IndirectLightingPass(osg::Camera *mainCamera, ImportanceSamplingPass *impPass, osg::TextureRectangle *lightDirTex, osg::TextureRectangle *worldPosTex, osg::TextureRectangle *viewPositionTex, osg::TextureRectangle *viewNormalTex, osg::Texture2D *depthBufferTex, DirectionalLight *mainLight, osg::Texture2D *sharedDepthBuffer);
    virtual ~IndirectLightingPass();

    inline osg::ref_ptr<osg::TextureRectangle> getIndirectLightingTex()
    {
        return getOutputTexture(_out_indirectLightingTex_id);
    }
    
    // overriding
    virtual int addOutTexture();
    void createSampleTexcoordMap();
    
protected:
    virtual void configureStateSet();
    
    // override
    virtual void setupCamera();
    
private:
    void loadRegularSampleTexture();
    osg::Vec3 getCameraPosition();
    
    int _in_worldPosTex_id; // light view world pos
    int _in_lightDirTex_id;
    
    int _in_viewPositionTex_id;
    int _in_viewNormalTex_id;
    
    osg::ref_ptr<osg::Texture2D> _sampleTex;
    osg::ref_ptr<osg::Texture2D> _impSampleTex; // TODO: support multiple local lights later
    
    osg::ref_ptr<osg::TextureRectangle> _sampleTexcoordMap;
    
    //std::map<int, osg::ref_ptr<osg::Texture2D> > _importanceSampleTextures; // for each light source
    
    int _out_indirectLightingTex_id;
    
    int _indirectLightShader;
    
    int _nSplats;
    int _splats;
    
    int _splatSampleNum;
    
    int _rsmWidth;
    int _rsmHeight;
    
    ImportanceSamplingPass *_impPass;
    VPLGroup *_vplGroup;
    
    osg::ref_ptr<osg::Texture2D> _depthBufferTex;
    DirectionalLight *_mainLight;
    
    osg::ref_ptr<osg::Texture2D> _sharedDepthBufferTex;
    
};

#endif /* defined(__vrphysics__IndirectLightingPass__) */
