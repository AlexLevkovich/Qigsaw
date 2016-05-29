#ifndef CLASSIC4CUTTER_H_INCLUDED
#define CLASSIC4CUTTER_H_INCLUDED

#include "JigsawCutter.h"

class Knob;

class Classic4Cutter: public JigsawCutter {
public:
    QString toString() const;
    QString getName() const;
    QString getDescription() const;

    Classic4Cutter();
    Classic4Cutter(int prefPieces);
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

#endif // CLASSIC4CUTTER_H_INCLUDED
