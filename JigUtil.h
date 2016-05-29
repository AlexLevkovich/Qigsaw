#ifndef _JIGUTIL_H_
#define _JIGUTIL_H_

#include <QThread>
#include <QImage>
#include <QVector>
#include <QPainterPath>

#define max(a,b) ((a>b)?a:b)
#define min(a,b) ((a<b)?a:b)
#define abs(a) ((a<0)?-a:a)
#define _round(a) ((int)((double)a + 0.5))
#define _rand(a) ((double)rand()/(double)RAND_MAX)

class JThread: public QThread {
public:
    static void sleep(unsigned long usec) {
        QThread::msleep(usec);
    }
};

class JigUtil {
public:
    static const int fd =  7;
    static const int fn = 10;
    static const int maxB =  255 * fd / fn;

    static int darker(int val);
    static int brighter(int val);
    static bool isTransparent(int rgb);
    static int getMax(int * arr,int count);
    static int invertColor(int rgb);
    static const QVector<QRgb> image_data(const QImage & image);
    static const QVector<QRgb> image_data(const QImage & image,const QPainterPath & path,int minX,int minY);
    static void set_image_data(QImage & image,const QVector<QRgb> & data,int width);
    static int grayFilter (int rgb);
    static const QImage create_hint_image(const QImage & image);
};

#endif // _JIGUTIL_H_
