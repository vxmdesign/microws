#include "WsMessage.h"

WsMessage::WsMessage(unsigned char pOp){
  mData = (unsigned char*)malloc(DEFAULT_MSG_SIZE);
  mLen = 0;
  mDataSize = DEFAULT_MSG_SIZE;
  mOp = pOp;
}

WsMessage::~WsMessage(){
  free(mData);
}

int WsMessage::size(){
  return mLen;  
}

const unsigned char *WsMessage::getData(){
  return mData;
}

unsigned char WsMessage::opcode(){
  return mOp;
}

void WsMessage::insertData(const unsigned char *pData, int pLen){
  unsigned char *tmp;
  int c;
  if(mLen + pLen >= mDataSize){
    tmp = (unsigned char *)malloc(mLen + pLen + DEFAULT_MSG_SIZE);
    for(c = 0; c < mLen; c++){
      tmp[c] = mData[c];
    }
    free(mData);
    mData = tmp;
    mDataSize = mLen + pLen + DEFAULT_MSG_SIZE;
  }
  for(c = 0; c < pLen; c++){
    mData[c + mLen] = pData[c];
  }
  mLen = mLen + pLen;
}

unsigned char *WsMessage::insertData(int pLen){
  unsigned char *tmp;
  int c;
  if(mLen + pLen >= mDataSize){
    tmp = (unsigned char *)malloc(mLen + pLen + DEFAULT_MSG_SIZE);
    for(c = 0; c < mLen; c++){
      tmp[c] = mData[c];
    }
    free(mData);
    mData = tmp;
    mDataSize = mLen + pLen + DEFAULT_MSG_SIZE;
  }
  tmp = &(mData[mLen]);
  mLen += pLen;
  return tmp;
}

