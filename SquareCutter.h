#ifndef _SQUARECUTTER_
#define _SQUARECUTTER_

#include "JigsawCutter.h"

#include <QImage>
#include <QVector>

class SquareCutter: public JigsawCutter {
public:
    virtual QString toString() const { return getName(); }
    virtual QString getName() const;
    virtual QString getDescription() const;

    SquareCutter();
    SquareCutter(int count);
    virtual void setPreferredPieceCount(int count);

    virtual void writeExternal(QDataStream * out);
    virtual void readExternal(QDataStream * in);

    static const int defaultEdge = 32;
    static const int minimumEdge = 2;
    int prefCount;

    const QList<Piece *> cut(const QImage & image);
private:
    void init(int count);
    const QVector<QRgb> getImageData(const QImage & image, int x, int y,int width, int height);
};

#endif // _SQUARECUTTER_
