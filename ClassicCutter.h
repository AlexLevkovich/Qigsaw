#ifndef _CLASSICCUTTER_H_
#define _CLASSICCUTTER_H_

#include "JigsawCutter.h"

class Knob;

class ClassicCutter: public JigsawCutter {
public:

    QString toString() const;
    QString getName() const;
    QString getDescription() const;

    ClassicCutter();
    ClassicCutter(int prefPieces);
    void setPreferredPieceCount(int count);
    void writeExternal(QDataStream * out);
    void readExternal(QDataStream * in);
    const QList<Piece *> cut(const QImage & image);

    static const int defPrefPieces;
    static const double widthToHeightRatio;

private:
    int prefPieces;

    Piece * makePiece(const QImage & image,
                      const QPoint & nw,const QPoint & sw,
                      const QPoint & ne,const QPoint & se,
                      Knob * knobN, Knob * knobE, Knob * knobS,
                      Knob * knobW,int tWidth, int tHeight);
};

#endif // _CLASSICCUTTER_H_
