#include "fbx_physfs.h"

using namespace std;

// https://forums.autodesk.com/t5/fbx-forum/fbxstream-implementation-a-reading-memory-stream/td-p/6665752
// https://github.com/SharpLinesTech/slt_tech_runtime/blob/master/tools/model_compiler/fbx_memory_stream.h

namespace Fbx {
  PhysFSStream::PhysFSStream(FbxManager* manager, PhysFS::ifstream* stream)
      :  _error(0), _stream(stream) {
    auto registry = manager->GetIOPluginRegistry();
    _readerID = registry->FindReaderIDByExtension("fbx");
  }

  // Inherited via FbxStream
  FbxStream::EState PhysFSStream::GetState() {
    return _state;
  }

  bool PhysFSStream::Open(void* pStreamData) {
    _state = eOpen;
    _stream->clear();
    _stream->seekg(0);
    return true;
  }

  bool PhysFSStream::Close() {
    _state = eClosed;
    _stream->seekg(0);
    return true;
  }

  bool PhysFSStream::Flush() {
    return true;
  }

  int PhysFSStream::Write(const void*, int) {
    return 0;
  }

  int PhysFSStream::Read(void* dst, int amount) const {
    _stream->read((char*)dst, amount);
    if (_stream->eof()) {
      return _stream->gcount();
    }
    if (_stream->fail()) {
      const_cast<PhysFSStream&>(*this)._error = 1;
      return _stream->gcount();
    }
    return amount;
  }

  int PhysFSStream::GetReaderID() const {
    return _readerID;
  }

  int PhysFSStream::GetWriterID() const {
    return -1;
  }

  void PhysFSStream::Seek(const FbxInt64& pOffset, const FbxFile::ESeekPos& pSeekPos) {
    _stream->clear();
    switch(pSeekPos) {
      case FbxFile::eBegin:
        _stream->seekg(pOffset, ios_base::beg);
        break;
      case FbxFile::eCurrent:
        _stream->seekg(pOffset, ios_base::cur);
        break;
      case FbxFile::eEnd:
        _stream->seekg(pOffset, ios_base::end);
        break;
    }
  }

  long PhysFSStream::GetPosition() const {
    auto res = _stream->tellg();
    if (res == -1) {
      const_cast<PhysFSStream&>(*this)._error = 1;
      return 0;
    }
    return res;
  }

  void PhysFSStream::SetPosition(long pPosition) {
    _stream->clear();
    _stream->seekg(pPosition);
  }

  int PhysFSStream::GetError() const {
    return _error;
  }

  void PhysFSStream::ClearError() {
    _stream->clear();
    _error = 0;
  }
}
