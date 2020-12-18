#include "WsServer.h"

WsServer::WsServer(){
  mActive = false;
  mSock = -1;
  mPathList.clear();
}

WsServer::~WsServer(){
  if(mActive == true){
    close(mSock);
  }
}

void WsServer::insertPath(std::string pPath){
  mPathList.push_back(pPath);
}

int WsServer::startServer(){
  int optval;
  optval = 1;
  bzero((char*)&mServAddr, sizeof(mServAddr));
  mServAddr.sin_family = AF_INET;
  mServAddr.sin_addr.s_addr = INADDR_ANY;
  mServAddr.sin_port = htons(8080);
  mSock = socket(AF_INET, SOCK_STREAM, 0);
  if(mSock < 0){
    printf("Cannot start server\n");
    return -1;
  }
  if(setsockopt(mSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))< 0){
    printf("cannot set socket option\n");
    return -1;
  }
  if(bind(mSock, (struct sockaddr*)&mServAddr, sizeof(mServAddr))<0){
    printf("could not bind\n");
    return -1;
  }
  listen(mSock, 5);
  mActive = true;
  return 0;
}

WsConnection *WsServer::wsAccept(){
  int csock;
  sockaddr_in cli;
  socklen_t slen;
  WsHeader *ws;
  if(mActive == false){
    return NULL;
  }
  bzero(&(cli), sizeof(cli));
  csock = accept(mSock, (struct sockaddr*)&cli, &slen);
  if(csock < 0){
    close(mSock);
    mActive = false;
    return NULL;
  }
  ws = new WsHeader(csock);
  if(!ws->valid()){
    delete ws;
    close(csock);
    return NULL;
  }
  if(!validPath(ws->getPath())){
    delete ws;
    close(csock);
    return NULL;
  }
  if(ws->contains("Sec-WebSocket-Key") == false){
    delete ws;
    close(csock);
    return NULL;
  }
  if(writeHandShake(csock, ws->getHeader("Sec-WebSocket-Key").c_str())<0){
    delete ws;
    close(csock);
    return NULL;
  }
  return new WsConnection(csock, ws);
}

bool WsServer::serverActive(){
  return mActive;
}

bool WsServer::validPath(std::string pPath){
  unsigned int c;
  for(c = 0; c < mPathList.size(); c++){
    if(mPathList[c] == pPath){
      return true;
    }
  }
  return false;
}

int WsServer::writeHandShake(int pCsock,  const char *pSecKey){
  const char l1[] = "HTTP/1.1 101 Web Socket Protocol Handshake\r\n";
  const char l2[] = "Upgrade: websocket\r\n";
  const char l3[] = "Connection: Upgrade\r\n";
  const char magic[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  char l4[256];
  char sha[20];
  char key[256];
  int n;
  strcpy(key, pSecKey);
  n = strlen(key);
  strcpy(&(key[n]), magic);
  SHA1((unsigned char*)key, strlen(key), (unsigned char*)sha);
  base64((unsigned char*)sha, (unsigned char*)key);
  sprintf(l4,  "Sec-WebSocket-Accept:%s\r\n\r\n", key);
  n = strlen(l1);  
  if(write(pCsock, l1, n)!=n){
    return -1;
  }
  n = strlen(l2);  
  if(write(pCsock, l2, n)!=n){
    return -1;
  }
  n = strlen(l3);
  if(write(pCsock, l3, n)!= n){
    return -1;
  }
  n = strlen(l4);
  if(write(pCsock, l4, n)!= n){
    return -1;
  }
  return 0;
}


void WsServer::base64(unsigned char *pIn, unsigned char *pOut){
  int n;
  int c;
  int q;
  int r;
  int oidx;
  unsigned char s;
  unsigned int tmp;

  n = 20; //all keys are 20 bytes
  r = n % 3;
  n = n / 3;
  oidx = 0;

  for(c = 0; c < n; c++){
    tmp = pIn[c*3] << 16;
    tmp |= pIn[(c*3) + 1] << 8;
    tmp |= pIn[(c*3) + 2];
    for(q = 0; q < 4; q++){
      s = tmp & 0x3F;
      tmp = tmp >> 6;
      s = remap64(s);
      pOut[oidx + (3-q)] = s;
    }
    oidx+=4;
  }
  tmp = 0;
  n = n * 3;
  for(c = 0; c < r; c++){
    tmp = tmp << 8;
    tmp |= pIn[n + c];
  }
  if(r == 2){
    pOut[oidx+3] = '=';
    s = (tmp & 0xF)<<2;
    s = remap64(s);
    pOut[oidx+2] = s;
    tmp = tmp >> 4;
    pOut[oidx+1] = remap64(tmp & 0x3f);
    tmp = tmp >> 6;
    pOut[oidx] = remap64(tmp & 0x3f);
    oidx+=4;
  }else if(r == 1){
    pOut[oidx+3] = '=';
    pOut[oidx+2] = '=';
    s = (tmp & 0x3) << 4;
    pOut[oidx+1] = remap64(s);
    tmp = tmp>> 4;
    pOut[oidx] = remap64(tmp & 0x3f);
  }
  pOut[oidx] = '\0';  
}

unsigned char WsServer::remap64(unsigned char pC){
  if(pC <= 25){
    pC = pC + 'A';
  }else if(pC <= 51){
    pC = (pC - 26) + 'a';
  }else if(pC <= 61){
    pC = (pC - 52) + '0';
  }else if(pC == 62){
    pC = '+';
  }else if(pC == 63){
    pC = '/';
  }else{
    printf("invalid input\n");
  }
  return pC;
}
