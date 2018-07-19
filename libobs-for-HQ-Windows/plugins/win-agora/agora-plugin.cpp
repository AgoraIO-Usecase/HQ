#include <obs-module.h>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("win-agora", "en-US")

void RegisterAgoraVideoEncoder();
void RegisterAgoraAudioEncoder();
void RegisterAgoraService();
void RegisterAgoraOutput();
bool obs_module_load(void)
{
	RegisterAgoraVideoEncoder();
	RegisterAgoraAudioEncoder();
	RegisterAgoraOutput();
	RegisterAgoraService();
	return true;
}