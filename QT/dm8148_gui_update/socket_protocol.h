#ifndef SOCKET_PROTOCOL_H
#define SOCKET_PROTOCOL_H

#ifdef __cplusplus
extern "C"{
#endif

#define DIR_DATA       "/opt/data"
#define DIR_DATA_NEW       "/opt/dvr_rdk/ti814x/data"

#define DOWNLOAD_FILE_DIR           DIR_DATA"/download_file_dir"
#define DOWNLOAD_FILE_DIR_NEW       DIR_DATA_NEW"/para"
#define PARK_NEW_PROMPT_FILE        DIR_DATA"/new_prompt.data"  
#define PARK_NEW_PROMPT_FILE_NEW        DIR_DATA_NEW"/para/new_prompt.data"  

#define	LCD_PATH_LOGO				DIR_DATA"/lcd/logo/"
#define	LCD_PATH_LOGO_RIGHT			DIR_DATA"/lcd/logo_right/"
#define	LCD_PATH_BG				DIR_DATA"/lcd/bg/"

#define ATTRIBUTE_PACKET __attribute__((__packed__))

#define UDP_PORT                    9103
#define UDP_CLIENT_PORT             9104
#define RECV_DATA_CMD               0x1208
#define DEVICE_NO_DOWNLOAD_FIX_INFO	0x1209
#define DEVICE_NO_DOWNLOAD_PARA		0x120a
#define DEVICE_NO_INIT              0x120b
#define DEVICE_UNIVERSE_STATE       0x120c
#define INET_AUDIOCHANNEL_STATUS_QT 0x120d      //和QT通信，发送音视频通信状态
#define DEVICE_DECODE_ERR_NEED_REBOOT 0x120e    //device decoder error, need to reboot
#define DEVICE_HEARTBEAT_INFO       0x120f      //heartbeat
#define INET_IPCLINK_STATUS_QT      0x1210      //receive IPC status from Qt

#define IN_CMD_RECV_DATA                0x40
#define IN_CMD_NO_DOWNLOAD_FIX_INFO     0x41
#define IN_CMD_NO_DOWNLOAD_PARA         0x42
#define IN_CMD_NO_INIT                  0x43
#define IN_CMD_UNIVERSE_STATE           0x44
#define IN_CMD_CAR_NUMBER               0x45
#define IN_CMD_HEARTBEAT_INFO           0x46
#define IN_CMD_AUDIOCHANNEL_STATUS      0x47
#define IN_CMD_IPCLINK_STATUS           0x48
#define IN_CMD_DECODE_STATUS            0x49
#define IN_CMD_QR_CODE_INFO             0X54   
#define IN_CMD_CANCEL_QR_CODE_INFO      0X55   
#define IN_CMD_QT_LEFT_SITE		0x56   
#define IN_CMD_QT_AD_PIC_FILE_UPDATE	0x57 


//数据区大小为一个int，其取值如下：
#define NS_AUDIO_CH_STATE_OVER              0
#define NS_AUDIO_CH_STATE_G3_REQUEST        1
#define NS_AUDIO_CH_STATE_CONNECT           2
#define NS_AUDIO_CH_STATE_CTRL_REQUEST      3
#define NS_AUDIO_CH_STATE_BUSY              4
#define NS_AUDIO_CH_STATE_CTRL_HANGUP       5
#define NS_AUDIO_CH_STATE_CTRL_CANCEL       6
#define NS_AUDIO_CH_STATE_CTRL_OFFLINE      7
#define NS_AUDIO_CH_STATE_CTRL_FREE         8

//module number
#define SDEMON_MODULE		0x01 //super demon number
#define CONTROL_MODULE		0x02 //control number
#define PARA_MODULE			0x03 //tcp para pthread number
#define VIDEO_MODULE		0x04
#define WEB_MODULE			0x05 //web module
#define RC523_MODULE		0x06 //rc523 module
#define LCD_MODULE           0x07 //qt module

#define MACHINE_TI814X_PARK 0x00C0

//unix socket path
#define CONTROL_UNIX		"/tmp/control"
#define DEVICE_SOUND_UNIX	"/tmp/dev2snd"
#define SDEMON_UNIX			"/tmp/sdemon"	    //super demon unix socket path.
#define SOUND_UNIX			"/tmp/sound"
#define VIDEO_UNIX			"/tmp/video_unix"
#define WEB_CARD_UNIX		"/tmp/web_card"		
#define RC523_UNIX_PATH		"/tmp/rc523_path"
#define LCD_UNIX            "/tmp/QT_LCD_UNIX"
#define GUI_UNIX			"/tmp/gui_unix"

#define TEN_SECS				10000 //10s
#define FIFTEEN_SECS			15000 //15s
#define LCD_WELCOME_BASE_LEN            120

      

/******************************
*unix
***************************/
struct ATTRIBUTE_PACKET cmd_inside_head
{
    unsigned char source_id;	
    unsigned char desc_id;		
    unsigned short session_id;	
    unsigned char cmd_fst;		
    unsigned char cmd_sed;		
    unsigned short length;		
};

struct ATTRIBUTE_PACKET cmd_inside
{
    struct cmd_inside_head head;
    unsigned char data[1024];
};

/******************************
*udp
***************************/
struct ATTRIBUTE_PACKET jse_head //24 bytes
{
    unsigned char version; 			
    unsigned char type;				
    unsigned short packet_id;		
    unsigned short length;			
    unsigned char next_packet_type;	
    unsigned char next_packet_offset; 
    unsigned long system_id;			
    unsigned long source_id;			
    unsigned long dest_id;			
    unsigned short source_port;		
    unsigned short dest_port; 		
};

struct ATTRIBUTE_PACKET jse_app_head
{
    unsigned short device_type; 
    unsigned short command;			
};

struct ATTRIBUTE_PACKET jse_head_and_cmd
{
    struct jse_head		head;
    struct jse_app_head	command;
};





/******************************
*lcd
***************************/
struct ATTRIBUTE_PACKET show_area1_s
{
    int flag; 
    char data[100];
};

struct ATTRIBUTE_PACKET time_area1_s
{
    unsigned char flag; 
    char in_time[30];
    char out_time[30];
    char stay_time[30];
};

struct ATTRIBUTE_PACKET show_fee_s
{
    int flag; 
    int fee;
};

struct ATTRIBUTE_PACKET left_site_s
{
    unsigned char flag; 
    int left_site;
};

struct ATTRIBUTE_PACKET show_car_s
{
    int flag; 
    char car_plate[12];
};

struct ATTRIBUTE_PACKET dev_status_s
{
    unsigned char dev_type;     
    unsigned char status;       
};

struct  ATTRIBUTE_PACKET  lcd_disp_str_s
{
    unsigned char bgimage_flag;
    unsigned char log_flag; 
    unsigned char file_renew_flag;
    unsigned char net_status;
    unsigned char gate_status;
    struct dev_status_s dev_status;
    unsigned char sensor_status;
    struct show_area1_s area1;
    struct time_area1_s time_area;
    struct show_fee_s fee_data;
    struct left_site_s left_site;
    struct show_car_s plate_area;
};

struct ATTRIBUTE_PACKET qr_code_s 
{
    char qr_code_pic[200];       
    char qr_code_content[200];   
};

struct ATTRIBUTE_PACKET lcd_disp_s  
{
    int sense_id;
    char carNo[50];
    char data[2][100];
};
enum enmIoType  
{
        NO_USE		           = 0,   
        BIG_ENTER		   = 1,   
        BIG_EXIT		   = 2,   
        SMALL_ENTER		   = 3,   
        SMALL_EXIT		   = 4,   
        CENTER_CHARGE              = 5,	  
        CENTER_CHARGE_EXIT         = 6,	  
};
struct ATTRIBUTE_PACKET lcd_site_left_s
{
    char left[256];
    unsigned char ioType;
    unsigned char web_disable_left_flag;
};

#ifdef __cplusplus
}
#endif
#endif // SOCKET_PROTOCOL_H
