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

		btRigidBody *CreateRigidBody(float mass, const btTransform &transform, btCollisionShape *shape, btRigidBody::btRigidBodyConstructionInfo *ci)
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
			return body;
		}

		//////////////////////////////////////////////////////////////////////

		void AddRigidBody(btRigidBody *b, uint16 collisionGroup, uint16 collisionMask)
		{
			DynamicsWorld->addRigidBody(b, collisionGroup, collisionMask);
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

				case CONVEX_TRIANGLEMESH_SHAPE_PROXYTYPE:
				{
					w->DrawConvexMesh(IdentityMatrix, (btBvhTriangleMeshShape const *)shape);
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

		Mesh::Mesh()
		{
		}

		Mesh::~Mesh()
		{
		}

		int Mesh::LoadFromMesh(aiNode *node, aiMesh *mesh)
		{
			// TODO (charlie): LoadDynamic() - must be convex (and allow them to have mass)
			// TODO (charlie): deal with multiple meshes, stored mass and center of mass offsets
			// TODO (charlie): deal with collision mask and group stored in the file somehow

			if(mesh == null)
			{
				return E_POINTER;
			}

			// empty mesh? that's ok man...
			if(!(mesh->HasPositions() && mesh->HasNormals() && mesh->HasFaces()))
			{
				return S_OK;
			}

			// for transforming the verts
			aiMatrix4x4 const *transform = &node->mTransformation;

			// for transforming the normals
			aiMatrix3x3 normalTransform(*transform);

			// transform the verts and normals
			mVertices.reserve(mesh->mNumVertices);
			for(uint i = 0; i < mesh->mNumVertices; ++i)
			{
				aiVector3D v = mesh->mVertices[i];
				aiVector3D n = mesh->mNormals[i];
				aiTransformVecByMatrix4(&v, transform);
				aiTransformVecByMatrix3(&n, &normalTransform);
				n.Normalize();
				mVertices.push_back({ { v.x, v.y, v.z }, { n.x, n.y, n.z } });
			}

			// grab the indices into a linear array
			mIndices.reserve(mesh->mNumFaces * 3);
			for(uint i = 0; i < mesh->mNumFaces; ++i)
			{
				for(uint j = 0; j < 3; ++j)
				{
					mIndices.push_back((uint32)mesh->mFaces[i].mIndices[j]);
				}
			}

			// create the physics objects
			mArray = new btTriangleIndexVertexArray((int)mesh->mNumFaces,
													(int *)mIndices.data(),
													3 * sizeof(uint32),
													(int)mesh->mNumVertices,
													(btScalar *)mVertices.data(),
													sizeof(Vert));
			bool useQuantizedAabbCompression = true;
			mShape = new btBvhTriangleMeshShape(mArray, useQuantizedAabbCompression);
			btTransform t(btQuaternion(0, 0, 0), btVector3(0, 0, 0));
			mBody = CreateRigidBody(0, t, mShape);
			mBody->setUserPointer(this);
			return S_OK;
		}

		int World::LoadFromNode(aiScene const *scene, aiNode *node)
		{
			mMeshes.resize(node->mNumMeshes);
			for(uint m = 0; m < node->mNumMeshes; ++m)
			{
				mMeshes[m] = new Mesh();
				DXR(mMeshes[m]->LoadFromMesh(node, scene->mMeshes[node->mMeshes[m]]));
				mMeshes[m]->AddToWorld(GroundMask, -1);
			}
			return S_OK;
		}

		void Mesh::DrawNormals()
		{
			for(auto &v : mVertices)
			{
				debug_line(Vec4(v.Position), Vec4(v.Position) + Vec4(v.Normal) * 2, Color::White);
			}
		}

		void World::DrawNormals()
		{
			for(auto &m : mMeshes)
			{
				m->DrawNormals();
			}
		}

		bool Mesh::IsConvex() const
		{
			for(int i = 0; i < mIndices.size(); i += 3)
			{
				Triangle t(*this, i);
				for(int j = 0; j < mVertices.size(); ++j)
				{
					if(t.DistanceFrom(Vec4(mVertices[j].Position)) > FLT_EPSILON)
					{
						return false;
					}
				}
			}
			return true;
		}

		// if h is not coplanar to and contained within the triangle the results are not valid

		Vec4f Mesh::Triangle::GetInterpolatedNormal(CVec4f h) const
		{
			Vec4f pos1 = Vec4(p1.Position), pos2 = Vec4(p2.Position), pos3 = Vec4(p3.Position);
			Vec4f edge1 = pos2 - pos1;
			Vec4f edge2 = pos3 - pos1;
			Vec4f edge3 = pos3 - pos2;
			assert(Length(edge1) > FLT_EPSILON);
			assert(Length(edge2) > FLT_EPSILON);
			assert(Length(edge3) > FLT_EPSILON);
			float p1p2p3 = LengthSquared(Cross(edge1, edge2));
			float p2p3p = LengthSquared(Cross(edge3, h - pos2));
			float p3p1p = LengthSquared(Cross(edge2, h - pos3));
			float u = sqrtf(p2p3p / p1p2p3);
			float v = sqrtf(p3p1p / p1p2p3);
			float w = 1.0f - u - v;
			Vec4f norm1 = Vec4(p1.Normal), norm2 = Vec4(p2.Normal), norm3 = Vec4(p3.Normal);
			Vec4f r = u * norm1 + v * norm2 + w * norm3;
			return r;
		}

		// normalized triangle normal

		Vec4f Mesh::Triangle::GetNormal() const
		{
			return Normalize(Cross(Vec4(p2.Position) - Vec4(p1.Position), Vec4(p3.Position) - Vec4(p1.Position)));
		}

		// xyzw where xyz is the normal and w is the distance from the origin

		Vec4f Mesh::Triangle::GetPlane() const
		{
			Vec4f normal = GetNormal();
			return SetW(normal, Dot(normal, Vec4(p1.Position)));
		}

		// if +ive, it's outside the plane, if -ive, it's inside the plane

		float Mesh::Triangle::DistanceFrom(CVec4f pos) const
		{
			return Dot(GetNormal(), pos - Vec4(p1.Position));
		}

	} // ::Physics}
}