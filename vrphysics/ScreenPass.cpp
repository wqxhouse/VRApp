//
//  ScreenPass.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "ScreenPass.h"

ScreenPass::ScreenPass(osg::Camera *mainCamera)
{
    _rootGroup = new osg::Group;
    
    _rttCamera = new osg::Camera;
    _mainCamera = mainCamera;
    _screenWidth = mainCamera->getViewport()->width();
    _screenHeight = mainCamera->getViewport()->height();
    _shaderProgram = new osg::Program;
}

ScreenPass::~ScreenPass()
{
}

// public
void ScreenPass::setShader(std::string vertex, std::string fragment)
{
    _shaderProgram->addShader(osgDB::readShaderFile(vertex));
    _shaderProgram->addShader(osgDB::readShaderFile(fragment));
}

// protected, default implementations
osg::ref_ptr<osg::Group> ScreenPass::createTexturedQuad()
{
    osg::ref_ptr<osg::Group> top_group = new osg::Group;
    
    osg::ref_ptr<osg::Geode> quad_geode = new osg::Geode;
    
    osg::ref_ptr<osg::Vec3Array> quad_coords = new osg::Vec3Array; // vertex coords
    // counter-clockwise
    quad_coords->push_back(osg::Vec3d(0, 0, -1));
    quad_coords->push_back(osg::Vec3d(1, 0, -1));
    quad_coords->push_back(osg::Vec3d(1, 1, -1));
    quad_coords->push_back(osg::Vec3d(0, 1, -1));
    
    osg::ref_ptr<osg::Vec2Array> quad_tcoords = new osg::Vec2Array; // texture coords
    quad_tcoords->push_back(osg::Vec2(0, 0));
    quad_tcoords->push_back(osg::Vec2(_screenWidth, 0));
    quad_tcoords->push_back(osg::Vec2(_screenWidth, _screenHeight));
    quad_tcoords->push_back(osg::Vec2(0, _screenHeight));
    
    osg::ref_ptr<osg::Geometry> quad_geom = new osg::Geometry;
    osg::ref_ptr<osg::DrawArrays> quad_da = new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4);
    
    osg::ref_ptr<osg::Vec4Array> quad_colors = new osg::Vec4Array;
    quad_colors->push_back(osg::Vec4(1.0f,0.0f,1.0f,1.0f));
    
    quad_geom->setVertexArray(quad_coords.get());
    quad_geom->setTexCoordArray(0, quad_tcoords.get());
    quad_geom->addPrimitiveSet(quad_da.get());
    quad_geom->setColorArray(quad_colors.get(), osg::Array::BIND_OVERALL);
    
    _stateSet = quad_geom->getOrCreateStateSet();
    _stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    quad_geode->addDrawable(quad_geom);
    top_group->addChild(quad_geode);
    return top_group;
}

int ScreenPass::addInTexture(osg::TextureRectangle *texture)
{
    _screenInTextures.push_back(texture);
    return (int)_screenInTextures.size() - 1;
}

int ScreenPass::addOutTexture()
{
    osg::ref_ptr<osg::TextureRectangle> tex = new osg::TextureRectangle;
        
    tex->setTextureSize(_screenWidth, _screenHeight);
    tex->setInternalFormat(GL_RGBA);
    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::LINEAR);
    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::LINEAR);
    
    _screenOutTexture.push_back(tex);
    
    return (int)_screenOutTexture.size() - 1;
}

osg::ref_ptr<osg::TextureRectangle> ScreenPass::getOutTexture(int _id)
{
    return _screenOutTexture[_id];
}

osg::ref_ptr<osg::TextureRectangle> ScreenPass::getInTexture(int _id)
{
    return _screenInTextures[_id];
}

void ScreenPass::setupCamera()
{
    // clearing
    _rttCamera->setClearColor(osg::Vec4(0.1f,0.1f,0.3f,1.0f));
//    _rttCamera->setClearColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
//    _rttCamera->setClearColor(osg::Vec4(0.4f,0.4f, 0.4f,1.0f));
    _rttCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // viewport
    _rttCamera->setViewport(0, 0, _screenWidth, _screenHeight);
    _rttCamera->setRenderOrder(osg::Camera::PRE_RENDER);
    _rttCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
}


