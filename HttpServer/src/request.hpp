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
    bool TryContentLength(size_t& length) const
    {
        length = 0;
        if(!HasHeader("Content-Length"))
            return true;
        const std::string value = GetHeader("Content-Length");
        try
        {
            size_t pos = 0;
            unsigned long long parsed = std::stoull(value, &pos);
            if(pos != value.size())
                return false;
            length = static_cast<size_t>(parsed);
            return true;
        }
        catch(...)
        {
            return false;
        }
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
        size_t length = 0;
        TryContentLength(length);
        return length;
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