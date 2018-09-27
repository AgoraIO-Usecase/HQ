#include "stdafx.h"
#include "audio-encoders.hpp"
#include "AgoraHQ.h"
#include "AgoraHQDlg.h"
#include "window-agora-output.hpp"

/* ------------------------------------------------------------------------ */

struct AgoraOutput : AgoraOutputHandler {
	OBSEncoder             pcmStreaming;
	OBSEncoder             yuvStreaming;
	
	std::string            pcmStreamEncID;
	std::string            videoEncoder;
	std::string            videoQuality;

	AgoraOutput(CAgoraHQDlg *main_);

	//int CalcCRF(int crf);

	void UpdateStreamingSettings_amd(obs_data_t *settings, int bitrate);
	virtual void Update() override;

	void SetupOutputs();
	int GetAudioBitrate() const;
	void LoadAgoraPreset_yuv(const char *encoder);

	void UpdateRecording();
	virtual bool StartAgora(obs_service_t *service) override;
	virtual void StopAgora(bool force) override;
	virtual bool AgoraActive() const override;
	virtual void SetEncoderCallback(obsVideoCallback vcallback, obsAudioCallback acallback) override;
};

AgoraOutput::AgoraOutput(CAgoraHQDlg *main_) : AgoraOutputHandler(main_)
{
	const char *encoder = config_get_string(main->Config(), "AgoraOutput",
		"AgoraVideoEncoder");

	LoadAgoraPreset_yuv("agora_yuv");

	pcmStreamEncID = "agora_pcm";
	pcmStreaming = obs_audio_encoder_create(pcmStreamEncID.c_str(), "agora_raw_audio", nullptr, 0, nullptr);

	if (pcmStreaming)
		obs_encoder_release(pcmStreaming);
	else
 		throw "Failed to create agora pcm encoder (agora output)";
}

int AgoraOutput::GetAudioBitrate() const
{
	int bitrate = (int)config_get_uint(main->Config(), "AgoraOutput",
		"ABitrate");

	return FindClosestAvailableAACBitrate(bitrate);
}

void AgoraOutput::Update()
{
	obs_data_t *yuvSettings = obs_data_create();
	obs_data_t *pcmSettings = obs_data_create();

// 	obs_data_set_string(h264Settings, "rate_control", "CBR");
// 		obs_data_set_int(yuvSettings, "bitrate", videoBitrate);
// 	
// 		obs_data_set_string(pcmSettings, "rate_control", "CBR");
// 		obs_data_set_int(pcmSettings, "bitrate", audioBitrate);

	obs_service_t* agoraService = main->GetAgoraService();
	obs_service_apply_encoder_settings(agoraService,
		yuvSettings, pcmSettings);

	video_t *video = obs_get_video();
	enum video_format format = video_output_get_format(video);

	if (format != VIDEO_FORMAT_NV12 && format != VIDEO_FORMAT_I420)
		obs_encoder_set_preferred_video_format(yuvStreaming,
		VIDEO_FORMAT_NV12);
	
	obs_encoder_update(yuvStreaming, yuvSettings);
	obs_encoder_update(pcmStreaming, pcmSettings);

	obs_data_release(yuvSettings);
	obs_data_release(pcmSettings);
}

#define CROSS_DIST_CUTOFF 2000.0
/*
int AgoraOutput::CalcCRF(int crf)
{
	int cx = config_get_uint(main->Config(), "Video", "OutputCX");
	int cy = config_get_uint(main->Config(), "Video", "OutputCY");
	double fCX = double(cx);
	double fCY = double(cy);

	if (lowCPUx264)
		crf -= 2;

	double crossDist = sqrt(fCX * fCX + fCY * fCY);
	double crfResReduction =
		fmin(CROSS_DIST_CUTOFF, crossDist) / CROSS_DIST_CUTOFF;
	crfResReduction = (1.0 - crfResReduction) * 10.0;

	return crf - int(crfResReduction);
}
*/
inline void AgoraOutput::SetupOutputs()
{
	AgoraOutput::Update();
	obs_encoder_set_video(yuvStreaming, obs_get_video());
	obs_encoder_set_audio(pcmStreaming, obs_get_audio());
}

bool AgoraOutput::StartAgora(obs_service_t *service)
{
	if (!Active())
		SetupOutputs();

	/* --------------------- */

	const char *type = obs_service_get_output_type(service);
	if (!type)
		type = "agora_output";

	/* XXX: this is messy and disgusting and should be refactored */
	if (outputType != type) {
		agoraOutput = obs_output_create(type, "agora_output_name",
			nullptr, nullptr);
		if (!agoraOutput) {
			blog(LOG_WARNING, "Creation of agora output type '%s' "
				"failed!", type);
			return false;
		}
		obs_output_release(agoraOutput);

		const char *codec =
			obs_output_get_supported_audio_codecs(agoraOutput);
		if (!codec) {
			return false;
		}

		if (strcmp(codec, "pcm") != 0) {
			const char *id = "agora_pcm";
			int audioBitrate = GetAudioBitrate();
			obs_data_t *settings = obs_data_create();
			obs_data_set_int(settings, "bitrate", audioBitrate);

			pcmStreaming = obs_audio_encoder_create(id,
				"alt_audio_enc", nullptr, 0, nullptr);
			obs_encoder_release(pcmStreaming);
			if (!pcmStreaming)
				return false;

			obs_encoder_update(pcmStreaming, settings);
			obs_encoder_set_audio(pcmStreaming, obs_get_audio());
			obs_data_release(settings);
		}

		outputType = type;
	}

	obs_output_set_video_encoder(agoraOutput, yuvStreaming);
	obs_output_set_audio_encoder(agoraOutput, pcmStreaming, 0);
	obs_output_set_service(agoraOutput, service);

	/* --------------------- */

	//obs_data_t *settings = obs_data_create();
	//obs_output_update(agoraOutput, settings);
	//obs_data_release(settings);

	int	maxRetries = 0;

	obs_output_set_delay(agoraOutput, 0, 0);
	obs_output_set_reconnect_settings(agoraOutput, maxRetries, 0);

	if (obs_output_start(agoraOutput)) {
		return true;
	}

	const char *error = obs_output_get_last_error(agoraOutput);
	bool has_last_error = error && *error;

	blog(LOG_WARNING, "Stream output type '%s' failed to start!%s%s",
		type,
		has_last_error ? "  Last Error: " : "",
		has_last_error ? error : "");
	return false;
}

void AgoraOutput::StopAgora(bool force)
{
	if (force)
		obs_output_force_stop(agoraOutput);
	else
		obs_output_stop(agoraOutput);

	obs_encoder_add_agora_audio_callback(pcmStreaming, NULL, NULL);
	obs_encoder_add_agora_video_callback(yuvStreaming, NULL, NULL);
}

void AgoraOutput::LoadAgoraPreset_yuv(const char *encoderId)
{
	yuvStreaming = obs_video_encoder_create(encoderId,
		"agora_raw_video", nullptr, nullptr);
	if (!yuvStreaming)
		throw "Failed to create agora yuv encoder (agora output)";
	obs_encoder_release(yuvStreaming);
}

bool AgoraOutput::AgoraActive() const
{
	return obs_output_active(agoraOutput);
}

void AgoraOutput::SetEncoderCallback(obsVideoCallback vcallback, obsAudioCallback acallback)
{
	obs_encoder_add_agora_audio_callback(pcmStreaming, acallback, static_cast<void*>(main));
	obs_encoder_add_agora_video_callback(yuvStreaming, vcallback, static_cast<void*>(main));
}
/* ------------------------------------------------------------------------ */

AgoraOutputHandler *CreateAgoraOutputHandler(CAgoraHQDlg *main)
{
	return new AgoraOutput(main);
}
