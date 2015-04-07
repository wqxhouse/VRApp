//
//  VPLGroup.cpp
//  vrphysics
//
//  Created by Robin Wu on 2/6/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#include "VPLGroup.h"
#include <osg/BlendFunc>
#include <osg/BlendEquation>
#include <osg/Depth>
#include <osg/PolygonMode>
#include <osg/CullFace>

VPLGroup::VPLGroup()
: _numVPLs(0)
{
    _fluxFactor = 1;
    _numVPLs = 256;
    _lowerThreshold = 0.15;
    
    osg::ref_ptr<osg::Node> loadedSphere = osgDB::readNodeFile("lightSphere.obj");
    _lightSphere = loadedSphere->asGroup()->getChild(0)->asGeode();
    
    _stencilPass = new osg::Group;
    _renderPass = new osg::Group;
    
    _stencilPass->addChild(_lightSphere);
    _renderPass->addChild(_lightSphere);
    
    
    // enable blending
    osg::ref_ptr<osg::BlendFunc> blendFunc(new osg::BlendFunc);
    osg::ref_ptr<osg::BlendEquation> blendEquation(new osg::BlendEquation);
    blendFunc->setFunction(GL_ONE, GL_ONE);
    blendEquation->setEquation(osg::BlendEquation::FUNC_ADD);
    
    osg::ref_ptr<osg::StateSet> ss = _lightSphere->getOrCreateStateSet();
    
    // disable depth test for blending
    // testing against non-light geometry depth buffer
    ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON);
    osg::ref_ptr<osg::Depth> depth(new osg::Depth());
    depth->setFunction(osg::Depth::GEQUAL);
    depth->setWriteMask(false);
    ss->setAttribute(depth);
    
    // enable blending
    ss->setMode(GL_BLEND, osg::StateAttribute::ON);
    ss->setMode(GL_ALPHA_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
    ss->setAttributeAndModes(blendFunc, osg::StateAttribute::ON);
    ss->setAttributeAndModes(blendEquation, osg::StateAttribute::ON);
    
//    osg::ref_ptr<osg::PolygonMode> polyMode(new osg::PolygonMode);
//    polyMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
//    ss->setAttribute(polyMode);
    
    ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
    osg::ref_ptr<osg::CullFace> cullFace = new osg::CullFace;
    cullFace->setMode(osg::CullFace::FRONT);
    ss->setAttribute(cullFace);
//
    applyInstanceNumberChange();
}

VPLGroup::~VPLGroup()
{
}

void VPLGroup::applyInstanceNumberChange()
{
    for(int i = 0; i < _lightSphere->getNumDrawables(); i++)
    {
        osg::ref_ptr<osg::Drawable> drawable = _lightSphere->getDrawable(i);
        osg::ref_ptr<osg::Geometry> geom;
        // here sphere only look for geometry
        if((geom = drawable->asGeometry()) != NULL)
        {
            for(int j = 0; j < geom->getNumPrimitiveSets(); j++)
            {
                osg::ref_ptr<osg::PrimitiveSet> primSet = geom->getPrimitiveSet(j);
                primSet->setNumInstances(_numVPLs);
            }
            
            // though unnessary after being set for the first time
            geom->setUseDisplayList(false);
            geom->setUseVertexBufferObjects(true);
        }
        else
        {
            // something wrong here
            std::cerr << "VPLGroup:: something wrong when loading sphere drawable" << std::endl;
        }
    }
}

void VPLGroup::setNumVpls(int num)
{
    _numVPLs = num;
    applyInstanceNumberChange();
}

void VPLGroup::configStencilPass()
{
//    osg::ref_ptr<osg::Group> stencilGroup(new osg::Group);
//    osg::ref_ptr<osg::StateSet> sss = stencilGroup->getOrCreateStateSet();
//    osg::ref_ptr<osg::ColorMask> colorMask(new osg::ColorMask);
//    colorMask->setMask(false, false, false, false);
//    sss->setAttribute(colorMask, osg::StateAttribute::OVERRIDE);
//    
//    sss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);
//    sss->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
//    
//    osg::ref_ptr<osg::StencilTwoSided> stencil(new osg::StencilTwoSided);
//    stencil->setFunction(osg::StencilTwoSided::FRONT, osg::StencilTwoSided::ALWAYS, 0, 0);
//    stencil->setFunction(osg::StencilTwoSided::BACK, osg::StencilTwoSided::ALWAYS, 0, 0);
//    stencil->setOperation(osg::StencilTwoSided::FRONT, osg::StencilTwoSided::KEEP,
//                          osg::StencilTwoSided::DECR_WRAP, osg::StencilTwoSided::KEEP);
//    stencil->setOperation(osg::StencilTwoSided::BACK, osg::StencilTwoSided::KEEP,
//                          osg::StencilTwoSided::INCR_WRAP, osg::StencilTwoSided::KEEP);
//    
//    stencil->setWriteMask(osg::StencilTwoSided::FRONT, 0xFF);
//    stencil->setWriteMask(osg::StencilTwoSided::BACK, 0xFF);
//    
//    sss->setAttributeAndModes(stencil, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
//    sss->setRenderBinDetails(0, "RenderBin");
//    
//    sss->setAttributeAndModes(getShader(_stencil_shader_id), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
//    // under same node, better use different uniform name for the same thing,
//    // seems like a bug in osg that same name uniform may have conflicts, overriding each other.
//    sss->addUniform(new osg::Uniform("u_projectionMat", osg::Matrixf(_mainCamera->getProjectionMatrix())));
//    sss->addUniform(new osg::Uniform("u_viewMat", osg::Matrixf(_mainCamera->getViewMatrix())));
//    sss->setUpdateCallback(new LightStencilCallback(_mainCamera));
//    
//    std::vector<PointLight *> &pointLights = _lightGroup->getPointLightsArrayReference();
//    for (std::vector<PointLight *>::iterator it = pointLights.begin(); it != pointLights.end(); it++)
//    {
//        osg::ref_ptr<osg::MatrixTransform> mt = (*it)->getLightSphereTransformNode();
//        stencilGroup->addChild(mt);
//    }
    
//    _lightPassGroupNode->addChild(stencilGroup);
}

void VPLGroup::configRenderPass()
{
    
}
