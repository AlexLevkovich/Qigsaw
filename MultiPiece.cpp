#include "MultiPiece.h"
#include "exceptions.h"
#include "JigUtil.h"
#include <QMessageBox>

MultiPiece::MultiPiece(): Piece() {}

void MultiPiece::setData(const QList<Piece *> & subpieces,
                    int imageX, int imageY,
                    int imageWidth, int imageHeight,
                    int totalWidth, int totalHeight,
                    int rotation) {
    subs = subpieces;
    init(QVector<QRgb>(),imageX,imageY,imageWidth,imageHeight,totalWidth,totalHeight);
    forceSetRotation(rotation);
}

void MultiPiece::writeExternal(QDataStream * out) {
    Piece::writeExternal(out);
    *out<<subs.count();
    for (int i=0;i<subs.count();i++) {
        *out<<Piece::all_puzzles.indexOf(subs.at(i));
    }
}

void MultiPiece::readExternal(QDataStream * in) {
    *in>>imageX;
    *in>>imageY;
    *in>>totalWidth;
    *in>>totalHeight;
    *in>>rotatedX;
    *in>>rotatedY;
    *in>>origWidth;
    *in>>origHeight;
    *in>>curWidth;
    *in>>curHeight;
    *in>>frontier;
    *in>>origData;
    *in>>puzzleX;
    *in>>puzzleY;
    *in>>rotation;
    int count;
    *in>>count;
    for (int i=0;i<count;i++) {
        int id;
        *in>>id;
        if (id >= 0) neighbors.append(Piece::all_puzzles.at(id));
    }

    *in>>count;
    for (int i=0;i<count;i++) {
        int id;
        *in>>id;
        subs.append(Piece::all_puzzles.at(id));
    }

    forceSetRotation(rotation);
}

QList<Piece *> MultiPiece::subpieces() const {
    return subs;
}

void MultiPiece::setImageData(const QVector<QRgb> & data, int width, int height) {
    if (data.count() > 0) throw IllegalArgumentException("cannot set image data on a MultiPiece");
    setImageSize(width,height);
}

void MultiPiece::setImageSize(int width, int height) {
    curWidth  = origWidth  = width;
    curHeight = origHeight = height;
}

void MultiPiece::setRotation(int rot) {
    for (int i=0;i<subs.count();i++) {
        subs.at(i)->setRotation(rot);
    }
    Piece::setRotation(rot);
}

QString MultiPiece::toString() const {
    return "Multi"+Piece::toString()+"[pieces="+QString::number(subs.count())+"]";
}

Piece * MultiPiece::join(Piece * main,const QList<Piece *> & others) {
    QList<Piece *> neighbors;

    neighbors += main->neighbors;
    int mainPX = main->getPuzzleX();
    int mainPY = main->getPuzzleY();

    int minX = main->getImageX();
    int minY = main->getImageY();
    int maxX = minX + main->getImageWidth()  - 1;
    int maxY = minY + main->getImageHeight() - 1;
    for (int i = 0; i < others.count(); i++) {
      Piece * piece_obj = others.at(i);

      int minXT = piece_obj->getImageX();
      int minYT = piece_obj->getImageY();
      int maxXT = minXT + piece_obj->getImageWidth()  - 1;
      int maxYT = minYT + piece_obj->getImageHeight() - 1;
      minX = min(minX, minXT);
      minY = min(minY, minYT);
      maxX = max(maxX, maxXT);
      maxY = max(maxY, maxYT);
      for (int j=0;j<piece_obj->neighbors.count();j++) {
        Piece * neighbor = piece_obj->neighbors.at(j);
        if (!neighbors.contains(neighbor)) neighbors.append(neighbor);
      }
    }
    int width  = maxX-minX+1;
    int height = maxY-minY+1;

    int index = neighbors.indexOf(main);
    if (index >= 0) neighbors.removeAt(index);
    for (int i = 0; i < others.count();i++) {
        index = neighbors.indexOf(others.at(i));
        if (index >= 0) neighbors.removeAt(index);
    }

    QList<Piece *> subs;
    addSubs(subs,main);
    for (int i = 0;i < others.count();i++) {
      addSubs(subs,others.at(i));
    }

    MultiPiece * newPiece_obj = new MultiPiece();
    newPiece_obj->setData(subs,
                          minX, minY,    // image position
                          width, height, // image size
                          main->getTotalWidth(),
                          main->getTotalHeight(),
                          main->getRotation());

    int dx = newPiece_obj->getRotatedX() - main->getRotatedX();
    int dy = newPiece_obj->getRotatedY() - main->getRotatedY();
    newPiece_obj->setPuzzlePosition(mainPX+dx, mainPY+dy);

    for (int j=0;j<neighbors.count();j++) {
      Piece * piece_obj = neighbors.at(j);

      newPiece_obj->addNeighbor(piece_obj);
      piece_obj->removeNeighbor(main);
      for (int i = 0;i < others.count();i++) {
        piece_obj->removeNeighbor(others.at(i));
      }
      piece_obj->addNeighbor(newPiece_obj);
    }

    return newPiece_obj;
}

void MultiPiece::addSubs(QList<Piece *> & subset, Piece * piece) {
    if (piece->instanceOf() == MPiece) {
        subset += ((MultiPiece *)piece)->subs;
        delete piece;
    }
    else {
        subset.append(piece);
        piece->flush();
    }
}

void MultiPiece::overlay(QVector<QRgb> & data, int dataX, int dataY,
                         int width, int height, Piece * piece) {
    int pieceX = piece->getRotatedX();
    int pieceY = piece->getRotatedY();
    int pieceW = piece->getCurrentWidth();
    int pieceH = piece->getCurrentHeight();

    QVector<QRgb> newData = piece->curData;

    int offset = (pieceY-dataY) * width + (pieceX-dataX);
    for (int i = 0; i < pieceH; i++) {
      int iNDOffset = i*pieceW;
      int iDOffset  = i*width;
      for (int j = 0; j < pieceW; j++) {
        int ndOff = iNDOffset+j;
        int newDatum = newData[ndOff];
        if (newDatum != 0) data[ offset + iDOffset + j ] = newDatum;
      }
    }
}

PieceId MultiPiece::instanceOf() {
    return MPiece;
}

void MultiPiece::recomputeImageData() {
    setRotatedPosition();

    QVector<QRgb> data(curWidth*curHeight);
    int rotX = getRotatedX();
    int rotY = getRotatedY();
    for (int i=0;i<subs.count();i++) {
      Piece * sub = subs.at(i);
      overlay(data, rotX, rotY, curWidth, curHeight, sub);
    }

    JigUtil::set_image_data(image,data,curWidth);
    flashImage = image;
    flashImage.invertPixels();
}

bool MultiPiece::contains(int x, int y) {
     int puzX = getPuzzleX();
     int puzY = getPuzzleY();
     int w = getCurrentWidth();
     int h = getCurrentHeight();
     return (puzX <= x) && (x <= (puzX+w-1)) &&
            (puzY <= y) && (y <= (puzY+h-1)) &&
            !JigUtil::isTransparent(image.pixel(x-puzX,y-puzY));
}

int MultiPiece::getFrontier() {
      int front = -1;

      for(int i=0;i<subs.count();i++) {
          Piece * pieceObj = subs.at(i);
          if (front == -1 && pieceObj->getFrontier() != -1) {
              front = pieceObj->getFrontier();
          }
          else {
              if (pieceObj->getFrontier() == -1) continue;
              front = front | pieceObj->getFrontier();
          }
      }
      return front;
}


