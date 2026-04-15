#include <string>
#include <unordered_map>
#include <regex>

class HttpRequest
{
public:
    HttpRequest():_version("HTTP/1.1") {}
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
    void SetParam(const std::string& key,const std::string& value)
    {
        _params[key] = value;
    }
    std::string GetParam(const std::string& key)const
    {
        auto it = _params.find(key);
        if(it == _params.end())
            return "";
        return (*it).second;
    }
    bool HasParam(const std::string& key)const
    {
        auto it = _params.find(key);
        return it != _params.end();
    }
    size_t ContentLength()const
    {
        if(!HasHeader("Content-Length"))
            return 0;
        return std::stoul(GetHeader("Content-Length"));
    }
    bool IsKeepAlive()const
    {
        if(!HasHeader("Connection"))
            return false;
        std::string connection = GetHeader("Connection");
        return connection == "keep-alive" || connection == "Keep-Alive";
    }
    void Clear()
    {
        _method.clear();
        _path.clear();
        _version = "HTTP/1.1";
        _headers.clear();
        _params.clear();
        _body.clear();
        std::smatch match;
        _matches.swap(match);
    }
    bool IsKeepAlive()
    {
        if(!HasHeader("Connection"))
            return false;
        std::string connection = GetHeader("Connection");
        return connection == "keep-alive" || connection == "Keep-Alive";
    }
public:
    std::string _method;
    std::string _path;
    std::string _version;
    std::smatch _matches;
    std::unordered_map<std::string,std::string> _headers;
    std::unordered_map<std::string, std::string> _params;
    std::string _body;
};