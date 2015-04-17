//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

Vehicle::Vehicle()
	: mCompoundShape(null)
	, mBodyShape(null)
	, mBody(null)
	, mMotionState(null)
	, mRayCaster(null)
	, mVehicle(null)
{
}

//////////////////////////////////////////////////////////////////////

Vehicle::~Vehicle()
{
}

//////////////////////////////////////////////////////////////////////

int Vehicle::Create(Vec4f pos)
{
	mEngineForce = 0;
	mBrakeForce = 0;
	mSteerAngle = 0;

	mStartPosition = pos;

	mBodyShape = new btBoxShape(btVector3(1, 2, 0.5f));

	btTransform chassisTransform;
	chassisTransform.setIdentity();
	chassisTransform.setOrigin(btVector3(0, 0, 1));

	mCompoundShape = new btCompoundShape();
	mCompoundShape->addChildShape(chassisTransform, mBodyShape);

	mMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), pos));
	btScalar mass = 1800;
	btVector3 inertia(0,0,0);
	mBodyShape->calculateLocalInertia(mass, inertia);

	btRigidBody::btRigidBodyConstructionInfo bodyInfo(mass, mMotionState, mBodyShape, inertia);
	bodyInfo.m_friction = 0.6f;
	bodyInfo.m_restitution = 0.6f;
	bodyInfo.m_linearDamping = 0.2f;
	bodyInfo.m_angularDamping = 0.2f;

	mBody = Physics::CreateRigidBody(mass, chassisTransform, mCompoundShape, &bodyInfo);
	mBody->setActivationState(DISABLE_DEACTIVATION);

	mRayCaster = new btDefaultVehicleRaycaster(Physics::DynamicsWorld);

	mTuning.m_maxSuspensionTravelCm = 500.0f;
	mTuning.m_suspensionCompression = 4.4f;
	mTuning.m_suspensionDamping = 2.3f;
	mTuning.m_frictionSlip = 1000.0f;
	mTuning.m_suspensionStiffness = 20.0f;

	mVehicle = new btRaycastVehicle(mTuning, mBody, mRayCaster);

	// this is unneccessary, 0,2,1 is the default (eg Z up)
	mVehicle->setCoordinateSystem(0, 2, 1);

	Physics::DynamicsWorld->addAction(mVehicle);

	btVector3 wheelDir(0, 0, -1);
	btVector3 axleDir(1, 0, 0);
	btScalar suspensionRestLength = 0.5f;
	btScalar wheelRadius = 1.5f;
	btScalar connectionHeight = 0.5f;

	mVehicle->addWheel(btVector3(-1.5f, +1.8f, connectionHeight), wheelDir, axleDir, suspensionRestLength, wheelRadius, mTuning, true);
	mVehicle->addWheel(btVector3(+1.5f, +1.8f, connectionHeight), wheelDir, axleDir, suspensionRestLength, wheelRadius, mTuning, true);
	mVehicle->addWheel(btVector3(+1.5f, -1.8f, connectionHeight), wheelDir, axleDir, suspensionRestLength, wheelRadius, mTuning, false);
	mVehicle->addWheel(btVector3(-1.5f, -1.8f, connectionHeight), wheelDir, axleDir, suspensionRestLength, wheelRadius, mTuning, false);

	for(int i = 0; i < mVehicle->getNumWheels(); ++i)
	{
		btWheelInfo &wheel = mVehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness = 20.0f;
		wheel.m_frictionSlip = 1000.0f;
		wheel.m_rollInfluence = 0.1f;
		wheel.m_wheelsDampingCompression = 4.4f;
		wheel.m_wheelsDampingRelaxation = 2.3f;
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

void Vehicle::Release()
{
}

//////////////////////////////////////////////////////////////////////

void Vehicle::Draw(MyDXWindow *w)
{
	Matrix modelMatrix = ScaleMatrix(Vec4(1, 2, 0.5f)) * Physics::btTransformToMatrix(mBody->getWorldTransform());
	w->DrawCube(modelMatrix, w->cubeVerts, w->cubeTexture);
	for(int i = 0; i < mVehicle->getNumWheels(); ++i)
	{
		mVehicle->updateWheelTransform(i, false);
		btTransform const &t = mVehicle->getWheelTransformWS(i);
		Matrix wheelMatrix = Physics::btTransformToMatrix(t);
		w->DrawCylinder(wheelMatrix, w->cubeTexture);
	}

	btTransform const &t = mVehicle->getWheelTransformWS(0);
	btMatrix3x3 const &m = t.getBasis();
	btVector3 const &a = m.getRow(0);
	btVector3 const &b = m.getRow(1);
	btVector3 const &c = m.getRow(2);
	btVector3 const &d = m.getRow(3);
	debug_text(800, 200, "%8.4f %8.4f %8.4f %8.4f", a.x(), a.y(), a.z(), a.w());
	debug_text(800, 215, "%8.4f %8.4f %8.4f %8.4f", b.x(), b.y(), b.z(), b.w());
	debug_text(800, 230, "%8.4f %8.4f %8.4f %8.4f", c.x(), c.y(), c.z(), c.w());
	debug_text(800, 245, "%8.4f %8.4f %8.4f %8.4f", d.x(), d.y(), d.z(), d.w());
}

//////////////////////////////////////////////////////////////////////

void Vehicle::Reset()
{
	mSteerAngle = 0;
	mEngineForce = 0;
	mBody->setCenterOfMassTransform(btTransform(btQuaternion(0, 0, 0, 1), mStartPosition));
	mBody->setLinearVelocity(btVector3(0, 0, 0));
	mBody->setAngularVelocity(btVector3(0, 0, 0));
	Physics::DynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(mBody->getBroadphaseHandle(), Physics::DynamicsWorld->getDispatcher());
	if(mVehicle != null)
	{
		mVehicle->resetSuspension();
		for(int i = 0; i < mVehicle->getNumWheels(); i++)
		{
			mVehicle->updateWheelTransform(i, true);
		}
	}
}

//////////////////////////////////////////////////////////////////////

void Vehicle::Update(float deltaTime)
{
	float currentSpeed = mVehicle->getCurrentSpeedKmHour();

	float add = 0;
	if(Keyboard::Held(VK_LEFT))
	{
		add += 0.01f;
	}
	if(Keyboard::Held(VK_RIGHT))
	{
		add -= 0.01f;
	}
	if(add == 0.0f)
	{
		mSteerAngle *= 0.9f;
	}
	else
	{
		mSteerAngle = Constrain(mSteerAngle + add, -0.04f, 0.04f);
	}

	if(Keyboard::Held(VK_DOWN))
	{
		if(currentSpeed < 0.01f)
		{
			mEngineForce = -400;
			mBrakeForce = 0;
		}
		else
		{
			mEngineForce = 0;
			mBrakeForce = 0.75f;
		}
	}
	else if(Keyboard::Held(VK_UP))
	{
		mEngineForce = 5000;
		mBrakeForce = 0;
	}
	else
	{
		mEngineForce *= 0.95f;
	}

	mVehicle->setSteeringValue(mSteerAngle, 0);
	mVehicle->setSteeringValue(mSteerAngle, 1);
	mVehicle->applyEngineForce(mEngineForce, 2);
	mVehicle->applyEngineForce(mEngineForce, 3);
	mVehicle->setBrake(mBrakeForce, 2);
	mVehicle->setBrake(mBrakeForce, 3);

	debug_text(0, 300, "Speed: %f", currentSpeed);
	debug_text(0, 315, "Angle: %f", mSteerAngle);
}