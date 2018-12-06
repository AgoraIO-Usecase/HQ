#include "stdafx.h"
#include "CurlService.h"

#include <stdio.h>
#include <thread>
#include "Lock.h"
CCurlService* CCurlService::curl_service = NULL;

static CLock curl_lock;
CCurlService::CCurlService()
{
}


CCurlService::~CCurlService()
{
}

CCurlService* CCurlService::GetInstance()
{
	curl_lock.Lock();
	if (NULL == curl_service)
	{
		curl_service = new CCurlService;
	}
	curl_lock.UnLock();
	return curl_service;
}

void CCurlService::ReleaseInstance()
{
	curl_lock.Lock();
	if (NULL != curl_service)
	{
		delete curl_service;
		curl_service = NULL;
	}
	curl_lock.UnLock();
}

void CCurlService::setServerUrl(const std::string &url)
{
	resfapi_server_url = url;
}

bool CCurlService::init_curl_service(IUrlServiceCallback* url_callback)
{
	m_pServiceCallback = url_callback;
	init_error_code = curl_global_init(CURL_GLOBAL_ALL);
	if (init_error_code == CURLE_OK)
	{
		return true;
		
	}
	return false;
}

size_t CCurlService::http_callback(void *str, size_t size, size_t count, void *out_str)
{
	if (NULL != out_str)
	{
		std::string &s_res = *(std::string *)out_str;
		s_res.append((const char *)str, size * count);
	}
	return size*count;
}


bool CCurlService::http_request(std::string url, std::string data, std::string method, std::string function_name, std::string& js_res, CURLcode& err_code)
{
	gFileApp.write(url);
	if (init_error_code != CURLE_OK)
		return false;

	try
	{
		CURL* pSession = curl_easy_init();
		if (NULL == pSession)
		{
			throw false;
		}

		curl_easy_setopt(pSession, CURLOPT_CONNECTTIMEOUT, 5); //连接超时
		curl_easy_setopt(pSession, CURLOPT_TIMEOUT, 10);//获取数据超时
		
		//取消请求验证
		curl_easy_setopt(pSession, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(pSession, CURLOPT_SSL_VERIFYPEER, FALSE);

		// 设置文件读取的回调函数;
		//
		std::string json_res;
		json_res.clear();
		curl_easy_setopt(pSession, CURLOPT_WRITEFUNCTION, http_callback);
		curl_easy_setopt(pSession, CURLOPT_WRITEDATA, &json_res);
		
		curl_slist* header_list = NULL;
		header_list = curl_slist_append(header_list, "Content-Type:application/json;charset=UTF-8");
		/*header_list = curl_slist_append(header_list, auth_info.c_str());*/
		curl_easy_setopt(pSession, CURLOPT_HTTPHEADER, header_list);

		if (method == http_post)
		{
			curl_easy_setopt(pSession, CURLOPT_POST, 1);
			curl_easy_setopt(pSession, CURLOPT_POSTFIELDS, data.c_str());
		}
		else if (method == http_get)
		{
			//curl_easy_setopt(pSession, CURLOPT_POST, 1);
		}

		curl_easy_setopt(pSession, CURLOPT_URL, url.c_str());

		CURLcode code = curl_easy_perform(pSession);
		err_code = code;
		if (NULL != header_list)
		{
			curl_slist_free_all(header_list);
		}

		if (NULL != pSession)
		{
			curl_easy_cleanup(pSession);
		}

		if (code != CURLE_OK)
		{
			return false;
		}

		js_res = json_res;
		OutputDebugStringA(json_res.c_str());
		//解析json
// 		Json::CharReaderBuilder reader_builder;
// 		Json::CharReader* reader(reader_builder.newCharReader());
// 		JSONCPP_STRING errs;
// 		bool b = reader->parse(json_res.c_str(), json_res.c_str() + json_res.length(), &root, &errs);
// 
// 		if (!b)
// 		{
// 			delete reader;
// 			return false;
// 		}
// 
// 		delete reader;
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool CCurlService::request_channel(GetChannelRequest rquest_data)
{
	char js_request[2 * MAX_PATH] = { 0 };
	if (rquest_data.encrypt)
		sprintf_s(js_request, "{\"gid\":\"%s\", \"lang\":\"%s\", \"encrypt\":\"v1\"}", rquest_data.gid.c_str(), rquest_data.lang.c_str());
	else
		sprintf_s(js_request, "{\"gid\":\"%s\", \"lang\":\"%s\"}", rquest_data.gid.c_str(), rquest_data.lang.c_str());
	std::string str = js_request;
	std::thread request_channel_thread([=](std::string js_request)
	{
		OutputDebugStringA("\n");
		OutputDebugStringA("********************************************************\n");
		OutputDebugStringA("request_channel_thread");
		OutputDebugStringA(js_request.c_str());
	   std::string js_response = "";
	   CURLcode code = CURLE_OK;
	   if (!http_request(resfapi_server_url + request_channel_method, js_request, http_post, "request_channel", js_response, code))
	   {
		   std::string error = "";
		   if (CURLE_OPERATION_TIMEDOUT == code)
			   error = "curle_operation_timeout";
		   else
			   error = "http_error";
		   if (nullptr != m_pServiceCallback)
			   m_pServiceCallback->onRequestChannel_Callback("", error);
		    return;
	   }
	   OutputDebugStringA(js_response.c_str());
	   OutputDebugStringA("\n********************************************************\n");
	    if (nullptr != m_pServiceCallback)
	    {
	    	m_pServiceCallback->onRequestChannel_Callback(js_response);
	    }
	
	}, str);
	request_channel_thread.detach();
	return true;
}

bool CCurlService::reset(std::string gid)
{
	char js_request[2 * MAX_PATH] = { 0 };
	sprintf_s(js_request, "{\"gid\":\"%s\"}", gid.c_str());
	std::string str = js_request;
	std::thread reset_thread ([=](std::string js_request)
	{
		OutputDebugStringA("\n");
		OutputDebugStringA("********************************************************\n");
		OutputDebugStringA("reset_thread");
		OutputDebugStringA(js_request.c_str());
		std::string js_response = "";
		CURLcode code = CURLE_OK;
		if (!http_request(resfapi_server_url + reset_method, js_request, http_post, "reset", js_response, code))
		{
			std::string error = "";
			if (CURLE_OPERATION_TIMEDOUT == code)
				error = "curle_operation_timeout";
			else
				error = "http_error";
			if (nullptr != m_pServiceCallback)
			    m_pServiceCallback->onReset_Callback("", error);
			return;
		}
		OutputDebugStringA(js_response.c_str());
		OutputDebugStringA("\n********************************************************\n");
		if (nullptr != m_pServiceCallback)
		{
			m_pServiceCallback->onReset_Callback(js_response);
		}

	}, str);

	reset_thread.detach();
	return true;
}
bool CCurlService::publish(std::string gid)
{
	char js_request[2 * MAX_PATH] = { 0 };
	sprintf_s(js_request, "{\"gid\":\"%s\"}", gid.c_str());
	std::string str = js_request;
	std::thread publish_thread([=](std::string js_request)
	{
		OutputDebugStringA("\n");
		OutputDebugStringA("********************************************************\n");
		OutputDebugStringA("publish_thread");
		OutputDebugStringA(js_request.c_str());
		std::string js_response = "";
		CURLcode code = CURLE_OK;
		if (!http_request(resfapi_server_url + publish_method, js_request, http_post, "publish", js_response, code))
		{
			std::string error = "";
			if (CURLE_OPERATION_TIMEDOUT == code)
				error = "curle_operation_timeout";
			else
				error = "http_error";
			if (nullptr != m_pServiceCallback)
				m_pServiceCallback->onSendQuestion_Callback("", error);
			return;
		}
		OutputDebugStringA(js_response.c_str());
		OutputDebugStringA("\n********************************************************\n");
		if (m_pServiceCallback)
		{
			m_pServiceCallback->onSendQuestion_Callback(js_response);
		}
	}, str);

	publish_thread.detach();
	return true;
}
bool CCurlService::stop_answer(std::string gid)
{
	char js_request[2 * MAX_PATH] = { 0 };
	sprintf_s(js_request, "{\"gid\":\"%s\"}", gid.c_str());
	std::string str = js_request;
	OutputDebugStringA("\nstop_answer\n");
	OutputDebugStringA(js_request);
	OutputDebugStringA(":");
	OutputDebugStringA("\nstop_answer end\n");
	std::thread stop_answer_thread([=](std::string js_request)
	{
		std::string js_response = "";
		OutputDebugStringA("\n********************************************************\n");
		OutputDebugStringA("stop_answer_thread\n");

		if (js_request.empty())
			OutputDebugStringA("null json");
		else
			OutputDebugStringA(js_request.c_str());
		OutputDebugStringA("\n");
		CURLcode code = CURLE_OK;
		if (!http_request(resfapi_server_url + stop_answer_method, js_request, http_post, "stop_answer", js_response, code))
		{
			std::string error = "";
			if (CURLE_OPERATION_TIMEDOUT == code)
				error = "curle_operation_timeout";
			else
				error = "http_error";
			if (nullptr != m_pServiceCallback)
				m_pServiceCallback->onStopAnswer_Callback("", error);
			return;
		}
		OutputDebugStringA(js_response.c_str());
		OutputDebugStringA("\n********************************************************\n");
		if (nullptr != m_pServiceCallback)
		{
			m_pServiceCallback->onStopAnswer_Callback(js_response);
		}
	}, str);

	stop_answer_thread.detach();
	return true;
}

bool CCurlService::invite_request(std::string gid, std::string uid)
{
	char js_request[2 * MAX_PATH] = { 0 };
	sprintf_s(js_request, "{\"gid\":\"%s\", \"uid\":\"%s\"}", gid.c_str(), uid.c_str());
	std::string str = js_request;
	std::thread invite_request_thread = std::thread([=](std::string js_request)
	{
		std::string js_response = "";
		CURLcode code = CURLE_OK;
		if (!http_request(resfapi_server_url + invite_request_method, js_request, http_post, "invite_request", js_response, code))
		{
			std::string error = "";
			if (CURLE_OPERATION_TIMEDOUT == code)
				error = "curle_operation_timeout";
			else
				error = "http_error";
			if (nullptr != m_pServiceCallback)
				m_pServiceCallback->onInviteRequest_Callback("", error);
			return;
		}

		if (nullptr != m_pServiceCallback)
		{
			m_pServiceCallback->onInviteRequest_Callback(js_response);
		}
	}, str);


	invite_request_thread.detach();
	return true;
}

bool CCurlService::invite_status(std::string gid)
{
	char js_request[2 * MAX_PATH] = { 0 };
	sprintf_s(js_request, "{\"gid\":\"%s\"}", gid.c_str());
	std::string str = js_request;
	std::string js_response = "";
	
	std::thread invite_status_thread = std::thread([=](std::string js_request)
	{
		std::string js_response = "";
		CURLcode code = CURLE_OK;
		if (!http_request(resfapi_server_url + invite_status_method + "?gid=" + gid, js_request, http_get, "invite_request", js_response, code))
		{
			std::string error = "";
			if (CURLE_OPERATION_TIMEDOUT == code)
				error = "curle_operation_timeout";
			else
				error = "http_error";
			if (nullptr != m_pServiceCallback)
				m_pServiceCallback->onInviteStatus_Callback("", error);
			return ;
		}

		if (nullptr != m_pServiceCallback)
		{
			m_pServiceCallback->onInviteStatus_Callback(js_response);
		}
	
	}, str);

	invite_status_thread.detach();
	return true;
}