#include "HttpModule.h"

namespace HttpModule
{
    
    void HttpModuleBase::Get(HttpParser::HttpMessage* request, HttpParser::HttpMessage* response){
        return;
    }

    void HttpModuleBase::Post(HttpParser::HttpMessage* request, HttpParser::HttpMessage* response){
        return;
    }
} // namespace HttpModule
