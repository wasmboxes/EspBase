#include "HttpParser.h"

namespace HttpParser
{
	std::string HttpMessage::Parse(){
		std::string log = "";

		if(method == HttpMethod::RESPONSE){
			log += http_version + " ";
			if(response_code == HttpResponseCode::OK) log+="200 OK";
			else if(response_code == HttpResponseCode::NOTFOUND) log+="404 Not Found";
			else log += "NONE";
		}

		else{
			if(method == HttpMethod::GET) log+="GET";
			else if(method == HttpMethod::POST) log+="POST";
			else log += "NONE";
			log += " " + url + " " + http_version;
		}
		
		if(body.length() > 0) headers["Content-Length"] = std::to_string(body.length());

		for (const auto& it : headers) log += "\n" + it.first + ": " + it.second;
		if(headers.size() > 0) log += "\n";
		
		log += "\n";

		if(body.length() > 0) log += body + '\n'; 
				
		log += "\n";
		return log;
	};

	HttpMessage* HttpMessage::ParseHeaders(std::string& data, int& cur_index){
		// Process Headers
		std::string header = "";
		std::string value  = "";
		bool fillHeader = true;
		for(; cur_index < data.size(); ++cur_index) {
			if(data[cur_index] == '\r') cur_index++;
			
			if(data[cur_index] == '\n'){
				headers[header] = value; // end of header

				if(data[cur_index+1] == '\r') cur_index++;
				if(data[cur_index+1] == '\n'){
					break; // end of headers ... start of body
				}
				fillHeader = true;
				header = ""; // empty for new header
				value  = "";
				cur_index += 1; // skip current newline
			}

			if(data[cur_index] == ':' && data[cur_index+1] == ' '){
				fillHeader = false;
				cur_index += 1;
				continue;
			}
			if(fillHeader) header += data[cur_index];
			else value += data[cur_index];
		}	

		if(data[cur_index+1] == '\r') cur_index++;
		cur_index += 1; // skip newline before body
		// cur_index points to start of next section

		// process headers, ! TODO tolower
		std::string contentTypeTmp = "";
		if(headers.count("content-type")) contentTypeTmp = headers["content-type"];
		else if(headers.count("Content-Type")) contentTypeTmp = headers["Content-Type"];
		
		if(headers.count("content-length")) content_length = std::stoi(headers["content-length"]);
		else if(headers.count("Content-Length")) content_length = std::stoi(headers["Content-Length"]);
		
		return this;
	}

	HttpMessage* HttpMessage::ParseFirstLine(std::string& data, int& cur_index){
		error = HttpMessageError::None;
		int newline = 0;
		int spaces = 0;
		int i = 0;

		// Length Check
		// 123456789012345
		// GET / HTTP/1.1/n
		if(data.length() < 15){
			this->error = HttpMessageError::MissingData;
			return this;
		}

		while(data[i] != '\n' && i < data.size()){
			if(data[i] == '\n') newline += 1;	// has header
			if(data[i] == ' ')  spaces += 1; 	// count spaces
			i++;
		}
		if(newline != 1 && spaces > 2){
			error = HttpMessageError::MissingData;
			return this;
		}

		// Process Method
		std::string method_substr = data.substr(0, 3);
		if(method_substr == "GET"){
			method = HttpMethod::GET;
			cur_index += 3;
		}
		else if(method_substr == "POS"){
			method = HttpMethod::POST;
			cur_index += 4;
		}
		else method = HttpMethod::RESPONSE;
		
		if(cur_index >= data.size()){
			error = HttpMessageError::MissingData;
			return this;
		}

		// Parse url
		if(method != HttpMethod::RESPONSE) cur_index += 1; // space
		for(; cur_index < data.size() && data[cur_index] != ' '; ++cur_index) url += data[cur_index];
		
		// Check if url is actually http version
		cur_index += 1;
		if(data[cur_index] == '\n'){ // if the next char after reading url is newline then we read the http_version
			http_version = url;
			url = "";
		}
		else{ // Process http_version
			for(; cur_index < data.size() && data[cur_index] != '\n'; ++cur_index) http_version += data[cur_index];	
			//if(http_version != "") // TODO
		}

		// Split uri / /test /test/test2
		std::string endpoint_piece = "";
		for(int i = 0; i < url.length(); i++){
			if(url[i] == '/'){
				if(endpoint_piece != "") {
					endpoint.push_back(endpoint_piece);
					endpoint_piece = "";
				}
				else continue;
			}
			else endpoint_piece += url[i];
		}
		if(endpoint_piece.length() != 0) endpoint.push_back(endpoint_piece);

		// for(int i = 0 ; i< endpoint.size(); i++) printf(" Endpoint %d : %s \n", i, endpoint[i].c_str());

		cur_index += 1; // newline of first line
		// cur_index points to start of next section
		return this;
	}

	// ! TODO :
	// parse multipart
	HttpMessage* HttpMessage::ParseBody(std::string& data, int& cur_index){
		if(data.length() - cur_index > 0) body = data.substr(cur_index, data.length() - cur_index); 
		return this;
	}

	HttpMessage* HttpMessage::Parse(std::string data){
		HttpMessage* message = this;
		this->error = HttpMessageError::None;
		int cur_index = 0;

		ParseFirstLine(data, cur_index);

		if(data[cur_index] == '\n'){ // body and no content length ... empty body and empty headers
			// printf("\n Http no content\n");
			return message;
		}

		ParseHeaders(data, cur_index);
		cur_index++; // skip first newline

		ParseBody(data, cur_index);
		// verify(); // ! TODO
		return message;
	}

	HttpMessage* HttpMessage::GenerateResponse(HttpContentType content_type, HttpResponseCode response_code, std::string data){
		this->method = HttpMethod::RESPONSE;
		this->content_type = content_type;

		this->url = url;
		this->response_code = response_code;
		
		if(content_type == HttpContentType::ApplicationJson) this->headers["content-type"] = "application/json";
		else if(content_type == HttpContentType::TextHtml) this->headers["content-type"] = "text/html";
		else this->headers["Content-Type"] = "text/plain";
		
		this->body = data; // + '\n';
		this->headers["Content-Length"] = std::to_string(data.length());
		return this;
	}
} // namespace HttpParser