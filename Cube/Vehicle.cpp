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
	mWheelRadius = 2.0f;
	mWheelWidth = 0.5f;
	mEngineForce = 0;
	mBrakeForce = 0;
	mSteerAngle = 0;

	mStartPosition = pos;

	mBodyShape = new btBoxShape(btVector3(3, 6, 0.75f));

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

	mBody = Physics::CreateRigidBody(mass, chassisTransform, mCompoundShape, Physics::CarMask, -1, &bodyInfo);
	mBody->setActivationState(DISABLE_DEACTIVATION);

	mRayCaster = new MyVehicleRaycaster(Physics::DynamicsWorld, mWheelRadius, mWheelWidth);

	mTuning.m_maxSuspensionTravelCm = 500.0f;
	mTuning.m_suspensionCompression *= 2;
	mTuning.m_suspensionDamping *= 4;
	mTuning.m_frictionSlip *= 2;
	mTuning.m_suspensionStiffness *= 4;

	mVehicle = new WheelCastVehicle(mTuning, mBody, mRayCaster);

	mVehicle->setCoordinateSystem(0, 2, 1);

	Physics::DynamicsWorld->addAction(mVehicle);

	btVector3 wheelDir(0, 0, -1);
	btVector3 axleDir(1, 0, 0);
	btScalar suspensionRestLength = 0.5f;
	btScalar connectionHeight = 0.0f;

	Vec4f extents = mBodyShape->getHalfExtentsWithMargin().get128();
	float width = GetX(extents) + mWheelWidth + 0.1f;
	float length = GetY(extents) - mWheelRadius;
	float height = GetZ(extents);

	mVehicle->addWheel(btVector3(-width, +length, connectionHeight), wheelDir, axleDir, suspensionRestLength, mWheelRadius, mTuning, true);
	mVehicle->addWheel(btVector3(+width, +length, connectionHeight), wheelDir, axleDir, suspensionRestLength, mWheelRadius, mTuning, true);
	mVehicle->addWheel(btVector3(+width, -length, connectionHeight), wheelDir, axleDir, suspensionRestLength, mWheelRadius, mTuning, false);
	mVehicle->addWheel(btVector3(-width, -length, connectionHeight), wheelDir, axleDir, suspensionRestLength, mWheelRadius, mTuning, false);

	for(int i = 0; i < mVehicle->getNumWheels(); ++i)
	{
		btWheelInfo &wheel = mVehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness = 1800.0f;
		wheel.m_frictionSlip = 1;
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

void Vehicle::Draw(MyDXWindow *w)
{
	btDefaultMotionState *ms = (btDefaultMotionState*)mBody->getMotionState();
	Physics::DrawShape(Physics::btTransformToMatrix(ms->m_graphicsWorldTrans), mCompoundShape, (iPhysicsRenderer *)w);

	for(int i = 0; i < mVehicle->getNumWheels(); ++i)
	{
		// DrawCylinder uses Y cylinders
		btTransform const &t = mVehicle->getWheelTransformWS(i);
		Matrix wheelMatrix = ScaleMatrix(Vec4(mWheelRadius, mWheelWidth, mWheelRadius)) * RotationMatrix(Vec4(0, 0, PI / 2)) * Physics::btTransformToMatrix(t);
		w->DrawCylinder(wheelMatrix);
		if(Keyboard::Held('1'))
		{
			// debug_cylinder uses Z cylinders
			Matrix wheelMatrix = ScaleMatrix(Vec4(mWheelRadius, mWheelRadius, mWheelWidth)) * RotationMatrix(Vec4(0, PI / 2, 0)) * Physics::btTransformToMatrix(t);
			debug_cylinder(wheelMatrix, Color::White);
		}
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
			mEngineForce = -1000;
			mBrakeForce = 0;
		}
		else
		{
			mEngineForce = 0;
			mBrakeForce = 5;
		}
	}
	else if(Keyboard::Held(VK_UP))
	{
		mEngineForce = 2000;
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

// Aligned version of this

struct MyClosestConvexResultCallback: public btCollisionWorld::ConvexResultCallback, Aligned16
{
	MyClosestConvexResultCallback(btTransform const &convexFromWorld, btTransform const &convexToWorld)
		: btCollisionWorld::ConvexResultCallback()
		, m_convexFromWorld(convexFromWorld)
		, m_convexToWorld(convexToWorld)
		, m_hitCollisionObject(0)
	{
	}

	btTransform					m_convexFromWorld; //used to calculate hitPointWorld from hitFraction
	btTransform					m_convexToWorld;
	btVector3					m_hitNormalWorld;
	btVector3					m_hitPointWorld;
	btCollisionObject const *	m_hitCollisionObject;

	virtual	btScalar addSingleResult(btCollisionWorld::LocalConvexResult &convexResult, bool normalInWorldSpace)
	{
		//caller already does the filter on the m_closestHitFraction
		btAssert(convexResult.m_hitFraction <= m_closestHitFraction);

		m_hitCollisionObject = convexResult.m_hitCollisionObject;
		m_closestHitFraction = convexResult.m_hitFraction;
		if(normalInWorldSpace)
		{
			m_hitNormalWorld = convexResult.m_hitNormalLocal;
		}
		else
		{
			///need to transform normal into worldspace
			m_hitNormalWorld = m_hitCollisionObject->getWorldTransform().getBasis() * convexResult.m_hitNormalLocal;
		}
		m_hitPointWorld = convexResult.m_hitPointLocal;
		return convexResult.m_hitFraction;
	}
};

void *MyVehicleRaycaster::castConvex(btTransform const &from, btTransform const &to, btVehicleRaycasterResult &result)
{
	MyClosestConvexResultCallback collisionCallback(from, to);
	collisionCallback.m_collisionFilterGroup = Physics::WheelMask;
	collisionCallback.m_collisionFilterMask = ~Physics::CarMask;

	btVector3 e = mShape->getHalfExtentsWithMargin();

	Matrix wheelMatrix = RotationMatrix(Vec4(0, PI / 2, 0)) * ScaleMatrix(e.mVec128) * Physics::btTransformToMatrix(from);
	debug_cylinder(wheelMatrix, Color::Cyan);

	wheelMatrix = RotationMatrix(Vec4(0, PI / 2, 0)) * ScaleMatrix(e.mVec128) * Physics::btTransformToMatrix(to);
	debug_cylinder(wheelMatrix, Color::Yellow);


	Physics::DynamicsWorld->convexSweepTest(mShape, from, to, collisionCallback);
	if(collisionCallback.hasHit())
	{
		btCollisionObject const *b = collisionCallback.m_hitCollisionObject;
		if(b != null)
		{
			btRigidBody const *body = btRigidBody::upcast(b);
			if(body->hasContactResponse())
			{
				result.m_hitPointInWorld = collisionCallback.m_hitPointWorld;
				result.m_hitNormalInWorld = collisionCallback.m_hitNormalWorld;
				result.m_hitNormalInWorld.normalize();
				result.m_distFraction = collisionCallback.m_closestHitFraction;
				return (void *)body;
			}
		}
	}
	return null;
}

btScalar WheelCastVehicle::rayCast(btWheelInfo& wheel)
{
	updateWheelTransformsWS(wheel, false);

	btScalar depth = -1;
	btScalar raylen = wheel.getSuspensionRestLength() + wheel.m_wheelsRadius;

	btScalar param = btScalar(0.);

	btVehicleRaycaster::btVehicleRaycasterResult	rayResults;

	assert(m_vehicleRaycaster != null);
	btTransform from = wheel.m_worldTransform;
	btTransform to = from;
	btVector3 rayvector = wheel.m_raycastInfo.m_wheelDirectionWS * (raylen);
	to.setOrigin(to.getOrigin() + rayvector);

	void *object = m_vehicleRaycaster->castConvex(from, to, rayResults);

	wheel.m_raycastInfo.m_groundObject = 0;

	if(object)
	{
		param = rayResults.m_distFraction;
		depth = raylen * rayResults.m_distFraction;
		wheel.m_raycastInfo.m_contactNormalWS = rayResults.m_hitNormalInWorld;
		wheel.m_raycastInfo.m_isInContact = true;

		wheel.m_raycastInfo.m_groundObject = &getFixedBody();///@todo for driving on dynamic/movable objects!;
		//wheel.m_raycastInfo.m_groundObject = object;


		btScalar hitDistance = param*raylen;
		wheel.m_raycastInfo.m_suspensionLength = hitDistance - wheel.m_wheelsRadius;
		//clamp on max suspension travel

		btScalar  minSuspensionLength = wheel.getSuspensionRestLength() - wheel.m_maxSuspensionTravelCm*btScalar(0.01);
		btScalar maxSuspensionLength = wheel.getSuspensionRestLength() + wheel.m_maxSuspensionTravelCm*btScalar(0.01);
		if(wheel.m_raycastInfo.m_suspensionLength < minSuspensionLength)
		{
			wheel.m_raycastInfo.m_suspensionLength = minSuspensionLength;
		}
		if(wheel.m_raycastInfo.m_suspensionLength > maxSuspensionLength)
		{
			wheel.m_raycastInfo.m_suspensionLength = maxSuspensionLength;
		}

		wheel.m_raycastInfo.m_contactPointWS = rayResults.m_hitPointInWorld;

		btScalar denominator = wheel.m_raycastInfo.m_contactNormalWS.dot(wheel.m_raycastInfo.m_wheelDirectionWS);

		btVector3 chassis_velocity_at_contactPoint;
		btVector3 relpos = wheel.m_raycastInfo.m_contactPointWS - getRigidBody()->getCenterOfMassPosition();

		chassis_velocity_at_contactPoint = getRigidBody()->getVelocityInLocalPoint(relpos);

		btScalar projVel = wheel.m_raycastInfo.m_contactNormalWS.dot(chassis_velocity_at_contactPoint);

		if(denominator >= btScalar(-0.1))
		{
			wheel.m_suspensionRelativeVelocity = btScalar(0.0);
			wheel.m_clippedInvContactDotSuspension = btScalar(1.0) / btScalar(0.1);
		}
		else
		{
			btScalar inv = btScalar(-1.) / denominator;
			wheel.m_suspensionRelativeVelocity = projVel * inv;
			wheel.m_clippedInvContactDotSuspension = inv;
		}

	}
	else
	{
		//put wheel info as in rest position
		wheel.m_raycastInfo.m_suspensionLength = wheel.getSuspensionRestLength();
		wheel.m_suspensionRelativeVelocity = btScalar(0.0);
		wheel.m_raycastInfo.m_contactNormalWS = -wheel.m_raycastInfo.m_wheelDirectionWS;
		wheel.m_clippedInvContactDotSuspension = btScalar(1.0);
	}

	return depth;
}
