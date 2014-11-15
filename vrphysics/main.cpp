//
//  main.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include <stdio.h>
#include <osgDB/ReadFile>
#include <osgViewer/viewer>
#include <osg/Camera>
#include <osg/PolygonMode>

#include "GeometryPass.h"
#include "LightingPass.h"
#include "LightGroup.h"
#include "FinalPass.h"

osg::Camera *createHUDCamera(double left,
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

osg::Geode *createScreenQuad(float width,
                             float height,
                             float scaleX,
                             float scaleY,
                             osg::Vec3 corner)
{
    osg::Geometry* geom = osg::createTexturedQuadGeometry(
                                                          corner,
                                                          osg::Vec3(width, 0, 0),
                                                          osg::Vec3(0, height, 0),
                                                          0,
                                                          0,
                                                          scaleX,
                                                          scaleY);
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

osg::ref_ptr<osg::Camera> createTextureDisplayQuad(
                                                   const osg::Vec3 &pos,
                                                   osg::StateAttribute *tex,
                                                   float scaleX,
                                                   float scaleY,
                                                   float width,
                                                   float height)
{
    osg::ref_ptr<osg::Camera> hc = createHUDCamera(0,1,0,1);
    hc->addChild(createScreenQuad(width, height, scaleX, scaleY, pos));
    hc->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
    return hc;
}

osg::ref_ptr<osg::TextureRectangle> createTextureImage(const char *imageName)
{
    osg::ref_ptr<osg::TextureRectangle> texture(new osg::TextureRectangle);
    texture->setImage(osgDB::readImageFile(imageName));
    texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
    texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    return texture;
}

osg::ref_ptr<osg::Geode> createTexturedQuad(int _TextureWidth, int _TextureHeight)
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

int main()
{
    osgViewer::Viewer viewer;
    viewer.getCamera()->setViewport(new osg::Viewport(0, 0, 800, 600));
    osg::Camera *camera = viewer.getCamera();
    
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile("avatar.osg");
    osg::ref_ptr<osg::Group> sceneRoot(new osg::Group);
    
    LightGroup lightGroup;
    for(int i = 0; i < 50; i++)
    {
        lightGroup.addRandomLight();
    }
//
    lightGroup.addLight(osg::Vec3(5, 0, 0), osg::Vec3(1, 1, 0));
    lightGroup.addLight(osg::Vec3(5, 5, 5), osg::Vec3(1, 1, 0));
    lightGroup.addLight(osg::Vec3(-5, 5, 5), osg::Vec3(1, 0, 1));
    lightGroup.addLight(osg::Vec3(5, -5, 5), osg::Vec3(1, 1, 1));
    lightGroup.addLight(osg::Vec3(5, 5, -5), osg::Vec3(1, 0, 0));
    lightGroup.addLight(osg::Vec3(-5, 5, -5), osg::Vec3(0, 1, 1));
    lightGroup.addLight(osg::Vec3(-5, -5, 5), osg::Vec3(0, 1, 0));
    
    
    osg::ref_ptr<osg::Group> geometryGroup(new osg::Group);
    
    osg::ref_ptr<osg::MatrixTransform> enlargeAvatar(new osg::MatrixTransform);
    osg::Matrix tranlate;
    tranlate.makeTranslate(0, 0, -14);
    osg::Matrix enlarge;
    enlarge.makeScale(osg::Vec3(2, 2, 2));
    enlarge = enlarge * tranlate;
    enlargeAvatar->setMatrix(enlarge);
    enlargeAvatar->addChild(loadedModel);
    
    geometryGroup->addChild(enlargeAvatar);
    geometryGroup->addChild(lightGroup.getGeomTransformLightGroup());
    GeometryPass geomPass(camera, createTextureImage("concrete.jpg"), geometryGroup);
    sceneRoot->addChild(geomPass.getRoot());
    
    osg::ref_ptr<osg::Camera> qTexN =
    createTextureDisplayQuad(osg::Vec3(0, 0.7, 0),
                             geomPass.getNormalDepthOutTexture(),
                             800, 600, 0.3333, 0.3);
    
    osg::ref_ptr<osg::Camera> qTexD =
    createTextureDisplayQuad(osg::Vec3(0.3333, 0.7, 0),
                             geomPass.getPositionOutTexure(),
                             800, 600, 0.3333, 0.3);
    
    LightingPass lightPass(camera,
                           geomPass.getPositionOutTexure(),
                           geomPass.getAlbedoOutTexture(),
                           geomPass.getNormalDepthOutTexture(),
                           &lightGroup);
    
    sceneRoot->addChild(lightPass.getRoot());
    
    osg::ref_ptr<osg::Camera> qTexP =
    createTextureDisplayQuad(osg::Vec3(0.6666, 0.7, 0),
                             lightPass.getLightingOutTexture(),
                             800, 600, 0.3333, 0.3);
    
    FinalPass finalPass(camera, geomPass.getAlbedoOutTexture(),
                        lightPass.getLightingOutTexture());
    
    sceneRoot->addChild(finalPass.getRoot());
    osg::ref_ptr<osg::Camera> qTexF =
    createTextureDisplayQuad(osg::Vec3(0.0, 0.0, 0),
                             finalPass.getFinalPassTexture(),
                             800, 600, 1, 1);
    
    
    
    // ========================================================================
    auto plane = createTexturedQuad(320, 240);
    auto ss = plane->getOrCreateStateSet();
    
    // ======================================================================
    
    osg::ref_ptr<osg::Program> program(new osg::Program);
    program->addShader(osgDB::readShaderFile("v.vert"));
    program->addShader(osgDB::readShaderFile("v.frag"));
    ss->setAttributeAndModes(program, osg::StateAttribute::ON);
    ss->setTextureAttributeAndModes(0, geomPass.getOutputTexture(1), osg::StateAttribute::ON);
    
    sceneRoot->addChild(plane);
    
    sceneRoot->addChild(geometryGroup);
    
    sceneRoot->addChild(qTexF);
    
    sceneRoot->addChild(qTexN);
    sceneRoot->addChild(qTexD);
    sceneRoot->addChild(qTexP);
    
    viewer.setSceneData(sceneRoot);
    viewer.setUpViewInWindow(0, 0, 800, 600);
    viewer.run();
    
    return 0;
}