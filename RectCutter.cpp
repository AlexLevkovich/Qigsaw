#include "RectCutter.h"
#include "exceptions.h"
#include "JigUtil.h"
#include "Piece.h"
#include "TDArray.h"

#include <math.h>
#include <list>
using namespace std;

QString RectCutter::toString() const { return getName(); }
QString RectCutter::getName() const { return QObject::tr("Rectangles"); }
QString RectCutter::getDescription() const {
    return QObject::tr("Cuts the image into uniform rectangles.");
}

RectCutter::RectCutter() { init(20,25); }

RectCutter::RectCutter(int rows, int columns) {
    init(rows,columns);
}

void RectCutter::init(int rows, int columns) {
    if ((rows < 1) || (columns < 1)) throw IllegalArgumentException("rows and columns must be 1 or greater");
    this->rows = rows;
    this->columns = columns;
}

void RectCutter::setPreferredPieceCount(int count) {
    if (count < 2) throw IllegalArgumentException(QString("Invalid preferred pieces: %1").arg(count));
    rows = (int)sqrt(count);
    columns = rows+1;
}

void RectCutter::writeExternal(QDataStream * out) {
    *out << rows;
    *out << columns;
}

void RectCutter::readExternal(QDataStream * in) {
    *in >> rows;
    *in >> columns;
}

const QList<Piece *> RectCutter::cut(const QImage & image) {
    int height = image.height();
    int width  = image.width();

    // Create piece images
    TDArray<Piece *> matrix(rows,columns);
    for (int i = 0; i < rows; i++) {
      int y1 = i * height / rows;
      int y2 = (i+1) * height / rows;
      if (i>0) y1++;
      for (int j = 0; j < columns; j++) {
        int x1 = j * width / columns;
        int x2 = (j+1) * width / columns;
        if (j>0) x1++;

        int pieceW = x2 - x1 + 1;
        int pieceH = y2 - y1 + 1;
        int rotation = (int)(_rand()*4) * 90;
        matrix[i][j] = new Piece (getImageData (image, x1, y1, pieceW, pieceH),
                                  x1, y1, pieceW, pieceH,
                                  width, height,
                                  rotation);
        if ((i==0) && (j>0) && (j<(rows-1))) matrix[i][j]->setFrontier(Piece::LEFT_FRONT);
        if ((i == (columns-1)) && (j>0) && (j<(rows-1))) matrix[i][j]->setFrontier(Piece::RIGHT_FRONT);
        if ((j == 0) && (i>0) && (i<(columns-1))) matrix[i][j]->setFrontier(Piece::TOP_FRONT);
        if ((j == (rows-1)) && (i>0) && (i<(columns-1))) matrix[i][j]->setFrontier(Piece::BOTTOM_FRONT);
        if ((i==0) && (j==0)) matrix[i][j]->setFrontier(Piece::LEFT_TOP_FRONT);
        if ((i == (columns-1)) && (j==0)) matrix[i][j]->setFrontier(Piece::RIGHT_TOP_FRONT);
        if ((j==(rows-1)) && (i == 0)) matrix[i][j]->setFrontier(Piece::LEFT_BOTTOM_FRONT);
        if ((j == (rows-1)) && (i == (columns-1))) matrix[i][j]->setFrontier(Piece::RIGHT_BOTTOM_FRONT);
      }
    }

    // Set piece neighbors
    for (int i = 0; i < rows-1; i++) {
      for (int j = 0; j < columns-1; j++) {
        Piece * piece  = matrix[i  ][j  ];
        Piece * right  = matrix[i  ][j+1];
        Piece * bottom = matrix[i+1][j  ];
        piece->addNeighbor (right);
        right->addNeighbor (piece);
        piece->addNeighbor (bottom);
        bottom->addNeighbor (piece);
      }
      Piece * farRight      = matrix[i  ][columns-1];
      Piece * farRightLower = matrix[i+1][columns-1];
      farRight->addNeighbor (farRightLower);
      farRightLower->addNeighbor (farRight);
    }
    for (int j = 0; j < columns-1; j++) { // bottom row
      Piece * piece  = matrix[rows-1][j  ];
      Piece * right  = matrix[rows-1][j+1];
      piece->addNeighbor (right);
      right->addNeighbor (piece);
    }

    // Make the return array
    QList<Piece *> pieces = QList<Piece *>::fromStdList(list<Piece *>(rows*columns,(Piece *)NULL));
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < columns; j++)
        pieces[i*columns+j] = matrix[i][j];

    return pieces;
}

QVector<QRgb> RectCutter::getImageData(const QImage & image, int x, int y,int width, int height) const {
    return JigUtil::image_data(image.copy(x,y,width,height));
}
