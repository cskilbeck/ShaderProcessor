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
		virtual void DrawConvexMesh(Matrix const &m, btBvhTriangleMeshShape const *mesh) = 0;
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

		struct Mesh
		{
			//////////////////////////////////////////////////////////////////////

			struct Vert
			{
				Float3 Position;
				Float3 Normal;
			};

			//////////////////////////////////////////////////////////////////////

			struct Triangle
			{
				Vert const &p1, &p2, &p3;
				int index;

				Triangle(Mesh const &m, int i)
					: index(i)
					, p1(m.mVertices[m.mIndices[i + 0]])
					, p2(m.mVertices[m.mIndices[i + 1]])
					, p3(m.mVertices[m.mIndices[i + 2]])
				{
				}

				Vec4f GetInterpolatedNormal(CVec4f pos) const;
				Vec4f GetNormal() const;
				Vec4f GetPlane() const;
				float DistanceFrom(CVec4f pos) const;
			};

			//////////////////////////////////////////////////////////////////////

			Mesh();
			~Mesh();

			int LoadFromMesh(aiNode *node, aiMesh *mesh);

			void AddToWorld(uint16 collisionGroup, uint16 collisionMask)
			{
				DynamicsWorld->addRigidBody(mBody, collisionGroup, collisionMask);
			}

			vector<Vert> mVertices;
			vector<uint32> mIndices;	// 3 x uint16 per triangle
			btTriangleIndexVertexArray *mArray;
			btBvhTriangleMeshShape *mShape;
			btRigidBody *mBody;
			Triangle GetTriangle(int index)
			{
				return Triangle(*this, index * 3);
			}

			bool IsConvex() const;
			Vec4f GetInterpolatedNormal(int triangleIndex, CVec4f pos);
			void DrawNormals();
		};

		struct World
		{
			vector<Mesh *> mMeshes;

			int LoadFromNode(aiScene const *scene, aiNode *node);
			void DrawNormals();
		};

		//////////////////////////////////////////////////////////////////////

		enum
		{
			CarMask = 128,
			GroundMask = 256,
			WheelMask = 512
		};

		//////////////////////////////////////////////////////////////////////

		HRESULT Open(DXWindow *window);
		void Close();

		//////////////////////////////////////////////////////////////////////

		btRigidBody *CreateRigidBody(float mass, const btTransform &transform, btCollisionShape *shape, btRigidBody::btRigidBodyConstructionInfo *ci = null);
		void AddRigidBody(btRigidBody *b, uint16 collisionGroup, uint16 collisionMask);
		void DeleteRigidBody(btRigidBody * &b);
		btVector3 inertia(float mass, btCollisionShape *shape);
		btCompoundShape *InitCompoundShape(btCompoundShape *shape, btScalar *masses, btTransform &shift);
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
