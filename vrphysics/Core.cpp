//
//  Core.cpp
//  vrphysics
//
//  Created by Robin Wu on 1/17/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#include "Core.h"
#include <osgGA/TrackballManipulator>
#include "LightTrackBallManipulator.h"
#include "Utils.h"

void optGeoms()
{
    //    osg::ref_ptr<osg::Geode> aa(new osg::Geode);
    //    osg::ref_ptr<osg::Box> box(new osg::Box);
    //    box->set(osg::Vec3(0, 0, 2), osg::Vec3(2, 2, 2));
    //    osg::ref_ptr<osg::ShapeDrawable> draw(new osg::ShapeDrawable);
    //    draw->setShape(box);
    //    aa->addDrawable(draw);
    //    _geometryGroup->addChild(aa);
    //
    //    osg::ref_ptr<osg::Box> ground(new osg::Box);
    //    ground->set(osg::Vec3(0, 0, 0), osg::Vec3(10, 10, 0.01));
    //    osg::ref_ptr<osg::ShapeDrawable> drawGround(new osg::ShapeDrawable);
    //    drawGround->setShape(ground);
    //    aa->addDrawable(drawGround);
    //
    //    osg::ref_ptr<osg::Geode> db(new osg::Geode);
    //    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    //    vertices->push_back( osg::Vec3(0.0f, 0.0f, 0.0f) );
    //    vertices->push_back( osg::Vec3(10.0f, 0.0f, 0.0f) );
    //    vertices->push_back( osg::Vec3(10.0f, 0.0f, 15.0f) );
    //    vertices->push_back( osg::Vec3(0.0f, 0.0f, 10.0f) );
    //
    //    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    //    normals->push_back( osg::Vec3(0.0f,-1.0f, 0.0f) );
    //
    //    osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
    //    quad->setVertexArray(vertices);
    //    quad->setNormalArray(normals);
    //    quad->setNormalBinding(osg::Geometry::BIND_OVERALL);
    //    quad->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
    //
    //    db->addDrawable(quad);
    //    _geometryGroup->addChild(db);
    
    // for debugging
    //_sceneRoot->addChild(_geometryGroup);
}

osg::ref_ptr<osg::Group> constructPlane()
{
    osg::ref_ptr<osg::Group> group(new osg::Group);
    osg::ref_ptr<osg::Geode> ground(new osg::Geode);
    osg::ref_ptr<osg::Geometry> geom(new osg::Geometry);
    geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
    osg::ref_ptr<osg::Vec3Array> vertices(new osg::Vec3Array);
    vertices->push_back(osg::Vec3(1, 1, 0));
    vertices->push_back(osg::Vec3(-1, 1, 0));
    vertices->push_back(osg::Vec3(-1, -1, 0));
    vertices->push_back(osg::Vec3(1, -1, 0));
    geom->setVertexArray(vertices);
    osg::ref_ptr<osg::Vec3Array> normals(new osg::Vec3Array);
    normals->push_back(osg::Vec3(0, 0, 1));
    normals->push_back(osg::Vec3(0, 0, 1));
    normals->push_back(osg::Vec3(0, 0, 1));
    normals->push_back(osg::Vec3(0, 0, 1));
    
    geom->setNormalArray(normals);
    geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
    
    ground->addDrawable(geom);
    
    osg::ref_ptr<osg::MatrixTransform> plane1(new osg::MatrixTransform);
    osg::Matrix m1;
    m1.makeRotate(90, osg::Vec3(1, 0, 0));
    plane1->addChild(ground);
    
    osg::Matrix translate;
    translate.makeTranslate(osg::Vec3(-1, 0, 0));
    translate = m1 * translate;
    plane1->setMatrix(translate);
    
    group->addChild(ground);
    group->addChild(plane1);
    
    return group;
}

Core::Core()
:_handleGeometries(NULL), _handlePointLights(NULL), _handleDirLights(NULL)
{
    _winWidth = 800;
    _winHeight = 600;
    
    _viewer = new osgViewer::Viewer;
    _mainCamera = _viewer->getCamera();

    _sceneRoot = new osg::Group;
    
    _assetDB = new AssetDB;
    _assets = new Assets(_assetDB); // adapter for public api
   
    _geometryGroup = new osg::Group;
    _loadedGeometryGroup = _assetDB->getGeomRoot();
    _customGeometryGroup = new osg::Group;
    _lightVisualizeGeometryGroup = new osg::Group;
    
    _geometryGroup->addChild(_loadedGeometryGroup);
    _geometryGroup->addChild(_customGeometryGroup);
    _geometryGroup->addChild(_lightVisualizeGeometryGroup);
    
    _shadowGroup = new ShadowGroup(_mainCamera, _geometryGroup, _sceneAABB);
    _sceneRoot->addChild(_shadowGroup->getShadowCamerasRoot());
    
    _dirLightGroup = new DirectionalLightGroup(_shadowGroup);
    _pointLightGroup = new LightGroup;
}

Core::~Core()
{
    freeHeap();
}

void Core::run()
{
    _mainCamera->setViewport(new osg::Viewport(0, 0, _winWidth, _winHeight));
    
    // order coupling
    configGeometries();
    configLights();
    configPasses();
    
    _keyboardHandler = new KeyboardHandler(_sceneRoot, _debugHUD, _pointLightGroup);
    _viewer->setSceneData(_sceneRoot);
    _viewer->setUpViewInWindow(0, 0, _winWidth, _winHeight);
    _viewer->addEventHandler(_keyboardHandler);
    
    _viewer->run();
}

void Core::configGeometries()
{
    if(_handleGeometries != NULL)
    {
        (*_handleGeometries)(_customGeometryGroup, _assets);
    }
    
    _geometryGroup->accept(_computeBound);
    _sceneAABB = _computeBound.getBoundingBox();
}

void Core::configLights()
{
    _dirLightGroup->addMultipleLights(_assetDB->getDirectionalLights());
    _shadowGroup->addMultipleDirectionalLights(_assetDB->getDirectionalLights(), ShadowGroup::BASIC);
    _pointLightGroup->addMultipleLights(_assetDB->getPointLights());
   
    if (_handleDirLights != NULL)
    {
        (*_handleDirLights)(_dirLightGroup);
    }
    
    if(_handlePointLights != NULL)
    {
        (*_handlePointLights)(_pointLightGroup);
    }
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
                                               osg::Vec3(width, 0, 0),
                                               osg::Vec3(0, height, 0), 1, 1);
        
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
    _StateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    
    quad_geode->addDrawable(quad_geom.get());
    
    return quad_geode;
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
                                                        _shadowGroup,
                                                        _dirLightGroup);
}

void Core::configPointLightPass()
{
    // point light pass
    _pointLightPass = new LightingPass(_mainCamera,
                                       _geomPass->getPositionOutTexure(),
                                       _geomPass->getAlbedoOutTexture(),
                                       _geomPass->getNormalDepthOutTexture(),
                                       _geomPass->getSharedDepthStencilTexture(),
                                       _pointLightGroup);
}

void Core::configSSAOPass()
{
    _ssaoPass = new SSAOPass(_mainCamera, _geomPass->getPositionOutTexure(), _geomPass->getNormalDepthOutTexture(), createTexture2DImage("random.png"), _geomPass->getSharedDepthStencilTexture());
}

void Core::configFinalPass()
{
    _finalPass = new FinalPass(_mainCamera, _geomPass->getAlbedoOutTexture(),
                               _directionalLightPass->getLightingOutTexture(),
                               _pointLightPass->getLightingOutTexture(),
                               _ssaoPass->getSSAOOutTexture(),
                               _indLPass->getIndirectLightingTex());
}

void Core::configHDRPass()
{
    _hdrPass = new HDRPass(_mainCamera, _finalPass->getFinalPassTexture());
}

void Core::configImportanceSamplingPass()
{
    _impPass = new ImportanceSamplingPass(_mainCamera, _shadowGroup, _dirLightGroup);
}

void Core::configIndirectLightPass()
{
    // TODO: support multiple lights
    _indLPass = new IndirectLightingPass(_mainCamera, _impPass,
                                         _shadowGroup->getDirLightDirFluxTexture(0),
                                         _shadowGroup->getDirLightViewWorldPosTexture(0),
                                         _geomPass->getPositionOutTexure(),
                                         _geomPass->getNormalDepthOutTexture(),
                                         _geomPass->getSharedDepthStencilTexture(),
                                         _dirLightGroup->getDirectionalLight(0),
                                         _geomPass->getSharedDepthStencilTexture());
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
                             //_shadowGroup->getDirLightShadowTexture(0),
                             //_shadowGroup->getDirLightFluxTexture(0),
                             //_impPass->getImportanceSampleTexture(0),
                             //_geomPass->getPositionOutTexure(),
                             ///_impPass->getFluxMipMapTexture(),
                             _indLPass->getIndirectLightingTex(),
                             //_geomPass->getNormalDepthOutTexture(),
                             _winWidth, _winHeight, 0.3333, 0.3, true);
    
    osg::ref_ptr<osg::Camera> qTexD =
    createTextureDisplayQuad(osg::Vec3(0.3333, 0.7, 0),
                             //_shadowGroup->getDirLightViewWorldPosTexture(0),
                             //_shadowGroup->getDirLightDirFluxTexture(0),
                             _shadowGroup->getDirLightShadowTexture(0),
                             //_ssaoPass->getOutputTexture(0),
                             //_geomPass->getPositionOutTexure(),
                             //_geomPass->getSharedDepthStencilTexture(),
                             //_impPass->getPossiowTexture(),
                             //_impPass->getImportanceSampleTexture(0),
                             //_shadowGroup->getDirLightPosFluxTexture(0),
                             _winWidth, _winHeight, 0.3333, 0.3, true);
    
    osg::ref_ptr<osg::Camera> qTexP =
    createTextureDisplayQuad(osg::Vec3(0.6666, 0.7, 0),
                             //_directionalLightPass->getLightingOutTexture(),
                             //_geomPass->getNormalDepthOutTexture(),
                             _shadowGroup->getDirLightDirFluxTexture(0),
                             //_ssaoPass->getSSAOOutTexture(),
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
    
    // gi
    configImportanceSamplingPass();
    configIndirectLightPass();
    
    configFinalPass();
    configHDRPass();
    
    _sceneRoot->addChild(_geomPass->getRoot());
    
    _sceneRoot->addChild(_impPass->getRoot());
    _sceneRoot->addChild(_indLPass->getRoot());
    
    _sceneRoot->addChild(_directionalLightPass->getRoot());
    _sceneRoot->addChild(_pointLightPass->getRoot());
    _sceneRoot->addChild(_ssaoPass->getRoot());
    _sceneRoot->addChild(_finalPass->getRoot());
    _sceneRoot->addChild(_hdrPass->getRoot());
    
    setupHUDForPasses();
    
    _screenPasses.push_back(_impPass);
    _screenPasses.push_back(_ssaoPass);
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