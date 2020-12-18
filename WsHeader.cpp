#include "WsHeader.h"


WsHeader::WsHeader(int pSock){
  unsigned char hbuf[2048];
  unsigned char tbuf[2048];
  int idx;
  int state;
  int len;
  unsigned char v;
  std::string tg;
  std::string opt;
  
  mValid = false;
  state = 0;
  idx = 0;
  while(state != 4){
    if(read(pSock, &v, 1) != 1){
      return;
    }
    hbuf[idx] = v;
    if(state == 0){
      if(v == '\r'){
	state = 1;
      }
    }else if(state == 1){
      state = (v=='\n')?2:0;
    }else if(state == 2){
      state = (v=='\r')?3:0;      
    }else if(state == 3){
      state = (v=='\n')?4:0;
    }
    idx++;
    if(idx == 2048){
      return;
    }
  }
  len = idx;
  idx = 0;
  while(hbuf[idx] != '\r'){
    tbuf[idx] = hbuf[idx];
    idx++;
  }
  tbuf[idx] = '\0';
  if(parseMethod(tbuf) < 0){
    return;
  }
  idx = idx + 2;
  mHeaderList.clear();
  while(hbuf[idx] != '\r'){
    tg = "";
    opt = "";
    while(hbuf[idx] != ':'){
      tg+=(char)hbuf[idx];
      idx++;
    }
    idx+=2;
    while(hbuf[idx] != '\r'){
      opt+=hbuf[idx];
      idx++;
    }
    mHeaderList[tg] = opt;
    idx+=2;
    if(idx >= len){
      return;
    }
  }
  mValid = true;
}

int WsHeader::parseMethod(unsigned char *pMth){
  int idx;
  if(strncmp((char*)pMth, "GET", 3) != 0){
    return -1;
  }
  mMethod = "GET";
  mPath = "";
  mVersion = "";
  idx = 4;
  while(pMth[idx] != ' '){
    mPath+=pMth[idx];
    idx++;
  }
  idx++;
  if(strncmp((char*)&(pMth[idx]), "HTTP/", 5) != 0){
    return -1;
  }
  idx+=5;
  while(pMth[idx] != '\0'){
    mVersion += pMth[idx];
    idx++;
  }
  return 0;
}


std::string WsHeader::getPath(){
  return mPath;
}

std::string WsHeader::getMethod(){
  return mMethod;
}

std::string WsHeader::getVersion(){
  return mVersion;
}

std::string WsHeader::getHeader(std::string pName){
  if(mHeaderList.find(pName) == mHeaderList.end()){
    return std::string("");
  }
  return mHeaderList[pName];
}

bool WsHeader::contains(std::string pName){
  if(mHeaderList.find(pName) == mHeaderList.end()){
    return false;
  }
  return true;
}

bool WsHeader::valid(){
  return mValid;
}
