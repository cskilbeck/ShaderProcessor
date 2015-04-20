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

		HRESULT Open(DXWindow *window)
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

		void Close()
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

		btRigidBody *CreateRigidBody(float mass, const btTransform &transform, btCollisionShape *shape, uint16 collisionGroup, uint16 collisionMask, btRigidBody::btRigidBodyConstructionInfo *ci)
		{
			assert(shape != null && shape->getShapeType() != INVALID_SHAPE_PROXYTYPE);
			btVector3 localInertia(0, 0, 0);
			if(mass != 0)
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
			DynamicsWorld->addRigidBody(body, collisionGroup, collisionMask);
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

		void DrawShape(Matrix const &parent, btCollisionShape const *shape, iPhysicsRenderer *w)
		{
			switch(shape->getShapeType())
			{
				// add the rest...

				case BOX_SHAPE_PROXYTYPE:
				{
					btVector3 halfExtent = ((btBoxShape  *)shape)->getHalfExtentsWithoutMargin();
					w->DrawCube(ScaleMatrix(halfExtent.mVec128) * parent);
				}
				break;

				case SPHERE_SHAPE_PROXYTYPE:
				{
					btSphereShape const *s = (btSphereShape const *)shape;
					float r = s->getRadius();
					w->DrawSphere(ScaleMatrix(Vec4(r, r, r)) * parent);
				}
				break;

				case CYLINDER_SHAPE_PROXYTYPE:
				{
					btCylinderShape const *c = (btCylinderShape const *)shape;
					Matrix m = ScaleMatrix(c->getHalfExtentsWithoutMargin().get128());
					switch(c->getUpAxis())
					{
						case 0: m = RotationMatrix(Vec4(0, 0, 1), PI / 2) * m; break;
						case 2: m = RotationMatrix(Vec4(1, 0, 0), PI / 2) * m; break;
					}
					w->DrawCylinder(m * parent);
				}
				break;

				case CAPSULE_SHAPE_PROXYTYPE:
				{
					btCapsuleShape const *c = (btCapsuleShape const *)shape;
					float r = c->getRadius();
					float l = c->getHalfHeight();
					// Draw a capsule
				}
				break;

				case COMPOUND_SHAPE_PROXYTYPE:
				{
					const btCompoundShape *c = static_cast<btCompoundShape const *>(shape);
					for(int i = c->getNumChildShapes() - 1; i >= 0; i--)
					{
						DrawShape(Physics::btTransformToMatrix(c->getChildTransform(i)) * parent, c->getChildShape(i), w);
					}
				}
				break;
			}
		}

	} // ::Physics}
}