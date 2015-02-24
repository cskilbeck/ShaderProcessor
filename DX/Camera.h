//////////////////////////////////////////////////////////////////////

#pragma once

namespace DX
{
	class Camera : Aligned16
	{
	public:

		Camera();
		Matrix const &		GetTransformMatrix() const;
		Matrix				GetTransformMatrix(Matrix const &modelMatrix) const;
		Matrix const &		GetProjectionMatrix() const;
		Vec4f				GetForwardVector() const;
		Vec4f				GetFlatForwardVector() const;
		Vec4f				GetUpVector() const;
		Vec4f				GetStrafeVector() const;
		void				SetViewMatrix(Matrix const &m);
		Matrix const &		GetViewMatrix() const;
		Vec4f				GetYawPitchRoll() const;
		
		void				CalculateViewMatrix(Vec4f target, Vec4f position, Vec4f up);
		void				CalculateViewMatrix(Vec4f position, float yaw, float pitch, float roll);
		void				CalculatePerspectiveProjectionMatrix(float fov = 0.5f, float aspectRatio = 4.0f / 3.0f, float nearZ = 1.0f, float farZ = 1000.0f);
		void				CalculateOrthoProjectionMatrix(float width = 40, float height = 30, float nearZ = 1.0f, float farZ = 1000.0f);
		void				CalculateOrthoProjectionMatrix2D(int width, int height);
		void				CalculateViewProjectionMatrix();
		void				CalculateViewProjectionMatrix(Matrix const &modelMatrix);

		//////////////////////////////////////////////////////////////////////

	protected:

		Matrix			mViewMatrix;
		Matrix			mProjectionMatrix;
		Matrix			mTransformMatrix;
	};


	//////////////////////////////////////////////////////////////////////

	inline Matrix const &Camera::GetTransformMatrix() const
	{
		return mTransformMatrix;
	}

	//////////////////////////////////////////////////////////////////////

	inline Matrix Camera::GetTransformMatrix(Matrix const &modelMatrix) const
	{
		return modelMatrix * mTransformMatrix;
	}

	//////////////////////////////////////////////////////////////////////

	inline Matrix const &Camera::GetProjectionMatrix() const
	{
		return mProjectionMatrix;
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f Camera::GetForwardVector() const
	{
		return GetZ3(mViewMatrix.r[0], mViewMatrix.r[1], mViewMatrix.r[2]);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f Camera::GetFlatForwardVector() const
	{
		Vec4f f = SetZ(GetForwardVector(), 0);
		if(GetX(f) == 0 && GetY(f) == 0)
		{
			return f;
		}
		return Normalize(f);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f Camera::GetUpVector() const
	{
		return GetY3(mViewMatrix.r[0], mViewMatrix.r[1], mViewMatrix.r[2]);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f Camera::GetStrafeVector() const
	{
		return GetX3(mViewMatrix.r[0], mViewMatrix.r[1], mViewMatrix.r[2]);
	}

	//////////////////////////////////////////////////////////////////////

	inline void Camera::SetViewMatrix(Matrix const &m)
	{
		mViewMatrix = m;
	}

	//////////////////////////////////////////////////////////////////////

	inline Matrix const &Camera::GetViewMatrix() const
	{
		return mViewMatrix;
	}


}