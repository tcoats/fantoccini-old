#include "state.h"
#include "../fantoccini.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

namespace State {
  void Bind() {
  }
}

using namespace std;

#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>

#define strtok_r strtok_s

namespace State {
  string win32_utf16_to_utf8(const wchar_t* wstr)
  {
    string res;
    // If the 6th parameter is 0 then WideCharToMultiByte
    // returns the number of bytes needed to store the result.
    auto actualSize = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
    if (actualSize > 0) {
      //If the converted UTF-8 string could not be in the
      // initial buffer. Allocate one that can hold it.
      vector<char> buffer(actualSize);
      actualSize = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &buffer[0],
        buffer.size(), nullptr, nullptr);
      res = buffer.data();
    }
    if (actualSize == 0) {
      // WideCharToMultiByte return 0 for errors.
      string errorMsg = "UTF16 to UTF8 failed with error code: "
        + GetLastError();
      Bus::Emit(Message::OnError, 0, errorMsg);
      throw runtime_error(errorMsg.c_str());
    }
    return res;
  }

  string GetDataDirectory() {
    wchar_t szPath[MAX_PATH];
    szPath[0] = 0;
    if (!SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, szPath)))
    {
      Bus::Emit(Message::OnError, 0, "Unable to find %APPDATA%");
      throw runtime_error("Unable to find %APPDATA%");
    }
    string res = win32_utf16_to_utf8(szPath) + "\\Fantoccini";
    if (!CreateDirectory(res.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
      Bus::Emit(Message::OnError, 0, "Unable to create %APPDATA% directory");
    return res;
  }
}

#else

#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace State {
  string GetHome() {
    string res;
    auto uid = getuid();
    auto homeEnv = getenv("HOME");
    if (uid != 0 && homeEnv) {
      //We only acknowlegde HOME if not root.
      res = homeEnv;
      return res;
    }
    auto pw = getpwuid(uid);
    if (!pw) {
      Bus::Emit(Message::OnError, 0, "Unable to get passwd struct");
      throw runtime_error("Unable to get passwd struct");
    }
    auto tempRes = pw->pw_dir;
    if (!tempRes) {
      Bus::Emit(Message::OnError, 0, "User has no home directory");
      throw runtime_error("User has no home directory");
    }
    res = tempRes;
    return res;
  }

  string GetDataDirectory() {
    string envName = "XDG_DATA_HOME";
    string res;
    auto envHome = getenv(envName.c_str());
    if (envHome) {
      if (envHome[0] != '/') {
        string errorMsg = "$" + envName + " does not start with an '/'. XDG specifies that the value must be absolute. The current value is: \"" + envHome + "\"";
        Bus::Emit(Message::OnError, 0, errorMsg);
        throw runtime_error(errorMsg);
      }
      res = envHome;
    }
    else {
      res = GetHome();
    }
    #if defined(__APPLE__)
    res += "/Library/Application Support/Fantoccini";
    #else
    res += "/.local/share/fantoccini";
    #endif
    struct stat sb;
    if (stat(res.c_str(), &sb) != 0 && mkdir(res.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
      Bus::Emit(Message::OnError, 0, "Unable to create data directory");
    return res;
  }
}
#endif
