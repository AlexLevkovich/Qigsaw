#ifndef _RECTCUTTER_H_
#define _RECTCUTTER_H_

#include "JigsawCutter.h"

class RectCutter: public JigsawCutter {
public:
    RectCutter();
    RectCutter(int rows, int columns);

    QString toString() const;
    QString getName() const;
    QString getDescription() const;
    void writeExternal(QDataStream * out);
    void readExternal(QDataStream * in);
    const QList<Piece *> cut(const QImage & image);
    virtual void setPreferredPieceCount(int count);

    static const int defaultEdge = 32;
    static const int minimumEdge = 2;

private:
    int rows,columns;

    void init(int rows, int columns);
    QVector<QRgb> getImageData(const QImage & image, int x, int y,int width, int height) const;
};

#endif // _RECTCUTTER_H_
