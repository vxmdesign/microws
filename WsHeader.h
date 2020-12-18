#ifndef WS_HEADER_H
#define WS_HEADER_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <map>
#include <string.h>
#include <string>

class WsHeader{
 public:
  WsHeader(int pSock);
  bool validHeader();
  std::string getMethod();
  std::string getPath();
  std::string getVersion();
  std::string getHeader(std::string pName);
  bool contains(std::string pName);
  bool valid();
 private:
  int parseMethod(unsigned char *pMth);
  bool validPath(std::string pPath);
  std::string mMethod;
  std::string mPath;
  std::string mVersion;
  std::map<std::string, std::string> mHeaderList;
  bool mValid;
};

#endif
