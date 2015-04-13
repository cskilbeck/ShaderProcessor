//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	namespace Physics
	{
		//////////////////////////////////////////////////////////////////////

		extern btDefaultCollisionConfiguration *		CollisionConfiguration;
		extern btCollisionDispatcher *					Dispatcher;
		extern btBroadphaseInterface *					OverlappingPairCache;
		extern btSequentialImpulseConstraintSolver *	Solver;
		extern btDiscreteDynamicsWorld *				DynamicsWorld;

		//////////////////////////////////////////////////////////////////////

		void Open();
		void Close();

		//////////////////////////////////////////////////////////////////////

		void DeleteRigidBody(btRigidBody * &b);
		btVector3 inertia(float mass, btCollisionShape *shape);
		btCompoundShape *InitCompoundShape(btCompoundShape *shape, btScalar *masses, btTransform &shift);

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
