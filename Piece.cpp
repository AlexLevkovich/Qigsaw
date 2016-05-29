#include <stdio.h>
#include "Piece.h"
#include "MultiPiece.h"
#include "exceptions.h"
#include "JigUtil.h"
#include "IGap.h"

#include <QPainter>
#include <QPoint>
#include <QThread>
#include <QMessageBox>

QList<Piece *> Piece::all_puzzles;

const int Piece::LEFT_FRONT = 1;
const int Piece::RIGHT_FRONT = 2;
const int Piece::TOP_FRONT = 4;
const int Piece::BOTTOM_FRONT = 8;
const int Piece::LEFT_TOP_FRONT = 16;
const int Piece::RIGHT_TOP_FRONT = 32;
const int Piece::LEFT_BOTTOM_FRONT = 64;
const int Piece::RIGHT_BOTTOM_FRONT = 128;

const int Piece::posClose = 7;
const int Piece::pieceCenterClose = 14;

QDataStream & operator >> (QDataStream & in,Piece * & t) {
    quint64 t_ulong;
    in >> t_ulong;
    t = (Piece *)t_ulong;
    return in;
}

Piece::Piece() {
    init();
}

void Piece::deleteAllPieces() {
    for (int i=(Piece::all_puzzles.count()-1);i>=0;i--) {
        delete Piece::all_puzzles.at(i);
    }
    Piece::all_puzzles.clear();
}

int Piece::countsPieces() {
    int count = 0;
    for (int i=0;i<all_puzzles.count();i++) {
        if (Piece::all_puzzles.at(i)->instanceOf() != MPiece) count++;
    }
    return count;
}

Piece::~Piece() {
    Piece::all_puzzles.removeAll(this);
}

void Piece::init(const QVector<QRgb> & data,
            int imageX, int imageY,
            int imageWidth, int imageHeight,
            int totalWidth, int totalHeight) {
    setImageData (data, imageWidth, imageHeight);
    setImagePosition (imageX, imageY);
    this->totalWidth = totalWidth;
    this->totalHeight = totalHeight;
    Piece::all_puzzles.append(this);
    init();
}

Piece::Piece(const QVector<QRgb> & data,
             int imageX, int imageY,
             int imageWidth, int imageHeight,
             int totalWidth, int totalHeight,
             int rotation) {
    init(data,imageX,imageY,imageWidth,imageHeight,totalWidth,totalHeight);
    forceSetRotation(rotation);
}

void Piece::init() {
    frontier = -1;
    flashMode = false;
}

void Piece::setFlashMode(bool flashMode) {
    this->flashMode = flashMode;
}

bool Piece::isFlashMode() {
    return flashMode;
}

bool Piece::isFrontierSet(int front) {
    int curr_front = getFrontier();
    if (curr_front == -1) return false;
    return ((curr_front & front) == front);
}

int Piece::getFrontier() {
    return frontier;
}

void Piece::setFrontier(int frontier) {
    this->frontier = frontier;
}

void Piece::writeExternal(QDataStream * out) {
    *out<<imageX;
    *out<<imageY;
    *out<<totalWidth;
    *out<<totalHeight;
    *out<<rotatedX;
    *out<<rotatedY;
    *out<<origWidth;
    *out<<origHeight;
    *out<<curWidth;
    *out<<curHeight;
    *out<<frontier;
    *out<<origData;
    *out<<puzzleX;
    *out<<puzzleY;
    *out<<rotation;
    *out<<neighbors.count();
    for (int i=0;i<neighbors.count();i++) {
        *out<<Piece::all_puzzles.indexOf(neighbors.at(i));
    }
}

void Piece::readExternal(QDataStream * in) {
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

    if (origData.count() > 0) forceSetRotation(rotation);
}

void Piece::setImagePosition(int x, int y) {
    imageX = x;
    imageY = y;
}

int Piece::getRotation() { return rotation; }

void Piece::setRotation(int rot) {
    if (rot == rotation) return;
    forceSetRotation(rot);
}

void Piece::forceSetRotation() {
    forceSetRotation(rotation);
}

void Piece::forceSetRotation(int rot) {
    if ((rot < 0) || (rot > 359)) throw IllegalArgumentException(QString("invalid rotation: %1").arg(rot));
    // For now, allow only 0, 90, 180, 270.
    if (rot % 90 != 0) {
      rot = (rot / 90)*90;
    }
    rotation = rot;
    recomputeImageData();
}

void Piece::setPuzzlePosition (int x, int y) {
    puzzleX = x;
    puzzleY = y;
}

int Piece::getCurrentHeight() { return curHeight; }
int Piece::getCurrentWidth() { return curWidth; }
int Piece::getTotalWidth() { return totalWidth; }
int Piece::getTotalHeight() { return totalHeight; }
int Piece::getImageHeight() { return origHeight; }
int Piece::getImageWidth() { return origWidth; }
int Piece::getImageX() { return imageX; }
int Piece::getImageY() { return imageY; }
int Piece::getRotatedX() { return rotatedX; }
int Piece::getRotatedY() { return rotatedY; }
int Piece::getRotatedX(int) { return rotatedX; }
int Piece::getRotatedY(int) { return rotatedY; }
int Piece::getPuzzleX() { return puzzleX; }
int Piece::getPuzzleY() { return puzzleY; }

QRect Piece::getPuzzleRect() const {
    Piece * p_this = (Piece *)this;
    return QRect(p_this->getPuzzleX(),p_this->getPuzzleY(),p_this->getCurrentWidth(),p_this->getCurrentHeight());
}

QImage Piece::getImage() const { return image; }

void Piece::addNeighbor(Piece * neighbor) {
    if (neighbor == NULL) throw NullPointerException("null neighbor");
    if (!neighbors.contains(neighbor)) neighbors.append(neighbor);
}

void Piece::removeNeighbor(Piece * neighbor) {
    if (neighbor == NULL) return;
    int index = neighbors.indexOf(neighbor);
    if (index >= 0) neighbors.removeAt(index);
}

void Piece::moveTo(int x, int y) { setPuzzlePosition(x, y); }

QString Piece::toString() const {
    return "Piece[iPos=("+QString::number(imageX)+","+QString::number(imageY)+"),"
          +"iSiz="+QString::number(origWidth)+"x"+QString::number(origHeight)+","
          +"rot="+QString::number(rotation)+","
          +"rPos=("+QString::number(rotatedX)+","+QString::number(rotatedY)+"),"
          +"pPos=("+QString::number(puzzleX)+","+QString::number(puzzleY)+")]";
}

void Piece::draw(QPainter * g) {
    if (flashMode) {
        if (!flashImage.isNull()) g->drawImage(QPoint(getPuzzleX(), getPuzzleY()), flashImage);
    }
    else {
        if (!image.isNull()) g->drawImage(QPoint(getPuzzleX(), getPuzzleY()), image);
    }
}

void Piece::setImageData(const QVector<QRgb> & data, int width, int height) {
    curWidth  = origWidth  = width;
    curHeight = origHeight = height;
    origData = data;
}

void Piece::recomputeImageData() {
    setRotatedPosition();
    if (rotation == 0) {
        curData = origData;
    }
    else if (rotation == 90) {
      curData = QVector<QRgb>(origData.count());
      for (int i = 0; i < curWidth; i++)
        for (int j = 0; j < curHeight; j++)
            curData[j*curWidth+i] = origData[(origHeight-i-1)*origWidth + j];
    }
    else if (rotation == 180) {
      curData = QVector<QRgb>(origData.count());
      for (int i = 0; i < curWidth; i++)
        for (int j = 0; j < curHeight; j++)
          curData[j*curWidth+i] =
            origData[(origHeight-j-1)*origWidth + (origWidth-i-1)];
    }
    else if (rotation == 270) {
      curData = QVector<QRgb>(origData.count());
      for (int i = 0; i < curWidth; i++)
        for (int j = 0; j < curHeight; j++)
          curData[j*curWidth+i] = origData[i*origWidth + (origWidth-j-1)];
    }
    bevel(curData, curWidth, curHeight);
    JigUtil::set_image_data(image,curData,curWidth);
    flashImage = image;
    flashImage.invertPixels();
}

void Piece::setRotatedPosition() {
    int rot = getRotation();
    switch (rot) {
      case   0: rotatedX =  imageX; rotatedY =  imageY;
        curWidth = origWidth; curHeight = origHeight; break;
      case  90:
        rotatedX = totalHeight-imageY-origHeight;
        rotatedY = imageX;
        curWidth = origHeight; curHeight = origWidth; break;
      case 180:
        rotatedX = totalWidth -imageX-origWidth;
        rotatedY = totalHeight-imageY-origHeight;
        curWidth = origWidth; curHeight = origHeight; break;
      case 270:
        rotatedX =  imageY;
        rotatedY = totalWidth -imageX-origWidth;
        curWidth = origHeight; curHeight = origWidth; break;
      default:
        fprintf(stderr,"sRotPos() can't handle rotation: %d",rot);
    }
}

void Piece::bevel(QVector<QRgb> & data, int width, int height) {
    // Scan diagonal NW-SE lines.  The first and last lines can be skipped.
    for (int i = 0; i < width+height-3; i++) {
      int x = max(0, i-height+2);
      int y = max(0, height-i-2);
      bool nw, c, se; // true iff that pixel is opaque
      nw = c = se = false;
      c = (((data[y*width+x] >> 24) & 0xff) > 0);
      while ((x < width) && (y < height)) {
        if ((x+1 < width) && (y+1 < height))
          se = (((data[(y+1)*width+(x+1)] >> 24) & 0xff) > 0);
        else se = false;
        if (c) {
          int datum = data[y*width+x];
          if ( nw && !se) data[y*width+x] = JigUtil::darker(datum);
          if (!nw &&  se) data[y*width+x] = JigUtil::brighter(datum);
        }
        nw = c;
        c = se;
        x++; y++;
      }
    }
}

bool Piece::contains(int x, int y) {
    int puzX = getPuzzleX();
    int puzY = getPuzzleY();
    int w = getCurrentWidth();
    int h = getCurrentHeight();
    return (puzX <= x) && (x <= (puzX+w-1)) &&
           (puzY <= y) && (y <= (puzY+h-1)) &&
           (getAlpha(x-puzX, y-puzY) != 0);
  }

int Piece::getAlpha(int x, int y) {
    int pixel = curData[y*curWidth + x];
    return (pixel >> 24) & 0xff;
}

bool Piece::isCloseTo(Piece * piece) {
    if (piece == NULL) return false;

    int rot = getRotation();
    if (rot != piece->getRotation()) return false;

    int puzXD = getPuzzleX() - piece->getPuzzleX();
    int puzYD = getPuzzleY() - piece->getPuzzleY();
    int rotXD = getRotatedX() - piece->getRotatedX(rot);
    int rotYD = getRotatedY() - piece->getRotatedY(rot);
    return (abs(puzXD-rotXD) <= posClose) &&
           (abs(puzYD-rotYD) <= posClose) ;
}

QList<Piece *> Piece::join() {
    QList<Piece *> close;
    for(int i=0;i<neighbors.count();i++) {
        Piece * piece_obj = neighbors.at(i);
        if (piece_obj->isCloseTo(this)) close.append(piece_obj);
    }

    if (close.count() == 0) return QList<Piece *>();

    Piece * newPiece = MultiPiece::join(this,close);

    QList<Piece *> ret;
    ret.append(newPiece);
    ret.append(this);
    for (int i = 0; i < close.count(); i++) {
      Piece * piece = close.at(i);
      ret.append(piece);
    }
    return ret;
}

PieceId Piece::instanceOf() {
    return BPiece;
}

int Piece::gaps_compare(const void * item1,const void * item2) {
    IGap * gap1 = (IGap *)item1;
    IGap * gap2 = (IGap *)item2;
    return gap2->belong(gap1->getStart());
}

int Piece::getGapNum(IGap * gaps,int count,int color) {
    IGap gap(color & 0x00ffffff,0);
    IGap * ret = (IGap *)bsearch(&gap,gaps,count,sizeof(IGap),gaps_compare);
    if (ret == NULL) return -1;
    return (ret - gaps)/sizeof(IGap);
}

int Piece::getPieceIQ(IGap * gaps,int count) {
    int * gapsID = new int[count];
    for (int i=0;i<count;i++) {
        gapsID[i] = 0;
    }
    for (int i=0;i<origData.count();i++) {
        if (JigUtil::isTransparent(origData[i])) continue;
        gapsID[getGapNum(gaps,count,origData[i])]++;
    }
    int ret = JigUtil::getMax(gapsID,count);
    delete[] gapsID;
    return ret;
}

bool Piece::isOneOfNeighbors(Piece * piece) {
    return neighbors.contains(piece);
}

QPoint Piece::getClosedToRectNewPosition(QRect rect) const {
      Piece * p_this = (Piece *)this;

      QRect puzzleRect = getPuzzleRect();

      if (p_this->getRotation() != 0) return QPoint();
      if (!rect.intersects(puzzleRect)) return QPoint();

      if (p_this->isFrontierSet(Piece::LEFT_TOP_FRONT)) {
          if (puzzleRect.intersects(QRect(rect.x()-pieceCenterClose,rect.y()-pieceCenterClose,pieceCenterClose*2,pieceCenterClose*2))) {
              return QPoint(rect.x(),rect.y());
          }
      }
      if (p_this->isFrontierSet(Piece::LEFT_BOTTOM_FRONT)) {
          if (puzzleRect.intersects(QRect(rect.x()-pieceCenterClose,rect.y()+rect.height()-pieceCenterClose,pieceCenterClose*2,pieceCenterClose*2))) {
              return QPoint(rect.x(),rect.y()+rect.height()-puzzleRect.height());
          }
      }
      if (p_this->isFrontierSet(Piece::RIGHT_TOP_FRONT)) {
          if (puzzleRect.intersects(QRect(rect.x()+rect.width()-pieceCenterClose,rect.y()-pieceCenterClose,pieceCenterClose*2,pieceCenterClose*2))) {
              return QPoint(rect.x()+rect.width()-puzzleRect.width(),rect.y());
          }
      }
      if (p_this->isFrontierSet(Piece::RIGHT_BOTTOM_FRONT)) {
          if (puzzleRect.intersects(QRect(rect.x()+rect.width()-pieceCenterClose,rect.y()+rect.height()-pieceCenterClose,pieceCenterClose*2,pieceCenterClose*2))) {
              return QPoint(rect.x()+rect.width()-puzzleRect.width(),rect.y()+rect.height()-puzzleRect.height());
          }
      }
      if (p_this->isFrontierSet(Piece::LEFT_FRONT)) {
          if (puzzleRect.intersects(QRect(rect.x()-pieceCenterClose,rect.y(),pieceCenterClose*2,(int)rect.height()))) {
              return QPoint(rect.x(),puzzleRect.y());
          }
      }
      if (p_this->isFrontierSet(Piece::RIGHT_FRONT)) {
          if (puzzleRect.intersects(QRect(rect.x()+rect.width()-pieceCenterClose,rect.y(),pieceCenterClose*2,rect.height()))) {
              return QPoint(rect.x()+rect.width()-puzzleRect.width(),puzzleRect.y());
          }
      }
      if (p_this->isFrontierSet(Piece::TOP_FRONT)) {
          if (puzzleRect.intersects(QRect(rect.x(),rect.y()-pieceCenterClose,rect.width(),pieceCenterClose*2))) {
              return QPoint(puzzleRect.x(),rect.y());
          }
      }
      if (p_this->isFrontierSet(Piece::BOTTOM_FRONT)) {
          if (puzzleRect.intersects(QRect(rect.x(),rect.y()+rect.height()-pieceCenterClose,rect.width(),pieceCenterClose*2))) {
              return QPoint(puzzleRect.x(),rect.y()+rect.height()-puzzleRect.height());
          }
      }

      return QPoint();
}

void Piece::flush() {
    image = QImage();
    flashImage = QImage();
}
