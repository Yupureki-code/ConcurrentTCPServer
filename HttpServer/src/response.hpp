#include <string>
#include <unordered_map>

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
    void SetBody(const std::string& body)
    {
        _body = body;
    }
    std::string GetBody()const
    {        
        return _body;
    }
    bool Close()const
    {
        auto it = _headers.find("Connection");
        if(it != _headers.end() && (*it).second == "close")
            return true;
        return false;
    }
private:
    int _status;
    bool _is_redirect;
    std::string _redirect_url;
    std::unordered_map<std::string,std::string> _headers;
    std::string _body;
};