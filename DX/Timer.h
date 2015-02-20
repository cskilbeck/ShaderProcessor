//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct Timer
	{
		//////////////////////////////////////////////////////////////////////

		Timer()
		{
			Reset();
		}

		//////////////////////////////////////////////////////////////////////

		static uint64 Frequency()
		{
			uint64 frequency;
			QueryPerformanceFrequency((LARGE_INTEGER *)&frequency);
			return frequency;
		}

		//////////////////////////////////////////////////////////////////////

		static uint64 Ticks()
		{
			uint64 time;
			QueryPerformanceCounter((LARGE_INTEGER *)&time);
			return time;
		}

		//////////////////////////////////////////////////////////////////////

		void Reset()
		{
			mPaused = false;
			mOldTicks = Ticks();
			mCurrentTicks = 0;
		}

		//////////////////////////////////////////////////////////////////////

		void Pause()
		{
			if(!mPaused)
			{
				mPaused = true;
				mOldTicks = Ticks();
			}
		}

		//////////////////////////////////////////////////////////////////////

		void UnPause()
		{
			mPaused = false;
			mDeltaTicks = 0;
			mOldTicks = Ticks();
		}

		//////////////////////////////////////////////////////////////////////

		void Update()
		{
			uint64 ticks = Ticks();
			if(!mPaused)
			{
				mDeltaTicks = ticks - mOldTicks;
				mCurrentTicks += mDeltaTicks;
				mOldTicks = ticks;
			}
		}

		//////////////////////////////////////////////////////////////////////

		double Delta()
		{
			return (double)mDeltaTicks / Frequency();
		}

		//////////////////////////////////////////////////////////////////////

		double WallTime()
		{
			return (double)mCurrentTicks / Frequency();
		}

		//////////////////////////////////////////////////////////////////////

	protected:

		uint64 mOldTicks;		// last rdtsc
		uint64 mDeltaTicks;		// last difference (can't be paused)
		uint64 mCurrentTicks;	// current tick count, might have been paused
		bool mPaused;
	};

	//////////////////////////////////////////////////////////////////////
}