/**********************************************************
 * File Name: mainwindow.h
 * Author: sz_yys48
 * E-mail: sz_yys48@jieshun.com.cn
 * Created Time: 2015年08月24日 星期一 15时40分03秒
****************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QPainter>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDate>
#include <QTime>
#include <QDirIterator>
#include <QTimer>
#include <QDebug>
#include <QPixmap>
#include <QImage>
#include <QTextCodec>
#include <QVariant>
#include <QLabel>
#include <QString>
#include <QCursor>
#include <QPalette>
#include <QMovie>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QMessageBox>
#include <QPointer>
#include <QStackedWidget>

#undef USE_UNIX_COMM
#define USE_UNIX_COMM   1

#ifdef USE_UNIX_COMM
#include <sys/socket.h>
#include <stddef.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include "socket_protocol.h"
#include "qrcode.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void reload_timedata_frame_attribute();  
    void reload_top_frame_attribute();
    void reload_bgimage_attribute();
    void reload_logo_atttribute();
    void reload_top_main_frame_attribute();
    void reload_bottom_frame_attribute();

    void clear_prompt_top_content();
    void car_detect_status_switch();
    void set_company_logo(const char *pic_name);
    void set_company_logo_r(const char *pic_name);
    void set_bgimage(const char *bg_name);
    int ready_read_para_file();
    void set_prompt_bottom_content_warning();
    void set_main_prompt_content_busy(char *ptr);
    void set_main_prompt_content_init();
    void filter_pic_display(QString dir_path);
    void show_text_scroll_first(char *ptr);
    void show_text_scroll_second(char *ptr);
#ifdef USE_UNIX_COMM
    int create_unix_socket(const char *path);
#endif
    QString filter_file_absolute_path(QString dir_path, QString file_prefix);
    void set_before_init_device_prompt(QString prompt);
    void set_init_ip_prompt(char *prompt);
    QString gbk2utf8(const char *gbkstr);
    QByteArray utf2gbk(QString utfstr);
    //QString utf2gbk(QString utfstr);
    void mysleep(unsigned int msec);
	void line_data_deal(struct lcd_disp_s *data_ptr);
    void renew_text_info(struct lcd_disp_s *data_ptr);
    void talk_status_switch();

public slots:
    void date_time_update();
    void set_main_prompt_content_free();
#ifdef USE_UNIX_COMM
    void receive_unix_data();
#endif
    void slot_scrollCaption_first();
    void slot_scrollCaption_second();
    void set_talk_status_default();
    void set_qr_code_image(char *qr_data,char *content);
    void set_qr_code_default();

private:
    Ui::MainWindow *ui;

    enum{
        index_display_warning = 0,
        index_display_feetime = 1
    };

    enum{
        index_main_prompt_free = 0,
        index_main_prompt_busy = 1
    };

    QPointer <QTimer> time_timer;
    QPointer <QTimer> top_prompt_timer;     //用于循环播放图片和忙闲时切换定时器
    QPointer <QTimer> scroll_text_fir_timer;//屏幕底部第一行循环定时器
    QPointer <QTimer> scroll_text_sec_timer;//屏幕底部第二行循环定时器
    QPointer <QTimer> main_prompt_pic_timer;
    QPointer <QTimer> talk_status_timer;    //对讲时使用的定时器
    QPointer <QTimer> bottom_prompt_timer;
    QPointer <QTimer> left_num_timer;
#ifdef USE_UNIX_COMM
    QPointer <QTimer> unix_timer;
#endif

    QPointer <QLabel> label4;               //设备未初始化用label4显示
    QPointer <QLabel> label_pic;            //播放图片用label_pic显示
    QPointer <QLabel> label_first_bottom;   //屏幕底部第一行
    QPointer <QLabel> label_second_bottom;  //屏幕底部第二行

    QTextCodec *gbk;
    QTextCodec *utf8;

    QPointer <QStackedWidget> stackWidget;

    struct time_area1_s time_fee_prompt;

    QPointer <QVBoxLayout> verticalLayout_bottom;
    QPointer <QHBoxLayout> horizontalLayout_bottom_warning;
    QPointer <QHBoxLayout> horizontalLayout_bottom_fee;
    QPointer <QHBoxLayout> horizontalLayout_main_prompt;
    QPointer <QVBoxLayout> verticalLayout_main_prompt;
    //QPointer <QVBoxLayout> verticalLayout_main_prompt_pic;
    QPointer <QVBoxLayout> verticalLayout_top_prompt;
    QSpacerItem *horizontalSpacer_warning_left;
    QSpacerItem *horizontalSpacer_warning_right;

    QSpacerItem *horizontalSpacer_timefee_1;
    QSpacerItem *horizontalSpacer_timefee_2;
    QSpacerItem *horizontalSpacer_timefee_3;
    QSpacerItem *horizontalSpacer_timefee_4;

    QSpacerItem *verticalSpacer_enter_up;
    QSpacerItem *verticalSpacer_enter_down;
    QSpacerItem *verticalSpacer_exit_up;
    QSpacerItem *verticalSpacer_exit_down;
    QSpacerItem *verticalSpacer_stay_up;
    QSpacerItem *verticalSpacer_stay_down;

    QPointer <QHBoxLayout> horizontalLayout_enter_time;
    QPointer <QVBoxLayout> verticalLayout_enter_time;
    QPointer <QHBoxLayout> horizontalLayout_exit_time;
    QPointer <QVBoxLayout> verticalLayout_exit_time;
    QPointer <QHBoxLayout> horizontalLayout_stay_time;
    QPointer <QVBoxLayout> verticalLayout_stay_time;

    QString scroll_text_first;
    QString scroll_text_second;

    unsigned char private_talk_status;
    unsigned char private_car_detect_status;
    unsigned char private_network_status;
    unsigned char private_gate_status;
    /*
     * card machine or ticket machine status
     * 0: //device running ok
     * 1: //lack card
     * 2: //no card
     * 3: //error card
     * 4: //full card
     * 5: //lack paper
     * 6: //no paper
     * 7: //jam paper
     * 8: //full paper
     */
    unsigned char private_device_running_status;
    /*
     * 7: //IPC1 broken
     * 8: //IPC2 broken
     * 9: //IPC12 broken
     */
    unsigned char private_device_running_status_2;
    unsigned char private_ticket_card_status;
    unsigned char private_bg_image;

    int day_night; // 0 night; 1 day
    char pic_file_path[20][64];
    int pic_file_num;
#ifdef USE_UNIX_COMM
    int unix_fd;
#endif
    Qrcode *QR_code;
    QString qr_path;
    QTimer  *qrcode_timer;

};

#endif // MAINWINDOW_H
