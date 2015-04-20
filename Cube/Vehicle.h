#pragma once

struct MyDXWindow;

struct Vehicle
{
	btCompoundShape *				mCompoundShape;
	btBoxShape *					mBodyShape;
	btRigidBody *					mBody;

	btDefaultMotionState *			mMotionState;
	btDefaultVehicleRaycaster *		mRayCaster;

	btRaycastVehicle *				mVehicle;

	btRaycastVehicle::btVehicleTuning mTuning;

	float							mSteerAngle;
	float							mEngineForce;
	float							mBrakeForce;
	float							mWheelRadius;
	Vec4f							mStartPosition;

	Vehicle();
	~Vehicle();

	int Create(Vec4f pos);
	void Reset();
	void Draw(MyDXWindow *window);
	void Update(float deltaTime);
	void Release();
};