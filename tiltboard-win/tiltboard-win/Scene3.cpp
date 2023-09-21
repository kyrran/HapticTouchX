#include "Scene3.h"
#include <iostream>
#include <fstream>
using namespace std;

/**
* @file Scene3.cpp
* 
* Scene 3 ("Hard" Scene) Implementation
*/
Scene3::Scene3(shared_ptr<cGenericHapticDevice> a_hapticDevice):GenericScene(a_hapticDevice){
    
    cMaterial matBase;
    matBase.setGrayLevel(0.3);
    matBase.setStiffness(100);
    matBase.setDynamicFriction(0.5);
    matBase.setStaticFriction(0.9);
    double x = -0.013;
    double y = 0;

    for(int i =0; i<20; i++){
        int direction = i%2 == 0 ? 90:0;
        x += (pow(-1,i/2))*(0.013 + (i/2)*0.013);
        y += (pow(-1,(i+3)/2))*(((i+3)/2)*0.013);

        borderSetup({ 0.006, 0.026 * (i / 2 + 1.5), 0.015 }, { x, y, -0.2 }, { 0,0,(direction * 1.0) }, matBase);
    }

    
    destinations.push_back(cVector3d(0.00,0.00,-0.2+0.00025));
    destinations.push_back(cVector3d(-0.28,-0.28,-0.2+0.00025));


    checkpoints.push_back(cVector3d(-0.029, 0.02, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(-0.029, -0.028, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(0.028, -0.028, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(0.028, 0.052, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(-0.053, 0.052, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(-0.053, -0.052, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(0.055, -0.052, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(0.055, 0.077, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(-0.080, 0.077, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(-0.080, -0.080, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(0.080, -0.080, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(0.080, 0.105, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(-0.105, 0.105, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(-0.105, -0.105, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(0.105, -0.105, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(0.105, 0.130, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(-0.130, 0.130, -0.20025));
    checkpointsRange.push_back(0.01);
    checkpoints.push_back(cVector3d(-0.130, -0.140, -0.20025));
    checkpointsRange.push_back(0.01);

    // set up target
    target = new cBulletCylinder(bulletWorld, 0.0005, toolRadius*1.5);
    bulletWorld->addChild(target);
    target->createAABBCollisionDetector(toolRadius);
    target->setMaterial(matBase);
    target->buildDynamicModel();
    target->m_material->setPurpleIndigo();
    target->m_bulletRigidBody->setCollisionFlags(target->m_bulletRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    target->m_bulletRigidBody->setUserPointer(target);

    //set up fuzzy engine
    engineSetup();

}

void Scene3::initWaypoints(){
    last_waypoint_index = 0;
    waypoint_index = 0;
}

void Scene3::generateWaypoints(cVector3d positionSphere, cVector3d positionTarget){
    
    if(positionTarget.equals(destinations[1]))
    {   
        for(int i = 0; i < checkpoints.size(); i++){
            waypoints.push_back(checkpoints[i]);
            waypointsRange.push_back(checkpointsRange[i]);
        }
        waypoints.push_back(positionTarget);
        waypointsRange.push_back(0.01);
        waypoints.push_back(checkpoints[checkpoints.size()-1]);
        waypointsRange.push_back(checkpointsRange[checkpoints.size()-1]);
    }
    else if(positionTarget.equals(destinations[0]))
    {
        for(int i = checkpoints.size()-1; i >= 0; i--){
            waypoints.push_back(checkpoints[i]);
            waypointsRange.push_back(checkpointsRange[i]);
        }
        waypoints.push_back(positionTarget);
        waypointsRange.push_back(0.01);
        waypoints.push_back(checkpoints[0]);
        waypointsRange.push_back(checkpointsRange[0]);

    }
}


void Scene3::engineSetup() {
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