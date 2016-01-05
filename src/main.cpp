#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

#include <cstring>

//JsonCpp
#include "json/json.h"

//libcurl
#include "curl/curl.h"

struct DownloadedData {
	char* data;
	size_t size;
};


class HTTPHandler {
	protected:
		CURL* m_CurlHandle;
		CURLcode m_Res;

		//Since libcurl outputs the GET/POST to stdout, we write a callback that writes it to a string, which is contained in a "DownloadedData" struct.
		static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
			size_t realSize = size * nmemb;
			DownloadedData* downloaded = (DownloadedData*)userp;
		
			downloaded->data = (char*)realloc(downloaded->data, downloaded->size + realSize +1);
			if(downloaded->data == nullptr) {
				std::cerr << "Out of memory!\n";
				return 0;
			}
		
			memcpy(&(downloaded->data[downloaded->size]), contents, realSize);
			downloaded->size += realSize;
			downloaded->data[downloaded->size] = 0;
		
			return realSize;
		}
	public:
		HTTPHandler() {
		}
		~HTTPHandler() {
		}

		bool get(const std::string& where, std::string& result) {
			m_CurlHandle = curl_easy_init();

			std::cout << "Downloading file " << where<< "\n";

			char errorBuffer[CURL_ERROR_SIZE];
			DownloadedData downloaded;

			downloaded.data = (char*)malloc(1); 
			downloaded.size = 0;

			curl_easy_setopt(m_CurlHandle, CURLOPT_URL, where.c_str());
			curl_easy_setopt(m_CurlHandle, CURLOPT_ERRORBUFFER, errorBuffer);
			curl_easy_setopt(m_CurlHandle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
			curl_easy_setopt(m_CurlHandle, CURLOPT_WRITEFUNCTION, HTTPHandler::WriteCallback);
			curl_easy_setopt(m_CurlHandle, CURLOPT_WRITEDATA, (void*)&downloaded);
			curl_easy_setopt(m_CurlHandle, CURLOPT_FAILONERROR, true);

			errorBuffer[0] = 0;

			m_Res = curl_easy_perform(m_CurlHandle);

			if(m_Res != CURLE_OK) {
				std::size_t errLen = strlen(errorBuffer);
				fprintf(stderr, "libcurl: (%d) ", m_Res);

				if(errLen) {
					fprintf(stderr, "%s%s", errorBuffer, ((errorBuffer[errLen - 1] != '\n') ? "\n" : ""));
				} else {
					fprintf(stderr, "%s\n", curl_easy_strerror(m_Res));
				}

				return false;
			}

			result = downloaded.data;

			free(downloaded.data);

			curl_easy_cleanup(m_CurlHandle);

			return true;
		}
		bool post(const std::string& where, const std::string& data, std::string& response) {
			m_CurlHandle = curl_easy_init();

			std::cout << "Sending POST to " << where << "\n";

			char errorBuffer[CURL_ERROR_SIZE];
			DownloadedData downloaded;

			downloaded.data = (char*)malloc(1); 
			downloaded.size = 0;

			curl_easy_setopt(m_CurlHandle, CURLOPT_URL, where.c_str());
			curl_easy_setopt(m_CurlHandle, CURLOPT_ERRORBUFFER, errorBuffer);
			curl_easy_setopt(m_CurlHandle, CURLOPT_POSTFIELDS, data.c_str());
			curl_easy_setopt(m_CurlHandle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
			curl_easy_setopt(m_CurlHandle, CURLOPT_WRITEFUNCTION, HTTPHandler::WriteCallback);
			curl_easy_setopt(m_CurlHandle, CURLOPT_WRITEDATA, (void*)&downloaded);
			curl_easy_setopt(m_CurlHandle, CURLOPT_FAILONERROR, true);

			errorBuffer[0] = 0;

			m_Res = curl_easy_perform(m_CurlHandle);

			if(m_Res != CURLE_OK) {
				std::size_t errLen = strlen(errorBuffer);
				fprintf(stderr, "libcurl: (%d) ", m_Res);

				if(errLen) {
					fprintf(stderr, "%s%s", errorBuffer, ((errorBuffer[errLen - 1] != '\n') ? "\n" : ""));
				} else {
					fprintf(stderr, "%s\n", curl_easy_strerror(m_Res));
				}

				return false;
			}

			response = downloaded.data;

			free(downloaded.data);

			curl_easy_cleanup(m_CurlHandle);

			return true;
		}
};

class DataPresenter {
	protected:
	public:
		DataPresenter() {
		}
		~DataPresenter() {
		}

		std::string getNicelyFormattedData(const std::string& toParse) {
			std::string result = toParse;

			//This would be very bad in a production setting
			//The byte order of 0xEF, 0xBB, and 0xBF is a UTF-8 BOM
			//It is ignored here for convenience's sake.
			//
			//https://en.wikipedia.org/wiki/Byte_order_mark
			//
			//without removing these bytes, JsonCpp can't parse the file without additional effort
			//this would be handled correctly if it was meant to be used, especially on non-x86 machines to handle endianess correctly
			//
			//We also just assume that the next two bytes after 0xEF are also part of the BOM,
			//and that there's just one BOM in the text
			//
			//Additionally, we only check for a UTF-8 BOM and not a UTF-16 or UTF-32 BOM
			std::size_t found = result.find(0xEF);
			if(found != std::string::npos) {
				result = result.substr(3); //The new string will not contain the BOM
			}

			Json::Value root;
			Json::Reader reader;
			bool success = reader.parse(result, root, false);
			
			if(!success || !reader.good()) {
				std::cerr << "Failed to parse Json. Error:\n" << reader.getFormattedErrorMessages() << "\n";
				return "";
			}

			return result;
		}
};

int main(void) {
	DataPresenter presenter;
	HTTPHandler http;

	std::string response;

	//A POST request containing a Json "question" to filter out the years we want in the table ME0104B8
	http.post("http://api.scb.se/OV0104/v1/doris/sv/ssd/START/ME/ME0104/ME0104D/ME0104T4",
	"{\"query\": [{ \"code\": \"ContentsCode\", \"selection\": { \"filter\": \"item\", \"values\": [ \"ME0104B8\" ]}},{ \"code\": \"Tid\",\"selection\": { \"filter\": \"item\",\"values\": [ \"1973\",\"1976\",\"1979\",\"1982\",\"1985\",\"1988\",\"1991\",\"1994\",\"1998\",\"2002\",\"2006\",\"2010\",\"2014\"]}}],\"response\": {\"format\": \"json\"}}",
	response);

	std::cout << presenter.getNicelyFormattedData(response) << "\n";

	return 0;
}
