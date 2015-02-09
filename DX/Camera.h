//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{

	class Camera : Aligned16
	{
	public:

		//////////////////////////////////////////////////////////////////////

		Camera();

		//////////////////////////////////////////////////////////////////////

		Matrix const &GetTransformMatrix() const
		{
			return mTransformMatrix;
		}

		//////////////////////////////////////////////////////////////////////

		Matrix GetTransformMatrix(Matrix const &modelMatrix) const
		{
			return modelMatrix * mTransformMatrix;
		}

		//////////////////////////////////////////////////////////////////////

		Matrix const &GetProjectionMatrix() const
		{
			return mProjectionMatrix;
		}

		//////////////////////////////////////////////////////////////////////

		Vec4f GetForwardVector() const
		{
			return GetZ3(mViewMatrix.r[0], mViewMatrix.r[1], mViewMatrix.r[2]);
		}

		//////////////////////////////////////////////////////////////////////

		Vec4f GetFlatForwardVector() const
		{
			Vec4f f = SetZ(GetForwardVector(), 0);
			if(GetX(f) == 0 && GetY(f) == 0)
			{
				return f;
			}
			return Normalize(f);
		}

		//////////////////////////////////////////////////////////////////////

		Vec4f GetUpVector() const
		{
			return GetY3(mViewMatrix.r[0], mViewMatrix.r[1], mViewMatrix.r[2]);
		}

		//////////////////////////////////////////////////////////////////////

		Vec4f GetStrafeVector() const
		{
			return GetX3(mViewMatrix.r[0], mViewMatrix.r[1], mViewMatrix.r[2]);
		}

		//////////////////////////////////////////////////////////////////////

		void SetViewMatrix(Matrix const &m)
		{
			mViewMatrix = m;
		}

		//////////////////////////////////////////////////////////////////////

		Matrix const &GetViewMatrix() const
		{
			return mViewMatrix;
		}

		//////////////////////////////////////////////////////////////////////

		Vec4f GetYawPitchRoll() const;

		//////////////////////////////////////////////////////////////////////

		void CalculateViewMatrix(Vec4f target, Vec4f position, Vec4f up);
		void CalculateViewMatrix(Vec4f position, float yaw, float pitch, float roll);
		void CalculatePerspectiveProjectionMatrix(float fov = 0.5f, float aspectRatio = 4.0f / 3.0f, float nearZ = 1.0f, float farZ = 1000.0f);
		void CalculateOrthoProjectionMatrix(float width = 40, float height = 30, float nearZ = 1.0f, float farZ = 1000.0f);
		void CalculateOrthoProjectionMatrix2D(int width, int height);
		void CalculateViewProjectionMatrix();
		void CalculateViewProjectionMatrix(Matrix const &modelMatrix);

		static Matrix ViewMatrix(Vec4f target, Vec4f position, Vec4f up);
		static Matrix ViewMatrix(Vec4f position, float yaw, float pitch, float roll);

		static Matrix PerspectiveProjection(float fov = 0.5f, float aspectRatio = 4.0f / 3.0f, float nearZ = 1.0f, float farZ = 1000.0f);
		static Matrix OrthoProjection3D(float width, float height, float nearZ = 1.0f, float farZ = 1000.0f);
		static Matrix OrthoProjection2D(int width, int height);

		//////////////////////////////////////////////////////////////////////

	protected:

		Matrix			mViewMatrix;
		Matrix			mProjectionMatrix;
		Matrix			mTransformMatrix;
	};

}