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
#include <osg/Camera>
#include <osg/StateSet>
#include <osg/FrameBufferObject>
#include "ScreenPass.h"
#include "AssetDB.h"

class GeometryPassCallback : public osg::StateSet::Callback
{
public:
    GeometryPassCallback(osg::Camera *mainCamera);
    virtual ~GeometryPassCallback();
    
private:
    void getNearFarPlane();
    virtual void operator ()(osg::StateSet *ss, osg::NodeVisitor *visitor);
    
    osg::ref_ptr<osg::Camera> _mainCamera;
    float _nearPlane;
    float _farPlane;
};

class GeometryPass : public ScreenPass
{
public:
    
    GeometryPass(osg::Camera *mainCamera, AssetDB *assetDB, osg::Group *geomRootNode);
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
    
    inline osg::ref_ptr<osg::Texture2D> getSharedDepthStencilTexture()
    {
        return _sharedDepthStencilTex;
    }
    
protected:
    virtual void configureStateSet();
private:
    void configSharedDepthStencilTexture();
    
    float _nearPlaneDist;
    float _farPlaneDist;
    
    int _out_albedo_tex_id;
    int _out_normal_depth_tex_id;
    
    int _out_position_tex_id;
    
    AssetDB *_assetDB;
    osg::ref_ptr<osg::Group> _geomRootNode;
   
    int _gbuffer_tex_shader;
    int _gbuffer_notex_shader;
    
    osg::ref_ptr<osg::Texture2D> _sharedDepthStencilTex;
};

#endif /* defined(__vrphysics__GeometryPass__) */
