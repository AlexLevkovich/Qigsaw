#include "Knob.h"
#include "exceptions.h"
#include "JigUtil.h"

#include <QMatrix>
#include <QString>

const double Knob::ctl[DATA_LEN][DATA_LEN] = {
      {     0.0,       0.0,  1.0/8.0,       0.0,  3.0/16.0 },
      { 1.0/3.0, -4.0/32.0,  5.0/4.0, -1.0/ 8.0, -1.0/10.0 },
      { 1.0/2.0, -5.0/16.0,      0.0,  1.0/16.0,  1.0/16.0 },
      { 2.0/3.0, -4.0/32.0, -5.0/4.0, -1.0/10.0, -1.0/ 8.0 },
      {     1.0,       0.0, -1.0/8.0,  3.0/16.0,       0.0 }
};

const int Knob::X     = 0;
const int Knob::Y     = 1;
const int Knob::SLOPE = 2;
const int Knob::XDB   = 3;
const int Knob::XDF   = 4;

// x,y each varies by + or - itsVar
const double Knob::XVARY   = 1.0/20.0;
const double Knob::YVARY   = 1.0/20.0;
// b,f each varies by + or - it*itsVar
const double Knob::XDBVARY = 1.0/4.0;
const double Knob::XDFVARY = 1.0/4.0;

Knob::Knob(int x1, int y1, int x2, int y2) {
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
    initPath();
}

Knob::Knob() {
    this->x1 = 0;
    this->y1 = 0;
    this->x2 = 0;
    this->y2 = 0;
    initPath();
}

Knob::~Knob() {}

Knob & Knob::operator= (const Knob & knob) {
    this->x1 = knob.x1;
    this->y1 = knob.y1;
    this->x2 = knob.x2;
    this->y2 = knob.y2;
    cPath=knob.cPath;
    cPathReverse=knob.cPathReverse;
    return *this;
}

void Knob::initPath() {
    double data[DATA_LEN][DATA_LEN];
    for (int i=0;i<DATA_LEN;i++) {
        for (int j=0;j<DATA_LEN;j++) {
            data[i][j] = ctl[i][j];
        }
    }

    jitter (data, XVARY, YVARY, XDBVARY, XDFVARY);
    cPath.setFillRule(Qt::WindingFill);
    cPathReverse.setFillRule(Qt::WindingFill);
    cPath.moveTo(data[0][X],data[0][Y]);
    cPathReverse.moveTo(data[DATA_LEN-1][X],data[DATA_LEN-1][Y]);
    for (int i = 0; i < (DATA_LEN-1); i++) {
      curveTo (cPath,        data,               i, true);
      curveTo (cPathReverse, data,    DATA_LEN-1-i, false);
    }

    // Transform to coincide with line segment (x1,y1)-(x2,y2)

    QMatrix affine(x2-x1, y2-y1,  y1-y2, x2-x1,  x1, y1);
    cPath=affine.map(cPath);
    cPathReverse=affine.map(cPathReverse);
}

void Knob::curveTo(QPainterPath & path,const float_arr & data,int idx, bool forward) {
    int delta = forward ? 1 : -1;
    double cx1 = data[idx][X];
    double cy1 = data[idx][Y];
    double  m1 = data[idx][SLOPE];
    double d1f = data[idx][forward?XDF:XDB];
    double cx2 = data[idx+delta][X];
    double cy2 = data[idx+delta][Y];
    double  m2 = data[idx+delta][SLOPE];
    double d2b = data[idx+delta][forward?XDB:XDF];
    if (!forward) { d1f *= -1.0; d2b *= -1.0; }
    double x1 = cx1 + d1f;
    double y1 = cy1 + d1f*m1;
    double x2 = cx2 - d2b;
    double y2 = cy2 - d2b*m2;
    path.cubicTo( x1,y1, x2,y2, cx2,cy2 );
}

QPainterPath Knob::getCurvePath() const { return getCurvePath(x1,y1); }

QPainterPath Knob::getCurvePath(int x,int y) const {
    if ((x==x1) && (y==y1)) return cPath;
    else if ((x==x2) && (y==y2)) return cPathReverse;
    throw IllegalArgumentException(QString("Not an endpoint: (%1,%2)").arg(x).arg(y));
}

/** Returns the start point's X coordinate. */
int Knob::getX1() { return x1; }

/** Returns the end point's X coordinate. */
int Knob::getX2() { return x2; }

/** Returns the start point's Y coordinate. */
int Knob::getY1() { return y1; }

/** Returns the end point's Y coordinate. */
int Knob::getY2() { return y2; }

/** Returns a rectangle bounding this Knob. */
QRect Knob::getBounds() const { return cPath.boundingRect().toRect(); }

QRect Knob::getBounds(int x,int y) const {
    if ((x==x1) && (y==y1)) return cPath.boundingRect().toRect();
    else if ((x==x2) && (y==y2)) return cPathReverse.boundingRect().toRect();
    throw IllegalArgumentException(QString("Not an endpoint: (%1,%2)").arg(x).arg(y));
}

void Knob::jitter(float_arr & pts, double xVar, double yVar,double bVar, double fVar) {
    for (int i = 0; i < DATA_LEN; i++) {
      double b = pts[i][XDB];
      double f = pts[i][XDF];

      if ((i>0) && (i < DATA_LEN-1))
        pts[i][X] += _rand()*xVar*2.0 - xVar;
      pts[i][Y] +=   _rand()*yVar*2.0 - yVar;
      // b,f each varies by + or - it*itsVar
      pts[i][XDB] += _rand()*b*bVar*2.0 - b*bVar;
      pts[i][XDF] += _rand()*f*fVar*2.0 - f*fVar;
    }
}

QString Knob::toString() const {
    return QString("Knob[p1=(%1,%2),p2=(%3,%4)]").arg(x1).arg(y1).arg(x2).arg(y2);
}
