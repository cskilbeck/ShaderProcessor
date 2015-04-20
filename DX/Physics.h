//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct iPhysicsRenderer
	{
		virtual void DrawCube(Matrix const &m) = 0;
		virtual void DrawCylinder(Matrix const &m) = 0;
		virtual void DrawSphere(Matrix const &m) = 0;
		virtual void DrawCapsule(Matrix const &m) = 0;
		virtual void DrawTorus(Matrix const &m) = 0;
		virtual void DrawCone(Matrix const &m) = 0;
		virtual void DrawConvexMesh(Matrix const &m) = 0;
		virtual void DrawTetrahedron(Matrix const &m) = 0;
	};

	namespace Physics
	{
		//////////////////////////////////////////////////////////////////////

		extern btDefaultCollisionConfiguration *		CollisionConfiguration;
		extern btCollisionDispatcher *					Dispatcher;
		extern btBroadphaseInterface *					OverlappingPairCache;
		extern btSequentialImpulseConstraintSolver *	Solver;
		extern btDiscreteDynamicsWorld *				DynamicsWorld;

		//////////////////////////////////////////////////////////////////////

		HRESULT Open(DXWindow *window);
		void Close();

		//////////////////////////////////////////////////////////////////////

		btRigidBody *CreateRigidBody(float mass, const btTransform &transform, btCollisionShape *shape, btRigidBody::btRigidBodyConstructionInfo *ci = null);
		void DeleteRigidBody(btRigidBody * &b);
		btVector3 inertia(float mass, btCollisionShape *shape);
		btCompoundShape *InitCompoundShape(btCompoundShape *shape, btScalar *masses, btTransform &shift);

		//////////////////////////////////////////////////////////////////////

		void DrawShape(Matrix const &parent, btCollisionShape const *shape, iPhysicsRenderer *w);
			
		void DebugBegin(Camera *camera);
		btIDebugDraw *DebugDrawer();
		void DebugEnd();

		//////////////////////////////////////////////////////////////////////

		template <typename T> void DeleteConstraint(T * &c)
		{
			if(c != null)
			{
				DynamicsWorld->removeConstraint(c);
				Delete(c);
			}
		}

		//////////////////////////////////////////////////////////////////////

		inline Matrix btTransformToMatrix(btTransform const &trans)
		{
			btVector3 R = trans.getBasis().getColumn(0);
			btVector3 U = trans.getBasis().getColumn(1);
			btVector3 L = trans.getBasis().getColumn(2);
			btVector3 P = trans.getOrigin();

			return Matrix(R.x(), R.y(), R.z(), 0,
						  U.x(), U.y(), U.z(), 0,
						  L.x(), L.y(), L.z(), 0,
						  P.x(), P.y(), P.z(), 1);
		}

		//////////////////////////////////////////////////////////////////////
	}

	inline HLSLVec3<float>::HLSLVec3(btVector3 const &b)
		: x(b.x())
		, y(b.y())
		, z(b.z())
	{
	}
}
