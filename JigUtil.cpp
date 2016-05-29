#include "JigUtil.h"

#include <QPainter>

int JigUtil::darker (int val) {
    int r = (val >> 16) & 0xff;
    int g = (val >>  8) & 0xff;
    int b = (val      ) & 0xff;
    r = r * fd / fn;
    g = g * fd / fn;
    b = b * fd / fn;
    return ((((0xff00 | r) << 8) | g) << 8) | b;
}

int JigUtil::brighter(int val) {
    int r = (val >> 16) & 0xff;
    int g = (val >>  8) & 0xff;
    int b = (val      ) & 0xff;

    // Black goes to #030303 gray
    if ((r==0) && (g==0) && (b==0)) return 0xff030303;
    r = (r < 3) ? 3 : r;
    g = (g < 3) ? 3 : g;
    b = (b < 3) ? 3 : b;

    r = (r >= maxB) ? 255 : (r * fn / fd);
    g = (g >= maxB) ? 255 : (g * fn / fd);
    b = (b >= maxB) ? 255 : (b * fn / fd);
    return ((((0xff00 | r) << 8) | g) << 8) | b;
}

bool JigUtil::isTransparent(int rgb) {
    return ((rgb & 0xff000000) == 0);
}

int JigUtil::getMax(int * arr,int count) {
    int max = 0;
    int id = 0;

    for (int i=0;i<count;i++) {
        if (arr[i] > max) {
            max = arr[i];
            id = i;
        }
    }

    return id;
}

int JigUtil::invertColor(int rgb) {
    int a = rgb & 0xff000000;

    if (a == 0) return rgb;

    int r = (rgb >> 16) & 0xff;
    int g = (rgb >> 8) & 0xff;
    int b = rgb & 0xff;
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
    return a | (r << 16) | (g << 8) | b;
}

const QVector<QRgb> JigUtil::image_data(const QImage & image) {
    int width = image.width();
    int height = image.height();
    QVector<QRgb> ret(width*height);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            ret[j*width+i] = image.pixel(i,j);
        }
    }
    return ret;
}

void JigUtil::set_image_data(QImage & image,const QVector<QRgb> & data,int width) {
    int height = data.count()/width;
    image = QImage(width,height,QImage::Format_ARGB32);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            image.setPixel(i,j,data[j*width+i]);
        }
    }
}

const QVector<QRgb> JigUtil::image_data(const QImage & image,const QPainterPath & path,int minX,int minY) {
    int width = image.width();
    int height = image.height();

    QMatrix matrix;
    matrix.translate(-minX,-minY);
    QPainterPath tempPath = matrix.map(path);
    QImage tempImage(width,height,QImage::Format_ARGB32);
    tempImage.fill(0);
    QPainter painter(&tempImage);
    painter.setBrush(QBrush(image));
    painter.setPen(QColor(Qt::transparent));
    painter.drawPath(tempPath);

    QVector<QRgb> ret(width*height);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            ret[j*width+i] = tempImage.pixel(i,j);
        }
    }

    return ret;
}

int JigUtil::grayFilter(int rgb) {
    int a = rgb & 0xff000000;
    int r = (rgb >> 16) & 0xff;
    int g = (rgb >> 8) & 0xff;
    int b = rgb & 0xff;
    r = (r+255)/2;
    g = (g+255)/2;
    b = (b+255)/2;
    return a | (r << 16) | (g << 8) | b;
}

const QImage JigUtil::create_hint_image(const QImage & image) {
    int width = image.width();
    int height = image.height();
    QVector<QRgb> data(width*height);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            data[j*width+i] = grayFilter(image.pixel(i,j));
        }
    }

    QImage retImage;
    set_image_data(retImage,data,width);
    return retImage;
}
