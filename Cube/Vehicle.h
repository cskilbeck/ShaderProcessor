//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct MyDXWindow;
class MyVehicleRaycaster;
class WheelCastVehicle;

//////////////////////////////////////////////////////////////////////

class MyVehicleRaycaster: public btVehicleRaycaster
{
public:
	MyVehicleRaycaster(btDynamicsWorld *world)
		: btVehicleRaycaster()
		, m_dynamicsWorld(world)
	{
	}

	void *castRay(const btVector3& from, const btVector3& to, btVehicleRaycasterResult& result) override;

private:
	btDynamicsWorld *m_dynamicsWorld;
};

struct Vehicle
{
	btCompoundShape *				mCompoundShape;
	btBoxShape *					mBodyShape;
	btRigidBody *					mBody;
	btDefaultMotionState *			mMotionState;
	btRaycastVehicle *				mVehicle;
	MyVehicleRaycaster *			mRayCaster;
	btRaycastVehicle::btVehicleTuning mTuning;
	float							mSteerAngle;
	float							mEngineForce;
	float							mBrakeForce;
	float							mWheelRadius;
	float							mWheelWidth;
	btTransform						mStartTransform;

	Vehicle();
	~Vehicle();

	int Create(btTransform transform);
	void Reset();
	void Draw(MyDXWindow *window);
	void Update(float deltaTime);
	void Release();
};

