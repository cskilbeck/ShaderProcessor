#pragma once

//implement FileStream that derives from IStream
namespace DX
{
	class FileStream: public IStream
	{
		FileStream(FileBase &file)
			: _hFile(file)
			, _refcount(1)
		{
		}

		~FileStream()
		{
		}

	public:

		static HRESULT Create(FileBase *file, FileStream **stream)
		{
			*stream = new FileStream(*file);
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject)
		{
			if(iid == __uuidof(IUnknown)
			   || iid == __uuidof(IStream)
			   || iid == __uuidof(ISequentialStream))
			{
				*ppvObject = static_cast<IStream*>(this);
				AddRef();
				return S_OK;
			}
			else
			{
				return E_NOINTERFACE;
			}
		}

		virtual ULONG STDMETHODCALLTYPE AddRef(void)
		{
			return (ULONG)InterlockedIncrement(&_refcount);
		}

		virtual ULONG STDMETHODCALLTYPE Release(void)
		{
			ULONG res = (ULONG)InterlockedDecrement(&_refcount);
			if(res == 0)
			{
				delete this;
			}
			return res;
		}

		virtual HRESULT STDMETHODCALLTYPE Read(void* pv, ULONG cb, ULONG* pcbRead)
		{
			uint64 got;
			SXR(_hFile.Read(pv, cb, &got));
			*pcbRead = (ULONG)got;
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE Write(void const* pv, ULONG cb, ULONG* pcbWritten)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*,
												 ULARGE_INTEGER*)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE Commit(DWORD)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE Revert(void)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE Clone(IStream **)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER liDistanceToMove, DWORD dwOrigin,
											   ULARGE_INTEGER* lpNewFilePointer)
		{
			if(_hFile.CanSeek())
			{
				return _hFile.Seek(liDistanceToMove.QuadPart, dwOrigin, (intptr *)lpNewFilePointer);
			}
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG* pStatstg, DWORD grfStatFlag)
		{
			DXR(_hFile.GetSize((uint64 &)(pStatstg->cbSize)));
			return S_OK;
		}

	private:
		FileBase &_hFile;
		LONG _refcount;
	};
}
