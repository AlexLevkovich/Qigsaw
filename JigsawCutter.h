#ifndef _JIGSAWCUTTER_H_
#define _JIGSAWCUTTER_H_

#include <QImage>
#include <QList>
#include <QString>
#include <QDataStream>

class Piece;

class JigsawCutter {
public:
  virtual QString getName() const = 0;

  /* Returns a description of how this cutter will work, suitable for
   * display in a user interface.  The description may contain several
   * sentences, and is expected to be about a paragraph long.  */
  virtual QString getDescription() const = 0;

  /* Cuts the given Image into Pieces, and returns them. */

  virtual const QList<Piece *> cut(const QImage & image) = 0;

  virtual void writeExternal(QDataStream * out) = 0;
  virtual void readExternal(QDataStream * in) = 0;

  virtual void setPreferredPieceCount(int /*count*/) {}
};

#endif // _JIGSAWCUTTER_H_
