#include <osg/Group>

#include "Core.h"

#include "AssetDB.h"
#include "DirectionalLightGroup.h"

void handleGeometries(const osg::ref_ptr<osg::Group> geoms, Assets *const asset)
{
//    asset->addGeometryWithFile("Testing/gi_test.dae");
    asset->addGeometryWithFile("Testing/test2gi.dae");
//    asset->addGeometryWithFile("Testing/uuu.dae");
}

void handleDirLights(DirectionalLightGroup *const dirLightGroup)
{
//    dirLightGroup->addLight(osg::Vec3(5, 5, 5), osg::Vec3(0, 0, 0), osg::Vec3(1, 0.5, 0), true);
}

int main()
{
    Core core;
    core.setGeometryHandler(handleGeometries);
    core.setDirectionalLightHandler(handleDirLights);
    core.setWindowSize(800, 600);
    
    core.run();
}
