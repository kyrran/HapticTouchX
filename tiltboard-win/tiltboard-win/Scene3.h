#pragma once

#include "chai3d.h"
#include "GenericScene.h"

class Scene3: public GenericScene{
    public:
        /**
        * \file Scene3.h
        *
        * Header file for Scene 3 ("Hard" scene)
        *
        */
        Scene3(std::shared_ptr<cGenericHapticDevice> a_hapticDevice);

        virtual ~Scene3() {};
    
    public:
        /**
        Initialises first waypoints
        */
        void initWaypoints() override;

        /**
        * Generates and places the waypoints in the waypoint list based on the position of the target and main sphere.
        * @param positionSphere Position of the main sphere
        * @param positionTarget Position of target
        */
        void generateWaypoints(cVector3d positionSphere, cVector3d positionTarget) override;

        /**
        * Sets up the fuzzy engine for the scene
        */
        void engineSetup();

};