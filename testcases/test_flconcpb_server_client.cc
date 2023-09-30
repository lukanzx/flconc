#include <iostream>
#include <google/protobuf/service.h>

#include "flconcpb_rpc_channel.h"
#include "flconcpb_rpc_async_channel.h"
#include "flconcpb_rpc_controller.h"
#include "flconcpb_rpc_closure.h"
#include "net_address.h"
#include "test_flconcpb_server.pb.h"

void test_client() {

  flconc::IPAddress::ptr addr = std::make_shared<flconc::IPAddress>("127.0.0.1", 39999);

  flconc::FlconcPbRpcChannel channel(addr);
  QueryService_Stub stub(&channel);

  flconc::FlconcPbRpcController rpc_controller;
  rpc_controller.SetTimeout(5000);

  queryAgeReq rpc_req;
  queryAgeRes rpc_res;

  std::cout << "Send to flconc server " << addr->toString() << ", requeset body: " << rpc_req.ShortDebugString() << std::endl;
  stub.query_age(&rpc_controller, &rpc_req, &rpc_res, NULL);

  if (rpc_controller.ErrorCode() != 0) {
    std::cout << "Failed to call flconc server, error code: " << rpc_controller.ErrorCode() << ", error info: " << rpc_controller.ErrorText() << std::endl; 
    return;
  }

  std::cout << "Success get response frrom flconc server " << addr->toString() << ", response body: " << rpc_res.ShortDebugString() << std::endl;

} 

int main(int argc, char* argv[]) {

  test_client();

  return 0;
}
