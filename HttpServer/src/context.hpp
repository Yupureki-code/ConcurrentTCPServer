#include "request.hpp"
#include <CTS/buffer.h>
#include "Util.hpp"
#include <string>
#include <vector>

#define MAX_LINE 8192

enum RECV_HTTP_STATUS{
    RECV_HTTP_LINE = 0,
    RECV_HTTP_HEADER = 1,
    RECV_HTTP_BODY = 2,
    RECV_HTTP_FINISH = 3,
    RECV_HTTP_ERROR = 4
};

class HttpContext
{
public:
    HttpContext():_recv_status(RECV_HTTP_LINE),_resp_status(200) {}
    void Clear()
    {
        _recv_status = RECV_HTTP_LINE;
        _resp_status = 200;
        _req.Clear();
    }
    bool PraseLine(Buffer& buffer)
    {
        std::string line = buffer.GetLine();
        if(line.empty())
        {
            if(buffer.Size() > MAX_LINE)
            {
                _resp_status = 414;
                _recv_status = RECV_HTTP_ERROR;
            }
            return false;
        }
        if(line.size() > MAX_LINE)
        {
            _resp_status = 414;
            _recv_status = RECV_HTTP_ERROR;
            return false;
        }
        std::regex e("(GET|HEAD|POST|PUT|DELETE) ([^?]*)(?:\\?(.*))? (HTTP/1\\.[01])", std::regex::icase);
        std::smatch matches;
        if(!std::regex_match(line, matches, e))
        {
            _resp_status = 400;
            _recv_status = RECV_HTTP_ERROR;
            return false;
        }
        _req._method = matches[1];
        std::transform(_req._method.begin(), _req._method.end(), _req._method.begin(), ::toupper);
        _req._path = Util::UrlDecode(matches[2],false);
        std::vector<std::string> params;
        std::string params_str = matches[3];
        Util::Split(params_str, "&", params);
        for(auto & it : params)
        {
            size_t pos = it.find('=');
            if(pos != std::string::npos)
            {
                std::string key = Util::UrlDecode(it.substr(0, pos), false);
                std::string value = Util::UrlDecode(it.substr(pos + 1), false);
                _req.SetParam(key, value);
            }
            else
            {
                _resp_status = 400;
                _recv_status = RECV_HTTP_ERROR;
                return false;
            }
        }
        _req._version = matches[4];
        _recv_status = RECV_HTTP_HEADER;
        return true;
    }
    bool PraseHeader(Buffer& buffer)
    {
        std::string line = buffer.GetLine("\r\n\r\n",true);
        if(line.empty())
        {
            if(buffer.Size() > MAX_LINE)
            {
                _resp_status = 414;
                _recv_status = RECV_HTTP_ERROR;
            }
            return false;
        }
        if(line.size() > MAX_LINE)
        {
            _resp_status = 414;
            _recv_status = RECV_HTTP_ERROR;
            return false;
        }
        if(line.size() < 4)
        {
            _resp_status = 400;
            _recv_status = RECV_HTTP_ERROR;
            return false;
        }
        line.erase(line.size() - 4);
        if(line.empty())
        {
            _recv_status = RECV_HTTP_BODY;
            return true;
        }
        std::vector<std::string> headers;
        Util::Split(line, "\r\n", headers);
        auto trim = [](std::string value) {
            while(!value.empty() && (value.front() == ' ' || value.front() == '\t'))
                value.erase(value.begin());
            while(!value.empty() && (value.back() == ' ' || value.back() == '\t'))
                value.pop_back();
            return value;
        };
        for(auto & it : headers)
        {
            size_t pos = it.find(':');
            if(pos != std::string::npos)            
            {
                std::string key = trim(it.substr(0, pos));
                std::string value = trim(it.substr(pos + 1));
                if(key.empty() || value.empty())
                {
                    _resp_status = 400;
                    _recv_status = RECV_HTTP_ERROR;
                    return false;
                }
                _req.SetHeader(key, value);
            }
            else 
            {
                _resp_status = 400;
                _recv_status = RECV_HTTP_ERROR;
                return false;
            }
        }
        _recv_status = RECV_HTTP_BODY;
        return true;

    }
    bool PraseBody(Buffer& buffer)
    {
        size_t content_length = 0;
        if(!_req.TryContentLength(content_length))
        {
            _resp_status = 400;
            _recv_status = RECV_HTTP_ERROR;
            return false;
        }
        if(buffer.Size() < content_length)
            return false;
        std::string body = buffer.Read(content_length);
        _req._body = body;
        _recv_status = RECV_HTTP_FINISH;
        return true;
    }
    bool PraseRequest(Buffer& buffer)
    {
        switch (_recv_status) 
        {
            case RECV_HTTP_LINE:
                if(!PraseLine(buffer))
                    return false;
            case RECV_HTTP_HEADER:
                if(!PraseHeader(buffer))
                    return false;
            case RECV_HTTP_BODY:
                if(!PraseBody(buffer))
                    return false;
                return true;
            default:
                return false;
        }
    }
    HttpRequest& GetRequest()
    {
        return _req;
    }
    int GetResponseStatus()const
    {
        return _resp_status;
    }
    bool IsBodyPending()const
    {
        return _recv_status == RECV_HTTP_BODY;
    }
private:
    int _resp_status;
    enum RECV_HTTP_STATUS _recv_status;
    HttpRequest _req;
};