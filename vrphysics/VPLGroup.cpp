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

VPLGroup::VPLGroup()
: _numVPLs(0)
{
    _fluxFactor = 1;
    _numVPLs = 256;
    _lowerThreshold = 0.15;
    
    osg::ref_ptr<osg::Node> loadedSphere = osgDB::readNodeFile("lightSphere.obj");
    _lightSphere = loadedSphere->asGroup()->getChild(0)->asGeode();
    
    // enable blending
    osg::ref_ptr<osg::BlendFunc> blendFunc(new osg::BlendFunc);
    osg::ref_ptr<osg::BlendEquation> blendEquation(new osg::BlendEquation);
    blendFunc->setFunction(GL_ONE, GL_ONE);
    blendEquation->setEquation(osg::BlendEquation::FUNC_ADD);
    
    osg::ref_ptr<osg::StateSet> ss = _lightSphere->getOrCreateStateSet();
    
    // disable depth test for blending
     ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    
    // enable blending
    ss->setMode(GL_BLEND, osg::StateAttribute::ON);
    ss->setAttributeAndModes(blendFunc, osg::StateAttribute::ON);
    ss->setAttributeAndModes(blendEquation, osg::StateAttribute::ON);
    
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
