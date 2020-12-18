#ifndef WS_CONNECTION_H
#define WS_CONNECTION_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include "WsMessage.h"
#include "WsHeader.h"
class WsConnection{
 public:
  WsConnection(int pSock, WsHeader *pHeader);
  ~WsConnection();
  WsMessage *getMessage();
  int sendMessage(WsMessage *pMsg);
  std::string getPath();
  void shutdown();
 private:
  int getLength(unsigned char pLenght);
  WsHeader *mHeader;
  int mSock;
  int mConnected;
};


#endif
