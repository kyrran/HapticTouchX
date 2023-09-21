#include "Scene2.h"
#include <fstream>
#include <iostream>
using namespace std;

/**
* @file Scene2.cpp
*
* Scene 2 ("Intermediate" scene) implementation
*/
Scene2::Scene2(shared_ptr<cGenericHapticDevice> a_hapticDevice):GenericScene(a_hapticDevice){
    
    // Describe the material to be used for obstacles
    cMaterial matBase;
    matBase.setGrayLevel(0.3);
    matBase.setStiffness(100);
    matBase.setDynamicFriction(0.5);
    matBase.setStaticFriction(0.9);

    // Set up static obstacles
    borderSetup({ 0.005, 0.33, 0.03 }, { 0.16, 0.18, -0.2 }, {0,0,-45}, matBase);
    borderSetup({ 0.005, 0.33, 0.03 }, { 0.18, 0.16, -0.2 }, {0,0,-45}, matBase);
    borderSetup({ 0.005, 0.33, 0.03 }, { -0.18, -0.16, -0.2 }, { 0,0,-45 }, matBase);
    borderSetup({ 0.005, 0.33, 0.03 }, { -0.16, -0.18, -0.2 }, { 0,0,-45 }, matBase);
    borderSetup({ 0.005, 0.33, 0.03 }, { 0.18, -0.16, -0.2 }, { 0,0,45 }, matBase);
    borderSetup({ 0.005, 0.33, 0.03 }, { 0.16, -0.18, -0.2 }, { 0,0,45 }, matBase);
    borderSetup({ 0.005, 0.33, 0.03 }, { -0.16, 0.18, -0.2 }, { 0,0,45 }, matBase);
    borderSetup({ 0.005, 0.33, 0.03 }, { -0.18, 0.16, -0.2 }, { 0,0,45 }, matBase);

    // Set up list of destinations
    destinations.push_back(cVector3d(-0.28, 0.28, -0.2+0.00025));
    destinations.push_back(cVector3d(0.28, 0.28, -0.2+0.00025));
    destinations.push_back(cVector3d(-0.28, -0.28, -0.2+0.00025));
    destinations.push_back(cVector3d(0.28, -0.28, -0.2+0.00025));
    destinations.push_back(cVector3d(0, -0.25, -0.2+0.00025));
    destinations.push_back(cVector3d(0, 0.25, -0.2+0.00025));
    destinations.push_back(cVector3d(-0.25, 0.0, -0.2+0.00025));
    destinations.push_back(cVector3d(0.25, 0.0, -0.2+0.00025));

    // Set up list of checkpoints
    checkpoints.push_back(cVector3d(-0.05, 0.05, -0.2 + 0.00025));
    checkpointsRange.push_back(0.02);
    checkpoints.push_back(cVector3d(0.05, 0.05, -0.2 + 0.00025));
    checkpointsRange.push_back(0.02);
    checkpoints.push_back(cVector3d(-0.05, -0.05, -0.2 + 0.00025));
    checkpointsRange.push_back(0.02);
    checkpoints.push_back(cVector3d(0.05, -0.05, -0.2 + 0.00025));
    checkpointsRange.push_back(0.02);
    checkpoints.push_back(cVector3d(0, -0.05, -0.2 + 0.00025));
    checkpointsRange.push_back(0.02);
    checkpoints.push_back(cVector3d(0, 0.05, -0.2 + 0.00025));
    checkpointsRange.push_back(0.02);
    checkpoints.push_back(cVector3d(-0.05, 0.0, -0.2 + 0.00025));
    checkpointsRange.push_back(0.02);
    checkpoints.push_back(cVector3d(0.05, 0.0, -0.2 + 0.00025));
    checkpointsRange.push_back(0.02);
    checkpoints.push_back(cVector3d(-0.00, -0.0, -0.20025));
    checkpointsRange.push_back(0.02);

    // Set up target
    target = new cBulletCylinder(bulletWorld, 0.0005, toolRadius*3);
    bulletWorld->addChild(target);
    target->createAABBCollisionDetector(toolRadius);
    target->setMaterial(matBase);
    target->buildDynamicModel();
    target->m_material->setPurpleIndigo();
    target->m_bulletRigidBody->setCollisionFlags(target->m_bulletRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    target->m_bulletRigidBody->setUserPointer(target);

    // Set up fuzzy engine
    engineSetup();

}

void Scene2::generateWaypoints(cVector3d positionSphere, cVector3d positionTarget){
    for(int i = 0; i < destinations.size();i++){
        if(positionTarget.equals(destinations[i]))
        {
            waypoints.push_back(checkpoints[i]);
            waypointsRange.push_back(checkpointsRange[i]);
            waypoints.push_back(destinations[i]);
            waypointsRange.push_back(0.01);
            waypoints.push_back(checkpoints[i]);
            waypointsRange.push_back(checkpointsRange[i]);
            break;
        }
       
    }
}

void Scene2::updateWaypoint(cVector3d positionSphere, cVector3d positionTarget){
    if (cDistance(positionSphere, positionTarget) < waypointsRange[waypointsRange.size() - 2]) {
        updateTarget();
        generateWaypoints(positionSphere, target->getLocalPos());
        return;
    }

    if ((cDistance(positionSphere, waypoints[waypoint_index + 1]) < cDistance(waypoints[waypoint_index], waypoints[waypoint_index + 1]) * 0.7) &&
        waypoint_index != waypoints.size() - 2 && !inForbiddenZone(positionSphere)) {
        cout << "Advancing to waypoint " << waypoint_index + 1 << endl;
        waypoint_index += 1;
        last_waypoint_index += 1;
    }

    if(waypoint_index == waypoints.size()-2){
         if(waypoint_index == last_waypoint_index)
        {
             waypoint_index = last_waypoint_index + 1;
            return;

        }
         else if (abs(positionTarget.get(0)) == abs(positionTarget.get(1)) && inForbiddenZone(positionSphere)){
             last_waypoint_index -= 1;
             waypoint_index = last_waypoint_index;
         }
    }
    else
    {
        if(waypoint_index == last_waypoint_index)
        {
            waypoint_index = last_waypoint_index + 1;
        }

        if(waypoint_index == waypoints.size()-1){
            updateTarget();
            generateWaypoints(positionSphere, target->getLocalPos());
        }

        waypoint_index = waypoint_index % waypoints.size();
        positionWaypoint = waypoints[waypoint_index];
    }
}


bool Scene2::inForbiddenZone(cVector3d positionSphere) {
    for (int i = 0; i < fz.size(); i += 2) {
        float line = cDistance(fz.at(i), fz.at(i+1));
            float line1 = cDistance(fz.at(i), positionSphere);
            float line2 = cDistance(positionSphere, fz.at(i+1));
            if (line1 + line2 <= (line+0.01)) {
                return true;
            }
    }
    return false;
}


void Scene2::engineSetup() {
    using namespace fl;

    InputVariable* time = new InputVariable;
    time->setName("time");
    time->setDescription("");
    time->setEnabled(true);
    time->setRange(0.000, 120.000);
    time->setLockValueInRange(false);
    time->addTerm(new Bell("acceptable", 0.000, 40.000, 5.000));
    time->addTerm(new Gaussian("slow", 70.000, 15.000));
    time->addTerm(new Bell("veryslow", 120.000, 30.000, 5.000));
    engine->addInputVariable(time);

    InputVariable* collisions = new InputVariable;
    collisions->setName("collisions");
    collisions->setDescription("");
    collisions->setEnabled(true);
    collisions->setRange(0.000, 5000.000);
    collisions->setLockValueInRange(false);
    collisions->addTerm(new Trapezoid("flawless", 0.000, 0.000, 500.000, 1000.000));
    collisions->addTerm(new Trapezoid("good", 0.000, 1000.000, 1500.000, 2000.000));
    collisions->addTerm(new Trapezoid("OK", 1000.000, 2000.000, 2500.000, 3000.000));
    collisions->addTerm(new Trapezoid("bad", 2000.000, 3000.000, 3500.000, 4000.000));
    collisions->addTerm(new Trapezoid("terrible", 3500.000, 4000.000, 5000.000, 99999.000));
    engine->addInputVariable(collisions);

    OutputVariable* alphachange = new OutputVariable;
    alphachange->setName("alphachange");
    alphachange->setDescription("");
    alphachange->setEnabled(true);
    alphachange->setRange(-1.000, 1.000);
    alphachange->setLockValueInRange(false);
    alphachange->setAggregation(new Maximum);
    alphachange->setDefuzzifier(new Centroid(100));
    alphachange->setDefaultValue(fl::nan);
    alphachange->setLockPreviousValue(false);
    alphachange->addTerm(new Trapezoid("greatdeduction", -1.000, -1.000, -0.800, -0.600));
    alphachange->addTerm(new Trapezoid("smalldeduction", -0.800, -0.600, -0.400, -0.200));
    alphachange->addTerm(new Gaussian("menialchange", 0.000, 0.100));
    alphachange->addTerm(new Trapezoid("smallincrease", 0.200, 0.400, 0.600, 0.800));
    alphachange->addTerm(new Trapezoid("greatincrease", 0.600, 0.800, 1.000, 1.000));
    engine->addOutputVariable(alphachange);

    RuleBlock* ruleBlock = new RuleBlock;
    ruleBlock->setName("");
    ruleBlock->setDescription("");
    ruleBlock->setEnabled(true);
    ruleBlock->setConjunction(new Minimum);
    ruleBlock->setDisjunction(new Maximum);
    ruleBlock->setImplication(new Minimum);
    ruleBlock->setActivation(new General);
    ruleBlock->addRule(Rule::parse("if time is acceptable and collisions is flawless then alphachange is greatincrease", engine));
    ruleBlock->addRule(Rule::parse("if time is acceptable and collisions is good then alphachange is smallincrease", engine));
    ruleBlock->addRule(Rule::parse("if time is acceptable and collisions is OK then alphachange is menialchange", engine));
    ruleBlock->addRule(Rule::parse("if time is acceptable and collisions is bad then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is acceptable and collisions is terrible then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is slow and collisions is flawless then alphachange is smallincrease", engine));
    ruleBlock->addRule(Rule::parse("if time is slow and collisions is good then alphachange is menialchange", engine));
    ruleBlock->addRule(Rule::parse("if time is slow and collisions is OK then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is slow and collisions is bad then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is slow and collisions is terrible then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is veryslow and collisions is flawless then alphachange is menialchange", engine));
    ruleBlock->addRule(Rule::parse("if time is veryslow and collisions is good then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is veryslow and collisions is OK then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is veryslow and collisions is bad then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is veryslow and collisions is terrible then alphachange is greatdeduction", engine));
    engine->addRuleBlock(ruleBlock);
}