#include "QuadCutter.h"
#include "exceptions.h"
#include "JigUtil.h"
#include "Piece.h"
#include "TDArray.h"

#include <math.h>
#include <list>
using namespace std;

const int QuadCutter::defPrefPieces = 150;
const double QuadCutter::defWHRatio = 1.2;

QString QuadCutter::toString() const { return getName(); }
QString QuadCutter::getName() const { return QObject::tr("Quads"); }

QString QuadCutter::getDescription() const {
    return QObject::tr("Cuts the image into random quadrilaterals.");
}

void QuadCutter::init(int prefPieces, double whRatio) {
    setPreferredPieceCount (prefPieces);
    if (whRatio <= 0.0) throw IllegalArgumentException(QString("Invalid width/height ratio: %1").arg(whRatio));
    widthToHeightRatio = whRatio;
}

QuadCutter::QuadCutter() {
    init(defPrefPieces, defWHRatio);
}

QuadCutter::QuadCutter(int prefPieces) {
    init(prefPieces, defWHRatio);
}

QuadCutter::QuadCutter(int prefPieces, double whRatio) {
    init(prefPieces, whRatio);
}

void QuadCutter::setPreferredPieceCount(int count) {
    if (count < 2) throw IllegalArgumentException(QString("Invalid preferred pieces: %1").arg(count));
    prefPieces = count;
}

void QuadCutter::writeExternal(QDataStream * out) {
    *out << prefPieces;
    *out << widthToHeightRatio;
}

void QuadCutter::readExternal(QDataStream * in) {
    *in >> prefPieces;
    *in >> widthToHeightRatio;
}

const QList<Piece *> QuadCutter::cut(const QImage & image) {
    int width = image.width();
    int height = image.height();

    int rows = _round(sqrt(widthToHeightRatio * prefPieces * height / width));
    int columns = _round(prefPieces / rows);

    int hVary = height / (rows * 10);
    int wVary = width / (columns * 10);

    TDArray<QPoint> points(columns+1,rows+1);
    // i varies horizontally; j varies vertically
    for (int j = 0; j <= rows; j++) {
      int baseY = j*height / rows;
      for (int i = 0; i <= columns; i++) {
        int baseX = i*width / columns;
        int x = baseX;
        int y = baseY;
        if ((i>0) && (i < columns))
          x += _rand()*(2*wVary+1) - wVary;
        if ((j>0) && (j < rows))
          y += _rand()*(2*hVary+1) - hVary;
        points[i][j] = QPoint(x,y);
      }
    }

    // Create the pieces.
    TDArray<Piece *> pieces(columns,rows);
    for (int j = 0; j < rows; j++)
      for (int i = 0; i < columns; i++) {
        pieces[i][j] = makePiece(image,
                                 points[i][j],
                                 points[i][j+1],
                                 points[i+1][j],
                                 points[i+1][j+1],
                                 width, height);
        if ((i==0) && (j>0) && (j<(rows-1))) pieces[i][j]->setFrontier(Piece::LEFT_FRONT);
        if ((i == (columns-1)) && (j>0) && (j<(rows-1))) pieces[i][j]->setFrontier(Piece::RIGHT_FRONT);
        if ((j == 0) && (i>0) && (i<(columns-1))) pieces[i][j]->setFrontier(Piece::TOP_FRONT);
        if ((j == (rows-1)) && (i>0) && (i<(columns-1))) pieces[i][j]->setFrontier(Piece::BOTTOM_FRONT);
        if ((i==0) && (j==0)) pieces[i][j]->setFrontier(Piece::LEFT_TOP_FRONT);
        if ((i == (columns-1)) && (j==0)) pieces[i][j]->setFrontier(Piece::RIGHT_TOP_FRONT);
        if ((j==(rows-1)) && (i == 0)) pieces[i][j]->setFrontier(Piece::LEFT_BOTTOM_FRONT);
        if ((j == (rows-1)) && (i == (columns-1))) pieces[i][j]->setFrontier(Piece::RIGHT_BOTTOM_FRONT);
      }

    // Set each piece's neighbors, and build the final array.
    QList<Piece *> ret = QList<Piece *>::fromStdList(list<Piece *>(rows*columns,(Piece *)NULL));
    for (int j = 0; j < rows; j++)
      for (int i = 0; i < columns; i++) {
        if (i>0) pieces[i][j]->addNeighbor(pieces[i-1][j]);
        if (j>0) pieces[i][j]->addNeighbor(pieces[i][j-1]);
        if (i<columns-1) pieces[i][j]->addNeighbor(pieces[i+1][j]);
        if (j<rows-1) pieces[i][j]->addNeighbor(pieces[i][j+1]);
        ret[j*columns+i] = pieces[i][j];
      }

    return ret;
}

Piece * QuadCutter::makePiece(const QImage & image,QPoint & nw,QPoint & sw,QPoint & ne,QPoint & se,int tWidth,int tHeight) {
    int minX = min(nw.x(), sw.x());
    int maxX = max(ne.x(), se.x());
    int minY = min(nw.y(), ne.y());
    int maxY = max(sw.y(), se.y());
    int width  = maxX - minX + 1;
    int height = maxY - minY + 1;

    QVector<QRgb> data = JigUtil::image_data(image.copy(minX,minY,width,height));

    // Mask out anything outside the lines.
    maskOutside (data, nw, ne, minX, minY, width, height);
    maskOutside (data, ne, se, minX, minY, width, height);
    maskOutside (data, se, sw, minX, minY, width, height);
    maskOutside (data, sw, nw, minX, minY, width, height);

    int rotation = ((int)(_rand()*4)) * 90;
    return new Piece(data, minX, minY, width, height, tWidth, tHeight, rotation);
}

void QuadCutter::maskOutside(QVector<QRgb> & data,QPoint & p1,QPoint & p2,int minX,int minY,int width,int height) {
    p1-=QPoint(minX,minY);
    p2-=QPoint(minX,minY);
    int mN = p2.y() - p1.y();
    int mD = p2.x() - p1.x();
    int bN = mD * p1.y() - mN * p1.x();

    for (int j = 0; j < height; j++) {
      int y = j;
      for (int i = 0; i < width; i++) {
        int x = i;
        if (y*mD < mN*x + bN)
          data[j*width+i] = 0;
      }
    }
    p1+=QPoint(minX,minY);
    p2+=QPoint(minX,minY);
}
