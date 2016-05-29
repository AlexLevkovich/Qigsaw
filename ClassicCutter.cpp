#include "ClassicCutter.h"
#include "exceptions.h"
#include "JigUtil.h"
#include "Piece.h"
#include "Knob.h"
#include "TDArray.h"

#include <math.h>
#include <list>
using namespace std;

const int ClassicCutter::defPrefPieces = 150;
const double ClassicCutter::widthToHeightRatio = 1.0;

QString ClassicCutter::toString() const { return getName(); }
QString ClassicCutter::getName() const { return QObject::tr("Classic"); }
QString ClassicCutter::getDescription() const {
    return QObject::tr("Creates pieces where one pair of opposite sides has holes, and"
                       " the other pair of sides has knobs.  Pieces are roughly square,"
                       " not counting knobs and holes.");
}

ClassicCutter::ClassicCutter() {
    setPreferredPieceCount(defPrefPieces);
}

ClassicCutter::ClassicCutter(int prefPieces) {
    setPreferredPieceCount(prefPieces);
}

void ClassicCutter::setPreferredPieceCount(int count) {
    if (count < 2) throw IllegalArgumentException(QString("Invalid preferred pieces: ").arg(count));
    prefPieces = count;
}

void ClassicCutter::writeExternal(QDataStream * out) {
    *out << prefPieces;
}

void ClassicCutter::readExternal(QDataStream * in) {
    *in >> prefPieces;
}

const QList<Piece *> ClassicCutter::cut(const QImage & image) {
    int width = image.width();
    int height = image.height();

    int rows = _round (sqrt(widthToHeightRatio * prefPieces * height / width));
    int columns = _round (prefPieces / rows);
    int hVary = height / (rows * 20);
    int wVary = width / (columns * 20);

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

    // Make a knob for each edge.  Two matrices, one for vertical edges,
    // one for horizontal.  Remember to alternate knob directions.
    bool flip1 = true;
    TDArray<Knob> vKnobs(columns-1,rows);
    for (int j = 0; j < rows; j++) {
      bool flip = flip1;
      for (int i = 0; i < columns-1; i++) {
        QPoint p1 = points[i+1][j];
        QPoint p2 = points[i+1][j+1];
        if (flip) { QPoint temp = p1; p1=p2; p2=temp; }
        vKnobs[i][j] = Knob(p1.x(), p1.y(), p2.x(), p2.y());
        flip = !flip;
      }
      flip1 = !flip1;
    }

    flip1 = true;
    TDArray<Knob> hKnobs(columns,rows-1);
    for (int j = 0; j < rows-1; j++) {
      bool flip = flip1;
      for (int i = 0; i < columns; i++) {
        QPoint p1 = points[i][j+1];
        QPoint p2 = points[i+1][j+1];
        if (flip) { QPoint temp = p1; p1=p2; p2=temp; }
        hKnobs[i][j] = Knob(p1.x(), p1.y(), p2.x(), p2.y());
        flip = !flip;
      }
      flip1 = !flip1;
    }

    // Create the pieces.
    TDArray<Piece *> pieces(columns,rows);
    for (int j = 0; j < rows; j++)
      for (int i = 0; i < columns; i++) {
        Knob * knobN = (j>        0) ? &hKnobs[i][j-1] : NULL;
        Knob * knobS = (j<   rows-1) ? &hKnobs[i][j]   : NULL;
        Knob * knobW = (i>        0) ? &vKnobs[i-1][j] : NULL;
        Knob * knobE = (i<columns-1) ? &vKnobs[i][j]   : NULL;
        pieces[i][j] = makePiece (image,
          points[i][j],
          points[i][j+1],
          points[i+1][j],
          points[i+1][j+1],
          knobN, knobE, knobS, knobW,
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
        if (i>0)
          pieces[i][j]->addNeighbor(pieces[i-1][j]);
        if (j>0)
          pieces[i][j]->addNeighbor(pieces[i][j-1]);
        if (i<columns-1)
          pieces[i][j]->addNeighbor(pieces[i+1][j]);
        if (j<rows-1)
          pieces[i][j]->addNeighbor(pieces[i][j+1]);
        ret[j*columns+i] = pieces[i][j];
      }

    return ret;
}

Piece * ClassicCutter::makePiece(const QImage & image,
                                 const QPoint & nw,const QPoint & sw,
                                 const QPoint & ne,const QPoint & se,
                                 Knob * knobN, Knob * knobE, Knob * knobS,
                                 Knob * knobW,int tWidth, int tHeight) {
    // Build a path out of the knobs/puzzle edges.
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.moveTo(nw.x(), nw.y());
    if (knobN != NULL) path.addPath(knobN->getCurvePath(nw.x(), nw.y()));
    else path.lineTo(ne.x(), ne.y());
    if (knobE != NULL) path.connectPath(knobE->getCurvePath(ne.x(), ne.y()));
    else path.lineTo(se.x(), se.y());
    if (knobS != NULL) path.connectPath(knobS->getCurvePath(se.x(), se.y()));
    else path.lineTo(sw.x(), sw.y());
    if (knobW != NULL) path.connectPath(knobW->getCurvePath(sw.x(), sw.y()));
    else path.lineTo(nw.x(), nw.y());

    QRect box = path.boundingRect().toRect();
    if (box.x() < 0) box.setX(0);
    if (box.y() < 0) box.setY(0);

    int width  = box.width();
    int height = box.height();
    int minX   = box.x();
    int minY   = box.y();
    QVector<QRgb> data = JigUtil::image_data(image.copy(box.x(),box.y(),width,height),path,minX,minY);

    return new Piece(data,minX,minY,width,height,tWidth,tHeight,((int)(_rand()*4)) * 90);
}

