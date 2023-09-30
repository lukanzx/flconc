#include <google/protobuf/service.h>
#include <atomic>
#include <future>

#include "start.h"
#include "http_request.h"
#include "http_response.h"
#include "http_servlet.h"
#include "http_define.h"
#include "flconcpb_rpc_channel.h"
#include "flconcpb_rpc_async_channel.h"
#include "flconcpb_rpc_controller.h"
#include "flconcpb_rpc_closure.h"
#include "net_address.h"
#include "test_flconcpb_server.pb.h"


const char* html = "<html><body><h1>Welcome to TinyRPC, just enjoy it!</h1><p>%s</p></body></html>";

flconc::IPAddress::ptr addr = std::make_shared<flconc::IPAddress>("127.0.0.1", 20000);

class BlockCallHttpServlet : public flconc::HttpServlet {
 public:
  BlockCallHttpServlet() = default;
  ~BlockCallHttpServlet() = default;

  void handle(flconc::HttpRequest* req, flconc::HttpResponse* res) {
    AppDebugLog("BlockCallHttpServlet get request");
    AppDebugLog("BlockCallHttpServlet success recive http request, now to get http response");
    setHttpCode(res, flconc::HTTP_OK);
    setHttpContentType(res, "text/html;charset=utf-8");

    queryAgeReq rpc_req;
    queryAgeRes rpc_res;
    AppDebugLog ("now to call QueryServer TinyRPC server to query who's id is %s", req->m_query_maps["id"].c_str());
    rpc_req.set_id(std::atoi(req->m_query_maps["id"].c_str()));

    flconc::FlconcPbRpcChannel channel(addr);
    QueryService_Stub stub(&channel);

    flconc::FlconcPbRpcController rpc_controller;
    rpc_controller.SetTimeout(5000);

    AppDebugLog("BlockCallHttpServlet end to call RPC");
    stub.query_age(&rpc_controller, &rpc_req, &rpc_res, NULL);
    AppDebugLog("BlockCallHttpServlet end to call RPC");

    if (rpc_controller.ErrorCode() != 0) {
      char buf[512];
      sprintf(buf, html, "failed to call QueryServer rpc server");
      setHttpBody(res, std::string(buf));
      return;
    }

    if (rpc_res.ret_code() != 0) {
      std::stringstream ss;
      ss << "QueryServer rpc server return bad result, ret = " << rpc_res.ret_code() << ", and res_info = " << rpc_res.res_info();
      AppDebugLog(ss.str().c_str());
      char buf[512];
      sprintf(buf, html, ss.str().c_str());
      setHttpBody(res, std::string(buf));
      return;
    }

    std::stringstream ss;
    ss << "Success!! Your age is," << rpc_res.age() << " and Your id is " << rpc_res.id();

    char buf[512];
    sprintf(buf, html, ss.str().c_str());
    setHttpBody(res, std::string(buf));

  }

  std::string getServletName() {
    return "BlockCallHttpServlet";
  }

};

class NonBlockCallHttpServlet: public flconc::HttpServlet {
 public:
  NonBlockCallHttpServlet() = default;
  ~NonBlockCallHttpServlet() = default;

  void handle(flconc::HttpRequest* req, flconc::HttpResponse* res) {
    AppInfoLog("NonBlockCallHttpServlet get request");
    AppDebugLog("NonBlockCallHttpServlet success recive http request, now to get http response");
    setHttpCode(res, flconc::HTTP_OK);
    setHttpContentType(res, "text/html;charset=utf-8");

    std::shared_ptr<queryAgeReq> rpc_req = std::make_shared<queryAgeReq>();
    std::shared_ptr<queryAgeRes> rpc_res = std::make_shared<queryAgeRes>();
    AppDebugLog("now to call QueryServer TinyRPC server to query who's id is %s", req->m_query_maps["id"].c_str());
    rpc_req->set_id(std::atoi(req->m_query_maps["id"].c_str()));

    std::shared_ptr<flconc::FlconcPbRpcController> rpc_controller = std::make_shared<flconc::FlconcPbRpcController>();
    rpc_controller->SetTimeout(10000);

    AppDebugLog("NonBlockCallHttpServlet begin to call RPC async");


    flconc::FlconcPbRpcAsyncChannel::ptr async_channel = 
      std::make_shared<flconc::FlconcPbRpcAsyncChannel>(addr);

    auto cb = [rpc_res]() {
      printf("call succ, res = %s\n", rpc_res->ShortDebugString().c_str());
      AppDebugLog("NonBlockCallHttpServlet async call end, res=%s", rpc_res->ShortDebugString().c_str());
    };

    std::shared_ptr<flconc::FlconcPbRpcClosure> closure = std::make_shared<flconc::FlconcPbRpcClosure>(cb); 
    async_channel->saveCallee(rpc_controller, rpc_req, rpc_res, closure);

    QueryService_Stub stub(async_channel.get());

    stub.query_age(rpc_controller.get(), rpc_req.get(), rpc_res.get(), NULL);
    AppDebugLog("NonBlockCallHttpServlet async end, now you can to some another thing");

    async_channel->wait();
    AppDebugLog("wait() back, now to check is rpc call succ");

    if (rpc_controller->ErrorCode() != 0) {
      AppDebugLog("failed to call QueryServer rpc server");
      char buf[512];
      sprintf(buf, html, "failed to call QueryServer rpc server");
      setHttpBody(res, std::string(buf));
      return;
    }

    if (rpc_res->ret_code() != 0) {
      std::stringstream ss;
      ss << "QueryServer rpc server return bad result, ret = " << rpc_res->ret_code() << ", and res_info = " << rpc_res->res_info();
      char buf[512];
      sprintf(buf, html, ss.str().c_str());
      setHttpBody(res, std::string(buf));
      return;
    }

    std::stringstream ss;
    ss << "Success!! Your age is," << rpc_res->age() << " and Your id is " << rpc_res->id();

    char buf[512];
    sprintf(buf, html, ss.str().c_str());
    setHttpBody(res, std::string(buf));
  }

  std::string getServletName() {
    return "NonBlockCallHttpServlet";
  }

};

class QPSHttpServlet : public flconc::HttpServlet {
 public:
  QPSHttpServlet() = default;
  ~QPSHttpServlet() = default;

  void handle(flconc::HttpRequest* req, flconc::HttpResponse* res) {
    AppDebugLog("QPSHttpServlet get request");
    setHttpCode(res, flconc::HTTP_OK);
    setHttpContentType(res, "text/html;charset=utf-8");

    std::stringstream ss;
    ss << "QPSHttpServlet Echo Success!! Your id is," << req->m_query_maps["id"];
    char buf[512];
    sprintf(buf, html, ss.str().c_str());
    setHttpBody(res, std::string(buf));
    AppDebugLog(ss.str().c_str());
  }

  std::string getServletName() {
    return "QPSHttpServlet";
  }

};


int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Start TinyRPC server error, input argc is not 2!");
    printf("Start TinyRPC server like this: \n");
    printf("./server a.xml\n");
    return 0;
  }

  flconc::InitConfig(argv[1]);

  REGISTER_HTTP_SERVLET("/qps", QPSHttpServlet);

  REGISTER_HTTP_SERVLET("/block", BlockCallHttpServlet);
  REGISTER_HTTP_SERVLET("/nonblock", NonBlockCallHttpServlet);

  flconc::StartRpcServer();
  return 0;
}
