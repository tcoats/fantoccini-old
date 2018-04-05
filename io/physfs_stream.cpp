#include "../fantoccini.h"
#include "physfs_stream.h"

#include <vector>

using namespace std;

namespace PhysFS {
  class physfsbuffer : public streambuf {
  private:
    physfsbuffer(const physfsbuffer & other);
    physfsbuffer& operator=(const physfsbuffer& other);

    int_type underflow() {
      if (PHYSFS_eof(file)) return traits_type::eof();
      auto bytesRead = PHYSFS_readBytes(file, buffer, bufferSize);
      setg(buffer, buffer, buffer + bytesRead);
      return (unsigned char) *gptr();
    }

    pos_type seekoff(off_type pos, ios_base::seekdir dir, ios_base::openmode mode) {
      switch (dir) {
      case ios_base::beg:
        PHYSFS_seek(file, pos);
        break;
      case ios_base::cur:
        PHYSFS_seek(file, (PHYSFS_tell(file) + pos) - (egptr() - gptr()));
        break;
      case ios_base::end:
        PHYSFS_seek(file, PHYSFS_fileLength(file) + pos);
        break;
      }
      if (mode & ios_base::in) setg(egptr(), egptr(), egptr());
      if (mode & ios_base::out) setp(buffer, buffer);
      auto res = PHYSFS_tell(file);
      if (res == -1)
        Bus::Emit(Event::OnError, 0,
          PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
      return res;
    }

    pos_type seekpos(pos_type pos, ios_base::openmode mode) {
      PHYSFS_seek(file, pos);
      if (mode & ios_base::in) setg(egptr(), egptr(), egptr());
      if (mode & ios_base::out) setp(buffer, buffer);
      auto res = PHYSFS_tell(file);
      if (res == -1)
        Bus::Emit(Event::OnError, 0,
          PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
      return res;
    }

    int_type overflow( int_type c = traits_type::eof() ) {
      if (pptr() == pbase() && c == traits_type::eof()) return 0;
      if (PHYSFS_writeBytes(file, pbase(), pptr() - pbase()) < 1)
        return traits_type::eof();
      if (c != traits_type::eof() && PHYSFS_writeBytes(file, &c, 1) < 1)
        return traits_type::eof();

      return 0;
    }

    int sync() {
      return overflow();
    }

    char* buffer;
    size_t const bufferSize;
  protected:
    PHYSFS_File* const file;
  public:
    physfsbuffer(PHYSFS_File* file, size_t bufferSize = 2048)
      : bufferSize(bufferSize), file(file) {
      buffer = new char[bufferSize];
      char* end = buffer + bufferSize;
      setg(end, end, end);
      setp(buffer, end);
    }

    ~physfsbuffer() {
      sync();
      delete[] buffer;
    }
  };

  basefstream::basefstream(PHYSFS_File* file)
    : file(file) {
    if (file == NULL) {
      Bus::Emit(Event::OnError, 0, "PhysFS file not found");
      throw invalid_argument("attempted to construct fstream with NULL ptr");
    }
  }

  basefstream::~basefstream() {
    PHYSFS_close(file);
  }

  size_t basefstream::length() {
    return PHYSFS_fileLength(file);
  }

  ifstream::ifstream(const string& filename)
    : basefstream(PHYSFS_openRead(filename.c_str())), istream(new physfsbuffer(file))
  {}

  ifstream::~ifstream() {
    delete rdbuf();
  }

  ofstream::ofstream(const string& filename)
    : basefstream(PHYSFS_openWrite(filename.c_str())), ostream(new physfsbuffer(file))
  {}

  ofstream::~ofstream() {
    delete rdbuf();
  }
}
