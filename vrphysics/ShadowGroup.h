//
//  ShadowGroup.h
//  vrphysics
//
//  Created by Robin Wu on 1/7/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#ifndef __vrphysics__ShadowGroup__
#define __vrphysics__ShadowGroup__

#include <stdio.h>
#include <map>
#include <vector>
#include <osg/Texture2D>
#include <osg/TextureRectangle>

#include "DirectionalLight.h"

class ShadowGroup
{
public:
    enum ShadowMode
    {
        BASIC
    };
    
    ShadowGroup(osg::Camera *mainCamera, osg::Group *geoms);
    ~ShadowGroup() {};
    
    void addDirectionalLight(DirectionalLight *dirLight, enum ShadowMode mode);
    void addMultipleDirectionalLights(std::vector<DirectionalLight *> lights, enum ShadowMode mode);
    void setDepthMapResolution(float width, float height);
    
    osg::ref_ptr<osg::TextureRectangle> getDirLightShadowTexture(int light_id);
    
    inline osg::ref_ptr<osg::Group> getShadowCamerasRoot()
    {
        return _shadowCameras;
    }
    
private:
    osg::ref_ptr<osg::TextureRectangle> createShadowTexture(int width, int height);
    void addBasicShadowCam(osg::TextureRectangle *outDepthTex, const osg::Matrixf &shadowMV, const osg::Matrixf &shadowMVP, DirectionalLight *dirLight);
    
    osg::ref_ptr<osg::Camera> _mainCamera;
    
    std::map<int, osg::ref_ptr<osg::TextureRectangle> > _dir_depthMaps;
    std::map<int, osg::ref_ptr<osg::Texture2D> > _spot_depthMaps;
    std::map<int, std::vector<osg::ref_ptr<osg::Texture2D> > > _point_depthMaps;
    osg::Matrix _shadowProjection;
    
    osg::ref_ptr<osg::Group> _geoms;
    osg::ref_ptr<osg::Group> _shadowCameras;
    
    osg::ref_ptr<osg::Program> _depthMapShader;
    
    float _depthTexWidth;
    float _depthTexHeight;
    
    float _nearPlane;
    float _farPlane;
};

#endif /* defined(__vrphysics__ShadowGroup__) */