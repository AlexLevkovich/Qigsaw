#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

namespace Ui
{
    class MainWindowClass;
}

class PreviewFrame;
class MainWidget;
class QScrollArea;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QScrollArea * scrollArea();
    void enableMenu(bool enable);
    QAction * getNewAction();
    QAction * getOpenAction();
    QAction * getImageAction();
    QLabel * getInfoLabel();
    bool isAudioEnabled();
    bool isAutoScrollEnabled();
private:
    Ui::MainWindowClass *ui;
    PreviewFrame * previewFrame;
    MainWidget * mainWidget;
    QString saveFileName;
    QString openDir;
    QLabel * infoLabel;
    bool playAudio;
    bool autoScroll;

private slots:
    void on_actionAutoscroll_triggered(bool checked);
    void on_actionAudio_triggered(bool checked);
    void on_actionOpen_triggered();
    void on_actionNew_triggered();
    void on_actionHint_triggered();
    void on_actionBackground_triggered();
    void on_actionPuzzles_triggered(bool checked);
    void on_actionBy_color_triggered();
    void on_actionDisorder_triggered();
    void on_actionNo_rotations_triggered();
    void on_actionFree_center_triggered();
    void on_actionInfo_triggered();
    void on_actionImage_triggered(bool checked);
    void on_actionPreview_triggered();
    void on_actionExit_triggered();
    void on_actionSave_triggered();
    void on_actionAbout_triggered();
    void previewClosed();
};

#endif // MAINWINDOW_H
