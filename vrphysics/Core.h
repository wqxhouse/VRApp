//
//  Core.h
//  vrphysics
//
//  Created by Robin Wu on 1/17/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#ifndef __vrphysics__Core__
#define __vrphysics__Core__

#include <stdio.h>
#include <osgDB/ReadFile>
#include <osgViewer/viewer>
#include <osg/Camera>
#include <osg/PolygonMode>
#include <osg/TextureCubeMap>

#include "GeometryPass.h"
#include "DirectionalLightingPass.h"
#include "DirectionalLightGroup.h"
#include "ShadowGroup.h"
#include "LightingPass.h"
#include "LightGroup.h"
#include "SSAOPass.h"
#include "FinalPass.h"
#include "HDRPass.h"
#include "IndirectLightingPass.h"
// #include "FrustumData.h"

#include "ImportanceSamplingPass.h"
#include "AssetDB.h"
#include "KeyboardHandler.h"

#include <osg/ComputeBoundsVisitor>

class LightTrackBallManipulator;
class Core
{
public:
    Core();
    ~Core();
    
    inline void setWindowSize(int width, int height)
    {
        _winWidth = width;
        _winHeight = height;
    }
    
    inline void setDirectionalLightHandler(void (*handleDirectionalLights)(DirectionalLightGroup *const))
    {
        _handleDirLights = handleDirectionalLights;
    }
    
    inline void setPointLightHandler(void (*handlePointLights)(LightGroup *const))
    {
        _handlePointLights = handlePointLights;
    }
    
    inline void setGeometryHandler(void (*handleGeometries)(const osg::ref_ptr<osg::Group>, Assets *const))
    {
        _handleGeometries = handleGeometries;
    }
    
    inline osg::ref_ptr<osgViewer::Viewer> getViewer()
    {
        return _viewer;
    }
    
    void run();
    
private:
    
    osg::ref_ptr<osg::TextureCubeMap> loadCubeMapTextures();
    osg::Camera *createHUDCamera(double left,
                                 double right,
                                 double bottom,
                                 double top);
    
    
    osg::Geode *createScreenQuad(float width,
                                 float height,
                                 float scaleX,
                                 float scaleY,
                                 osg::Vec3 corner, bool isRect);
    
    osg::ref_ptr<osg::Camera> createTextureDisplayQuad(
                                                       const osg::Vec3 &pos,
                                                       osg::StateAttribute *tex,
                                                       float scaleX,
                                                       float scaleY,
                                                       float width,
                                                       float height, bool isRect);
    
    osg::ref_ptr<osg::TextureRectangle> createTextureImage(const char *imageName);
    osg::ref_ptr<osg::Texture2D> createTexture2DImage(const char *imageName);
    osg::ref_ptr<osg::Geode> createTexturedQuad(int _TextureWidth, int _TextureHeight);
    
    //    DirectionalLightGroup *addDirectionalLights();
    //    LightGroup *addPointLights();
    //
    void configGeomPass();
    void configRSMPass();
    void configDirectionalLightPass();
    void configPointLightPass();
    void configSSAOPass();
    void configFinalPass();
    void configHDRPass();
    void setupHUDForPasses();
    void configPasses();
    void configImportanceSamplingPass();
    void configIndirectLightPass();
    
    void configGeometries();
    void configLights();
    
    void freeHeap();
    
private:
    DirectionalLightGroup *_dirLightGroup;
    LightGroup *_pointLightGroup;
    ShadowGroup *_shadowGroup;
    
    void (*_handleDirLights)(DirectionalLightGroup *const);
    void (*_handlePointLights)(LightGroup *const);
    void (*_handleGeometries)(const osg::ref_ptr<osg::Group>, Assets *const);
    
    GeometryPass *_geomPass;
    DirectionalLightingPass *_directionalLightPass;
    LightingPass *_pointLightPass;
    SSAOPass *_ssaoPass;
    FinalPass *_finalPass;
    HDRPass *_hdrPass;
    IndirectLightingPass *_indLPass;
    ImportanceSamplingPass *_impPass;
    
    osg::ref_ptr<osg::Group> _sceneRoot;
    osg::ref_ptr<osg::Group> _geometryGroup;
    osg::ref_ptr<osg::Group> _customGeometryGroup;
    osg::ref_ptr<osg::Group> _loadedGeometryGroup;
    osg::ref_ptr<osg::Group> _lightVisualizeGeometryGroup;
    
    osg::ref_ptr<osg::Camera> _mainCamera;
    osg::ref_ptr<osg::Group> _debugHUD;
    osg::ref_ptr<KeyboardHandler> _keyboardHandler;
    
    AssetDB *_assetDB;
    Assets *_assets;
    
    std::vector<ScreenPass *> _screenPasses;
    
    int _winWidth;
    int _winHeight;
    
    osg::ref_ptr<osgViewer::Viewer> _viewer;
    
    osg::ref_ptr<LightTrackBallManipulator> _lightTrackBallManipulator;
    osg::ComputeBoundsVisitor _computeBound;
    osg::BoundingBox _sceneAABB;
    
};

class CameraUpdateCallback : public osg::NodeCallback
{
public:
    CameraUpdateCallback(osg::Camera *mainCamera) : _mainCamera(mainCamera) {};
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
    
private:
    osg::ref_ptr<osg::Camera> _mainCamera;
};

#endif /* defined(__vrphysics__Core__) */
