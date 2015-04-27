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
	btRaycastVehicle *				mVehicle;
	btDefaultVehicleRaycaster *		mRayCaster;
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

