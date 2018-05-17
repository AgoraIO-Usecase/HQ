#include <obs-module.h>
#include <obs-encoder.h>
#include <stdio.h>
FILE* fPcm;
obs_encoder_info agora_pcm_encoder = {};

static const char *AgoraGetAudioEncoderName(void*)
{
	return obs_module_text("AgoraAudioEnc");
}

static void* AgoraPCM_Create(obs_data_t* settings, obs_encoder_t* encoder )
{
	fPcm = fopen("D:\\AgoraPcm.pcm", "ab+");
	return encoder;
}

static void AgoraPCM_Destroy(void* data)
{
	fclose(fPcm);
}

static bool AgoraPCM_Encode(void* data, struct encoder_frame* frame,
    struct encoder_packet* packet, bool *receive_packet)
{
	obs_encoder_t* enc = static_cast<obs_encoder_t*>(data);
	audio_convert_info info;
	agora_pcm_encoder.get_audio_info(NULL, &info);
	int plane = 4 * get_audio_channels(info.speakers);
	int dataLen = frame->frames * plane;//

	fwrite(frame->data[0], 1, 4 * 1024, fPcm);

	if (agora_pcm_encoder.agora_audio_encoder_callback){
		agora_pcm_encoder.agora_audio_encoder_callback(frame, plane, agora_pcm_encoder.param);
	}
// 	if (agora_pcm_encoder.agora_encoder_audio.agora_audio_data_callback)
// 		agora_pcm_encoder.agora_encoder_audio.agora_audio_data_callback(frame, 0, NULL);

	//audioObserver->pCircleBuffer->writeBuffer(frame->data[0], dataLen);
	return true;
}

static void AgoraPCM_GetDefaults(obs_data_t *settings)
{
	
}

static bool AgoraPCM_GetExtraData(void *data, uint8_t **extra_data, size_t *size)
{
	return false;
}

//
static void AgoraPCM_GetAudioInfo(void *, struct audio_convert_info *info)
{
	info->format          = AUDIO_FORMAT_16BIT;
	info->samples_per_sec = 44100;
	info->speakers        = SPEAKERS_STEREO;
}

static size_t AgoraPCM_GetFrameSize(void *data)
{
	UNUSED_PARAMETER(data);
	return 1024;
}

static void AgoraPCM_SetCallback(AudioCallback acallback, void* param)
{
	agora_pcm_encoder.agora_audio_encoder_callback = acallback;
	agora_pcm_encoder.param = param;
}

void RegisterAgoraAudioEncoder()
{
	agora_pcm_encoder.id       = "agora_pcm";
	agora_pcm_encoder.type     = OBS_ENCODER_AUDIO;
	agora_pcm_encoder.codec    = "PCM";
	agora_pcm_encoder.get_name = AgoraGetAudioEncoderName;
	agora_pcm_encoder.create   = AgoraPCM_Create;
	agora_pcm_encoder.destroy  = AgoraPCM_Destroy;
	agora_pcm_encoder.encode   = AgoraPCM_Encode;
	agora_pcm_encoder.get_defaults   = AgoraPCM_GetDefaults;
	agora_pcm_encoder.get_extra_data = AgoraPCM_GetExtraData;
	agora_pcm_encoder.get_audio_info = AgoraPCM_GetAudioInfo;
	agora_pcm_encoder.get_frame_size = AgoraPCM_GetFrameSize;
	agora_pcm_encoder.set_agora_audio_callback = AgoraPCM_SetCallback;
	obs_register_encoder(&agora_pcm_encoder);
}



