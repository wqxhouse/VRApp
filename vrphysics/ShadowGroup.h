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
    
    ShadowGroup(osg::Camera *mainCamera, osg::Group *geoms, const osg::BoundingBox &sceneAABB);
    ~ShadowGroup() {};
    
    void addDirectionalLight(DirectionalLight *dirLight, enum ShadowMode mode);
    void addMultipleDirectionalLights(std::vector<DirectionalLight *> lights, enum ShadowMode mode);
    void setDepthMapResolution(float width, float height);
    osg::ref_ptr<osg::Group> createQuad();
    
    osg::ref_ptr<osg::TextureRectangle> getDirLightShadowTexture(int light_id);
    
    osg::ref_ptr<osg::TextureRectangle> getDirLightDirFluxTexture(int light_id);
    osg::ref_ptr<osg::TextureRectangle> getDirLightViewWorldPosTexture(int light_id);
    osg::ref_ptr<osg::Texture2D> getDirMipmapFluxTexture(int light_id);
    
    inline osg::ref_ptr<osg::Group> getShadowGroupRoot()
    {
        return _shadowRootGroup;
    }
    
    inline float getRsmWidth()
    {
        return _rsmTexWidth;
    }
    
    inline float getRsmHeight()
    {
        return _rsmTexHeight;
    }
    
private:
    osg::ref_ptr<osg::TextureRectangle> createShadowTexture(int width, int height);
    osg::ref_ptr<osg::TextureRectangle> createLightDirFluxTexture(int width, int height);
    osg::ref_ptr<osg::Texture2D> createFluxMipmapTexture(int width, int height);
    osg::ref_ptr<osg::TextureRectangle> createLightPositionTexture(int width, int height);
    
    // TODO: use a single camera with different matrix transform for light views to save memory
    // a single osg camera = a render buffer and couple lighting/indirect lighting stages to
    // reuse the render buffer
    void addBasicShadowCam(osg::TextureRectangle *outDepthTex, osg::TextureRectangle *outFluxTex, osg::TextureRectangle *outPosTex, const osg::Matrixf &shadowMV, const osg::Matrixf &shadowMVP, DirectionalLight *dirLight);
    
    void addBlurCamera(osg::TextureRectangle *outDepthTex);
    osg::ref_ptr<osg::Camera> _mainCamera;
    
    void configBlurQuadStateSet(osg::Group *g, char dir, osg::TextureRectangle *outDepthTex);
    
    std::map<int, osg::ref_ptr<osg::TextureRectangle> > _dir_depthMaps; // if gi enabled, then rgb is normal; alpha is depth
    std::map<int, osg::ref_ptr<osg::Texture2D> > _spot_depthMaps;
    std::map<int, std::vector<osg::ref_ptr<osg::Texture2D> > > _point_depthMaps;
    osg::Matrix _shadowProjection;
    
    osg::ref_ptr<osg::Group> _geoms;
    osg::ref_ptr<osg::Group> _shadowRootGroup;
    
    osg::ref_ptr<osg::Group> _shadowCameras;
    osg::ref_ptr<osg::Group> _blurCameras;
    
    
    osg::ref_ptr<osg::Program> _depthMapShader;
    osg::ref_ptr<osg::Program> _blurShaderX;
    osg::ref_ptr<osg::Program> _blurShaderY;
    
    float _depthTexWidth;
    float _depthTexHeight;
    
    float _nearPlane;
    float _farPlane;
    
    // global illumination
    std::map<int, osg::ref_ptr<osg::TextureRectangle> > _dir_lightDir_fluxMaps;
    std::map<int, osg::ref_ptr<osg::Texture2D> > _dir_mipmapFluxMaps;
    std::map<int, osg::ref_ptr<osg::TextureRectangle> > _dir_worldPos_Maps;
    float _rsmTexWidth;
    float _rsmTexHeight;
    
    bool _isGIEnabled;
    
    const osg::BoundingBox &_sceneAABB;
};

#endif /* defined(__vrphysics__ShadowGroup__) */
