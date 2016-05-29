#ifndef _QUADCUTTER_H_
#define _QUADCUTTER_H_

#include "JigsawCutter.h"

class Piece;

class QuadCutter: public JigsawCutter {
public:
    QuadCutter();
    QuadCutter(int prefPieces);
    QuadCutter(int prefPieces, double whRatio);

    QString toString() const;
    QString getName() const;
    QString getDescription() const;
    void setPreferredPieceCount(int count);
    void writeExternal(QDataStream * out);
    void readExternal(QDataStream * in);
    const QList<Piece *> cut(const QImage & image);

    static const int defPrefPieces;
    static const double defWHRatio;
private:
    void init(int prefPieces, double whRatio);
    Piece * makePiece(const QImage & image,QPoint & nw,QPoint & sw,QPoint & ne,QPoint & se,int tWidth,int tHeight);
    void maskOutside(QVector<QRgb> & data,QPoint & p1,QPoint & p2,int minX,int minY,int width,int height);

    int prefPieces;
    double widthToHeightRatio;
};

#endif // _QUADCUTTER_H_
