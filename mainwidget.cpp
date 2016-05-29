#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "mainwindow.h"
#include "JigsawCutter.h"
#include "Piece.h"
#include "FreeShuffler.h"
#include "FreeCenterShuffler.h"
#include "NoRotationsShuffler.h"
#include "JigUtil.h"
#include "ByColorShuffler.h"
#include "finaldialog.h"
#include "MultiPiece.h"
#include "aqsound.h"

#include <QPaintEvent>
#include <QPalette>
#include <QBrush>
#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>
#include <QMessageBox>
#include <QTimer>
#include <QLabel>

const Qt::Key MainWidget::ROTATE_LEFT  = Qt::Key_L;
const Qt::Key MainWidget::ROTATE_RIGHT = Qt::Key_R;

class Selector {
private:
    MainWidget * selectionPanel;
    QPoint start;
    QPoint prev;
    bool dragging;
 public:
    Selector(MainWidget * isp) {
        selectionPanel = isp;
        dragging = false;
    }

    void mousePressed(QMouseEvent * e) {
        if (e->button() != Qt::LeftButton) return;

        prev = start = e->pos();

        Piece * piece = NULL;
        if ((piece = selectionPanel->mouseOntoPiece(e->x(),e->y())) != NULL) {
            if (!selectionPanel->isSelectedPiece(piece)) {
                selectionPanel->deselectPieces();
            }
            dragging = false;
            return;
        }
        else {
            selectionPanel->deselectPieces();
        }

        dragging = true;
    }

    void mouseReleased(QMouseEvent * e) {
        if (dragging) {
            mouseDragged(e);
            selectionPanel->selectPieces(start,e->pos());
            dragging = false;
            selectionPanel->setClipFrame(start,start);
            selectionPanel->update();
        }
    }

    void mouseDragged(QMouseEvent * e) {
        if(dragging) selectionPanel->setClipFrame(start, e->pos());

        selectionPanel->moveSelectedPieces(prev,e->pos());

        prev = e->pos();
    }
};

MainWidget::MainWidget(MainWindow * mainWindow) :
    QWidget(NULL),
    m_ui(new Ui::MainWidget) {
    m_ui->setupUi(this);
    this->mainWindow = mainWindow;
    selector = new Selector(this);
    init();
}

void MainWidget::init() {
    showPieces = true;
    finished = false;
    showHint = false;
    mouseDown = false;
    focusPiece = NULL;
    prevTime = 0;
}

MainWidget::~MainWidget() {
    delete m_ui;
    delete selector;
}

void MainWidget::changeEvent(QEvent *e) {
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWidget::paintEvent(QPaintEvent * event) {
    QWidget::paintEvent(event);

    QPainter g(this);
    if (!finished && showHint) {
        QRect imageRect(QPoint(imagePos.x(),imagePos.y()),hintImage.size());
        if (imageRect.intersects(event->rect())) {
            g.drawImage(event->rect().topLeft(),hintImage,QRect(QPoint(event->rect().x()-imagePos.x(),event->rect().y()-imagePos.y()),event->rect().size()));
        }
    }

    int x = 0, y = 0;

    if (showPieces) {
        for (int i=0;i<zOrder.count();i++) {
            Piece * piece = zOrder.at(i);
            if (piece->getPuzzleRect().intersects(event->rect())) piece->draw(&g);
        }
    }

    if (finished && !finishedImage.isNull()) {
        Piece * lastPiece = zOrder.at(0);
        x = lastPiece->getPuzzleX();
        y = lastPiece->getPuzzleY();
        g.drawImage(x,y,finishedImage);
    }

    if (!finished && !clip.isNull()) {
        if (clip.x() != 0 || clip.y() != 0 || clip.width() != 0 || clip.height() != 0) {
            QColor color;
            color.setRgb(JigUtil::invertColor(palette().color(backgroundRole()).rgb()));
            g.setPen(color);
            g.drawRect(clip);
        }
    }
}

void MainWidget::setCutter(const QImage & image,JigsawCutter * cutter,const QColor & background) {
    Piece::deleteAllPieces();
    zOrder.clear();
    repaint();

    this->image = image;
    hintImage = JigUtil::create_hint_image(image);
    resize((int)(image.width()*2.5),(int)(image.height()*1.5));
        FreeShuffler shuffler(cutter->cut(image),size().width(),size().height());
    shuffler.shuffle();
    zOrder = QList<Piece *>(shuffler.getOrder());
    imagePos = QPoint((width()-image.width())/2,(height()-image.height())/2);
    finishedImage = QImage();
    setBackground(background);
    init();
    showHint = mainWindow->getImageAction()->isChecked();
    repaint();
    currTime = time(NULL);
    zorderCount = zOrder.count();
    updateStatusBarInfo();
}

void MainWidget::setBackground(const QColor & background) {
    QPalette palette;
    palette.setColor(backgroundRole(),background);
    setPalette(palette);
    repaint();
}

QColor MainWidget::background() const {
    return palette().color(backgroundRole());
}

void MainWidget::setHintImageEnabled(bool enable) {
    showHint = enable;
    repaint();
}

void MainWidget::mousePressEvent(QMouseEvent * event) {
    setFocus();
    if (event->button() == Qt::LeftButton) {
        if (!finished) {
            mouseDown = true;
            grabPiece(event);
        }
    }
    else {
        if (event->button() == Qt::RightButton) {
            if (!finished) {
                mouseDown = true;
                grabPiece(event);
            }
        }
    }
    selector->mousePressed(event);
}

void MainWidget::grabPiece(QMouseEvent * e) {
    int x = e->x();
    int y = e->y();
    focusPiece = NULL;

    for (int i=(zOrder.count()-1);i>=0;i--) {
        Piece * piece = zOrder.at(i);
        if (piece->contains(x,y)) {
            focusPiece = zOrder.at(i);
            zOrder.removeAt(i);
            break;
        }
    }

    if (focusPiece != NULL) {
      zOrder.append(focusPiece);

      transX = x - focusPiece->getPuzzleX();
      transY = y - focusPiece->getPuzzleY();
      // The focusPiece might have moved up in Z-order.  At worst, we have
      // to repaint its bounding rectangle.
      update(focusPiece->getPuzzleX(),focusPiece->getPuzzleY(),
             focusPiece->getCurrentWidth(),focusPiece->getCurrentHeight());
    }
}

void MainWidget::mouseReleaseEvent(QMouseEvent * e) {
    if (e->button() == Qt::LeftButton) {
        if (!finished) {
            mouseDown = false;
            releasePiece(e);
        }
    }
    else {
        if (e->button() == Qt::RightButton) {
            if (selectedPieces.count() <= 0 && !finished) rotatePiece(90);
        }
    }
    selector->mouseReleased(e);
}

void MainWidget::releasePiece(QMouseEvent * /*e*/) {
    if (focusPiece == NULL) return;
    if (selectedPieces.count() > 0) return;

    QList<Piece *> result = focusPiece->join();
    if (result.count() > 0) {
      Piece * newPiece = result[0];
      for (int i = 1; i < result.count(); i++) {
        zOrder.removeAll(result.at(i));
      }
      focusPiece = newPiece;
      zOrder.append(focusPiece);
      update();
      if (mainWindow->isAudioEnabled()) QAlsaSound::play(":/audio/click.wav");
      updateStatusBarInfo();
      if (zOrder.count() == 1) {
          finish();
          return;
      }
    }

    QRect center(imagePos.x(),imagePos.y(),
                 image.width(),image.height());
    if (focusPiece != NULL) {
        QPoint newPos = focusPiece->getClosedToRectNewPosition(center);
        if (!newPos.isNull()) {
            focusPiece->moveTo(newPos.x(),newPos.y());
            update();
        }
    }
}

void MainWidget::updateStatusBarInfo() {
    mainWindow->getInfoLabel()->setText(QString(tr("Puzzles: %1/%2")).arg(zOrder.count()).arg(zorderCount));
}

void MainWidget::rotatePiece(int amount) {
    if (focusPiece == NULL) return;

    int newRotation = focusPiece->getRotation()+amount;
    newRotation %= 360;
    int prevW = focusPiece->getCurrentWidth();
    int prevH = focusPiece->getCurrentHeight();
    int prevX = focusPiece->getPuzzleX();
    int prevY = focusPiece->getPuzzleY();
    focusPiece->setRotation(newRotation);
    // Make the piece appear to rotate about its center.
    // ### Feature: When the mouse is down, rotate about the cursor instead
    //   of the center.
    int centerX = prevX + prevW/2;
    int centerY = prevY + prevH/2;
    int currW = focusPiece->getCurrentWidth();
    int currH = focusPiece->getCurrentHeight();
    int currX = centerX - currW/2;
    int currY = centerY - currH/2;
    focusPiece->moveTo(currX, currY);
    update(prevX, prevY, prevW, prevH);
    update(currX, currY, currW, currH);
}

void MainWidget::mouseMoveEvent(QMouseEvent * e) {
    if (e->button() != Qt::RightButton) {
        if (!finished) {
            dragPiece(e);

            if (selectedPieces.count() <= 0) {
                QRect r(e->x(),e->y(),1,1);
                if (focusPiece != NULL) r = focusPiece->getPuzzleRect();
                if (mainWindow->isAutoScrollEnabled()) {
                    mainWindow->scrollArea()->ensureVisible(r.topLeft().x(),r.topLeft().y());
                    mainWindow->scrollArea()->ensureVisible(r.topRight().x(),r.topRight().y());
                    mainWindow->scrollArea()->ensureVisible(r.bottomLeft().x(),r.bottomLeft().y());
                    mainWindow->scrollArea()->ensureVisible(r.bottomRight().x(),r.bottomRight().y());
                }
            }
        }
    }
    selector->mouseDragged(e);
}

void MainWidget::dragPiece(QMouseEvent * e) {
    if (focusPiece == NULL || selectedPieces.count() > 0) return;

    int prevX = focusPiece->getPuzzleX();
    int prevY = focusPiece->getPuzzleY();
    int prevW = focusPiece->getCurrentWidth();
    int prevH = focusPiece->getCurrentHeight();

    int moveX = e->x()-transX;
    int moveY = e->y()-transY;
    if (moveX < 0) moveX = 0;
    if ((moveX+focusPiece->getCurrentWidth()) > width()) moveX = width()-focusPiece->getCurrentWidth();
    if (moveY < 0) moveY = 0;
    if ((moveY+focusPiece->getCurrentHeight()) > height()) moveY = height()-focusPiece->getCurrentHeight();

    focusPiece->moveTo(moveX, moveY);
    // Repaint the focusPiece' previous and current bounding rects.
    update(prevX, prevY, prevW, prevH);
    update(focusPiece->getPuzzleX(),focusPiece->getPuzzleY(),
           focusPiece->getCurrentWidth(),focusPiece->getCurrentHeight());
}

void MainWidget::keyPressEvent(QKeyEvent * event) {
    switch (event->key()) {
        case MainWidget::ROTATE_LEFT:
            rotatePiece(270);
            break;
        case MainWidget::ROTATE_RIGHT:
            rotatePiece(90);
            break;
    };
}

Piece * MainWidget::mouseOntoPiece(int x,int y) {
    for (int i=0;i<zOrder.count();i++) {
        Piece * piece = zOrder.at(i);
        if (piece->contains(x,y)) {
            return piece;
        }
    }
    return NULL;
}

bool MainWidget::isSelectedPiece(Piece * piece) {
    return selectedPieces.contains(piece);
}

void MainWidget::deselectPieces() {
    for (int i=0;i<selectedPieces.count();i++) {
        Piece * piece = selectedPieces.at(i);
        piece->setFlashMode(false);
        update(piece->getPuzzleX(),piece->getPuzzleY(),piece->getCurrentWidth(),piece->getCurrentHeight());
    }
    selectedPieces.clear();
}

void MainWidget::selectPieces(const QPoint & start,const QPoint & end) {
      if (start == end) return;

      QRect rect(start,end);

      for (int i=0;i<zOrder.count();i++) {
          Piece * piece = zOrder.at(i);

          if (rect.intersects(piece->getPuzzleRect())) {
              piece->setFlashMode(true);
              selectedPieces.append(piece);
          }
      }
      update(rect);
}

void MainWidget::setClipFrame(const QPoint & start,const QPoint & end) {
      QRect oldClip = clip;
      clip.setRect(0,0,0,0);
      repaint(oldClip.x()-1,oldClip.y()-1,2,oldClip.height()+1);
      repaint(oldClip.x()-1,oldClip.y()-1,oldClip.width()+1,2);
      repaint(oldClip.x()+oldClip.width()-1,oldClip.y()-1,2,oldClip.height()+1);
      repaint(oldClip.x()-1,oldClip.y()+oldClip.height()-1,oldClip.width()+1,2);

      if (start == end) {
          return;
      }

      clip = QRect(start, end).normalized();
      repaint(clip.x()-1,clip.y()-1,2,clip.height()+1);
      repaint(clip.x()-1,clip.y()-1,clip.width()+1,2);
      repaint(clip.x()+clip.width()-1,clip.y()-1,2,clip.height()+1);
      repaint(clip.x()-1,clip.y()+clip.height()-1,clip.width()+1,2);
}

void MainWidget::moveSelectedPieces(const QPoint & prev,const QPoint & curr) {
      int dX = curr.x() - prev.x();
      int dY = curr.y() - prev.y();

      if (selectedPieces.count() <= 0) return;
      QRect fullRect = selectedPieces.at(0)->getPuzzleRect();
      for (int i=1;i<selectedPieces.count();i++) {
          fullRect |= selectedPieces.at(i)->getPuzzleRect();
      }
      int rectX = fullRect.x() + dX;
      int rectY = fullRect.y() + dY;
      if (rectX < 0) rectX = 0;
      if ((rectX+fullRect.width()) > width()) rectX = width()-fullRect.width();
      if (rectY < 0) rectY = 0;
      if ((rectY+fullRect.height()) > height()) rectY = height()-fullRect.height();
      dX = rectX - fullRect.x();
      dY = rectY - fullRect.y();

      for (int i=0;i<selectedPieces.count();i++) {
          Piece * piece = selectedPieces.at(i);

          QRect rect = piece->getPuzzleRect();

          piece->moveTo(piece->getPuzzleX() + dX,piece->getPuzzleY() + dY);
          update(rect);
          update(piece->getPuzzleX(),piece->getPuzzleY(),piece->getCurrentWidth(),piece->getCurrentHeight());
      }

      fullRect.setRect(curr.x()-10,curr.y()-10,20,20);
      if (mainWindow->isAutoScrollEnabled()) {
          mainWindow->scrollArea()->ensureVisible(fullRect.topLeft().x(),fullRect.topLeft().y());
          mainWindow->scrollArea()->ensureVisible(fullRect.topRight().x(),fullRect.topRight().y());
          mainWindow->scrollArea()->ensureVisible(fullRect.bottomLeft().x(),fullRect.bottomLeft().y());
          mainWindow->scrollArea()->ensureVisible(fullRect.bottomRight().x(),fullRect.bottomRight().y());
      }
}

void MainWidget::onFreeCenterShuffle() {
    FreeCenterShuffler(zOrder,width(),height(),image.width()).shuffle();
    repaint();
}

void MainWidget::onNoRotationsShuffle() {
    NoRotationsShuffler(zOrder).shuffle();
    repaint();
}

void MainWidget::onFreeShuffle() {
    FreeShuffler(zOrder,width(),height()).shuffle();
    repaint();
}

void MainWidget::onByColorShuffle() {
    ByColorShuffler(zOrder,width(),height()).shuffle();
    repaint();
}

void MainWidget::onShowPieces(bool enable) {
    showPieces = enable;
    repaint();
}

void MainWidget::onHintPieces() {
    Piece * piece1 = NULL;
    Piece * piece2 = NULL;

    QRect viewRect(QPoint(mainWindow->scrollArea()->horizontalScrollBar()->sliderPosition(),
                          mainWindow->scrollArea()->verticalScrollBar()->sliderPosition()),
                   mainWindow->scrollArea()->viewport()->size());

    QList<Piece *> mpieces;
    QList<Piece *> pieces;

    for (int i=0;i<zOrder.count();i++) {
        Piece * piece = zOrder.at(i);

        if (!viewRect.intersects(piece->getPuzzleRect())) continue;
        if (piece->instanceOf() == MPiece) {
            mpieces.append(piece);
        }
        else {
            pieces.append(piece);
        }
    }

    for (int i=0;i<mpieces.count();i++) {
        for (int j=0;j<mpieces.count();j++) {
            if (i == j) continue;

            Piece * mpiece1 = mpieces.at(i);
            Piece * mpiece2 = mpieces.at(j);
            if (mpiece1->isOneOfNeighbors(mpiece2)) {
                piece1=mpiece1;
                piece2=mpiece2;
                break;
            }
        }
    }

    if (piece1 == NULL || piece2 == NULL) {
      for (int i=0;i<mpieces.count();i++) {
          for (int j=0;j<pieces.count();j++) {
              Piece * mpiece1 = mpieces.at(i);
              Piece * mpiece2 = pieces.at(j);
              if (mpiece1->isOneOfNeighbors(mpiece2)) {
                  piece1=mpiece1;
                  piece2=mpiece2;
                  break;
              }
          }
      }
    }

    if (piece1 == NULL || piece2 == NULL) {
      for (int i=0;i<pieces.count();i++) {
          for (int j=0;j<pieces.count();j++) {
              if (i == j) continue;

              Piece * mpiece1 = pieces.at(i);
              Piece * mpiece2 = pieces.at(j);
              if (mpiece1->isOneOfNeighbors(mpiece2)) {
                  piece1=mpiece1;
                  piece2=mpiece2;
                  break;
              }
          }
      }
    }

    if (piece1 == NULL || piece2 == NULL) return;

    flashPiece(piece1);
    flashPiece(piece2);
    repaint();
}

void MainWidget::flashPiece(Piece * piece) {
    for (int i=0;i<3;i++) {
        piece->setFlashMode(true);
        repaint(piece->getPuzzleRect());
        JThread::sleep(200);
        piece->setFlashMode(false);
        repaint(piece->getPuzzleRect());
        JThread::sleep(200);
    }
}

void MainWidget::fader() {
    for (int i = 0; i < fader_data.count(); i++)  fader_data[i] = (fader_data[i] & 0x00ffffff) | (trans << 24);
    JigUtil::set_image_data(finishedImage,fader_data,image.width());
    update(imagePos.x(), imagePos.y(), image.width(), image.height());
    if (trans < 0xff) {
        trans += 0x11; if (trans >= 0xff) trans = 0xff;
        QTimer::singleShot(200, this, SLOT(fader()));
    }
    else {
        JThread::sleep(1000);
        FinalDialog(getSpentTime(),mainWindow).exec();
    }
}

void MainWidget::finish() {
    trans = 0x00;

    if (zOrder.count() != 1) return;
    finished = true;

    mainWindow->enableMenu(false);

    Piece * lastPiece = zOrder.at(0);

    // Auto-rotate the puzzle to its correct position.
    lastPiece->setRotation(0);

    // Center the last piece in the middle of the panel.
    int width  = lastPiece->getImageWidth();
    int height = lastPiece->getImageHeight();
    lastPiece->moveTo(imagePos.x(), imagePos.y());
    update(lastPiece->getPuzzleX(), lastPiece->getPuzzleY(), width, height);
    update(imagePos.x(), imagePos.y(), width, height);

    // Draw the original image on top of the last piece in increasing
    // opaqueness.  This should make the pieces appear to fade into the
    // original image.
    fader_data = JigUtil::image_data(image);

    for (int i = 0; i < fader_data.count(); i++) fader_data[i] = fader_data[i] & 0x00ffffff;

    QTimer::singleShot(200, this, SLOT(fader()));
}

time_t MainWidget::getSpentTime() const {
    return time(NULL)-currTime+prevTime;
}

QDataStream & operator<< ( QDataStream & out,const MainWidget * widget) {
     out << widget->image;
     out << widget->mouseDown;
     out << widget->finished;
     out << widget->transX;
     out << widget->transY;
     out << Piece::all_puzzles.count();
     for (int i=0;i<Piece::all_puzzles.count();i++) {
         out << (Piece::all_puzzles.at(i)->instanceOf() == MPiece);
     }
     for (int i=0;i<Piece::all_puzzles.count();i++) {
         Piece::all_puzzles.at(i)->writeExternal(&out);
     }
     out<<widget->zOrder.count();
     for (int i=0;i<widget->zOrder.count();i++) {
         out<<Piece::all_puzzles.indexOf(widget->zOrder.at(i));
     }
     out<<widget->background();
     out<<Piece::all_puzzles.indexOf(widget->focusPiece);
     out<<widget->imagePos;
     out<<(quint32)widget->getSpentTime();
     return out;
}

QDataStream & operator>> ( QDataStream & in, MainWidget * widget) {
     in >> widget->image;
     in >> widget->mouseDown;
     in >> widget->finished;
     in >> widget->transX;
     in >> widget->transY;
     Piece::deleteAllPieces();
     int count;
     in >> count;
     for (int i=0;i<count;i++) {
         bool isMulti;
         in >> isMulti;
         Piece::all_puzzles.append(isMulti?new MultiPiece():new Piece());
     }
     for (int i=0;i<count;i++) {
         Piece::all_puzzles.at(i)->readExternal(&in);
     }
     in >> count;
     widget->zOrder.clear();
     int index;
     for (int i=0;i<count;i++) {
         in >> index;
         widget->zOrder.append(Piece::all_puzzles.at(index));
     }
     QColor color;
     in >> color;
     widget->setBackground(color);
     in >> index;
     widget->focusPiece = (index == -1)?NULL:Piece::all_puzzles.at(index);
     in >> widget->imagePos;
     quint32 _time;
     in >> _time;
     widget->prevTime = (time_t)_time;
     widget->currTime = time(NULL);
     widget->hintImage = JigUtil::create_hint_image(widget->image);
     widget->resize((int)(widget->image.width()*2.5),(int)(widget->image.height()*1.5));
     widget->zorderCount = Piece::countsPieces();
     widget->updateStatusBarInfo();
     return in;
}

void MainWidget::fixBadOpen() {
    image = QImage();
    hintImage = QImage();
    zOrder.clear();
    Piece::deleteAllPieces();
    repaint();
}
