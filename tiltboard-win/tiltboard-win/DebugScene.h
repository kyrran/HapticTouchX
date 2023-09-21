#pragma once

#include <chai3d.h>
#include "GenericScene.h"

class DebugScene: public GenericScene {
    public:
        DebugScene(std::shared_ptr<cGenericHapticDevice> a_hapticDevice);

        void setStiffness(double a_stiffness);
        void initWaypoints();
        void generateWaypoints(cVector3d positionSphere, cVector3d positionTarget);

        virtual ~DebugScene() {};

};
