//
//  VPLGroup.h
//  vrphysics
//
//  Created by Robin Wu on 2/6/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#ifndef __vrphysics__VPLGroup__
#define __vrphysics__VPLGroup__

#include <stdio.h>
#include <osgDB/ReadFile>

class VPLGroup
{
public:
	VPLGroup();
	~VPLGroup();

	void setNumVpls(int num);

	inline int getNumVpls()
	{
		return _numVPLs;
	}

	inline float getFluxFactor()
	{
		return _fluxFactor;
	}

	inline float getLowerThreshold()
	{
		return _lowerThreshold;
	}

	inline void setFluxFactor(float factor)
	{
		_fluxFactor = factor;
	}

	inline void setLowerThreshold(float ilow)
	{
		_lowerThreshold = ilow;
	}
    
    inline osg::ref_ptr<osg::Geode> getLightSphereGeode()
    {
        return _lightSphere;
    }
    
    void configStencilPass();
    void configRenderPass();

private:
	void applyInstanceNumberChange();
    
private:
    osg::ref_ptr<osg::Geode> _lightSphere;
    
    osg::ref_ptr<osg::Group> _vplLightGroupNode;
    osg::ref_ptr<osg::Group> _stencilPass;
    osg::ref_ptr<osg::Group> _renderPass;
    
    int _numVPLs;
    float _fluxFactor;
    float _lowerThreshold;
};

#endif /* defined(__vrphysics__VPLGroup__) */
