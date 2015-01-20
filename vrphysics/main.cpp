////
////  main.cpp
////  vrphysics
////
////  Created by Robin Wu on 11/11/14.
////  Copyright (c) 2014 WSH. All rights reserved.
////
//
//#include <stdio.h>
//#include <osgDB/ReadFile>
//#include <osgViewer/viewer>
//#include <osg/Camera>
//#include <osg/PolygonMode>
//#include <osgOcean/OceanScene>
//#include <osg/TextureCubeMap>
//
//#include "GeometryPass.h"
//#include "DirectionalLightingPass.h"
//#include "DirectionalLightGroup.h"
//#include "LightingPass.h"
//#include "LightGroup.h"
//#include "SSAOPass.h"
//#include "FinalPass.h"
//#include "HDRPass.h"
//#include "AssetDB.h"
//#include "KeyboardHandler.h"
//#include "SkyDome.h"
//
//static osg::ref_ptr<osg::Group> g_sceneNode;
//static osg::ref_ptr<osg::Group> g_hudNode;
//
//osg::Camera *createHUDCamera(double left,
//                             double right,
//                             double bottom,
//                             double top)
//{
//    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
//    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
//    camera->setClearMask(GL_DEPTH_BUFFER_BIT);
//    camera->setRenderOrder(osg::Camera::POST_RENDER);
//    camera->setAllowEventFocus(false);
//    camera->setProjectionMatrix(osg::Matrix::ortho2D(left, right, bottom, top));
//    camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
//    return camera.release();
//}
//
//osg::ref_ptr<osg::TextureCubeMap> loadCubeMapTextures()
//{
//    enum {POS_X, NEG_X, POS_Y, NEG_Y, POS_Z, NEG_Z};
//    
//    std::string filenames[6];
//    
//    filenames[POS_X] = "east.png";
//    filenames[NEG_X] = "west.png";
//    filenames[POS_Z] = "north.png";
//    filenames[NEG_Z] = "south.png";
//    filenames[POS_Y] = "down.png";
//    filenames[NEG_Y] = "up.png";
//    
//    osg::ref_ptr<osg::TextureCubeMap> cubeMap = new osg::TextureCubeMap;
//    cubeMap->setInternalFormat(GL_RGBA);
//    
//    cubeMap->setFilter( osg::Texture::MIN_FILTER,    osg::Texture::LINEAR_MIPMAP_LINEAR);
//    cubeMap->setFilter( osg::Texture::MAG_FILTER,    osg::Texture::LINEAR);
//    cubeMap->setWrap  ( osg::Texture::WRAP_S,        osg::Texture::CLAMP_TO_EDGE);
//    cubeMap->setWrap  ( osg::Texture::WRAP_T,        osg::Texture::CLAMP_TO_EDGE);
//    
//    cubeMap->setImage(osg::TextureCubeMap::NEGATIVE_X, osgDB::readImageFile( filenames[NEG_X] ) );
//    cubeMap->setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile( filenames[POS_X] ) );
//    cubeMap->setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile( filenames[NEG_Y] ) );
//    cubeMap->setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile( filenames[POS_Y] ) );
//    cubeMap->setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile( filenames[NEG_Z] ) );
//    cubeMap->setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile( filenames[POS_Z] ) );
//    
//    return cubeMap;
//}
//
//osg::Geode *createScreenQuad(float width,
//                             float height,
//                             float scaleX,
//                             float scaleY,
//                             osg::Vec3 corner)
//{
//    osg::Geometry* geom = osg::createTexturedQuadGeometry(
//                                                          corner,
//                                                          osg::Vec3(width, 0, 0),
//                                                          osg::Vec3(0, height, 0),
//                                                          0,
//                                                          0,
//                                                          scaleX,
//                                                          scaleY);
//    osg::ref_ptr<osg::Geode> quad = new osg::Geode;
//    quad->addDrawable(geom);
//    int values = osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED;
//    quad->getOrCreateStateSet()->setAttribute(
//                                              new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,
//                                                                   osg::PolygonMode::FILL),
//                                              values);
//    quad->getOrCreateStateSet()->setMode(GL_LIGHTING, values);
//    return quad.release();
//}
//
//osg::ref_ptr<osg::Camera> createTextureDisplayQuad(
//                                                   const osg::Vec3 &pos,
//                                                   osg::StateAttribute *tex,
//                                                   float scaleX,
//                                                   float scaleY,
//                                                   float width,
//                                                   float height)
//{
//    osg::ref_ptr<osg::Camera> hc = createHUDCamera(0,1,0,1);
//    hc->addChild(createScreenQuad(width, height, scaleX, scaleY, pos));
//    hc->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
//    return hc;
//}
//
//osg::ref_ptr<osg::TextureRectangle> createTextureImage(const char *imageName)
//{
//    osg::ref_ptr<osg::TextureRectangle> texture(new osg::TextureRectangle);
//    texture->setImage(osgDB::readImageFile(imageName));
//    texture->setWrap(osg::TextureRectangle::WRAP_S, osg::TextureRectangle::REPEAT);
//    texture->setWrap(osg::TextureRectangle::WRAP_T, osg::TextureRectangle::REPEAT);
//    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
//    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
//    return texture;
//}
//
//osg::ref_ptr<osg::Texture2D> createTexture2DImage(const char *imageName)
//{
//    osg::ref_ptr<osg::Texture2D> texture(new osg::Texture2D);
//    texture->setImage(osgDB::readImageFile(imageName));
//    texture->setWrap(osg::TextureRectangle::WRAP_S, osg::TextureRectangle::REPEAT);
//    texture->setWrap(osg::TextureRectangle::WRAP_T, osg::TextureRectangle::REPEAT);
//    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
//    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
//    return texture;
//}
//
//osg::ref_ptr<osg::Geode> createTexturedQuad(int _TextureWidth, int _TextureHeight)
//{
//    osg::ref_ptr<osg::Geode> quad_geode = new osg::Geode;
//    
//    osg::ref_ptr<osg::Vec3Array> quad_coords = new osg::Vec3Array; // vertex coords
//    // counter-clockwise
//    quad_coords->push_back(osg::Vec3d(0, 0, -10));
//    quad_coords->push_back(osg::Vec3d(10, 0, -10));
//    quad_coords->push_back(osg::Vec3d(10, 10, -10));
//    quad_coords->push_back(osg::Vec3d(0, 10, -10));
//    
//    osg::ref_ptr<osg::Vec2Array> quad_tcoords = new osg::Vec2Array; // texture coords
//    quad_tcoords->push_back(osg::Vec2(0, 0));
//    quad_tcoords->push_back(osg::Vec2(_TextureWidth, 0));
//    quad_tcoords->push_back(osg::Vec2(_TextureWidth, _TextureHeight));
//    quad_tcoords->push_back(osg::Vec2(0, _TextureHeight));
//    
//    osg::ref_ptr<osg::Geometry> quad_geom = new osg::Geometry;
//    osg::ref_ptr<osg::DrawArrays> quad_da = new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4);
//    
//    quad_geom->setVertexArray(quad_coords.get());
//    quad_geom->setTexCoordArray(0, quad_tcoords.get());
//    quad_geom->addPrimitiveSet(quad_da.get());
//    
//    auto _StateSet = quad_geom->getOrCreateStateSet();
//    _StateSet->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
//    
//    quad_geode->addDrawable(quad_geom.get());
//    
//    return quad_geode;
//}
//
//// Derive a class from NodeVisitor to find a node with a
////   specific name.
//class FindNamedNode : public osg::NodeVisitor
//{
//public:
//    FindNamedNode( )
//    : osg::NodeVisitor( // Traverse all children.
//                       osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ){}
//    
//    // This method gets called for every node in the scene
//    //   graph. Check each node to see if its name matches
//    //   out target. If so, save the node's address.
//    virtual void apply( osg::Node& node )
//    {
//        if(node.getOrCreateStateSet()->getAttributeList().size() != 0)
//        {
//            printf("GOTCHA = %s\n", node.getName().c_str());
//        }
//        
//        // Keep traversing the rest of the scene graph.
//        traverse( node );
//    }
//    
//    osg::Node* getNode() { return _node.get(); }
//    
//protected:
//    std::string _name;
//    osg::ref_ptr<osg::Node> _node;
//};
//
//osg::ref_ptr<osg::Node> addTable()
//{
//    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile("Testing/NewlyTest/new.osg");
//    FindNamedNode abc;
//    loadedModel->accept(abc);
//    
//    
//  
//    // transform to center
//    osg::Vec3 loadedModelCenter = loadedModel->getBound().center();
//    osg::ref_ptr<osg::PositionAttitudeTransform> modelToCenter(new osg::PositionAttitudeTransform);
//    modelToCenter->setPosition(-loadedModelCenter);
//    modelToCenter->addChild(loadedModel);
//    return modelToCenter;
//}
//
//osg::ref_ptr<osg::Group> addGeometries()
//{
//    osg::ref_ptr<osg::Group> geometryGroup(new osg::Group);
//    geometryGroup->addChild(addTable());
//    return geometryGroup;
//}
//
//DirectionalLightGroup *addDirectionalLights(osg::Group *geomGroup)
//{
//    // Directional Lights
//    DirectionalLightGroup *dirLightGroup = new DirectionalLightGroup;
//    // here we can optionally choose to display the geom of the directional light
//    dirLightGroup->addLight(osg::Vec3(0, 0, 2), osg::Vec3(0, 1, 0), osg::Vec3(0.6, 0.6, 0.8));
////    dirLightGroup->addLight(osg::Vec3(-2, -5, -2), osg::Vec3(0, 0, 0), osg::Vec3(0.3, 0.3, 0.4));
////    dirLightGroup->addLight(osg::Vec3(10, 6, 0), osg::Vec3(0, 0, 0), osg::Vec3(0.2, 0.2, 0.4));
//    return dirLightGroup;
//}
//
//LightGroup *addPointLights(osg::Group *geomGroup)
//{
//    
//    //    for(int i = -20; i < 20; i += 4)
//    //    {
//    //        lightGroup->addLight(osg::Vec3(i, 0, -60), osg::Vec3(1, 1, 0));
//    //    }
//    
//    
//    //    lightGroup.addLight(osg::Vec3(5, 0, 0), osg::Vec3(1, 1, 0));
//    //    lightGroup.addLight(osg::Vec3(5, 5, 5), osg::Vec3(1, 1, 0));
//    //    lightGroup.addLight(osg::Vec3(-5, 5, 5), osg::Vec3(1, 0, 1));
//    //    lightGroup.addLight(osg::Vec3(5, -5, 5), osg::Vec3(1, 1, 1));
//    //    lightGroup.addLight(osg::Vec3(5, 5, -5), osg::Vec3(1, 0, 0));
//    //    lightGroup.addLight(osg::Vec3(-5, 5, -5), osg::Vec3(0, 1, 1));
//    //    lightGroup.addLight(osg::Vec3(-5, -5, 5), osg::Vec3(0, 1, 0));
//    
//    // Point lights
//    LightGroup *lightGroup = new LightGroup;
//        osg::BoundingSphere sp;
//    sp.center() = osg::Vec3(0, 2, -3);
//    sp.radius() = 5;
////    for(int i = 0; i < 20; i++)
////    {
////        lightGroup->addRandomLightWithBoundingSphere(sp);
////    }
////
////    lightGroup->addLight(osg::Vec3(-1.09, -3.71, 2.97), osg::Vec3(0.213, 1, 0.305), osg::Vec3(0, 0, 0), osg::Vec3(0.6, 0.4, 0.4), 8.0);
////    lightGroup->addLight(osg::Vec3(2.54, -2.26, -1.47), osg::Vec3(0.98, 0.54, 1), osg::Vec3(0, 0, 0), osg::Vec3(0.6, 0.4, 0.4), 8.0);
////    lightGroup->addLight(osg::Vec3(-0.26, 2.08, 4.89), osg::Vec3(0.24, 1, 0.222), osg::Vec3(0, 0, 0), osg::Vec3(0.6, 0.2, 0.1), 8.0);
//    geomGroup->addChild(lightGroup->getGeomTransformLightGroup()); // point light geoms
//
//    return lightGroup;
//}
//
//GeometryPass *configGeomPass(osg::Camera *camera, AssetDB &assetDB)
//{
//    GeometryPass *geomPass = new GeometryPass(camera, assetDB);
//    return geomPass;
//}
//
//DirectionalLightingPass *configDirectionalLightPass(osg::Camera *camera, GeometryPass *geomPass, DirectionalLightGroup *dirLightGroup)
//{
//    // directional light pass
//    DirectionalLightingPass *dirLightPass = new DirectionalLightingPass(camera, geomPass->getPositionOutTexure(),
//                                         geomPass->getAlbedoOutTexture(),
//                                         geomPass->getNormalDepthOutTexture(),
//                                         dirLightGroup);
//    return dirLightPass;
//}
//
//LightingPass *configPointLightPass(osg::Camera *camera, GeometryPass *geomPass, LightGroup *lightGroup)
//{
//    // point light pass
//    LightingPass *lightPass = new LightingPass(camera,
//                           geomPass->getPositionOutTexure(),
//                           geomPass->getAlbedoOutTexture(),
//                           geomPass->getNormalDepthOutTexture(),
//                           lightGroup);
//    return lightPass;
//}
//
//SSAOPass *configSSAOPass(osg::Camera *camera, GeometryPass *geomPass)
//{
//    SSAOPass *ssaoPass = new SSAOPass(camera, geomPass->getPositionOutTexure(), geomPass->getNormalDepthOutTexture(), createTexture2DImage("random.png"));
//    return ssaoPass;
//}
//
//FinalPass *configFinalPass(osg::Camera *camera, GeometryPass *geomPass, DirectionalLightingPass *dirLightPass, LightingPass *lightPass, SSAOPass *ssaoPass)
//{
//    FinalPass *finalPass = new FinalPass(camera, geomPass->getAlbedoOutTexture(),
//                        dirLightPass->getLightingOutTexture(),
//                        lightPass->getLightingOutTexture(),
//                        ssaoPass->getSSAOOutTexture());
//    return finalPass;
//}
//
//HDRPass *configHDRPass(osg::Camera *camera, osg::TextureRectangle *inScreenTex)
//{
//    HDRPass *hdrPass = new HDRPass(camera, inScreenTex);
//    return hdrPass;
//}
//
//void setupHUDForPasses(GeometryPass *gp, DirectionalLightingPass *dp, LightingPass *lp, SSAOPass *ssaoPass, FinalPass *fp, HDRPass *hdrPass, osg::Group *sceneRoot)
//{
//    osg::ref_ptr<osg::Group> hud(new osg::Group);
//    g_hudNode = hud;
//    
//    osg::ref_ptr<osg::Camera> qTexN =
//    createTextureDisplayQuad(osg::Vec3(0, 0.7, 0),
//                             gp->getNormalDepthOutTexture(),
//                             800, 600, 0.3333, 0.3);
//
//    osg::ref_ptr<osg::Camera> qTexD =
//    createTextureDisplayQuad(osg::Vec3(0.3333, 0.7, 0),
//                             ssaoPass->getOutputTexture(0),
//                             800, 600, 0.3333, 0.3);
//
//    osg::ref_ptr<osg::Camera> qTexP =
//    createTextureDisplayQuad(osg::Vec3(0.6666, 0.7, 0),
//                             lp->getLightingOutTexture(),
//                             800, 600, 0.3333, 0.3);
//    
//    osg::ref_ptr<osg::Camera> qTexF =
//    createTextureDisplayQuad(osg::Vec3(0.0, 0.0, 0),
//                             fp->getFinalPassTexture(),
//                             800, 600, 1, 1);
//    sceneRoot->addChild(qTexF);
//    //hud->addChild(qTexF);
//    hud->addChild(qTexN);
//    hud->addChild(qTexD);
//    hud->addChild(qTexP);
//    sceneRoot->addChild(hud);
//}
//
//std::vector<ScreenPass *> configPasses(AssetDB &assetDB, osg::Camera *camera, DirectionalLightGroup *dirLightGroup, LightGroup *pointLightGroup, osg::Group *sceneRoot)
//{
//    GeometryPass *gp = configGeomPass(camera, assetDB);
//    DirectionalLightingPass *dirLightPass = configDirectionalLightPass(camera, gp, dirLightGroup);
//    LightingPass *pointLightPass = configPointLightPass(camera, gp, pointLightGroup);
//    SSAOPass *ssaoPass = configSSAOPass(camera, gp);
//    FinalPass *finalPass = configFinalPass(camera, gp, dirLightPass, pointLightPass,ssaoPass);
//    HDRPass *hdrPass = configHDRPass(camera, finalPass->getFinalPassTexture());
//    
//    sceneRoot->addChild(gp->getRoot());
//    sceneRoot->addChild(dirLightPass->getRoot());
//    sceneRoot->addChild(pointLightPass->getRoot());
//    sceneRoot->addChild(ssaoPass->getRoot());
//    sceneRoot->addChild(finalPass->getRoot());
//    sceneRoot->addChild(hdrPass->getRoot());
//    
//    setupHUDForPasses(gp, dirLightPass, pointLightPass, ssaoPass, finalPass, hdrPass, sceneRoot);
//    
//    std::vector<ScreenPass *> passes;
//    passes.push_back(gp);
//    passes.push_back(dirLightPass);
//    passes.push_back(pointLightPass);
//    passes.push_back(finalPass);
//    passes.push_back(hdrPass);
//    return passes;
//}
//
//osg::ref_ptr<SkyDome> createSkyDome()
//{
//    osg::ref_ptr<SkyDome> _skyDome(new SkyDome);
//    osg::ref_ptr<osg::TextureCubeMap> cubeMap = loadCubeMapTextures();
//    _skyDome = new SkyDome( 10.f, 16, 16, cubeMap.get() );
//    _skyDome->setNodeMask(0x4);
//    return _skyDome;
//}
//
//void freeHeap(std::vector<ScreenPass *> screenPasses, DirectionalLightGroup *dirLightGroup, LightGroup *ptLightGroup)
//{
//    for(int i = 0; i < (int)screenPasses.size(); i++)
//    {
//        delete screenPasses[i];
//    }
//    delete dirLightGroup;
//    delete ptLightGroup;
//}
//
//int main()
//{
//    osgViewer::Viewer viewer;
//    osg::Camera *mainCamera = viewer.getCamera();
//    mainCamera->setViewport(new osg::Viewport(0, 0, 800, 600));
//    osg::ref_ptr<osg::Group> sceneRoot(new osg::Group);
//    g_sceneNode = sceneRoot;
//    
//    osg::Matrix mat = mainCamera->getProjectionMatrix();
//    Utils::print(mat);
//    
//    osg::ref_ptr<SkyDome> skyDome = createSkyDome();
//   
//    AssetDB assets;
//    //assets.addGeometryWithFile("Testing/buddha.obj");
//    assets.addGeometryWithFile("Testing/uu.dae");
////    assets.addGeometryWithFile("Testing/cornell-box/engine/a.dae");
//    sceneRoot->addChild(assets.getGeomRoot());
//    //assets.getGeomRoot()->addChild(skyDome);
//    //sceneRoot->addChild(skyDome);
//    
//    DirectionalLightGroup *dirLightGroup = addDirectionalLights(assets.getGeomRoot());
//    dirLightGroup->addMultipleLights(assets.getDirectionalLights());
//    LightGroup *pointLightGroup = addPointLights(assets.getGeomRoot());
//    pointLightGroup->addMultipleLights(assets.getPointLights());
//    std::vector<ScreenPass *> passes = configPasses(assets, mainCamera, dirLightGroup, pointLightGroup, sceneRoot);
//    
//    osg::ref_ptr<KeyboardHandler> h(new KeyboardHandler(g_sceneNode, g_hudNode));
//    viewer.setSceneData(sceneRoot);
//    viewer.setUpViewInWindow(0, 0, 800, 600);
//    viewer.addEventHandler(h);
//    
//    viewer.run();
//
//    freeHeap(passes, dirLightGroup, pointLightGroup);
//    return 0;
//}
#include "Core.h"
int main()
{
    Core core;
    core.setWindow(1280, 720);
    core.run();
}
