//
//  Core.cpp
//  vrphysics
//
//  Created by Robin Wu on 1/17/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#include "Core.h"
#include <osgGA/TrackballManipulator>
#include "Utils.h"

Core::Core()
{
    _winWidth = 800;
    _winHeight = 600;
   
    _viewer = new osgViewer::Viewer;
    
    _mainCamera = _viewer->getCamera();
    _mainCamera->setViewport(new osg::Viewport(0, 0, _winWidth, _winHeight));
    _sceneRoot = new osg::Group;
    //_viewer->setCameraManipulator(new osgGA::TrackballManipulator);
    //_viewer->getCameraManipulator()->setHomePosition(osg::Vec3(1, 1, 1), osg::Vec3(0, 0, 0), osg::Vec3(0, 0, 1));
    
    _assetDB = new AssetDB;
    _geometryGroup = _assetDB->getGeomRoot();
    _assetDB->addGeometryWithFile("Testing/uu.dae");
    
    osg::ref_ptr<osg::Geode> db(new osg::Geode);
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    vertices->push_back( osg::Vec3(0.0f, 0.0f, 0.0f) );
    vertices->push_back( osg::Vec3(10.0f, 0.0f, 0.0f) );
    vertices->push_back( osg::Vec3(10.0f, 0.0f, 15.0f) );
    vertices->push_back( osg::Vec3(0.0f, 0.0f, 10.0f) );
//
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    normals->push_back( osg::Vec3(0.0f,-1.0f, 0.0f) );
    
    osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
    quad->setVertexArray(vertices);
    quad->setNormalArray(normals);
    quad->setNormalBinding(osg::Geometry::BIND_OVERALL);
    quad->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
    
    db->addDrawable(quad);
    _geometryGroup->addChild(db);
    
    _sceneRoot->addChild(_geometryGroup);
   
    configShadowGroup();
    
    // coupling here, addDirectionalLights & addPointLights needs to be called after configShadowGroup
    _dirLightGroup = addDirectionalLights();
    _pointLightGroup = addPointLights();
    configPasses();
    
    _keyboardHandler = new KeyboardHandler(_sceneRoot, _debugHUD);
    _viewer->setSceneData(_sceneRoot);
    _viewer->setUpViewInWindow(0, 0, _winWidth, _winHeight);
    _viewer->addEventHandler(_keyboardHandler);
}

Core::~Core()
{
    freeHeap();
}

void Core::run()
{
    _viewer->run();
}

void Core::setWindow(float width, float height)
{
    _winWidth = width;
    _winHeight = height;
}

osg::Camera *Core::createHUDCamera(double left,
                             double right,
                             double bottom,
                             double top)
{
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setClearMask(GL_DEPTH_BUFFER_BIT);
    camera->setRenderOrder(osg::Camera::POST_RENDER);
    camera->setAllowEventFocus(false);
    camera->setProjectionMatrix(osg::Matrix::ortho2D(left, right, bottom, top));
    camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    return camera.release();
}

osg::ref_ptr<osg::TextureCubeMap> Core::loadCubeMapTextures()
{
    enum {POS_X, NEG_X, POS_Y, NEG_Y, POS_Z, NEG_Z};
    
    std::string filenames[6];
    
    filenames[POS_X] = "east.png";
    filenames[NEG_X] = "west.png";
    filenames[POS_Z] = "north.png";
    filenames[NEG_Z] = "south.png";
    filenames[POS_Y] = "down.png";
    filenames[NEG_Y] = "up.png";
    
    osg::ref_ptr<osg::TextureCubeMap> cubeMap = new osg::TextureCubeMap;
    cubeMap->setInternalFormat(GL_RGBA);
    
    cubeMap->setFilter( osg::Texture::MIN_FILTER,    osg::Texture::LINEAR_MIPMAP_LINEAR);
    cubeMap->setFilter( osg::Texture::MAG_FILTER,    osg::Texture::LINEAR);
    cubeMap->setWrap  ( osg::Texture::WRAP_S,        osg::Texture::CLAMP_TO_EDGE);
    cubeMap->setWrap  ( osg::Texture::WRAP_T,        osg::Texture::CLAMP_TO_EDGE);
    
    cubeMap->setImage(osg::TextureCubeMap::NEGATIVE_X, osgDB::readImageFile( filenames[NEG_X] ) );
    cubeMap->setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile( filenames[POS_X] ) );
    cubeMap->setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile( filenames[NEG_Y] ) );
    cubeMap->setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile( filenames[POS_Y] ) );
    cubeMap->setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile( filenames[NEG_Z] ) );
    cubeMap->setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile( filenames[POS_Z] ) );
    
    return cubeMap;
}

osg::Geode *Core::createScreenQuad(float width,
                             float height,
                             float scaleX,
                             float scaleY,
                             osg::Vec3 corner, bool isRect)
{
    osg::Geometry *geom;
    if(isRect)
    {
        geom = osg::createTexturedQuadGeometry(
                                               corner,
                                               osg::Vec3(width, 0, 0),
                                               osg::Vec3(0, height, 0),
                                               0,
                                               0,
                                               scaleX,
                                               scaleY);
    }
    else
    {
        geom = osg::createTexturedQuadGeometry(
                                                          corner,
                                                          osg::Vec3(1024, 0, 0),
                                                          osg::Vec3(0, 1024, 0), 1, 1);
        
    }
    
    osg::ref_ptr<osg::Geode> quad = new osg::Geode;
    quad->addDrawable(geom);
    int values = osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED;
    quad->getOrCreateStateSet()->setAttribute(
                                              new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,
                                                                   osg::PolygonMode::FILL),
                                              values);
    quad->getOrCreateStateSet()->setMode(GL_LIGHTING, values);
    return quad.release();
}

osg::ref_ptr<osg::Camera> Core::createTextureDisplayQuad(
                                                   const osg::Vec3 &pos,
                                                   osg::StateAttribute *tex,
                                                   float scaleX,
                                                   float scaleY,
                                                   float width,
                                                   float height, bool isRect)
{
    osg::ref_ptr<osg::Camera> hc = createHUDCamera(0,1,0,1);
    hc->addChild(createScreenQuad(width, height, scaleX, scaleY, pos, isRect));
    hc->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
    return hc;
}

osg::ref_ptr<osg::TextureRectangle> Core::createTextureImage(const char *imageName)
{
    osg::ref_ptr<osg::TextureRectangle> texture(new osg::TextureRectangle);
    texture->setImage(osgDB::readImageFile(imageName));
    texture->setWrap(osg::TextureRectangle::WRAP_S, osg::TextureRectangle::REPEAT);
    texture->setWrap(osg::TextureRectangle::WRAP_T, osg::TextureRectangle::REPEAT);
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    return texture;
}

osg::ref_ptr<osg::Texture2D> Core::createTexture2DImage(const char *imageName)
{
    osg::ref_ptr<osg::Texture2D> texture(new osg::Texture2D);
    texture->setImage(osgDB::readImageFile(imageName));
    texture->setWrap(osg::TextureRectangle::WRAP_S, osg::TextureRectangle::REPEAT);
    texture->setWrap(osg::TextureRectangle::WRAP_T, osg::TextureRectangle::REPEAT);
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    return texture;
}

osg::ref_ptr<osg::Geode> Core::createTexturedQuad(int _TextureWidth, int _TextureHeight)
{
    osg::ref_ptr<osg::Geode> quad_geode = new osg::Geode;
    
    osg::ref_ptr<osg::Vec3Array> quad_coords = new osg::Vec3Array; // vertex coords
    // counter-clockwise
    quad_coords->push_back(osg::Vec3d(0, 0, -10));
    quad_coords->push_back(osg::Vec3d(10, 0, -10));
    quad_coords->push_back(osg::Vec3d(10, 10, -10));
    quad_coords->push_back(osg::Vec3d(0, 10, -10));
    
    osg::ref_ptr<osg::Vec2Array> quad_tcoords = new osg::Vec2Array; // texture coords
    quad_tcoords->push_back(osg::Vec2(0, 0));
    quad_tcoords->push_back(osg::Vec2(_TextureWidth, 0));
    quad_tcoords->push_back(osg::Vec2(_TextureWidth, _TextureHeight));
    quad_tcoords->push_back(osg::Vec2(0, _TextureHeight));
    
    osg::ref_ptr<osg::Geometry> quad_geom = new osg::Geometry;
    osg::ref_ptr<osg::DrawArrays> quad_da = new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4);
    
    quad_geom->setVertexArray(quad_coords.get());
    quad_geom->setTexCoordArray(0, quad_tcoords.get());
    quad_geom->addPrimitiveSet(quad_da.get());
    
    auto _StateSet = quad_geom->getOrCreateStateSet();
    _StateSet->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    
    quad_geode->addDrawable(quad_geom.get());
    
    return quad_geode;
}

DirectionalLightGroup *Core::addDirectionalLights()
{
    // Directional Lights
    DirectionalLightGroup *dirLightGroup = new DirectionalLightGroup;
    // here we can optionally choose to display the geom of the directional light
   
    // add lights from db
    // TODO: determine whether enable shadow
    dirLightGroup->addMultipleLights(_assetDB->getDirectionalLights());
    
    // custom lights
    int _id = dirLightGroup->addLight(osg::Vec3(10, 10, 10), osg::Vec3(0, 0, 0), osg::Vec3(0.6, 0.6, 0.8));
    DirectionalLight *light = dirLightGroup->getDirectionalLight(_id);
    _shadowGroup->addDirectionalLight(light, ShadowGroup::BASIC);
    
    return dirLightGroup;
}

LightGroup *Core::addPointLights()
{
    // Point lights
    LightGroup *lightGroup = new LightGroup;
    osg::BoundingSphere sp;
    sp.center() = osg::Vec3(0, 2, -3);
    sp.radius() = 5;
    //    for(int i = 0; i < 20; i++)
    //    {
    //        lightGroup->addRandomLightWithBoundingSphere(sp);
    //    }
    //
    //    lightGroup->addLight(osg::Vec3(-1.09, -3.71, 2.97), osg::Vec3(0.213, 1, 0.305), osg::Vec3(0, 0, 0), osg::Vec3(0.6, 0.4, 0.4), 8.0);
    //    lightGroup->addLight(osg::Vec3(2.54, -2.26, -1.47), osg::Vec3(0.98, 0.54, 1), osg::Vec3(0, 0, 0), osg::Vec3(0.6, 0.4, 0.4), 8.0);
    //    lightGroup->addLight(osg::Vec3(-0.26, 2.08, 4.89), osg::Vec3(0.24, 1, 0.222), osg::Vec3(0, 0, 0), osg::Vec3(0.6, 0.2, 0.1), 8.0);
   
    // optionally display light geometry
    _geometryGroup->addChild(lightGroup->getGeomTransformLightGroup()); // point light geoms
    
    // add point lights from db
    lightGroup->addMultipleLights(_assetDB->getPointLights());
    
    // add custom point lights
    
    return lightGroup;
}

void Core::configGeomPass()
{
    _geomPass = new GeometryPass(_mainCamera, _assetDB);
}

void Core::configDirectionalLightPass()
{
    // directional light pass
    _directionalLightPass = new DirectionalLightingPass(_mainCamera, _geomPass->getPositionOutTexure(),
                                                                        _geomPass->getAlbedoOutTexture(),
                                                                        _geomPass->getNormalDepthOutTexture(),
                                                                        _dirLightGroup);
}

void Core::configPointLightPass()
{
    // point light pass
    _pointLightPass = new LightingPass(_mainCamera,
                                               _geomPass->getPositionOutTexure(),
                                               _geomPass->getAlbedoOutTexture(),
                                               _geomPass->getNormalDepthOutTexture(),
                                               _pointLightGroup);
}

void Core::configSSAOPass()
{
    _ssaoPass = new SSAOPass(_mainCamera, _geomPass->getPositionOutTexure(), _geomPass->getNormalDepthOutTexture(), createTexture2DImage("random.png"));
}

void Core::configFinalPass()
{
    _finalPass = new FinalPass(_mainCamera, _geomPass->getAlbedoOutTexture(),
                                         _directionalLightPass->getLightingOutTexture(),
                                         _pointLightPass->getLightingOutTexture(),
                                         _ssaoPass->getSSAOOutTexture());
}

void Core::configHDRPass()
{
    _hdrPass = new HDRPass(_mainCamera, _finalPass->getFinalPassTexture());
}

void Core::configShadowGroup()
{
    _shadowGroup = new ShadowGroup(_mainCamera, _geometryGroup);
    _sceneRoot->addChild(_shadowGroup->getShadowCamerasRoot());
}

void Core::setupHUDForPasses()
{
    osg::ref_ptr<osg::Group> hud(new osg::Group);
    _debugHUD = hud;
    
//    osg::ref_ptr<osg::Camera> qTexN =
//    createTextureDisplayQuad(osg::Vec3(0, 0.7, 0),
//                             _geomPass->getNormalDepthOutTexture(),
//                             _winWidth, _winHeight, 0.3333, 0.3);
    osg::ref_ptr<osg::Camera> qTexN =
    createTextureDisplayQuad(osg::Vec3(0, 0.7, 0),
                             _shadowGroup->getDirLightShadowTexture(0),
                             _winWidth, _winHeight, 0.3333, 0.3, true);
    
    osg::ref_ptr<osg::Camera> qTexD =
    createTextureDisplayQuad(osg::Vec3(0.3333, 0.7, 0),
                             _ssaoPass->getOutputTexture(0),
                             _winWidth, _winHeight, 0.3333, 0.3, true);
    
    osg::ref_ptr<osg::Camera> qTexP =
    createTextureDisplayQuad(osg::Vec3(0.6666, 0.7, 0),
                             _pointLightPass->getLightingOutTexture(),
                             _winWidth, _winHeight, 0.3333, 0.3, true);
    
    osg::ref_ptr<osg::Camera> qTexF =
    createTextureDisplayQuad(osg::Vec3(0.0, 0.0, 0),
                             _finalPass->getFinalPassTexture(),
                             _winWidth, _winHeight, 1, 1, true);
    _sceneRoot->addChild(qTexF);
    //hud->addChild(qTexF);
    hud->addChild(qTexN);
    hud->addChild(qTexD);
    hud->addChild(qTexP);
    _sceneRoot->addChild(hud);
}

void Core::configPasses()
{
    configGeomPass();
    configDirectionalLightPass();
    configPointLightPass();
    configSSAOPass();
    configFinalPass();
    configHDRPass();
    
    _sceneRoot->addChild(_geomPass->getRoot());
    _sceneRoot->addChild(_directionalLightPass->getRoot());
    _sceneRoot->addChild(_pointLightPass->getRoot());
    _sceneRoot->addChild(_ssaoPass->getRoot());
    _sceneRoot->addChild(_finalPass->getRoot());
    _sceneRoot->addChild(_hdrPass->getRoot());
    
    setupHUDForPasses();
    
    _screenPasses.push_back(_geomPass);
    _screenPasses.push_back(_directionalLightPass);
    _screenPasses.push_back(_pointLightPass);
    _screenPasses.push_back(_finalPass);
    _screenPasses.push_back(_hdrPass);
}


void Core::freeHeap()
{
    for(int i = 0; i < (int)_screenPasses.size(); i++)
    {
        delete _screenPasses[i];
    }
    delete _assetDB;
    delete _dirLightGroup;
    delete _pointLightGroup;
    delete _shadowGroup;
}