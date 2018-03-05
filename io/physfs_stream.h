#ifndef _physfs_stream_h
#define _physfs_stream_h

#include <physfs.h>
#include <string>
#include <iostream>

namespace PhysFS {
  class basefstream {
  protected:
    PHYSFS_File* const file;
  public:
    basefstream(PHYSFS_File * file);
    virtual ~basefstream();
    size_t length();
  };

  class ifstream : public basefstream, public std::istream {
  public:
    ifstream(std::string const& filename);
    virtual ~ifstream();
  };

  class ofstream : public basefstream, public std::ostream {
  public:
    ofstream(std::string const& filename);
    virtual ~ofstream();
  };

  class fstream : public basefstream, public std::iostream {
  public:
    fstream(std::string const& filename);
    virtual ~fstream();
  };
}

#endif
