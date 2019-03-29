#pragma once

#define WM_URL_MSG(code) (WM_USER + 0x500 + code) //48个

#define URL_REQUEST_CHANNEL      0x00000001
#define URL_PUBLISH              0x00000002
#define URL_STOP_ANS             0x00000003
#define URL_RESET                0x00000004
#define URL_INVITE               0x00000005
#define URL_INVITE_STATUS        0x00000006
#define URL_INVITE_STATS_SUCCESS 0x00000007
// #define WM_USER_WND_MSGD(code)  (WM_USER + 0x530 + code) 
// #define WND_CLOSE_CAMERA         0x00000001
// #define WND_OPEN_CAMERA          0x00000002
// 
// 设置界面消息
// #define WM_SETTING_TIMER_MIC_RECORD (WM_USER + 0x546)
// #define WM_SETTING_TIMER_MIC_PLAY_RECORD (WM_USER + 0x547)
// #define WM_SETTING_CLEAR_ALLCACHE (WM_USER +0x548)
// #define WM_SETTING_GETFILE_SIZE (WM_USER +0x549)
// #define WM_AFTER_BROSWER_CREATE (WM_USER +0x550)