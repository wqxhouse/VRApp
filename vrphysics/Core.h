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
#include <osgOcean/OceanScene>
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

#include "ImportanceSamplingPass.h"
#include "AssetDB.h"
#include "KeyboardHandler.h"
#include "SkyDome.h"

class LightTrackBallManipulator;
class Core
{
public:
    Core();
    ~Core();
    
    void setWindow(float width, float height);
    
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

    DirectionalLightGroup *addDirectionalLights();
    LightGroup *addPointLights();
   
    void configShadowGroup();
    
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

    void freeHeap();
    
private:
    DirectionalLightGroup *_dirLightGroup;
    LightGroup *_pointLightGroup;
    ShadowGroup *_shadowGroup;
    
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
    osg::ref_ptr<osg::Camera> _mainCamera;
    osg::ref_ptr<osg::Group> _debugHUD;
    osg::ref_ptr<KeyboardHandler> _keyboardHandler;
    
    AssetDB *_assetDB;
    std::vector<ScreenPass *> _screenPasses;
    
    float _winWidth;
    float _winHeight;
    
    osg::ref_ptr<osgViewer::Viewer> _viewer;
    
    osg::ref_ptr<LightTrackBallManipulator> _lightTrackBallManipulator;
};

#endif /* defined(__vrphysics__Core__) */
