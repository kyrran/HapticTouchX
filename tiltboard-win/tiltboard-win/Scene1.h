#pragma once

#include <chai3d.h>
#include "GenericScene.h"
/**
* \file Scene1.h
*
* Header file for Scene 1 ("Easy" scene)
* 
*/
class Scene1: public GenericScene{
    public:
        /**
        * Constructor for Scene 1
        * 
        * \param a_hapticDevice A pointer to the connected haptic device
        */
        Scene1(std::shared_ptr<cGenericHapticDevice> a_hapticDevice);

        virtual ~Scene1() {};
    
    public:
        /**
        Determines the initial waypoints based on the position of the target.
        */
        void initWaypoints() override;

        /**
        * Generates and places the waypoints in the waypoint list based on the position of the target and main sphere.
        * @param positionSphere Position of the main sphere
        * @param positionTarget Position of target
        */
        void generateWaypoints(cVector3d positionSphere, cVector3d positionTarget) override;

        /**
        Sets up the fuzzy engine for the scene
        */
        void engineSetup();

};