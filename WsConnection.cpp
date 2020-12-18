#include "WsConnection.h"

WsConnection::WsConnection(int pSock, WsHeader *pHeader){
  mSock = pSock;
  mHeader = pHeader;
  mConnected = 1;
}

WsConnection::~WsConnection(){
  shutdown();
}

std::string WsConnection::getPath(){
  return mHeader->getPath();
}

void WsConnection::shutdown(){
  mConnected = 0;
  close(mSock);
}

int WsConnection::getLength(unsigned char pLength){
  int len;
  unsigned char d[8];
  int c;
  if((pLength & 0x80) == 0){
    return -1;
  }
  pLength &= 0x7f;
  if(pLength == 126){
    if(read(mSock, d, 2) != 2){
      return -1;
    }
    len = d[0] << 8;
    len |= d[1];
  }else if(pLength == 127){
    if(read(mSock, d, 8) != 8){
      return -1;
    }
    for(c = 0; c < 4; c++){
      if(d[c] != 0){
	printf("Message too large. Currently unsupported\n");
	return -1;
      }
    }
    if((d[4] & 0x80) != 0){
      printf("Message too large. Currently unsupported\n");
      return -1;
    }
    len = d[4] << 24;
    len |= d[5] << 16;
    len |= d[6] << 8;
    len |= d[7];
  }else{
    len = pLength;
  }
  return len;    
}
  
WsMessage *WsConnection::getMessage(){
  unsigned char op[2];
  unsigned char mask[4];
  int len;
  WsMessage *msg;
  unsigned char *data;
  int c;
  if(mConnected != 1){
    return NULL;
  }
  msg = NULL;
  do{
    if(read(mSock, op, 2)!=2){
      shutdown();
      return NULL;
    }
    len = getLength(op[1]);
    if(len < 0){
      shutdown();
      return NULL;
    }
    if(read(mSock, mask, 4)!= 4){
      shutdown();
      return NULL;
    }
    if(msg == NULL){
      msg = new WsMessage(op[0] & 0xF);
    }
    data = msg->insertData(len);
    if(read(mSock, data, len)!=len){
      delete msg;
      shutdown();
      return NULL;
    }
    for(c = 0; c < len; c++){
      data[c] = data[c] ^ mask[c%4];    
    }
  }while((op[0] & 0x80) == 0);
  return msg;
}


int WsConnection::sendMessage(WsMessage *pMsg){
  unsigned char header[10];
  int len;
  int txlen;
  header[0] = pMsg->opcode();
  len = pMsg->size();
  if(len >= 126){
    if(len > 0xffff){
      header[1] = 127;
      header[2] = 0;
      header[3] = 0;
      header[4] = 0;
      header[5] = 0;
      header[6] = (len >> 24) & 0x7F;
      header[7] = (len >> 16) & 0xFF;
      header[8] = (len >> 8) & 0xFF;
      header[9] = (len & 0xFF);
      txlen = 10;
    }else{
      header[1] = 126;
      header[2] = (len >> 8) & 0xFF;
      header[3] = (len & 0xFF);
    }
  }else{
    header[1] = len;
    txlen = 2;
  }
  if(write(mSock, header, txlen) != txlen){
    printf("send message failed header\n");
    return -1;
  }
  if(write(mSock, pMsg->getData(), len) != len){
    printf("send message failed payload\n");
    return -1;
  }
  return 0;
}
