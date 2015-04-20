//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct MyDXWindow;
class MyVehicleRaycaster;
class WheelCastVehicle;

//////////////////////////////////////////////////////////////////////

struct Vehicle
{
	btCompoundShape *				mCompoundShape;
	btBoxShape *					mBodyShape;
	btRigidBody *					mBody;
	btDefaultMotionState *			mMotionState;
	MyVehicleRaycaster *			mRayCaster;
	WheelCastVehicle *				mVehicle;
	btRaycastVehicle::btVehicleTuning mTuning;
	float							mSteerAngle;
	float							mEngineForce;
	float							mBrakeForce;
	float							mWheelRadius;
	float							mWheelWidth;
	Vec4f							mStartPosition;

	Vehicle();
	~Vehicle();

	int Create(Vec4f pos);
	void Reset();
	void Draw(MyDXWindow *window);
	void Update(float deltaTime);
	void Release();
};

//////////////////////////////////////////////////////////////////////

class MyVehicleRaycaster: public btVehicleRaycaster
{
	btDynamicsWorld	*m_dynamicsWorld;
	btCylinderShape *mShape;
	

public:
	MyVehicleRaycaster(btDynamicsWorld *world, float radius, float width)
		: m_dynamicsWorld(world)
		, mShape(new btCylinderShapeX(btVector3(width / 2, radius, radius)))
	{
	}

	void *castConvex(btTransform const &from, btTransform const &to, btVehicleRaycasterResult& result);
};

//////////////////////////////////////////////////////////////////////

class WheelCastVehicle: public btRaycastVehicle
{
public:
	WheelCastVehicle(btRaycastVehicle::btVehicleTuning const &tuning, btRigidBody *chassis, btVehicleRaycaster *rayCaster)
		: btRaycastVehicle(tuning, chassis, rayCaster)
	{
	}

	btScalar rayCast(btWheelInfo& wheel) override;
};
