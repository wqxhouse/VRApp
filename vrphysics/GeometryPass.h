//
//  GeometryPass.h
//  vrphysics
//
//  Created by Robin Wu on 11/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__GeometryPass__
#define __vrphysics__GeometryPass__

#include <stdio.h>
#include "ScreenPass.h"
#include "AssetDB.h"

class GeometryPass : public ScreenPass
{
public:
    
    GeometryPass(osg::Camera *mainCamera, AssetDB *assetDB);
    virtual ~GeometryPass();
    
    osg::ref_ptr<osg::TextureRectangle> getAlbedoOutTexture()
    {
        return getOutputTexture(_out_albedo_tex_id);
    }
    
    osg::ref_ptr<osg::TextureRectangle> getNormalDepthOutTexture()
    {
        return getOutputTexture(_out_normal_depth_tex_id);
    }
    
    osg::ref_ptr<osg::TextureRectangle> getPositionOutTexure()
    {
        return getOutputTexture(_out_position_tex_id);
    }
   
    // overload
    virtual int addOutTexture(bool isDepth);
    
protected:
    virtual void configureStateSet();
private:
    float _nearPlaneDist;
    float _farPlaneDist;
    osg::ref_ptr<osg::Node> _worldObjects;
    
    int _out_albedo_tex_id;
    int _out_normal_depth_tex_id;
    
    int _out_position_tex_id;
    
    AssetDB *_assetDB;
   
    int _gbuffer_tex_shader;
    int _gbuffer_notex_shader;
};

#endif /* defined(__vrphysics__GeometryPass__) */
