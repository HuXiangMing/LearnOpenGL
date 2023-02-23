#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <cstdlib>
#include <direct.h>

class FileSystem
{
private:
  typedef std::string (*Builder) (const std::string& path);

public:
  static std::string getPath(const std::string& path)
  {
    static std::string(*pathBuilder)(std::string const &) = getPathBuilder();
    return (*pathBuilder)(path);
  }

private:
  static std::string const & getRoot()
  {
    char* envRootbuff=nullptr;
    size_t sz = 0;
    if (_dupenv_s(&envRootbuff, &sz, "LOGL_ROOT_PATH") == 0 && envRootbuff != nullptr)
    {
        printf(envRootbuff);
    }
    
    char currentDir[250];
    static char const * givenRoot = (envRootbuff != nullptr ? envRootbuff : _getcwd(currentDir, 250));
    static std::string givenRootStr = (givenRoot != nullptr ? givenRoot : "");
    
    static std::string rootDir = givenRootStr.substr(0, givenRootStr.find_last_of("\\"));
    printf(rootDir.c_str());
    return rootDir;
  }

  //static std::string(*foo (std::string const &)) getPathBuilder()
  static Builder getPathBuilder()
  {
    if (getRoot() != "")
      return &FileSystem::getPathRelativeRoot;
    else
      return &FileSystem::getPathRelativeBinary;
  }

  static std::string getPathRelativeRoot(const std::string& path)
  {
    return getRoot() + std::string("/") + path;
  }

  static std::string getPathRelativeBinary(const std::string& path)
  {
    return "../../../" + path;
  }


};

// FILESYSTEM_H
#endif
