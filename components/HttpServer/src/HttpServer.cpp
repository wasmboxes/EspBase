#include "HttpServer.h"


namespace HttpServer
{
    NetworkController::TcpTranscieverResponse SimpleHttpServerBase::operator()(NetworkController::tcp_transceiver_task_context_t* context){
        ESP_LOGI("HttpSocketHandler", "Running Handler with Tcp Context");
        char* message = "HTTP/1.0 200 OK\n" 
            "Date: Fri, 31 Dec 1999 23:59:59 GMT\n" 
            "Content-Type: text/html\n" 
            "Content-Length: 43\n" 
            "\n"
            "<html><body><h1>TIN TEST</h1></body></html>\0";

        context->outBytes = (char*)malloc((strlen(message)+1) * sizeof(char));
        strncpy(context->outBytes, message, strlen(message)+1);
        context->outLen = (int*)malloc(sizeof(int));
        *(context->outLen) = strlen(message)+1;
        ESP_LOGI("HttpSocketHandler", "End of Handler with Tcp Context");
        return NetworkController::TcpTranscieverResponse::WriteAndClose;
    };

    HttpParser::HttpMessage* HttpServerBase::serve(HttpParser::HttpMessage* request){
        HttpParser::HttpMessage* response = new HttpParser::HttpMessage();
        response->method = HttpParser::HttpMethod::RESPONSE;
        response->body = "<html><body><h1>TIN TEST</h1></body></html>";
        ESP_LOGI("HttpServerBase", "Request on path : %s", request->url.c_str());
        response->response_code = HttpParser::HttpResponseCode::OK;
        return response;
    }

    NetworkController::TcpTranscieverResponse HttpServerBase::operator()(NetworkController::tcp_raw_transceiver_task_context_t* context){
        ESP_LOGW("HttpServerBase", "Running Handler with Raw Tcp Context 1 with sock : %d", *context->socket_fd);
        std::string message = "HTTP/1.0 200 OK\n" 
            "Date: Fri, 31 Dec 1999 23:59:59 GMT\n" 
            "Content-Type: text/html\n" 
            "Content-Length: 43\n" 
            "\n"
            "<html><body><h1>TIN TEST</h1></body></html>\0";

        ESP_LOGI("HttpServerBase", "Running Handler with Raw Tcp Context 2 with sock : %d", *context->socket_fd);
        int len;
        int cur_index = 0;
        char rx_buffer[1024];
        std::string raw_message;

        while(true){
            len = recv(*context->socket_fd, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len < 0) {
                ESP_LOGI("HttpServerBase", "Connection closed: errno %d with sock : %d", errno, *context->socket_fd);
                break;
            } else if (len == 0) {
                ESP_LOGI("HttpServerBase", "Connection closed with sock : %d", *context->socket_fd);
                break;
            }
            else{
                HttpParser::HttpMessage* message = new HttpParser::HttpMessage();
                raw_message = std::string(rx_buffer, len);
                // message->ParseFirstLine(raw_message, cur_index);
                message->Parse(raw_message);

                if(message->error == HttpParser::HttpMessageError::MissingData){
                    ESP_LOGW("HttpServerBase", "Missing Data with sock : %d", *context->socket_fd);
                    continue;
                }
                else if(message->error == HttpParser::HttpMessageError::None){
                    ESP_LOGW("HttpServerBase", "Got Message With Header type : %d with sock fd : %d", (int)message->method, *context->socket_fd);

                    HttpParser::HttpMessage* response = this->serve(message);
                    std::string outBytes = response->Parse();
                    send(*context->socket_fd, outBytes.c_str(), (outBytes.length()) * sizeof(char), 0);
                    free(response);
                }
                else break;
            }
        }
        
        ESP_LOGW("HttpServerBase", "Closing Server  with sock : %d", *context->socket_fd);
        return NetworkController::TcpTranscieverResponse::Close;
    };

} 
