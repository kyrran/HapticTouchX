#pragma once

#include "chai3d.h"
#include "GenericScene.h"

/**
* \file Scene2.h
*
* Header file for Scene 2 ("Intermediate" scene)
*
*/
class Scene2 : public GenericScene {
public:
    /**
    * Constructor for Scene 2
    *
    * \param a_hapticDevice A pointer to the connected haptic device
    */
    Scene2(std::shared_ptr<cGenericHapticDevice> a_hapticDevice);

    virtual ~Scene2() {};

public:
    /**
    * Generates and places the waypoints in the waypoint list based on the position of the target and main sphere.
    * @param positionSphere Position of the main sphere
    * @param positionTarget Position of target
    */
    void generateWaypoints(cVector3d positionSphere, cVector3d positionTarget) override;

    /**
    * Generates and places the waypoints in the waypoint list based on the position of the target and main sphere.
    * @param positionSphere Position of the main sphere
    * @param positionTarget Position of target
    */
    void updateWaypoint(cVector3d positionSphere, cVector3d positionTarget) override;

    /**
    * Determines if the main sphere is in a forbidden zone
    * @param positionSphere Position of main sphere
    */
    bool inForbiddenZone(cVector3d positionSphere);
    /**
    * Sets up the fuzzy engine for the scene
    */
    void engineSetup();
    

public:
    //Determines the placement of the forbidden zones
    std::vector<cVector3d> fz = {cVector3d(- 0.04, -0.07, -0.2 + 0.00025), cVector3d(0.04, -0.07, -0.2 + 0.00025),
                                  cVector3d(- 0.04, 0.07, - 0.2 + 0.00025), cVector3d(0.04, 0.07, - 0.2 + 0.00025),
                                  cVector3d(-0.07, -0.05, -0.2 + 0.00025), cVector3d(-0.07, 0.05, -0.2 + 0.00025),
                                  cVector3d(0.07, -0.05, -0.2 + 0.00025), cVector3d(0.07, 0.05, -0.2 + 0.00025) };


};