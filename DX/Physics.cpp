//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

namespace
{
	PhysicsDebug physicsDebug;
}

namespace DX
{
	namespace Physics
	{
		btDefaultCollisionConfiguration *		CollisionConfiguration;
		btCollisionDispatcher *					Dispatcher;
		btBroadphaseInterface *					OverlappingPairCache;
		btSequentialImpulseConstraintSolver *	Solver;
		btDiscreteDynamicsWorld *				DynamicsWorld;

		//////////////////////////////////////////////////////////////////////

		HRESULT Physics::Open(DXWindow *window)
		{
			CollisionConfiguration = new btDefaultCollisionConfiguration();
			Dispatcher = new btCollisionDispatcher(CollisionConfiguration);
			OverlappingPairCache = new btDbvtBroadphase();
			Solver = new btSequentialImpulseConstraintSolver;
			DynamicsWorld = new btDiscreteDynamicsWorld(Dispatcher, OverlappingPairCache, Solver, CollisionConfiguration);
			DynamicsWorld->setGravity(btVector3(0, 0, -9.8f));
			DXR(physicsDebug.Create(window));
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		void Physics::Close()
		{
			physicsDebug.Release();

			// brute force cleanup
			if(DynamicsWorld->getNumCollisionObjects() > 0)
			{
				TRACE("Deleting %d orphaned physics objects\n", DynamicsWorld->getNumCollisionObjects());

				for(int i = DynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
				{
					btCollisionObject* obj = DynamicsWorld->getCollisionObjectArray()[i];
					btRigidBody* body = btRigidBody::upcast(obj);
					if(body && body->getMotionState())
					{
						delete body->getMotionState();
					}
					DynamicsWorld->removeCollisionObject(obj);
					delete obj;
				}
			}

			//delete dynamics world
			delete DynamicsWorld;

			//delete solver
			delete Solver;

			//delete broadphase
			delete OverlappingPairCache;

			//delete dispatcher
			delete Dispatcher;

			delete CollisionConfiguration;
		}

		btRigidBody *CreateRigidBody(float mass, const btTransform &transform, btCollisionShape *shape, btRigidBody::btRigidBodyConstructionInfo *ci)
		{
			assert(shape != null && shape->getShapeType() != INVALID_SHAPE_PROXYTYPE);
			btVector3 localInertia(0, 0, 0);
			if(mass != 0.0f)
			{
				shape->calculateLocalInertia(mass, localInertia);
			}
			btDefaultMotionState *ms = new btDefaultMotionState(transform);
			btRigidBody::btRigidBodyConstructionInfo cInfo(mass, ms, shape, localInertia);
			if(ci == null)
			{
				ci = &cInfo;
			}
			else
			{
				ci->m_mass = mass;
				ci->m_motionState = ms;
				ci->m_collisionShape = shape;
				ci->m_localInertia = localInertia;
			}

			btRigidBody *body = new btRigidBody(*ci);
			body->setContactProcessingThreshold(BT_LARGE_FLOAT);
			DynamicsWorld->addRigidBody(body);
			return body;
		}

		//////////////////////////////////////////////////////////////////////

		void DeleteRigidBody(btRigidBody * &b)
		{
			if(b != null)
			{
				delete b->getMotionState();
				DynamicsWorld->removeRigidBody(b);
				delete b;
				b = null;
			}
		}

		//////////////////////////////////////////////////////////////////////

		btVector3 inertia(float mass, btCollisionShape *shape)
		{
			btVector3 i(0, 0, 0);
			shape->calculateLocalInertia(mass, i);
			return i;
		}

		//////////////////////////////////////////////////////////////////////

		btCompoundShape *InitCompoundShape(btCompoundShape *shape, btScalar *masses, btTransform &shift)
		{
			btCompoundShape* newShape(null);
			btTransform principal;
			btVector3 principalInertia;
			shape->calculatePrincipalAxisTransform(masses, principal, principalInertia);
			newShape = new btCompoundShape();
			for(int i = 0; i < shape->getNumChildShapes(); i++)
			{
				btTransform newChildTransform = principal.inverse() * shape->getChildTransform(i);
				newShape->addChildShape(newChildTransform, shape->getChildShape(i));
			}
			shift = principal;
			return newShape;
		}

		//////////////////////////////////////////////////////////////////////

		void Physics::DebugBegin(Camera *camera)
		{
			DynamicsWorld->setDebugDrawer(&physicsDebug);
			physicsDebug.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
			physicsDebug.BeginScene(camera);
		}

		//////////////////////////////////////////////////////////////////////

		void Physics::DebugEnd()
		{
			physicsDebug.EndScene();
			DynamicsWorld->setDebugDrawer(null);
		}

		//////////////////////////////////////////////////////////////////////

		btIDebugDraw *Physics::DebugDrawer()
		{
			return &physicsDebug;
		}

		//////////////////////////////////////////////////////////////////////

	} // ::Physics}
}