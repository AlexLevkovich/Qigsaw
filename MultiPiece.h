#ifndef _MULTIPIECE_H_
#define _MULTIPIECE_H_

#include "Piece.h"

class MultiPiece: public Piece {
    friend class Piece;
public:
  MultiPiece();
  void setData(const QList<Piece *> & subpieces,
               int imageX, int imageY,
               int imageWidth, int imageHeight,
               int totalWidth, int totalHeight,
               int rotation);

  virtual void writeExternal(QDataStream * out);
  virtual void readExternal(QDataStream * in);

  QList<Piece *> subpieces() const;
  virtual void setRotation(int rot);
  virtual QString toString() const;
  virtual PieceId instanceOf();
  virtual bool contains(int x, int y);
  virtual int getFrontier();
protected:
  virtual void setImageData(const QVector<QRgb> & data, int width, int height);
  void setImageSize(int width, int height);
  static Piece * join(Piece * main,const QList<Piece *> & others);
  static void overlay(QVector<QRgb> & data, int dataX, int dataY,
                      int width, int height, Piece * piece);
  virtual void recomputeImageData();
private:
  QList<Piece *> subs;

  static void addSubs(QList<Piece *> & subset, Piece * piece);
};

#endif // _MULTIPIECE_H_
