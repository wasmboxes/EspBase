#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include <string>
#include <map>
#include <vector>

namespace HttpParser
{
    
	enum class HttpResponseCode{
		OK,
		NOTFOUND,
		INTERNALSERVERERROR,
		NONE,
	};

	enum class HttpTransferEncoding{
		CHUNKED,
		GZIP,
		NONE,
	};

	enum class HttpMethod{
		GET = 1,
		POST = 2,
		RESPONSE = 3,
		NONE = -1,
	};

	enum class HttpContentType{
		TextHtml,			// text/html
		TextPlain,			// text/plain
		
		ApplicationJson,	// application/json
		ApplicationForm, 	// application/x-www-form-urlencoded
		
		MultipartFormData, 	// multipart/form-data
		MultipartMixed, 	// multipart/mixed
		
		None,
	};

	enum class HttpMessageError{
		WrongSize,
		MissingData,
		FirstLineError,
		HeaderError,
		BodyError,
		None
	};

	class HttpMessage{
		private:

			

			
		public:
			// First line
			std::string url = "";
			std::vector<std::string> endpoint;
			std::map<std::string,std::string> query;
			std::string http_version = "HTTP/1.0";
			// Headers
			std::map<std::string,std::string> headers;
			std::string boundary = "";
			// Body
			std::string body = ""; 
			std::map<std::string, std::string> body_files;

			HttpMethod method = HttpMethod::NONE;
			HttpContentType content_type = HttpContentType::None;
			HttpTransferEncoding transfer_encoding = HttpTransferEncoding::NONE;
			HttpResponseCode response_code = HttpResponseCode::NONE;
			int content_length = 0;

			HttpMessageError error = HttpMessageError::None;
			
			// Section Parsers
			HttpMessage* ParseFirstLine(std::string& data, int& cur_index);
			HttpMessage* ParseHeaders(std::string& data, int& cur_index);
			HttpMessage* ParseBody(std::string& data, int& cur_index);

			HttpMessage* GenerateResponse(HttpContentType content_type, HttpResponseCode response_code, std::string data);
			HttpMessage* GenerateRequest(HttpMethod method, HttpContentType content_type, std::string url, std::string data);
			HttpMessage* Parse(std::string data);
			std::string Parse();
	};
} // namespace HttpParser
