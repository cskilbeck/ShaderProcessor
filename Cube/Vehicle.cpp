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
	mWheelRadius = 1.5f;
	mEngineForce = 0;
	mBrakeForce = 0;
	mSteerAngle = 0;

	mStartPosition = pos;

	mBodyShape = new btBoxShape(btVector3(3, 4, 0.75f));

	btTransform chassisTransform;
	chassisTransform.setIdentity();
	chassisTransform.setOrigin(btVector3(0, 0, 0.7f));

	mCompoundShape = new btCompoundShape();
	mCompoundShape->addChildShape(chassisTransform, mBodyShape);

	mMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), pos));
	btScalar mass = 1800;
	btVector3 inertia;
	mCompoundShape->calculateLocalInertia(mass, inertia);

	btRigidBody::btRigidBodyConstructionInfo bodyInfo(mass, mMotionState, mBodyShape, inertia);
	bodyInfo.m_friction = 0.6f;
	bodyInfo.m_restitution = 0.6f;
	bodyInfo.m_linearDamping = 0.1f;
	bodyInfo.m_angularDamping = 0.1f;

	mBody = Physics::CreateRigidBody(mass, chassisTransform, mCompoundShape, &bodyInfo);
	mBody->setActivationState(DISABLE_DEACTIVATION);

	mRayCaster = new btDefaultVehicleRaycaster(Physics::DynamicsWorld);

	mTuning.m_maxSuspensionTravelCm = 500.0f;
	mTuning.m_suspensionCompression = 5.4f;
	mTuning.m_suspensionDamping = 3.3f;
	mTuning.m_frictionSlip = 500.0f;
	mTuning.m_suspensionStiffness = 30.0f;

	mVehicle = new btRaycastVehicle(mTuning, mBody, mRayCaster);

	mVehicle->setCoordinateSystem(0, 2, 1);

	Physics::DynamicsWorld->addAction(mVehicle);

	btVector3 wheelDir(0, 0, -1);
	btVector3 axleDir(1, 0, 0);
	btScalar suspensionRestLength = 0.5f;
	btScalar connectionHeight = 0.5f;

	Vec4f extents = mBodyShape->getHalfExtentsWithoutMargin().get128();
	float width = GetX(extents) + 0.15f;
	float length = GetY(extents) - 0.15f;
	float height = GetZ(extents);

	mVehicle->addWheel(btVector3(-width, +length, connectionHeight), wheelDir, axleDir, suspensionRestLength, mWheelRadius, mTuning, true);
	mVehicle->addWheel(btVector3(+width, +length, connectionHeight), wheelDir, axleDir, suspensionRestLength, mWheelRadius, mTuning, true);
	mVehicle->addWheel(btVector3(+width, -length, connectionHeight), wheelDir, axleDir, suspensionRestLength, mWheelRadius, mTuning, false);
	mVehicle->addWheel(btVector3(-width, -length, connectionHeight), wheelDir, axleDir, suspensionRestLength, mWheelRadius, mTuning, false);

	for(int i = 0; i < mVehicle->getNumWheels(); ++i)
	{
		btWheelInfo &wheel = mVehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness = 40.0f;
		wheel.m_frictionSlip = 1;
		wheel.m_rollInfluence = 0.25f;
		wheel.m_wheelsDampingCompression = 4.4f;
		wheel.m_wheelsDampingRelaxation = 2.3f;
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

void Vehicle::Release()
{
}

void Vehicle::Draw(MyDXWindow *w)
{
	btDefaultMotionState *ms = (btDefaultMotionState*)mBody->getMotionState();
	Physics::DrawShape(Physics::btTransformToMatrix(ms->m_graphicsWorldTrans), mCompoundShape, (iPhysicsRenderer *)w);

	for(int i = 0; i < mVehicle->getNumWheels(); ++i)
	{
		btTransform const &t = mVehicle->getWheelTransformWS(i);
		Matrix wheelMatrix = ScaleMatrix(Vec4(mWheelRadius, 0.25f, mWheelRadius)) * RotationMatrix(Vec4(0, 0, PI / 2)) * Physics::btTransformToMatrix(t);
		w->DrawCylinder(wheelMatrix);
	}
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
		add += 0.08f;
	}
	if(Keyboard::Held(VK_RIGHT))
	{
		add -= 0.08f;
	}
	if(add == 0)
	{
		mSteerAngle *= 0.9f;
	}
	else
	{
		mSteerAngle = Constrain(mSteerAngle + add, -0.5f, 0.5f);
	}

	if(Keyboard::Held(VK_DOWN))
	{
		if(currentSpeed < 0.01f)
		{
			mEngineForce = -4000;
			mBrakeForce = 0;
		}
		else
		{
			mEngineForce = 0;
			mBrakeForce = 50;
		}
	}
	else if(Keyboard::Held(VK_UP))
	{
		mEngineForce = 8000;
		mBrakeForce = 0;
	}
	else
	{
		mEngineForce = 0.0f;
	}

	mVehicle->setSteeringValue(mSteerAngle, 0);
	mVehicle->setSteeringValue(mSteerAngle, 1);
	mVehicle->applyEngineForce(mEngineForce, 2);
	mVehicle->applyEngineForce(mEngineForce, 3);
	mVehicle->setBrake(mBrakeForce, 0);
	mVehicle->setBrake(mBrakeForce, 1);
	mVehicle->setBrake(mBrakeForce, 2);
	mVehicle->setBrake(mBrakeForce, 3);

	debug_text(0, 300, "Speed: %f", currentSpeed);
	debug_text(0, 315, "Angle: %f", mSteerAngle);
}