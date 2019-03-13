//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

Vehicle::Vehicle() : mCompoundShape(null), mBodyShape(null), mBody(null), mMotionState(null), mVehicle(null), mRayCaster(null)
{
}

//////////////////////////////////////////////////////////////////////

Vehicle::~Vehicle()
{
}

//////////////////////////////////////////////////////////////////////

int Vehicle::Create(btTransform transform)
{
    mWheelRadius = 2.0f;
    mWheelWidth = 0.5f;
    mEngineForce = 0;
    mBrakeForce = 0;
    mSteerAngle = 0;

    mStartTransform = transform;

    mBodyShape = new btBoxShape(btVector3(3, 6, 0.75f));

    btTransform chassisTransform;
    chassisTransform.setIdentity();
    chassisTransform.setOrigin(btVector3(0, 0, 0.7f));

    mCompoundShape = new btCompoundShape();
    mCompoundShape->addChildShape(chassisTransform, mBodyShape);

    mMotionState = new btDefaultMotionState(mStartTransform);
    btScalar mass = 2000;
    btVector3 inertia;
    mCompoundShape->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo bodyInfo(mass, mMotionState, mBodyShape, inertia);
    bodyInfo.m_friction = 0.6f;
    bodyInfo.m_restitution = 0.6f;
    bodyInfo.m_linearDamping = 0.1f;
    bodyInfo.m_angularDamping = 0.1f;

    mBody = Physics::CreateRigidBody(mass, chassisTransform, mCompoundShape, &bodyInfo);
    Physics::AddRigidBody(mBody, Physics::CarMask, -1);
    mBody->setActivationState(DISABLE_DEACTIVATION);

    mTuning.m_suspensionStiffness = btScalar(5.88) * 4;
    mTuning.m_suspensionCompression = btScalar(0.83) * 4;
    mTuning.m_suspensionDamping = btScalar(0.88) * 4;
    mTuning.m_maxSuspensionTravelCm = btScalar(500.);
    mTuning.m_frictionSlip = btScalar(10.5) / 4;
    mTuning.m_maxSuspensionForce = btScalar(6000) * 2;

    mRayCaster = new MyVehicleRaycaster(Physics::DynamicsWorld);
    mVehicle = new btRaycastVehicle(mTuning, mBody, mRayCaster);

    mVehicle->setCoordinateSystem(0, 2, 1);

    Physics::DynamicsWorld->addAction(mVehicle);

    btVector3 wheelDir(0, 0, -1);
    btVector3 axleDir(1, 0, 0);
    btScalar suspensionRestLength = 1.0f;
    btScalar connectionHeight = 0.7f;

    Vec4f extents = mBodyShape->getHalfExtentsWithMargin().get128();
    float width = GetX(extents) + mWheelWidth + 0.1f;
    float length = GetY(extents) - mWheelRadius;
    float height = GetZ(extents);

    mVehicle->addWheel(btVector3(-width, +length, connectionHeight), wheelDir, axleDir, suspensionRestLength, mWheelRadius, mTuning, true);
    mVehicle->addWheel(btVector3(+width, +length, connectionHeight), wheelDir, axleDir, suspensionRestLength, mWheelRadius, mTuning, true);
    mVehicle->addWheel(btVector3(+width, -length, connectionHeight), wheelDir, axleDir, suspensionRestLength, mWheelRadius, mTuning, false);
    mVehicle->addWheel(btVector3(-width, -length, connectionHeight), wheelDir, axleDir, suspensionRestLength, mWheelRadius, mTuning, false);

    Reset();
    return S_OK;
}

//////////////////////////////////////////////////////////////////////

void Vehicle::Release()
{
}

//////////////////////////////////////////////////////////////////////

void Vehicle::Draw(MyDXWindow *w)
{
    btDefaultMotionState *ms = (btDefaultMotionState *)mBody->getMotionState();
    Physics::DrawShape(Physics::btTransformToMatrix(ms->m_graphicsWorldTrans), mCompoundShape, (iPhysicsRenderer *)w);

    for(int i = 0; i < mVehicle->getNumWheels(); ++i) {
        // DrawCylinder uses Y cylinders
        btTransform const &t = mVehicle->getWheelTransformWS(i);
        Matrix wheelMatrix = ScaleMatrix(Vec4(mWheelRadius, mWheelWidth, mWheelRadius)) * RotationMatrix(Vec4(0, 0, PI / 2)) * Physics::btTransformToMatrix(t);
        w->DrawCylinder(wheelMatrix);
        if(Keyboard::Held('1')) {
            // debug_cylinder uses Z cylinders
            Matrix wheelMatrix = ScaleMatrix(Vec4(mWheelRadius, mWheelRadius, mWheelWidth)) * RotationMatrix(Vec4(0, PI / 2, 0)) * Physics::btTransformToMatrix(t);
            debug_cylinder(wheelMatrix, Color::White);
        }
    }
}

//////////////////////////////////////////////////////////////////////

void Vehicle::UnFlip()
{
    btTransform const &car = mVehicle->getChassisWorldTransform();
    btTransform t;
    t.setOrigin(car.getOrigin() + btVector3(0, 0, 5));
    btQuaternion q;
    q.setRotation(btVector3(0, 0, 1), car.getRotation().getAngle());
    t.setRotation(q);
    mSteerAngle = 0;
    mEngineForce = 0;
    mBody->setCenterOfMassTransform(t);
    mBody->setLinearVelocity(btVector3(0, 0, 0));
    mBody->setAngularVelocity(btVector3(0, 0, 0));
    Physics::DynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(mBody->getBroadphaseHandle(), Physics::DynamicsWorld->getDispatcher());
    if(mVehicle != null) {
        mVehicle->resetSuspension();
        for(int i = 0; i < mVehicle->getNumWheels(); i++) {
            mVehicle->updateWheelTransform(i, true);
        }
    }
}

//////////////////////////////////////////////////////////////////////

void Vehicle::Reset()
{
    mSteerAngle = 0;
    mEngineForce = 0;
    mBody->setCenterOfMassTransform(mStartTransform);
    mBody->setLinearVelocity(btVector3(0, 0, 0));
    mBody->setAngularVelocity(btVector3(0, 0, 0));
    Physics::DynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(mBody->getBroadphaseHandle(), Physics::DynamicsWorld->getDispatcher());
    if(mVehicle != null) {
        mVehicle->resetSuspension();
        for(int i = 0; i < mVehicle->getNumWheels(); i++) {
            mVehicle->updateWheelTransform(i, true);
        }
    }
}

//////////////////////////////////////////////////////////////////////

void Vehicle::Update(float deltaTime)
{
    float currentSpeed = mVehicle->getCurrentSpeedKmHour();
    float add = 0;
    float handBrake = 0;
    if(Keyboard::Held(VK_LEFT)) {
        add += 0.02f;
    }
    if(Keyboard::Held(VK_RIGHT)) {
        add -= 0.02f;
    }
    if(add == 0) {
        mSteerAngle *= 0.9f;
    } else {
        mSteerAngle = Constrain(mSteerAngle + add, -0.15f, 0.15f);
    }

    if(Keyboard::Held(VK_CONTROL)) {
        handBrake = 25;
        mEngineForce = 0;
        mBrakeForce = 5;
    } else {
        if(Keyboard::Held(VK_DOWN)) {
            if(currentSpeed < 0.01f) {
                mEngineForce = -4000;
                mBrakeForce = 0;
                handBrake = 0;
            } else {
                mEngineForce = 0;
                mBrakeForce = 10;
                handBrake = 20;
            }
        } else if(Keyboard::Held(VK_UP)) {
            mEngineForce = 8000;
            mBrakeForce = 0;
        } else {
            mEngineForce = 0.0f;
        }
    }

    mVehicle->setSteeringValue(mSteerAngle, 0);
    mVehicle->setSteeringValue(mSteerAngle, 1);
    mVehicle->applyEngineForce(mEngineForce, 2);
    mVehicle->applyEngineForce(mEngineForce, 3);
    mVehicle->setBrake(mBrakeForce, 0);
    mVehicle->setBrake(mBrakeForce, 1);
    mVehicle->setBrake(handBrake, 2);
    mVehicle->setBrake(handBrake, 3);

    debug_text(0, 300, "Speed: %f", currentSpeed);
    debug_text(0, 315, "Angle: %f", mSteerAngle);
}

//////////////////////////////////////////////////////////////////////

Vec4f Physics::Mesh::GetInterpolatedNormal(int triangleIndex, CVec4f pos)
{
    Triangle t(*this, triangleIndex * 3);
    return t.GetInterpolatedNormal(pos);
}

//////////////////////////////////////////////////////////////////////

class ClosestWithNormal : public btCollisionWorld::ClosestRayResultCallback
{
public:
    ClosestWithNormal(btVector3 const &from, btVector3 const &to) : btCollisionWorld::ClosestRayResultCallback(from, to)
    {
        m_triangle_index = -1;
    }

    btScalar addSingleResult(btCollisionWorld::LocalRayResult &rayResult, bool normalInWorldSpace) override
    {
        if(rayResult.m_localShapeInfo && rayResult.m_localShapeInfo->m_shapePart > -1) {
            m_triangle_index = rayResult.m_localShapeInfo->m_triangleIndex;
        }
        return btCollisionWorld::ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
    }

    int m_triangle_index;
};

//////////////////////////////////////////////////////////////////////

void *MyVehicleRaycaster::castRay(btVector3 const &from, btVector3 const &to, btVehicleRaycasterResult &result)
{
    ClosestWithNormal rayCallback(from, to);

    m_dynamicsWorld->rayTest(from, to, rayCallback);

    if(rayCallback.hasHit()) {
        btRigidBody const *body = btRigidBody::upcast(rayCallback.m_collisionObject);
        if(body && body->hasContactResponse()) {
            result.m_hitPointInWorld = rayCallback.m_hitPointWorld;
            result.m_distFraction = rayCallback.m_closestHitFraction;

            Physics::Mesh *mesh = (Physics::Mesh *)body->getUserPointer();
            if(mesh != null && rayCallback.m_triangle_index > -1) {
                result.m_triangle_index = rayCallback.m_triangle_index;
                result.m_hitNormalInWorld = mesh->GetInterpolatedNormal(rayCallback.m_triangle_index, result.m_hitPointInWorld.mVec128);
            } else {
                result.m_hitNormalInWorld = rayCallback.m_hitNormalWorld;
                result.m_hitNormalInWorld.normalize();
                result.m_triangle_index = -1;
            }
            return (void *)body;
        }
    }
    return null;
}
