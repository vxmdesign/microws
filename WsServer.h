#ifndef WS_SERVER_H
#define WS_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <openssl/sha.h>
#include <string>
#include <vector>
#include "WsConnection.h"
#include "WsHeader.h"

class WsServer{
 public:
  WsServer();
  ~WsServer();
  void insertPath(std::string pPath);
  int startServer();
  WsConnection *wsAccept();
  bool serverActive();
 private:
  unsigned char remap64(unsigned char pC);
  void base64(unsigned char *pIn, unsigned char *pOut);
  int writeHandShake(int pCsock, const char *pSecKey);
  bool validPath(std::string pPath);
  int mSock;
  sockaddr_in mServAddr;
  bool mActive;
  std::vector<std::string> mPathList;  
};


#endif
