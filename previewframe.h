#ifndef PREVIEWFRAME_H
#define PREVIEWFRAME_H

#include <QMainWindow>

namespace Ui {
    class PreviewFrame;
}

#include <QImage>

class QCloseEvent;
class QLabel;
class QResizeEvent;

class PreviewFrame : public QMainWindow {
    Q_OBJECT
    Q_DISABLE_COPY(PreviewFrame)
public:
    explicit PreviewFrame(const QImage & image,QWidget *parent = 0);
    virtual ~PreviewFrame();

protected:
    virtual void changeEvent(QEvent *e);
    virtual void closeEvent(QCloseEvent *event);
    virtual void resizeEvent(QResizeEvent * event);

signals:
    void closed();

private:
    Ui::PreviewFrame *m_ui;
    QImage image;
    QLabel * label;

private slots:
    void on_actionFitToPage_triggered(bool checked);
    void on_actionClose_triggered();
};

#endif // PREVIEWFRAME_H
