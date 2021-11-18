/****************************************************************
 * File Name: mainwindow.cpp
 * Author: sz_yys48
 * E-mail: sz_yys48@jieshun.com.cn
 * Created Time: 2015年08月24日 星期一 15时49分03秒
****************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "socket_protocol.h"


#if 0
#define JSST_CHECK_PTR(p) \
    do{ \
        if(!(p)) \
        { \
            qWarning("[%s] line %d: Out of memory", __FILE__,__LINE__); \
            QMessageBox::warning(this, tr("Warning"), \
            tr("Null Pointer"), \
            QMessageBox::Ok); \
            while(1); \
        } \
    } while (0)
#else
#define JSST_CHECK_PTR(p) \
    do{ \
        if(!(p)) \
        { \
            qWarning("[%s] line %d: Out of memory", __FILE__,__LINE__); \
        } \
    } while (0)
#endif

//#define SET_COMPANY_TEL_FLAG    //  显示公司名和电话号码

static const char weeks[][12]={"","星期一","星期二","星期三","星期四","星期五","星期六","星期日"};

static char lcd_welcome_base[LCD_WELCOME_BASE_LEN];
static const char *font_color_yellow = "color: rgb(255, 189, 74);";
static const char *font_color_white = "color: rgb(255, 255, 255);";
static struct lcd_site_left_s site_left;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    day_night = 0; //use day style
    private_talk_status = NS_AUDIO_CH_STATE_CTRL_FREE;
    private_car_detect_status = 0;

    stackWidget = new QStackedWidget();
    JSST_CHECK_PTR(stackWidget);

    verticalLayout_bottom = new QVBoxLayout(ui->bottom_frame);
    JSST_CHECK_PTR(verticalLayout_bottom);

    horizontalLayout_main_prompt = new QHBoxLayout(ui->main_prompt_frame);
    JSST_CHECK_PTR(horizontalLayout_main_prompt);

    verticalLayout_main_prompt = new QVBoxLayout(ui->main_prompt_frame);
    JSST_CHECK_PTR(verticalLayout_main_prompt);

    //verticalLayout_main_prompt_pic = new QVBoxLayout(ui->main_prompt_frame);
    //JSST_CHECK_PTR(verticalLayout_main_prompt_pic);

    verticalLayout_top_prompt = new QVBoxLayout(ui->top_frame);
    JSST_CHECK_PTR(verticalLayout_top_prompt);

    label4 = new QLabel();
    JSST_CHECK_PTR(label4);
    label4->setAlignment(Qt::AlignCenter);
    // 自动换行
    label4->setWordWrap(true);
    verticalLayout_top_prompt->addWidget(label4);

    horizontalLayout_main_prompt->setContentsMargins(0,0,0,0);
    horizontalLayout_main_prompt->addWidget(ui->label_pic);

    verticalLayout_main_prompt->addWidget(ui->label_top_main);
    verticalLayout_main_prompt->addWidget(ui->label_bottom_main);

    label_first_bottom = new QLabel();
    JSST_CHECK_PTR(label_first_bottom);
    label_second_bottom = new QLabel();
    JSST_CHECK_PTR(label_second_bottom);
    verticalLayout_bottom->addWidget(label_first_bottom);
    verticalLayout_bottom->addWidget(label_second_bottom);
    verticalLayout_bottom->setSpacing(10);
    ui->bottom_frame->setLayout(verticalLayout_bottom);

    setWindowFlags(Qt::FramelessWindowHint);

    //set unicode
    gbk = NULL;
    gbk = QTextCodec::codecForName("GB18030");
    JSST_CHECK_PTR(gbk);
    utf8 = NULL;
    utf8 = QTextCodec::codecForName("utf-8");   // utf-8
    JSST_CHECK_PTR(utf8);
    QTextCodec::setCodecForTr(utf8);
    QTextCodec::setCodecForCStrings(utf8);
    QTextCodec::setCodecForLocale(utf8);

    //创建定时器用于界面上的时间显示
    time_timer = new QTimer(this);
    JSST_CHECK_PTR(time_timer);
    QObject::connect(time_timer,SIGNAL(timeout()),this, SLOT(date_time_update()));
    time_timer->start(1000);

    top_prompt_timer = new QTimer(this);
    JSST_CHECK_PTR(top_prompt_timer);
    QObject::connect(top_prompt_timer,SIGNAL(timeout()),this, SLOT(set_main_prompt_content_free()));

    scroll_text_fir_timer = new QTimer(this);
    JSST_CHECK_PTR(scroll_text_fir_timer);
    QObject::connect(scroll_text_fir_timer,SIGNAL(timeout()),this, SLOT(slot_scrollCaption_first()));

    scroll_text_sec_timer = new QTimer(this);
    JSST_CHECK_PTR(scroll_text_sec_timer);
    QObject::connect(scroll_text_sec_timer,SIGNAL(timeout()),this, SLOT(slot_scrollCaption_second()));

    talk_status_timer = new QTimer(this);
    JSST_CHECK_PTR(talk_status_timer);
    QObject::connect(talk_status_timer,SIGNAL(timeout()),this, SLOT(set_talk_status_default()));


#ifdef USE_UNIX_COMM
    unix_timer = new QTimer(this);
    JSST_CHECK_PTR(unix_timer);
    QObject::connect(unix_timer, SIGNAL(timeout()), this, SLOT(receive_unix_data()));
    unix_timer->start(300);
#endif

#ifdef USE_UNIX_COMM
    unix_fd = create_unix_socket(GUI_UNIX);
    if(unix_fd < 0)
    {
        qDebug() << "create unix socket error";
    }
#endif
    printf("*******qt test here 2**********\r\n");

    ready_read_para_file();

    date_time_update();

    //二维码
    qrcode_timer = new QTimer(this);
    JSST_CHECK_PTR(qrcode_timer);
    connect(qrcode_timer,SIGNAL(timeout()),this,SLOT(set_qr_code_default()));
    QR_code = new Qrcode();
#ifdef Q_WS_QWS
    qr_path = "/opt/data/QR_code.png";
#else
    qr_path = "/sdcworkspace/hc_test/dm8148_gui_update/QR_code.png";
#endif   
    qDebug() << "Construct Done";
    set_qr_code_default();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reload_timedata_frame_attribute()
{
    QFont font1;
    font1.setPointSize(20);
    QFont font2;
    font2.setPointSize(20);

    ui->label_hms->setFont(font1);
    ui->label_hms->setAlignment(Qt::AlignCenter);

    ui->label_date->setFont(font2);
    ui->label_date->setAlignment(Qt::AlignCenter);

    ui->label_week->setAlignment(Qt::AlignCenter);
    ui->label_week->setFont(font2);

    ui->label_hms->setStyleSheet(0);
    ui->label_week->setStyleSheet(0);
    ui->label_date->setStyleSheet(0);

    if(day_night)
    {
        ui->label_hms->setStyleSheet(QString::fromUtf8(font_color_white));
        ui->label_week->setStyleSheet(QString::fromUtf8(font_color_white));
        ui->label_date->setStyleSheet(QString::fromUtf8(font_color_white));
    }
    else
    {
        ui->label_hms->setStyleSheet(QString::fromUtf8(font_color_yellow));
        ui->label_week->setStyleSheet(QString::fromUtf8(font_color_yellow));
        ui->label_date->setStyleSheet(QString::fromUtf8(font_color_yellow));
    }
}

void MainWindow::reload_top_main_frame_attribute()
{

    QFont font2;
    ui->label_top_main->setStyleSheet(0);
    font2.setPointSize(93);
    ui->label_top_main->setFont(font2);
    ui->label_top_main->setWordWrap(true);
    ui->label_top_main->setAlignment(Qt::AlignCenter);

    if(day_night)
    {
        ui->label_top_main->setStyleSheet(QString::fromUtf8(font_color_white));
    }
    else
    {
        ui->label_top_main->setStyleSheet(QString::fromUtf8(font_color_yellow));
    }
}

void MainWindow::reload_top_frame_attribute()
{
    label4->setStyleSheet(0);

    if(day_night)
    {
        label4->setStyleSheet(QString::fromUtf8(font_color_white));
    }
    else
    {
        label4->setStyleSheet(QString::fromUtf8(font_color_yellow));
    }
}

void MainWindow::reload_bottom_frame_attribute()
{
    label_first_bottom->setStyleSheet(0);
    label_first_bottom->setWordWrap(false);

    QFont font3;
    label_second_bottom->setStyleSheet(0);
    font3.setPointSize(93);
    label_second_bottom->setFont(font3);
    label_second_bottom->setWordWrap(false);

    if(day_night)
    {
        label_second_bottom->setStyleSheet(QString::fromUtf8(font_color_white));
        label_first_bottom->setStyleSheet(QString::fromUtf8(font_color_white));
    }
    else
    {
        label_second_bottom->setStyleSheet(QString::fromUtf8(font_color_yellow));
        label_first_bottom->setStyleSheet(QString::fromUtf8(font_color_yellow));
    }
}

void MainWindow::reload_bgimage_attribute()
{
    if(private_talk_status!=NS_AUDIO_CH_STATE_CTRL_FREE)
    {
        return;
    }
    QString absolute_bgimage_path = filter_file_absolute_path(QString(LCD_PATH_BG), QString("bgimage"));
    if(!absolute_bgimage_path.isEmpty())
    {
        //set_bgimage(DOWNLOAD_FILE_DIR"/bgimage.png");
        set_bgimage(qPrintable(absolute_bgimage_path));
    }
    else
    {
        if(day_night)
            set_bgimage(":/day/background.png");
        else
            set_bgimage(":/night/background.png");
    }
}

void MainWindow::reload_logo_atttribute()
{
    QString check_logo_file_path = filter_file_absolute_path(QString(LCD_PATH_LOGO), QString("customer_logo"));
    if(!check_logo_file_path.isEmpty())
    {
        set_company_logo(qPrintable(check_logo_file_path));
    }
    else
    {
        set_company_logo(":/logo.png");
    }

    QString check_logo_r_file_path = filter_file_absolute_path(QString(LCD_PATH_LOGO_RIGHT), QString("customer_logo"));
      qDebug()<<check_logo_r_file_path;
    if(!check_logo_r_file_path.isEmpty())
    {
        set_company_logo_r(qPrintable(check_logo_r_file_path));
    }
    else
    {
        qDebug()<<"logo_r_file"<<" err";
        ui->label_logo_r->clear();
    }
}

int MainWindow::ready_read_para_file()
{
    qint64 file_len;
    memset(lcd_welcome_base, 0, sizeof(lcd_welcome_base));

    QString para_file_1 = "/opt/hi3536/data/para/lcd_welcome";
    char lcd_welcome[LCD_WELCOME_BASE_LEN] ={0};
    QFile fd(para_file_1);
    memcpy(lcd_welcome_base,"捷顺科技欢迎您",strlen("捷顺科技欢迎您"));
    if(!(fd.open(QFile::ReadOnly)))
    {
        qDebug()<<"open "<<para_file_1<<" err";
        return -1;
    }

    qDebug()<<"open "<<para_file_1<<" success";

    file_len = fd.read((char *)lcd_welcome, LCD_WELCOME_BASE_LEN);

    if(file_len <=0)
    {
        qDebug()<<"read "<<para_file_1<<"err";
        fd.close();
        return -1;
    }
    memset(lcd_welcome_base,0,LCD_WELCOME_BASE_LEN);
    memcpy(lcd_welcome_base,lcd_welcome,LCD_WELCOME_BASE_LEN);
    fd.close();

    return 0;
}

//已调试
QString MainWindow::gbk2utf8(const char *gbkstr)
{
	#if 0
    JSST_CHECK_PTR(gbkstr);
    return (gbk->toUnicode(gbkstr));
	#else
	QString showtext = gbkstr;
	return showtext;
	#endif
}

//未调试
QByteArray MainWindow::utf2gbk(QString utfstr)
{
    //return utf8->toUnicode(utfstr.toLocal8Bit());
    //return gbk->toUnicode(utfstr);
    QString strUnicode = utf8->toUnicode(utfstr.toLocal8Bit().data());
    QByteArray gb_bytes = gbk->fromUnicode(strUnicode);
    return gb_bytes;
}

//界面上的时间显示格式
void MainWindow::date_time_update()
{
    QString hmsDisp;

    QDate cur_date;
    QTime cur_time;

    //define day start time and end time(7:00 ~ 18:00)
    QTime start_time(7, 0, 0);
    QTime end_time(18, 0, 0);

    static int old_year = 0;
    static int old_month = 0;
    static int old_week = 0;
    static int old_day = 0;
    static int ModeRefreshCount = 0;
    static int initFlag = 0;

    int cur_year;
    int cur_month;
    int cur_week;
    int cur_day;

    cur_time = QTime::currentTime();
    hmsDisp = cur_time.toString("hh:mm");
    ui->label_hms->clear();
    ui->label_hms->setText(hmsDisp);

    if(ModeRefreshCount == 0)
    {
        if((cur_time.secsTo(start_time) <= 0) && (cur_time.secsTo(end_time) >= 0))
        {
            day_night = 1;
        }
        else
        {
            day_night = 0;
        }

        if (initFlag == 0)
        {         
            set_main_prompt_content_init();
            initFlag = 1;
        }

        reload_bgimage_attribute();
        reload_logo_atttribute();        
        reload_timedata_frame_attribute();
        reload_top_frame_attribute();
        reload_bgimage_attribute();
        reload_top_main_frame_attribute();
        reload_bottom_frame_attribute();
        ready_read_para_file();
    }

    if(ModeRefreshCount++ >= 10)
    {
        ModeRefreshCount = 0;
    }

    cur_date = QDate::currentDate();

    cur_week = cur_date.dayOfWeek();
    if(old_week != cur_week)
    {
        ui->label_week->clear();
        ui->label_week->setText(weeks[cur_week]);
        old_week = cur_week;
    }

    cur_year = cur_date.year();
    cur_month = cur_date.month();
    cur_day = cur_date.dayOfYear();

    if((old_year != cur_year) || (old_month != cur_month) || (old_day != cur_day))
    {
        QString dayDisp;
        dayDisp = cur_date.toString("yyyy-MM-dd");
        ui->label_date->clear();
        ui->label_date->setText(dayDisp);
        old_year = cur_year;
        old_month = cur_month;
        old_day = cur_day;
    }
}

void MainWindow::car_detect_status_switch()
{
    static unsigned char tmp_state= 2;
    unsigned char state = private_car_detect_status;

    switch(state)
    {
        case 0://on car
        {     
            tmp_state = 0;
            break;
        }
        case 1://off car
        {          
            if(tmp_state==0)
            {
                tmp_state = 1;  //from 0 to 1
                set_qr_code_default();
            }
            break;
        }
        default:
            break;
    }
}

void MainWindow::set_company_logo(const char *pic_name)
{
    QImage image;
    QPixmap pixmapToShow;

    image.load(pic_name);
    pixmapToShow = QPixmap::fromImage(image.scaled(QSize(112,36), Qt::IgnoreAspectRatio));

    ui->label_logo->clear();
    ui->label_logo->setPixmap(pixmapToShow);
}

void MainWindow::set_company_logo_r(const char *pic_name)
{
    QImage image;
    QPixmap pixmapToShow;

    image.load(pic_name);
    pixmapToShow = QPixmap::fromImage(image.scaled(QSize(112,36), Qt::IgnoreAspectRatio));

    ui->label_logo_r->clear();
    ui->label_logo_r->setPixmap(pixmapToShow);
}
void MainWindow::set_bgimage(const char *bg_name)
{
    QImage image;
    QPixmap pixmap;
    image.load(bg_name);
    pixmap = QPixmap::fromImage( image.scaled(QSize(768,1366), Qt::IgnoreAspectRatio) );

    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(pixmap));
    this->setPalette(palette);
}
void MainWindow::set_main_prompt_content_free()
{
    bool ret;
    //char ptr[32];
    QImage image;
    QPixmap pixmapToShow;    
    //QFont font2;
    QString center_text;
    qDebug()<<"set_main_prompt_content_free";

    if(private_talk_status!=NS_AUDIO_CH_STATE_CTRL_FREE)
    {
    	qDebug()<<"private_talk_status:"<<private_talk_status;
        return;
    }

    top_prompt_timer->stop();
    label4->clear();        
    ui->label_pic->clear();
    ui->label_top_main->clear();
    ui->label_bottom_main->clear();
    //label_first_bottom->clear();
    label_second_bottom->clear();
    scroll_text_fir_timer->stop();
    scroll_text_sec_timer->stop();
    reload_bgimage_attribute();
    if(((site_left.ioType==BIG_ENTER)||(site_left.ioType==SMALL_ENTER))&&(site_left.web_disable_left_flag==0))
    {       
#if 0        
        if(site_left.left>999)
        {
            sprintf(ptr,"%s%d","剩余车位:",site_left.left);
        }
        else if(site_left.left==0)
        {
            sprintf(ptr,"%s","车位已满");
        }
        else
        {
            sprintf(ptr,"%s%d","剩余车位: ",site_left.left);
        }
#endif		
        qDebug()<<site_left.left;
        show_text_scroll_first(site_left.left);
       // font2.setPointSize(110);
       //label_first_bottom->setFont(font2);
       //label_first_bottom->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);
       //label_first_bottom->setText(tr(site_left.left));
    }
    else
    {
        label_first_bottom->clear();
    }

    if(image.load(pic_file_path[0])==false)
    {
        center_text = tr(lcd_welcome_base);
        ui->label_top_main->setText(center_text);
        qDebug()<<"center_text.length()="<<center_text.length();
       //ui->label_top_main->setText(tr(lcd_welcome_base));
       // image_bottom_main.load(":/day/park.png");
       // pixmap_bottom_main = QPixmap::fromImage(image_bottom_main.scaled(QSize(564,350), Qt::IgnoreAspectRatio));
       // ui->label_bottom_main->clear();
       // ui->label_bottom_main->setAlignment(Qt::AlignCenter);
       // ui->label_bottom_main->setPixmap(pixmap_bottom_main);
    }
    else
    {
        if(pic_file_num>=20)
        {
            pic_file_num=0;
        }
        ret = image.load(pic_file_path[pic_file_num]);
        if(ret==false)
        {
           pic_file_num=0;
           image.load(pic_file_path[pic_file_num]);
           pic_file_num++;
        }
        else
        {
            pic_file_num++;
        }
        qDebug()<<pic_file_num<<ret;
        pixmapToShow = QPixmap::fromImage(image.scaled(QSize(768,900), Qt::IgnoreAspectRatio));
        ui->label_pic->setAlignment(Qt::AlignCenter);
        ui->label_pic->setPixmap(pixmapToShow);       
     }
     top_prompt_timer->setSingleShot(true);
     top_prompt_timer->start(5000);
}

void MainWindow::show_text_scroll_first(char *ptr)
{
    int len = 0;
    QByteArray p;

     QFont font2;
     font2.setPointSize(93);
     label_first_bottom->setFont(font2);

     scroll_text_first = gbk2utf8(ptr);
     //label_first_bottom->clear();
     qDebug()<<"scroll_text_first.toLocal8Bit().length()="<<scroll_text_first.toLocal8Bit().length();

     p = utf2gbk(scroll_text_first);
     len = p.size();
     qDebug()<<"len="<<len;

     if(len<=16)//8 hanzi
     {
          label_first_bottom->setAlignment(Qt::AlignCenter);
          label_first_bottom->setText(scroll_text_first);
     }
     else
     {
         label_first_bottom->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
          scroll_text_fir_timer->setSingleShot(true);
          scroll_text_fir_timer->start(500);
     }
}

void MainWindow::slot_scrollCaption_first()
{
    static int nPos_fir = 0;
    QFont font2;
    font2.setPointSize(93);
    label_first_bottom->setFont(font2);
    if(nPos_fir>scroll_text_first.length())
    {
        nPos_fir = 0;
    }
    label_first_bottom->setText(scroll_text_first.mid(nPos_fir));
    nPos_fir++;
    scroll_text_fir_timer->setSingleShot(true);
    scroll_text_fir_timer->start(500);
}

void MainWindow::show_text_scroll_second(char *ptr)
{ 
     int len = 0;
     QByteArray p;
     scroll_text_second = gbk2utf8(ptr);
     label_second_bottom->clear();
     qDebug()<<"scroll_text_second.toLocal8Bit().length()="<<scroll_text_second.toLocal8Bit().length();

     p = utf2gbk(scroll_text_second);
     len = p.size();
     qDebug()<<"len="<<len;

     if(len<=16)//8 hanzi
     {
          label_second_bottom->setAlignment(Qt::AlignCenter);
          label_second_bottom->setText(scroll_text_second);
     }
     else
     {
         label_second_bottom->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
         scroll_text_sec_timer->setSingleShot(true);
         scroll_text_sec_timer->start(500);
     }
}

void MainWindow::slot_scrollCaption_second()
{
    static int nPos_sec = 0;
    if(nPos_sec>scroll_text_second.length())
    {
        nPos_sec = 0;
    }
    label_second_bottom->setText(scroll_text_second.mid(nPos_sec));
    nPos_sec++;
    scroll_text_sec_timer->setSingleShot(true);
    scroll_text_sec_timer->start(200);
}

void MainWindow::set_main_prompt_content_busy(char *ptr)
{
    QFont font2;
    char *p;
    qDebug()<<"set_main_prompt_content_busy ptr "<<ptr;
    qDebug() << "private_talk_status:" <<private_talk_status;
    if(private_talk_status!=NS_AUDIO_CH_STATE_CTRL_FREE)
    {
        return;
    }

    top_prompt_timer->stop();
    ui->label_pic->clear();
    label4->clear();
    ui->label_top_main->clear();
    ui->label_bottom_main->clear();
    scroll_text_fir_timer->stop();
    scroll_text_sec_timer->stop();

    p = strstr(ptr,"font");
    if(p!=NULL)
    {
        font2.setPointSize(100);
    }
    else
    {
        font2.setPointSize(100+12*5);
    }
    label4->setFont(font2);

   //QString text = "<font size='+2'>粤B12345</font>";
   // QString text = "<font size='5' color='Red'>粤B1234</font>";

    QString text = gbk2utf8(ptr);    
    qDebug()<<text;
    ui->label_top_main->setText(text);
    //label4->setText(text);
#if 0// _Aaron_
    top_prompt_timer->setSingleShot(true);
    top_prompt_timer->start(FIFTEEN_SECS);
#endif 
}
void MainWindow::set_main_prompt_content_init()
{    
    QString dir_path="/opt/data/ad/pic";
    filter_pic_display(dir_path);      
    set_main_prompt_content_free();
}

void MainWindow::set_prompt_bottom_content_warning()
{
    qDebug()<<"set_prompt_bottom_content_warning";
    label_first_bottom->clear();
    label_second_bottom->clear();
}

void MainWindow::clear_prompt_top_content()
{
    label4->clear();
}

#ifdef USE_UNIX_COMM
int MainWindow::create_unix_socket(const char *path)
{
    int sockfd, len, flag, ret;
    struct sockaddr_un address;

    unlink(path);

    if(strlen(path) > 50)
    {
        qDebug() << "unix socktet path is too long";
        return -1;
    }

    if((sockfd = ::socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
    {
        qDebug() << "create unix socktet failure";
        return -1;
    }

    bzero(&address, sizeof(address));
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, path);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(address.sun_path);

    if(bind(sockfd, (struct sockaddr *)&address, len) < 0)
    {
        qDebug() << "bind failed";
        ::close(sockfd);
        return -1;
    }

    flag = ::fcntl(sockfd, F_GETFL, 0);
    ret = ::fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);
    if(ret < 0)
    {
        qDebug() << "fcntl failure";
    }

    return sockfd;
}
#endif

void MainWindow::set_before_init_device_prompt(QString prompt)
{
    clear_prompt_top_content();

    QFont font2;
    font2.setPointSize(50);

    label4->setFont(font2);
    label4->setText(prompt);
}

void MainWindow::set_init_ip_prompt(char *prompt)
{
    top_prompt_timer->stop();
    label4->clear();
    ui->label_pic->clear();
    ui->label_top_main->clear();
    ui->label_bottom_main->clear();
    label_first_bottom->clear();
    label_second_bottom->clear();
    scroll_text_fir_timer->stop();
    scroll_text_sec_timer->stop();

    QFont font;
    font.setPointSize(50);
    label4->setFont(font);
    QString text = tr(prompt);
    label4->setText(text);
}

QString MainWindow::filter_file_absolute_path(QString dir_path, QString file_prefix)
{
    QString absolute_file_path;
    QFileInfo file_info;
    QStringList filters;

    QDir dir(dir_path);

    if(!dir.exists())
        return NULL;

    filters<<QString("*.png")<<QString("*.bmp")<<QString("*.jpg")<<QString("*.jpeg");

    QDirIterator dir_iterator(dir_path,filters, QDir::Files|QDir::NoSymLinks,
                              QDirIterator::NoIteratorFlags);

    while(dir_iterator.hasNext())
    {
        dir_iterator.next();
        file_info=dir_iterator.fileInfo();
        if(file_info.baseName()==file_prefix)
        {
            absolute_file_path=file_info.absoluteFilePath();
            return absolute_file_path;
        }
    }

    absolute_file_path.clear();
    return absolute_file_path;
}
void MainWindow::mysleep(unsigned int msec)
{
    QTime reachTime = QTime::currentTime().addMSecs(msec);
    while(QTime::currentTime()<reachTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
}

void MainWindow::filter_pic_display(QString dir_path)
{
    QString absolute_file_path;
    QFileInfo file_info;
    QStringList filters;
    QDir dir(dir_path);
    int i = 0;
    QByteArray tmp;

    if(!dir.exists())
    {
       clear_prompt_top_content();
       return ;
    }

    filters<<QString("*.png")<<QString("*.bmp")<<QString("*.jpg")<<QString("*.jpeg");

    QDirIterator dir_iterator(dir_path,filters, QDir::Files|QDir::NoSymLinks,
                              QDirIterator::NoIteratorFlags);
     qDebug()<<"filter_pic_display"<<dir_iterator.hasNext();
    while(dir_iterator.hasNext())
    {
        dir_iterator.next();
        file_info=dir_iterator.fileInfo();
        absolute_file_path=file_info.absoluteFilePath();
        tmp = absolute_file_path.toLocal8Bit();
        strcpy(&pic_file_path[i][0],tmp.data());
        qDebug()<<"filter_pic_display1"<<absolute_file_path;
         qDebug()<<"filter_pic_display2"<<tmp.data();
        qDebug()<<"filter_pic_display3"<<&pic_file_path[i][0];
        i++;
        if(i>=20)
        {
            i = 0;
        }
       // mysleep(2000);
    }
    return ;
}

void MainWindow::set_qr_code_default()
{
     ui->stackedWidget_parent->setCurrentWidget(ui->page_default);
}

void MainWindow::set_qr_code_image(char *qr_data,char *content)
{
    QString str1,str2;
    QPixmap qr_pix,qr_pix2;
    QFont font2;

    qrcode_timer->stop();    //二维码显示时长计时器

    str1 = gbk2utf8(qr_data);
    str2 = gbk2utf8(content);

    printf("day_night=%d\n",day_night);
    if(day_night==0)
    {
        QR_code->saveImageBlackGround(qr_path,748,str1);
    }
    else
    {
         QR_code->saveImage(qr_path,748,str1);
    }

    // QR_code->saveImage(qr_path,748,str1);
    ui->label_qrcode_pic->clear();
    ui->label_qrcode_pic->setStyleSheet(0); //样式表恢复默认

    qr_pix.load(qr_path);
    qr_pix = qr_pix.scaled(ui->label_qrcode_pic->width(),ui->label_qrcode_pic->height(),Qt::KeepAspectRatio); //自动调整大小

    if(day_night==0)
    {
       // ui->label_qrcode_pic->setStyleSheet("QFrame#label_qrcode_pic{border-width:1px;border-style:solid;border-radius:8px;border-color:black;background-color:black}");
        ui->label_qrcode_pic->setStyleSheet("QFrame#label_qrcode_pic{border-width:1px;border-style:solid;border-radius:8px;border-color:white;background-color:black}");
        ui->frame_qrcode_pic->setStyleSheet("QFrame#frame_qrcode_pic{background-color:black}");
    }
    else
    {
       ui->label_qrcode_pic->setStyleSheet("QFrame#label_qrcode_pic{border-width:1px;border-style:solid;border-radius:8px;border-color:white;background-color:white}");
       ui->frame_qrcode_pic->setStyleSheet("QFrame#frame_qrcode_pic{background-color:white}");
    }
    //ui->label_qrcode_pic->setMargin(10);
    ui->label_qrcode_pic->setPixmap(qr_pix); //显示二维码

    font2.setPointSize(93);

    if(day_night==0)
    {
         ui->label_qtcode_str->setStyleSheet("QLabel{background:#000000;color:white;}");
    }
    else
    {
        ui->label_qtcode_str->setStyleSheet("QLabel{background:#ffffff;color:black;}");
    }

    ui->label_qtcode_str->setFont(font2);
    ui->label_qtcode_str->setText(str2);
    ui->stackedWidget_parent->setCurrentWidget(ui->page_qrcode);

    qrcode_timer->setSingleShot(true);
    qrcode_timer->start(1800000);
}

void  MainWindow::line_data_deal(struct lcd_disp_s *data_ptr)
{
	char *p;

	if(strlen(data_ptr->data[1])>0)
	{
		return;
	}

	p=strchr(data_ptr->data[0],',');
		
	if(p==NULL)
	{
		return;
	}

	*p=0;p++;
	
	strcpy(data_ptr->data[1],p);
}

void MainWindow::renew_text_info(struct lcd_disp_s *data_ptr)
{
    QImage image_bottom_main;
    QPixmap pixmap_bottom_main;
	
	scroll_text_fir_timer->stop();
    scroll_text_sec_timer->stop();

#if 0
    if(data_ptr->sense_id==158) //禁止通行
    {
        label4->clear();
        ui->label_pic->clear();
        ui->label_top_main->clear();         
        image_bottom_main.load(":/day/nopark.png");
        pixmap_bottom_main = QPixmap::fromImage(image_bottom_main.scaled(QSize(240,238), Qt::IgnoreAspectRatio));
        ui->label_bottom_main->clear();
        ui->label_bottom_main->setAlignment(Qt::AlignCenter);
        ui->label_bottom_main->setPixmap(pixmap_bottom_main);
    }
#endif	
    if(strlen(data_ptr->carNo)>2) //有车牌号码，需刷新main_frame
    {
         set_main_prompt_content_busy(data_ptr->carNo);
    }
    label_first_bottom->clear();
    label_second_bottom->clear();

	line_data_deal(data_ptr);//行分隔
	
    show_text_scroll_first(data_ptr->data[0]);
    show_text_scroll_second(data_ptr->data[1]);
    top_prompt_timer->setSingleShot(true);
    top_prompt_timer->start(FIFTEEN_SECS);

}
void MainWindow::set_talk_status_default()
{
    private_talk_status = NS_AUDIO_CH_STATE_CTRL_FREE;
    set_main_prompt_content_free();
}

void MainWindow::talk_status_switch()
{
    unsigned char state = private_talk_status;
    QImage image_top_main;
    QPixmap pixmap_top_main;

    top_prompt_timer->stop();
    label4->clear();
    ui->label_pic->clear();
    ui->label_top_main->clear();
    ui->label_bottom_main->clear();
    label_first_bottom->clear();
    label_second_bottom->clear();
    scroll_text_fir_timer->stop();
    scroll_text_sec_timer->stop();

    set_bgimage(":/day/background2.png");
    image_top_main.load(":/day/talk.png");
    pixmap_top_main = QPixmap::fromImage(image_top_main.scaled(QSize(768,450), Qt::IgnoreAspectRatio));
    ui->label_top_main->setAlignment(Qt::AlignCenter);
    ui->label_top_main->setPixmap(pixmap_top_main);

    switch(state)
    {
        case NS_AUDIO_CH_STATE_OVER: //talk over
        {
            QMovie *movie = new QMovie(":/day/talk_cancel.gif");
            ui->label_bottom_main->setAlignment(Qt::AlignCenter);
            ui->label_bottom_main->setMovie(movie);
            movie->start();
            show_text_scroll_first((char*)"通话结束");
            break;
        }

        case NS_AUDIO_CH_STATE_G3_REQUEST: //G3 request talk
        {
            QMovie *movie = new QMovie(":/day/talk_request.gif");
            ui->label_bottom_main->setAlignment(Qt::AlignCenter);
            ui->label_bottom_main->setMovie(movie);
            movie->start();
            show_text_scroll_first((char*)"监控中心请求通话");
            break;
        }
        case NS_AUDIO_CH_STATE_CTRL_REQUEST: //device request talk
        {
            QMovie *movie = new QMovie(":/day/talk_request.gif");
            ui->label_bottom_main->setAlignment(Qt::AlignCenter);
            ui->label_bottom_main->setMovie(movie);
            movie->start();
            show_text_scroll_first((char*)"请求通话中");
            break;
        }
        case NS_AUDIO_CH_STATE_BUSY:
        {
            QMovie *movie = new QMovie(":/day/talk_cancel.gif");
            ui->label_bottom_main->setAlignment(Qt::AlignCenter);
            ui->label_bottom_main->setMovie(movie);
            movie->start();
            show_text_scroll_first((char*)"线路忙，请稍候");
            break;
        }
        case NS_AUDIO_CH_STATE_CONNECT: //talking
        {
            QMovie *movie = new QMovie(":/day/talk_connect.gif");
            ui->label_bottom_main->setAlignment(Qt::AlignCenter);
            ui->label_bottom_main->setMovie(movie);
            movie->start();
            show_text_scroll_first((char*)"正在通话中");
            break;
        }
        case NS_AUDIO_CH_STATE_CTRL_HANGUP: //hangup
        {
            QMovie *movie = new QMovie(":/day/talk_cancel.gif");
            ui->label_bottom_main->setAlignment(Qt::AlignCenter);
            ui->label_bottom_main->setMovie(movie);
            movie->start();
            show_text_scroll_first((char*)"对方挂断对讲");
            break;
        }
        case NS_AUDIO_CH_STATE_CTRL_CANCEL: //cancel
        {
            QMovie *movie = new QMovie(":/day/talk_cancel.gif");
            ui->label_bottom_main->setAlignment(Qt::AlignCenter);
            ui->label_bottom_main->setMovie(movie);
            movie->start();
            show_text_scroll_first((char*)"对方取消对讲");
            break;
        }
        case NS_AUDIO_CH_STATE_CTRL_OFFLINE:
        {
            QMovie *movie = new QMovie(":/day/talk_cancel.gif");
            ui->label_bottom_main->setAlignment(Qt::AlignCenter);
            ui->label_bottom_main->setMovie(movie);
            movie->start();
            show_text_scroll_first((char*)"对讲离线");
            break;
        }
        default:
            break;
    }
    if((state==NS_AUDIO_CH_STATE_OVER)||(state==NS_AUDIO_CH_STATE_CTRL_CANCEL)||(state==NS_AUDIO_CH_STATE_CTRL_HANGUP)||
       (state==NS_AUDIO_CH_STATE_BUSY))
    {
        talk_status_timer->setSingleShot(true);
        talk_status_timer->start(2000);
    }
}

#ifdef USE_UNIX_COMM
void MainWindow::receive_unix_data()
{
    int recv_len, address_len;
    struct sockaddr_un address;
    char unix_recvbuf[1024] = {0};
    char *line_data = NULL;
   // struct lcd_disp_str_s *data_ptr = NULL;
    struct lcd_disp_s *data_ptr = NULL;
    //struct cpr_result *cpr_result_ptr = NULL;
    struct cmd_inside_head *cmd_recv = (struct cmd_inside_head *)unix_recvbuf;
    struct qr_code_s *qr_code = NULL;
    struct lcd_site_left_s *site;

    address_len = sizeof(struct sockaddr_un);
    bzero(&address, sizeof(address));
    recv_len = recvfrom(unix_fd, unix_recvbuf, sizeof(unix_recvbuf), 0,
                        (struct sockaddr *)&address, (socklen_t *)&address_len);

	if(cmd_recv->cmd_fst>0)
	{
        qDebug() << "recv unix cmd:" << cmd_recv->cmd_fst;
        qDebug() << "source_id:" << cmd_recv->source_id;
	}

    if(recv_len > 0)
    {
        if(cmd_recv->desc_id == LCD_MODULE)
        {
            switch(cmd_recv->cmd_fst)
            {
                case IN_CMD_NO_DOWNLOAD_FIX_INFO:
                {
                    set_before_init_device_prompt(QString("设备未下载固定信息"));
                    break;
                }

                case IN_CMD_NO_DOWNLOAD_PARA:
                {
                    set_before_init_device_prompt(QString("设备未下载参数"));
                    break;
                }

                case IN_CMD_NO_INIT:
                {
                    line_data = (char *)(unix_recvbuf + sizeof(struct cmd_inside_head));
                    JSST_CHECK_PTR(line_data);
                    if(line_data == NULL)
                    {
                        break;
                    }

                    set_init_ip_prompt(line_data);
                    break;
                }

                case IN_CMD_UNIVERSE_STATE://psam
                {
                    set_before_init_device_prompt(QString("控制器安全认证失败"));
                    break;
                }

                case IN_CMD_AUDIOCHANNEL_STATUS://对讲状态
                {
                    JSST_CHECK_PTR(unix_recvbuf + sizeof(struct cmd_inside_head));
                    private_talk_status = *(unix_recvbuf + sizeof(struct cmd_inside_head));
                    talk_status_switch();
                    break;
                }

                case IN_CMD_IPCLINK_STATUS:
                {                 
                    break;
                }

                case IN_CMD_DECODE_STATUS:
                {
                    set_before_init_device_prompt(QString("识别自学习完成，请重启设备"));
                    break;
                }

                case IN_CMD_CAR_NUMBER:
                {                 
                    break;
                }

                case IN_CMD_CANCEL_QR_CODE_INFO:
                {
                    set_qr_code_default();
                    break;
                }
                case IN_CMD_QT_LEFT_SITE:
                {
                    site = (struct lcd_site_left_s *)(unix_recvbuf + sizeof(struct cmd_inside_head));
                    qDebug()<<site->ioType<<"  "<<site->left<<"  "<<site->web_disable_left_flag;
                    JSST_CHECK_PTR(site);
                    memcpy(&site_left,site,sizeof(struct lcd_site_left_s));
                    qDebug()<<"site_left"<<site_left.ioType<<"  "<<site_left.left<<"  "<<site_left.web_disable_left_flag;
                    //set_main_prompt_content_free();
                    break;
                }
                case IN_CMD_QT_AD_PIC_FILE_UPDATE:
                {
                    QString dir_path="/opt/data/ad/pic";
                    filter_pic_display(dir_path);
                    break;
                }
                case IN_CMD_RECV_DATA:
                {
                    data_ptr = (struct lcd_disp_s *)(unix_recvbuf + sizeof(struct cmd_inside_head));
                    JSST_CHECK_PTR(data_ptr);
                    renew_text_info(data_ptr);
                    break;
                }

                case IN_CMD_QR_CODE_INFO://二维码信息
                {
                	#if 0
                    qDebug() << "private_talk_status:" <<private_talk_status;
                    if(private_talk_status!=NS_AUDIO_CH_STATE_CTRL_FREE)
                    {
                        break;
                    }
					#endif
                    qr_code = (struct qr_code_s *)(unix_recvbuf + sizeof(struct cmd_inside_head));
                    qDebug() << "qr_pic:" <<qr_code->qr_code_pic<<"qr_content:"<<qr_code->qr_code_content ;
                    set_qr_code_image(qr_code->qr_code_pic,qr_code->qr_code_content);
                    break;
                }
                default:
                {
                    qDebug() << "cmd = " << cmd_recv->cmd_fst;
                    break;
                }
            }
        }
    }
}
#endif

