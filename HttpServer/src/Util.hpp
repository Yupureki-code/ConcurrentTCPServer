#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <sys/stat.h>

std::unordered_map<int, std::string> _status_msg = {
    {100,  "Continue"},
    {101,  "Switching Protocol"},
    {102,  "Processing"},
    {103,  "Early Hints"},
    {200,  "OK"},
    {201,  "Created"},
    {202,  "Accepted"},
    {203,  "Non-Authoritative Information"},
    {204,  "No Content"},
    {205,  "Reset Content"},
    {206,  "Partial Content"},
    {207,  "Multi-Status"},
    {208,  "Already Reported"},
    {226,  "IM Used"},
    {300,  "Multiple Choice"},
    {301,  "Moved Permanently"},
    {302,  "Found"},
    {303,  "See Other"},
    {304,  "Not Modified"},
    {305,  "Use Proxy"},
    {306,  "unused"},
    {307,  "Temporary Redirect"},
    {308,  "Permanent Redirect"},
    {400,  "Bad Request"},
    {401,  "Unauthorized"},
    {402,  "Payment Required"},
    {403,  "Forbidden"},
    {404,  "Not Found"},
    {405,  "Method Not Allowed"},
    {406,  "Not Acceptable"},
    {407,  "Proxy Authentication Required"},
    {408,  "Request Timeout"},
    {409,  "Conflict"},
    {410,  "Gone"},
    {411,  "Length Required"},
    {412,  "Precondition Failed"},
    {413,  "Payload Too Large"},
    {414,  "URI Too Long"},
    {415,  "Unsupported Media Type"},
    {416,  "Range Not Satisfiable"},
    {417,  "Expectation Failed"},
    {418,  "I'm a teapot"},
    {421,  "Misdirected Request"},
    {422,  "Unprocessable Entity"},
    {423,  "Locked"},
    {424,  "Failed Dependency"},
    {425,  "Too Early"},
    {426,  "Upgrade Required"},
    {428,  "Precondition Required"},
    {429,  "Too Many Requests"},
    {431,  "Request Header Fields Too Large"},
    {451,  "Unavailable For Legal Reasons"},
    {501,  "Not Implemented"},
    {502,  "Bad Gateway"},
    {503,  "Service Unavailable"},
    {504,  "Gateway Timeout"},
    {505,  "HTTP Version Not Supported"},
    {506,  "Variant Also Negotiates"},
    {507,  "Insufficient Storage"},
    {508,  "Loop Detected"},
    {510,  "Not Extended"},
    {511,  "Network Authentication Required"}
};

std::unordered_map<std::string, std::string> _mime_msg = {
    {".aac",        "audio/aac"},
    {".abw",        "application/x-abiword"},
    {".arc",        "application/x-freearc"},
    {".avi",        "video/x-msvideo"},
    {".azw",        "application/vnd.amazon.ebook"},
    {".bin",        "application/octet-stream"},
    {".bmp",        "image/bmp"},
    {".bz",         "application/x-bzip"},
    {".bz2",        "application/x-bzip2"},
    {".csh",        "application/x-csh"},
    {".css",        "text/css"},
    {".csv",        "text/csv"},
    {".doc",        "application/msword"},
    {".docx",       "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    {".eot",        "application/vnd.ms-fontobject"},
    {".epub",       "application/epub+zip"},
    {".gif",        "image/gif"},
    {".htm",        "text/html"},
    {".html",       "text/html"},
    {".ico",        "image/vnd.microsoft.icon"},
    {".ics",        "text/calendar"},
    {".jar",        "application/java-archive"},
    {".jpeg",       "image/jpeg"},
    {".jpg",        "image/jpeg"},
    {".js",         "text/javascript"},
    {".json",       "application/json"},
    {".jsonld",     "application/ld+json"},
    {".mid",        "audio/midi"},
    {".midi",       "audio/x-midi"},
    {".mjs",        "text/javascript"},
    {".mp3",        "audio/mpeg"},
    {".mpeg",       "video/mpeg"},
    {".mpkg",       "application/vnd.apple.installer+xml"},
    {".odp",        "application/vnd.oasis.opendocument.presentation"},
    {".ods",        "application/vnd.oasis.opendocument.spreadsheet"},
    {".odt",        "application/vnd.oasis.opendocument.text"},
    {".oga",        "audio/ogg"},
    {".ogv",        "video/ogg"},
    {".ogx",        "application/ogg"},
    {".otf",        "font/otf"},
    {".png",        "image/png"},
    {".pdf",        "application/pdf"},
    {".ppt",        "application/vnd.ms-powerpoint"},
    {".pptx",       "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
    {".rar",        "application/x-rar-compressed"},
    {".rtf",        "application/rtf"},
    {".sh",         "application/x-sh"},
    {".svg",        "image/svg+xml"},
    {".swf",        "application/x-shockwave-flash"},
    {".tar",        "application/x-tar"},
    {".tif",        "image/tiff"},
    {".tiff",       "image/tiff"},
    {".ttf",        "font/ttf"},
    {".txt",        "text/plain"},
    {".vsd",        "application/vnd.visio"},
    {".wav",        "audio/wav"},
    {".weba",       "audio/webm"},
    {".webm",       "video/webm"},
    {".webp",       "image/webp"},
    {".woff",       "font/woff"},
    {".woff2",      "font/woff2"},
    {".xhtml",      "application/xhtml+xml"},
    {".xls",        "application/vnd.ms-excel"},
    {".xlsx",       "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    {".xml",        "application/xml"},
    {".xul",        "application/vnd.mozilla.xul+xml"},
    {".zip",        "application/zip"},
    {".3gp",        "video/3gpp"},
    {".3g2",        "video/3gpp2"},
    {".7z",         "application/x-7z-compressed"}
};

class Util
{
public:
    static bool IsFileExist(const std::string& path)
    {
        std::ifstream file(path);
        return file.good();
    }
    static bool ReadFile(const std::string& path,std::string& out)
    {
        std::ifstream file(path);
        if(!file.is_open())
            return false;
        std::stringstream ss;
        ss<<file.rdbuf();
        out = ss.str();
        return true;
    }
    static bool WriteFile(const std::string& path,const std::string& info)
    {
        std::ofstream file(path);
        if(!file.is_open())
            return false;
        file<<info;
        return true;
    }
    static std::string UrlEncode(std::string& url)
    {
        std::string result;
        for(size_t i = 0;i<url.size();i++)
        {
            char c = url[i];
            if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '-' || c == '_' || c == '.' || c == '~')
                result += c;
            else
            {
                char buf[4];
                snprintf(buf,sizeof(buf),"%02X",(unsigned char)c);
                result += '%';
                result += buf;
            }
        }
        return result;
    }
    static std::string UrlDecode(std::string& url)
    {
        std::string result;
        for(size_t i = 0;i<url.size();i++)
        {
            char c = url[i];
            if(c == '%')
            {
                if(i + 2 >= url.size())
                    return "";
                char buf[3] = {url[i+1],url[i+2],'\0'};
                result += (char)strtol(buf,nullptr,16);
                i += 2;
            }
            else if(c == '+')
                result += ' ';
            else
                result += c;
        }
        return result;
    }
    static int Split(const std::string& str,const std::string& sep,std::vector<std::string>& out)
    {
        size_t start = 0;
        while(start < str.size())
        {
            size_t pos = str.find(sep,start);
            if(pos == std::string::npos)
                pos = str.size();
            out.push_back(str.substr(start,pos - start));
            start = pos + sep.size();
        }
        return out.size();
    }
    static std::string StatusToDesc(int status)
    {
        auto it = _status_msg.find(status);
        if(it == _status_msg.end())
            return "Unknown";
        return (*it).second;
    }
    static std::string GetMime(const std::string& str)
    {
        size_t pos = str.rfind('.');
        if(pos == std::string::npos)
            return "application/octet-stream";
        std::string ext = str.substr(pos);
        auto it = _mime_msg.find(ext);
        if(it == _mime_msg.end())
            return "application/octet-stream";
        return (*it).second;
    }
    static bool IsDirectory(const std::string& path)
    {
        struct stat st;
        if(stat(path.c_str(),&st) < 0)
            return false;
        return S_ISDIR(st.st_mode);
    }
    static bool IsRegularFile(const std::string& path)
    {
        struct stat st;
        if(stat(path.c_str(),&st) < 0)
            return false;
        return S_ISREG(st.st_mode);
    }
    static bool IsVaildPath(const std::string& path)
    {
        std::vector<std::string> _subdir;
        Split(path,"/",_subdir);
        int level = 0;
        for(size_t i = 0;i<_subdir.size();i++)
        {
            if(_subdir[i] == "" || _subdir[i] == ".")
                continue;
            if(_subdir[i] == "..")
            {
                level--;
                if(level < 0)
                    return false;
            }
            else
                level++;
        }
        return true;
    }
};