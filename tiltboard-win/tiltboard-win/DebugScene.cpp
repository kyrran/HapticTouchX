#include "DebugScene.h"
#include "CBullet.h"
#include <fstream>

using namespace std;

DebugScene::DebugScene(std::shared_ptr<cGenericHapticDevice> a_hapticDevice):GenericScene(a_hapticDevice){
	
    hapticDevice = a_hapticDevice;

    cMaterial matBase;
    matBase.setGrayLevel(0.3);
    matBase.setStiffness(100);
    matBase.setDynamicFriction(0.5);
    matBase.setStaticFriction(0.9);

    borderSetup({ 0.005,0.5,0.02 }, { -0.20, -0.05, -0.2 + toolRadius }, { 0,0,0 }, matBase);
    borderSetup({ 0.005, 0.5, 0.02 }, { 0.25, 0.05, -0.2 }, { 0,0,0 }, matBase);

    destinations.push_back(cVector3d(0.28, 0.28, -0.2 + 0.00025));
    destinations.push_back(cVector3d(-0.28, -0.28, -0.2 + 0.00025));
    
    checkpoints.push_back(cVector3d(- 0.26, 0.23, -0.20025));
    checkpointsRange.push_back(0.04);
    checkpoints.push_back(cVector3d(-0.16, 0.23, -0.20025));
    checkpointsRange.push_back(0.04);
    checkpoints.push_back(cVector3d(0.20, -0.23, -0.20025));
    checkpointsRange.push_back(0.04);
    checkpoints.push_back(cVector3d(0.28, -0.23, -0.20025));
    checkpointsRange.push_back(0.04);

    // set up target
    target = new cBulletCylinder(bulletWorld, 0.0005, toolRadius * 1.5);
    bulletWorld->addChild(target);
    target->createAABBCollisionDetector(toolRadius);
    target->setMaterial(matBase);
    target->buildDynamicModel();
    target->m_material->setGreen();
    target->m_bulletRigidBody->setCollisionFlags(target->m_bulletRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    target->m_bulletRigidBody->setUserPointer(target);
}

void DebugScene::setStiffness(double a_stiffness) {
    bulletGround->setStiffness(a_stiffness);
}

void DebugScene::initWaypoints() {
    if ((target->getLocalPos()).equals(destinations[0])) {
        last_waypoint_index = -1;
        waypoint_index = 0;
    }
    else if ((target->getLocalPos()).equals(destinations[1]) && last_waypoint_index == 4) {
        last_waypoint_index = 4;
        waypoint_index = 3;
    }
    else if ((target->getLocalPos()).equals(destinations[1])) {
        last_waypoint_index = 1;
        waypoint_index = 2;
    }
}
void DebugScene::generateWaypoints(cVector3d positionSphere, cVector3d positionTarget) {
    if (positionTarget.equals(destinations[0])) {
        for (int i = 0; i < checkpoints.size(); i++) {
            waypoints.push_back(checkpoints[i]);
            waypointsRange.push_back(checkpointsRange[i]);
        }

        waypoints.push_back(positionTarget);
        waypointsRange.push_back(0.005);
        waypoints.push_back(checkpoints.back());
        waypointsRange.push_back(checkpointsRange.back());
    }
    else if (positionTarget.equals(destinations[1])) {
        for (int i = checkpoints.size() - 1; i >= 0; i--) {
            waypoints.push_back(checkpoints[i]);
            waypointsRange.push_back(checkpointsRange[i]);
        }

        waypoints.push_back(positionTarget);
        waypointsRange.push_back(0.01);
        waypoints.push_back(checkpoints.back());
        waypointsRange.push_back(checkpointsRange.back());
    }
}

