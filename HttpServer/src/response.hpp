#include <string>
#include <unordered_map>
#include "Util.hpp"

class HttpResponse
{
public:
    HttpResponse(int status = 200,bool is_redirect = false,const std::string& redirect_url = "")
    :_status(status),_is_redirect(is_redirect),_redirect_url(redirect_url) {}
    void SetHeader(const std::string& key,const std::string& value)
    {
        _headers[key] = value;
    }
    std::string GetHeader(const std::string& key)const
    {
        auto it = _headers.find(key);
        if(it == _headers.end())
            return "";
        return (*it).second;
    }
    bool HasHeader(const std::string& key)const
    {
        auto it = _headers.find(key);
        return it != _headers.end();
    }
    bool Close()const
    {
        auto it = _headers.find("Connection");
        if(it != _headers.end() && (*it).second == "close")
            return true;
        return false;
    }
    std::string Serialize()const
    {
        std::string res = "HTTP/1.1 " + std::to_string(_status) + " " + Util::StatusToDesc(_status) + "\r\n";
        for(auto & it : _headers)
        {
            res += it.first + ": " + it.second + "\r\n";
        }
        res += "\r\n";
        res += _body;
        return res;
    }
public:
    int _status;
    bool _is_redirect;
    std::string _redirect_url;
    std::unordered_map<std::string,std::string> _headers;
    std::string _body;
};