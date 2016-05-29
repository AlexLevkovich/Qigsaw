#include "SquareCutter.h"
#include "exceptions.h"
#include "Piece.h"
#include <math.h>
#include "TDArray.h"
#include "JigUtil.h"

#include <list>
using namespace std;

void SquareCutter::init(int count) {
    setPreferredPieceCount(count);
}

SquareCutter::SquareCutter() {
    init(defaultEdge);
}

QString SquareCutter::getName() const {
    return QObject::tr("Squares");
}

QString SquareCutter::getDescription() const {
    return QObject::tr("Cuts the image into uniform squares.");
}

SquareCutter::SquareCutter(int count) {
    init(count);
}

void SquareCutter::writeExternal(QDataStream * out) {
    *out << prefCount;
}

void SquareCutter::readExternal(QDataStream * in) {
    *in >> prefCount;
}

void SquareCutter::setPreferredPieceCount(int count) {
    if (count < 2) throw IllegalArgumentException(QString("Invalid preferred pieces: %1").arg(count));
    prefCount = count;
}

const QList<Piece *> SquareCutter::cut(const QImage & image) {
    int height = image.height();
    int width  = image.width();
    int edge = width/(int)(sqrt((double)prefCount) + 1);
    int hRemain = height % edge;
    int wRemain = width  % edge;
    int hPieces = height / edge;
    int wPieces = width  / edge;
    int firstSouthEdge = edge + (hRemain / 2) - 1;
    int firstEastEdge  = edge + (wRemain / 2) - 1;
    int y1 = 0;
    int y2 = firstSouthEdge;

    // Create piece images
    TDArray<Piece *> matrix(hPieces,wPieces);
    for (int i = 0; i < hPieces; i++) {
      int x1 = 0;
      int x2 = firstEastEdge;
      for (int j = 0; j < wPieces; j++) {
        int pieceW = x2 - x1 + 1;
        int pieceH = y2 - y1 + 1;
        int rotation = (int)(_rand()*4) * 90;
        matrix[i][j] = new Piece (getImageData(image, x1, y1, pieceW, pieceH),
                                  x1, y1, pieceW, pieceH,
                                  width, height,
                                  rotation);
        if ((i==0) && (j>0) && (j<(wPieces-1))) matrix[i][j]->setFrontier(Piece::LEFT_FRONT);
        if ((i == (hPieces-1)) && (j>0) && (j<(wPieces-1))) matrix[i][j]->setFrontier(Piece::RIGHT_FRONT);
        if ((j == 0) && (i>0) && (i<(hPieces-1))) matrix[i][j]->setFrontier(Piece::TOP_FRONT);
        if ((j == (wPieces-1)) && (i>0) && (i<(hPieces-1))) matrix[i][j]->setFrontier(Piece::BOTTOM_FRONT);
        if ((i==0) && (j==0)) matrix[i][j]->setFrontier(Piece::LEFT_TOP_FRONT);
        if ((i == (hPieces-1)) && (j==0)) matrix[i][j]->setFrontier(Piece::RIGHT_TOP_FRONT);
        if ((j==(wPieces-1)) && (i == 0)) matrix[i][j]->setFrontier(Piece::LEFT_BOTTOM_FRONT);
        if ((j == (wPieces-1)) && (i == (hPieces-1))) matrix[i][j]->setFrontier(Piece::RIGHT_BOTTOM_FRONT);

        // Set up x1 and x2 for next slice
        x1 = x2+1;
        x2 += edge;
        if ((width - x2) < edge) x2 = width-1;
      }

      // Set up y1 and y2 for next slice
      y1 = y2+1;
      y2 += edge;
      if ((height - y2) < edge) y2 = height-1;
    }

    // Set piece neighbors
    for (int i = 0; i < hPieces-1; i++) {
      for (int j = 0; j < wPieces-1; j++) {
        Piece * piece  = matrix[i  ][j  ];
        Piece * right  = matrix[i  ][j+1];
        Piece * bottom = matrix[i+1][j  ];
        piece->addNeighbor (right);
        right->addNeighbor (piece);
        piece->addNeighbor (bottom);
        bottom->addNeighbor (piece);
      }
      Piece * farRight      = matrix[i  ][wPieces-1];
      Piece * farRightLower = matrix[i+1][wPieces-1];
      farRight->addNeighbor (farRightLower);
      farRightLower->addNeighbor (farRight);
    }
    for (int j = 0; j < wPieces-1; j++) { // bottom row
      Piece * piece  = matrix[hPieces-1][j  ];
      Piece * right  = matrix[hPieces-1][j+1];
      piece->addNeighbor (right);
      right->addNeighbor (piece);
    }

    // Make the return array
    QList<Piece *> pieces = QList<Piece *>::fromStdList(list<Piece *>(hPieces*wPieces,(Piece *)NULL));
    for (int i = 0; i < hPieces; i++)
      for (int j = 0; j < wPieces; j++)
        pieces[i*wPieces+j] = matrix[i][j];

    return pieces;
}

const QVector<QRgb> SquareCutter::getImageData(const QImage & image, int x, int y,int width, int height) {
    return JigUtil::image_data(image.copy(x,y,width,height));
}
