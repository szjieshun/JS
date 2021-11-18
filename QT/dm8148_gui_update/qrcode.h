#ifndef QRCODE_H
#define QRCODE_H

#include <QImage>
#include <QPainter>

class Qrcode
{
public:
    Qrcode();
    bool saveImage(QString filename, int size, QString content);
    bool saveImageBlackGround(QString filename, int size, QString content);
private:
    void setString(QString str);
    void draw(QPainter &painter, int width, int height);
    QString string1;
    //QRcode  *qr;
};

#endif // QRCODE_H
