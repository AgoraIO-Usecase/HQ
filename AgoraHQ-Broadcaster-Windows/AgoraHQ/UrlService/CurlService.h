#pragma once
#include "curl.h"
#include "URLData.h"
#include "IServiceCallback.h"

class CCurlService
{
protected:
	CCurlService();
public:
	~CCurlService();
	static CCurlService* GetInstance();
	static void ReleaseInstance();

	IUrlServiceCallback* GetUrlCallback(){ return m_pServiceCallback; }

	void clear_callback(){ m_pServiceCallback = nullptr; }
	bool init_curl_service(IUrlServiceCallback* url_callback);

	bool request_channel(GetChannelRequest rquest_data);
	bool reset(std::string gid);
	bool publish(std::string gid);
	bool stop_answer(std::string gid);
	bool invite_request(std::string gid, std::string uid);
	bool invite_status(std::string gid);
private:
	bool http_request(std::string url, std::string data, std::string method, std::string function_name, std::string& js_res, CURLcode& err_code);
	static size_t http_callback(void *str, size_t size, size_t count, void *out_str);

	//bool parse_course(Json::Value jsCourse);
private:
	int init_error_code = CURL_LAST; //curl_global_init·µ»Ø´íÎóÂë

	static CCurlService* curl_service;

	std::string access_token = "";
	std::string token_type = "";
	std::string auth_info_token = "";

	const std::string http_post = "post";
	const std::string http_get = "get";

	IUrlServiceCallback* m_pServiceCallback = nullptr;
};

#define AfxGetUrlService CCurlService::GetInstance
#define AfxReleaseUrlService CCurlService::ReleaseInstance



