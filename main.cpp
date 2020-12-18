#include <stdio.h>
#include "WsServer.h"
#include "WsConnection.h"
#include "WsMessage.h"


int main(){
  WsServer *ws;
  WsConnection *wc;
  WsMessage *msg;
  WsMessage *txmsg;
  ws = new WsServer();
  ws->insertPath("/echo");
  if(ws->startServer()<0){
    printf("failed to start server\n");
    return -1;
  }
  wc = ws->wsAccept();
  if(ws == NULL){
    printf("failed to get connection\n");
    return -1;
  }
  msg = wc->getMessage();
  if(msg == NULL){
    printf("failed to get message\n");
    return -1;
  }
  printf("Message: %s\n", msg->getData());
  txmsg = new WsMessage(129);
  txmsg->insertData(msg->getData(), msg->size());
  if(wc->sendMessage(txmsg) != 0){
    printf("Failed to write message\n");
  }
}
