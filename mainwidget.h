#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <time.h>

#include <QWidget>
#include <QList>
#include <QImage>
#include <QPoint>
#include <QRect>
#include <QDataStream>

class QPaintEvent;
class MainWindow;
class JigsawCutter;
class Piece;
class QMouseEvent;
class QKeyEvent;


namespace Ui {
    class MainWidget;
}

class Selector;

class MainWidget : public QWidget {
    friend class Selector;
    Q_OBJECT
    Q_DISABLE_COPY(MainWidget)
public:
    explicit MainWidget(MainWindow * mainWindow);
    virtual ~MainWidget();

    void setCutter(const QImage & image,JigsawCutter * cutter,const QColor & background);
    void setHintImageEnabled(bool enable);
    void onFreeCenterShuffle();
    void onNoRotationsShuffle();
    void onFreeShuffle();
    void onByColorShuffle();
    void onShowPieces(bool enable);
    void setBackground(const QColor & background);
    QColor background() const;
    void onHintPieces();
    QImage * getImage() { return &image; }
    void fixBadOpen();

    static const Qt::Key ROTATE_LEFT;
    static const Qt::Key ROTATE_RIGHT;

    friend QDataStream & operator<< ( QDataStream & out,const MainWidget * widget);
    friend QDataStream & operator>> ( QDataStream & in, MainWidget * widget);
protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual void changeEvent(QEvent *e);
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void mouseMoveEvent(QMouseEvent * event);
    virtual void keyPressEvent(QKeyEvent * event);
private:
    Ui::MainWidget *m_ui;
    MainWindow * mainWindow;
    QList<Piece *> zOrder;
    QList<Piece *> selectedPieces;
    bool showPieces;
    bool finished;
    bool showHint;
    bool mouseDown;
    QImage hintImage;
    QImage finishedImage;
    QImage image;
    QPoint imagePos;
    QRect clip;
    Piece * focusPiece;
    // Translation from a piece's upper-left corner to the point you clicked
    // on.
    int transX, transY;
    Selector * selector;
    time_t prevTime;
    time_t currTime;
    QVector<QRgb> fader_data;
    int trans;
    int zorderCount;

    void init();
    void grabPiece(QMouseEvent * e);
    void releasePiece(QMouseEvent * e);
    void finish();
    void rotatePiece(int amount);
    void dragPiece(QMouseEvent * e);
    Piece * mouseOntoPiece(int x,int y);
    bool isSelectedPiece(Piece * piece);\
    void deselectPieces();
    void selectPieces(const QPoint & start,const QPoint & end);
    void setClipFrame(const QPoint & start,const QPoint & end);
    void moveSelectedPieces(const QPoint & prev,const QPoint & curr);
    void flashPiece(Piece * piece);
    time_t getSpentTime() const;
    void updateStatusBarInfo();
private slots:
    void fader();
};

#endif // MAINWIDGET_H
