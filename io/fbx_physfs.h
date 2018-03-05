#ifndef _fbx_physfs_h
#define _fbx_physfs_h

#include "fbx.h"
#include "physfs_stream.h"
#include <fbxsdk/core/fbxstream.h>

namespace Fbx {
  class PhysFSStream : public FbxStream {
  public:
    PhysFSStream(FbxManager* manager, PhysFS::ifstream* stream);
    FbxStream::EState GetState() override;
    bool Open(void* pStreamData) override;
    bool Close() override;
    bool Flush() override;
    int Write(const void*, int) override;
    int Read(void* dst, int amount) const override;
    int GetReaderID() const override;
    int GetWriterID() const override;
    void Seek(const FbxInt64& pOffset, const FbxFile::ESeekPos& pSeekPos) override;
    long GetPosition() const override;
    void SetPosition(long pPosition) override;
    int GetError() const override;
    void ClearError() override;

  private:
    EState _state = eEmpty;
    int _readerID;
    int _error;
    PhysFS::ifstream* _stream;
  };
}

#endif
