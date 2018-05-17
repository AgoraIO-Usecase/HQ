#include <obs-module.h>
#include <obs-service.h>
#include <map>
struct agora_data
{
	char* agora_appid;
    long long uid;
	char* publish_url, *key, *channel_name;
	int out_cx, out_cy;
	int fps;
	int video_bitrate;
	std::map<uint32_t, std::pair<int, int>> user_out_resolution;
};

const char * AgoraService_GetName(void *type_data)
{
	return obs_module_text("");
}

void AgoraService_Update(void *data, obs_data_t *settings)
{
	struct agora_data *service = static_cast<struct agora_data*>(data);
	
	if (service->agora_appid)
		bfree(service->agora_appid);

	if (service->channel_name)
		bfree(service->channel_name);

	if (service->publish_url)
		bfree(service->publish_url);

	if (service->key)
		bfree(service->key);
	
	service->uid          = obs_data_get_int(settings, "agora_uid");
	service->agora_appid  = bstrdup("aab8b8f5a8cd4469a63042fcfafe7063");//app_id
	service->publish_url  = bstrdup(obs_data_get_string(settings, "agora_url"));
	service->key          = bstrdup(obs_data_get_string(settings, "agora_key"));
	service->channel_name = bstrdup(obs_data_get_string(settings, "agora_channel"));
	
	service->out_cx = obs_data_get_int(settings, "agora_out_cx");
	service->out_cy = obs_data_get_int(settings, "agora_out_cy");
	service->fps    = obs_data_get_int(settings, "fps");
	service->video_bitrate = obs_data_get_int(settings, "agora_video_bitrate");
}

void *AgoraService_Create(obs_data_t *settings, obs_service_t *service)
{
	struct agora_data* data = static_cast<struct agora_data*>(bzalloc(sizeof(struct agora_data)));
	AgoraService_Update((void*)data, settings);
	return data;
}

void  AgoraService_Destroy(void *data)
{
	struct agora_data* service = static_cast<struct agora_data*>(data);
	bfree(service->agora_appid);
	bfree(service->channel_name);
	bfree(service->publish_url);
	bfree(service->key);
}


bool AgoraService_Initialize(void *data, obs_output_t *output)
{
	return true;
}

void AgoraService_Activate(void *data, obs_data_t *settings)
{
	struct agora_data* service_data = static_cast<struct agora_data*>(data);
}

void AgoraService_Deactivate(void *data)
{
	struct agora_data* service_data = static_cast<struct agora_data*>(data);
}

void AgoraService_GetDefaults(obs_data_t *settings)
{

}

obs_properties_t * AgoraService_GetProperties(void *data)
{
	return nullptr;
}

/**
* Called when getting ready to start up an output, before the encoders
* and output are initialized
*
* @param  data    Internal service data
* @param  output  Output context
* @eturn          true to allow the output to start up,
*                 false to prevent output from starting up
*/

const char *AgoraService_GetUrl(void *data)
{
	return nullptr;
}

const char *AgoraService_GetKey(void *data)
{
	return nullptr;
}

const char *AgoraService_GetUserName(void *data)
{
	return nullptr;
}

void RegisterAgoraService()
{
	obs_service_info info = {};
	info.id         = "agora_service";
	info.get_name   = AgoraService_GetName;
	info.create     = AgoraService_Create;
	info.destroy    = AgoraService_Destroy;
	info.update     = AgoraService_Update;
	info.activate   = AgoraService_Activate;
	info.deactivate = AgoraService_Deactivate;
	info.initialize = AgoraService_Initialize;

	obs_register_service(&info);
}