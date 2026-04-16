#include <CTS/CTS.h>
#include "Util.hpp"
#include "context.hpp"
#include "response.hpp"
#include <CTS/connection.h>
#include <Logger/logstrategy.h>
#include <any>
#include <chrono>
#include <functional>
#include <future>
#include <cstdint>
#include <string>
#include <regex>
#include <thread>

#define DEFAULT_BASE_DIR "./wwwroot"
#define DEFAULT_BUSINESS_TIMEOUT 3
#define DEFAULT_REQUEST_TIMEOUT 2

using namespace ns_log;

class HttpServer
{
private:
    using handler = std::function<void(const HttpRequest&,HttpResponse&)>;
    using handlers = std::vector<std::pair<std::regex,handler>>;
    struct HttpSession
    {
        HttpContext context;
        uint64_t body_timeout_token = 0;
    };
    using Session = HttpSession;
public:
    void HandlerError(const HttpRequest&,HttpResponse& rep, int status = 404)
    {
        std::string body;
        Util::ReadFile(_base_dir + "/404.html", body);
        rep._status = status;
        rep.SetHeader("Content-Type", "text/html");
        rep._body = body;
    }
    void HandlerTimeout(HttpResponse& rep)
    {
        rep._status = 504;
        rep.SetHeader("Connection", "close");
        rep.SetHeader("Content-Type", "text/plain;charset=utf-8");
        rep._body = "Gateway Timeout";
    }
    bool RunHandlerWithTimeout(const handler& func, const HttpRequest& req, HttpResponse& rep)
    {
        std::packaged_task<HttpResponse()> task([func, req]() mutable {
            HttpResponse inner_rep;
            func(req, inner_rep);
            return inner_rep;
        });
        auto future = task.get_future();
        std::thread(std::move(task)).detach();
        if(future.wait_for(std::chrono::seconds(_business_timeout)) == std::future_status::ready)
        {
            rep = future.get();
            return true;
        }
        HandlerTimeout(rep);
        return false;
    }
    void SendAndClose(const PtrConnection& conn, const HttpRequest& req, HttpResponse& rep)
    {
        std::string res = MakeResponse(req, rep);
        conn->Send(res);
        conn->ShutDown();
    }
    std::string MakeResponse(const HttpRequest& req,HttpResponse& rep)
    {
        if(!rep.HasHeader("Connection"))
        {
            if(req.IsKeepAlive())
                rep.SetHeader("Connection", "keep-alive");
            else
                rep.SetHeader("Connection", "close");
        }
        if(!rep._body.empty() && !rep.HasHeader("Content-Length"))
            rep.SetHeader("Content-Length", std::to_string(rep._body.size()));
        if(!rep._body.empty() && !rep.HasHeader("Content-Type"))
            rep.SetHeader("Content-Type",  "application/octet-stream");
        if(rep._is_redirect)
            rep.SetHeader("Location", rep._redirect_url);
        return rep.Serialize();
    }
    bool IsStaticRequest(const HttpRequest& req)
    {
        if(_base_dir.empty())
            return false;
        if(req._method != "GET" && req._method != "HEAD")
            return false;
        if(req._path.empty())
            return false;
        std::string req_path = req._path;
        if(req_path == "/")
            req_path = "/index.html";
        std::string path = _base_dir + (req_path.front() == '/' ? req_path : "/" + req_path);
        return Util::IsRegularFile(path);
    }
    void HandleStaticRequest(const HttpRequest& req,HttpResponse& res)
    {
        std::string req_path = req._path;
        if(req_path == "/")
            req_path = "/index.html";
        std::string path = _base_dir + (req_path.front() == '/' ? req_path : "/" + req_path);
        std::string body;
        if(Util::ReadFile(path, body))
        {
            res._status = 200;
            res.SetHeader("Content-Type", Util::GetMime(path));
            res._body = body;
        }
        else
        {
            res._status = 404;
            Util::ReadFile(_base_dir + "/404.html", res._body);
            res.SetHeader("Content-Type", "text/html");
        }
    }
    bool Dispatcher(const HttpRequest& req,HttpResponse& rep,const handlers& handler)
    {
        for(auto & it : handler)
        {
            if(std::regex_match(req._path, it.first))
            {
                return RunHandlerWithTimeout(it.second, req, rep);
            }
        }
        return false;
    }
    void Route(const HttpRequest& req,HttpResponse& rep)
    {
        if(IsStaticRequest(req))
        {
            HandleStaticRequest(req, rep);
            return;
        }
        if(req._method == "GET")
        {
            if(!Dispatcher(req, rep, _get_routes))
            {
                if(rep._status != 504)
                    HandlerError(req, rep);
            }
        }
        else if(req._method == "POST")
        {
            if(!Dispatcher(req, rep, _post_routes))
            {
                if(rep._status != 504)
                    HandlerError(req, rep);
            }
        }
        else if(req._method == "PUT")
        {
            if(!Dispatcher(req, rep, _put_routes))
            {
                if(rep._status != 504)
                    HandlerError(req, rep);
            }
        }
        else if(req._method == "DELETE")
        {
            if(!Dispatcher(req, rep, _delete_routes))
            {
                if(rep._status != 504)
                    HandlerError(req, rep);
            }
        }
        else
            rep._status = 405;// Method Not Allowed
        return ;
    }
    void MessageHandler(const PtrConnection &conn, Buffer& buffer)
    {
        logger(ns_log::INFO)<<"收到"<<conn->GetPeerAddr().get_string()<<"的请求";
        Context context_any = conn->GetContext();
        Session session;
        Session* session_ptr = std::any_cast<Session>(&context_any);
        if(session_ptr != nullptr)
            session = *session_ptr;
        HttpContext& context = session.context;
        while(buffer.Size())
        {
            if(!context.PraseRequest(buffer))
            {
                if(context.GetResponseStatus() >= 400)
                {
                    HttpRequest req = context.GetRequest();
                    HttpResponse rep(context.GetResponseStatus());
                    HandlerError(req, rep, context.GetResponseStatus());
                    SendAndClose(conn, req, rep);
                    context.Clear();
                    session.body_timeout_token++;
                    Context saved = session;
                    conn->SetContext(saved);
                    return;
                }
                if(context.IsBodyPending())
                {
                    session.body_timeout_token++;
                    Context saved = session;
                    conn->SetContext(saved);
                    ArmBodyTimeout(conn, session.body_timeout_token);
                    return;
                }
                session.context = context;
                Context saved = session;
                conn->SetContext(saved);
                return;
            }
            HttpRequest req = context.GetRequest();
            HttpResponse rep(context.GetResponseStatus());
            if(context.GetResponseStatus() >= 400)
            {
                HandlerError(req, rep, context.GetResponseStatus());
                SendAndClose(conn, req, rep);
                context.Clear();
                session.body_timeout_token++;
                Context saved = session;
                conn->SetContext(saved);
                return;
            }
            Route(req, rep);
            std::string res = MakeResponse(req, rep);
            conn->Send(res);
            if(rep.Close())
            {
                conn->ShutDown();
                context.Clear();
                session.body_timeout_token++;
                Context saved = session;
                conn->SetContext(saved);
                return;
            }
            session.body_timeout_token++;
            context.Clear();
        }

        session.context = context;
        Context context_saved = session;
        conn->SetContext(context_saved);
    }
    void OnConnected(const PtrConnection &conn)
    {
        Session saved;
        Context context_saved = saved;
        conn->SetContext(context_saved);
    }
    void HandleBodyTimeout(const std::weak_ptr<Connection>& weak_conn, uint64_t token)
    {
        auto conn = weak_conn.lock();
        if(!conn)
            return;
        Context context_any = conn->GetContext();
        Session* session_ptr = std::any_cast<Session>(&context_any);
        if(session_ptr == nullptr)
            return;
        if(session_ptr->body_timeout_token != token)
            return;
        if(!session_ptr->context.IsBodyPending())
            return;
        HttpRequest req = session_ptr->context.GetRequest();
        HttpResponse rep(408);
        rep.SetHeader("Connection", "close");
        rep.SetHeader("Content-Type", "text/plain;charset=utf-8");
        rep._body = "Request Timeout";
        SendAndClose(conn, req, rep);
    }
    void ArmBodyTimeout(const PtrConnection& conn, uint64_t token)
    {
        std::weak_ptr<Connection> weak_conn = conn;
        _server.AddTimeTask([this, weak_conn, token]() {
            HandleBodyTimeout(weak_conn, token);
        }, _request_timeout);
    }
public:
    HttpServer(int port,size_t thread_nums = 0,int business_timeout = DEFAULT_BUSINESS_TIMEOUT,int request_timeout = DEFAULT_REQUEST_TIMEOUT)
    : _server(port,thread_nums)
    , _business_timeout(business_timeout)
    , _request_timeout(request_timeout)
    {
        _server.SetConnectedCallBack(std::bind(&HttpServer::OnConnected, this, std::placeholders::_1));
        _server.SetMessageCallBack(std::bind(&HttpServer::MessageHandler, this, std::placeholders::_1, std::placeholders::_2));
    }
    void EnableInactiveRelease(size_t timeout)
    {
        _server.EnableInactiveRelease(timeout);
    }
    void Get(const std::string& pattern,handler func)
    {
        _get_routes.emplace_back(std::regex(pattern), func);
    }
    void Post(const std::string& pattern,handler func)
    {
        _post_routes.emplace_back(std::regex(pattern), func);
    }
    void Put(const std::string& pattern,handler func)
    {
        _put_routes.emplace_back(std::regex(pattern), func);
    }
    void Delete(const std::string& pattern,handler func)
    {
        _delete_routes.emplace_back(std::regex(pattern), func);
    }
    void SetBaseDir(const std::string& base_dir)
    {
        _base_dir = base_dir;
    }
    void Listen()
    {
        _server.Run();
    }
private:
    handlers _post_routes;
    handlers _get_routes;
    handlers _put_routes;
    handlers _delete_routes;
    std::string _base_dir;
    int _business_timeout;
    int _request_timeout;
    CTS _server;
};
