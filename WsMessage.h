#ifndef WS_MESSAGE_H
#define WS_MESSAGE_H

#include <stdio.h>
#include <malloc.h>
#include <string>
#define DEFAULT_MSG_SIZE 128

class WsMessage{
 public:
  WsMessage(unsigned char pOp);
  ~WsMessage();
  int size();
  unsigned char opcode();
  const unsigned char *getData();
  std::string getString();
  void insertData(const unsigned char *pData, int pLen);
  unsigned char *insertData(int pLen);
 private:
  unsigned char mOp;
  unsigned char *mData;
  int mLen;
  int mDataSize;
};


#endif
