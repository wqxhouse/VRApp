#include <osg/Group>

#include "Core.h"

#include "AssetDB.h"
#include "DirectionalLightGroup.h"

void handleGeometries(const osg::ref_ptr<osg::Group> geoms, Assets *const asset)
{
    asset->addGeometryWithFile("Testing/gi_test.dae");
}

void handleDirLights(DirectionalLightGroup *const dirLightGroup)
{
}

int main()
{
    Core core;
    core.setGeometryHandler(handleGeometries);
    core.setDirectionalLightHandler(handleDirLights);
    core.setWindowSize(1280, 720);
    
    core.run();
}
