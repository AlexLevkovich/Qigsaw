#include "mainwindow.h"
#include "newdialog.h"
#include "ui_mainwindow.h"
#include "jigsawaboutbox.h"
#include "previewframe.h"
#include "preview.h"
#include "mainwidget.h"
#include "JigsawCutter.h"
#include "qpreviewfiledialog.h"

#include <QApplication>
#include <QMessageBox>
#include <QKeyEvent>
#include <QColorDialog>
#include <QDataStream>
#include <QFile>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass) {
    ui->setupUi(this);
    previewFrame = NULL;
    mainWidget = new MainWidget(this);
    ui->scrollArea->setWidget(mainWidget);
    infoLabel = new QLabel(tr("No game started"));
    infoLabel->setFrameShadow(QFrame::Sunken);
    infoLabel->setFrameShape(QFrame::Panel);
    statusBar()->addPermanentWidget(infoLabel);
    playAudio = true;
    autoScroll = true;
}

MainWindow::~MainWindow() {
    delete ui;
}

QLabel * MainWindow::getInfoLabel() {
    return infoLabel;
}

void MainWindow::on_actionAbout_triggered() {
    JigsawAboutBox(this).exec();
}

void MainWindow::on_actionNew_triggered() {
    NewDialog newDialog(this);
    if (newDialog.exec() == QDialog::Rejected) return;

    JigsawCutter * cutter = newDialog.getSelectedCutter();
    cutter->setPreferredPieceCount(newDialog.getPieceCount());
    mainWidget->setCutter(QImage(newDialog.getSelectedPath()),cutter,newDialog.getSelectedColor());
    previewClosed();
    enableMenu(true);
    saveFileName = "";
    setWindowTitle("Qigsaw");
}

void MainWindow::on_actionOpen_triggered() {
    QPreviewFileDialog fd(new Preview(),this);
    fd.setNameFilter(tr("Qigsaw files (*.qsw)"));
    fd.setWindowTitle(tr("Please select the file..."));
    fd.setDirectory(openDir.isEmpty()?QDir::home():QDir(openDir));
    if (fd.exec() == QDialog::Rejected) return;
    QString name = fd.selectedFiles().at(0);
    if (name.isNull()) return;
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly)) return;
    QDataStream in(&file);
    in >> mainWidget;
    if (file.error() != QFile::NoError) {
        QMessageBox::critical(this,tr("Open error..."),tr("The error during openning the file!"));
        mainWidget->fixBadOpen();
        return;
    }

    openDir = QFileInfo(name).dir().path();
    saveFileName = name;
    setWindowTitle("Qigsaw - "+saveFileName);
    previewClosed();
    enableMenu(true);
    repaint();
}

void MainWindow::on_actionSave_triggered() {
    if (saveFileName.isEmpty()) {
        QFileDialog fd(this);
        fd.setOptions(QFileDialog::DontUseNativeDialog | fd.options());
        fd.setNameFilter(tr("Qigsaw files (*.qsw)"));
        fd.setWindowTitle(tr("Please select the file..."));
        fd.setDirectory(openDir.isEmpty()?QDir::home():QDir(openDir));
        fd.setAcceptMode(QFileDialog::AcceptSave);
        if (fd.exec() == QDialog::Rejected) return;
        QString name = fd.selectedFiles().at(0);
        if (name.isNull()) return;
        if (QFileInfo(name).suffix() != "qsw") name+=".qsw";
        saveFileName = name;
    }
    QFile file(saveFileName);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Truncate)) return;
    QDataStream out(&file);
    out << mainWidget;
    if (file.error() != QFile::NoError) {
        QMessageBox::critical(this,tr("Save error..."),tr("The error during saving the file!"));
        return;
    }
    openDir = QFileInfo(saveFileName).dir().path();
    setWindowTitle("Qigsaw - "+saveFileName);
}

void MainWindow::on_actionExit_triggered() {
    qApp->quit();
}

void MainWindow::on_actionPreview_triggered() {
    if (previewFrame != NULL) {
        previewFrame->setVisible(true);
        return;
    }

    previewFrame = new PreviewFrame(*mainWidget->getImage());
    connect(previewFrame,SIGNAL(closed()),this,SLOT(previewClosed()));
    previewFrame->show();
}

void MainWindow::previewClosed() {
    delete previewFrame;
    previewFrame = NULL;
}

void MainWindow::on_actionImage_triggered(bool checked) {
    mainWidget->setHintImageEnabled(checked);
}

void MainWindow::on_actionInfo_triggered() {
    QMessageBox::information(this,tr("Information..."),tr("<html>"
      "Drag pieces with the mouse to fit them together.  If they do,"
      " they'll join and move as a unit from then on."
      "<p> Keyboard commands: <br>"
      "<table>"
      "<tr><td>L"
         " <td> rotate piece left 90 degrees"
      "<tr><td>R"
         " <td> rotate piece right 90 degrees"
      "</table>")
      );
}

QScrollArea * MainWindow::scrollArea() {
    return ui->scrollArea;
}

void MainWindow::on_actionFree_center_triggered() {
    mainWidget->onFreeCenterShuffle();
}

void MainWindow::on_actionNo_rotations_triggered() {
    mainWidget->onNoRotationsShuffle();
}

void MainWindow::on_actionDisorder_triggered() {
    mainWidget->onFreeShuffle();
}

void MainWindow::on_actionBy_color_triggered() {
    mainWidget->onByColorShuffle();
}

void MainWindow::on_actionPuzzles_triggered(bool checked) {
    mainWidget->onShowPieces(checked);
}

void MainWindow::on_actionBackground_triggered() {
    QColor color = QColorDialog::getColor(mainWidget->background(),this);
    if (!color.isValid()) return;

    mainWidget->setBackground(color);
}

void MainWindow::on_actionHint_triggered() {
    mainWidget->onHintPieces();
}

void MainWindow::enableMenu(bool enable) {
    ui->actionSave->setEnabled(enable);
    ui->actionPreview->setEnabled(enable);
    ui->actionFree_center->setEnabled(enable);
    ui->actionNo_rotations->setEnabled(enable);
    ui->actionDisorder->setEnabled(enable);
    ui->actionBy_color->setEnabled(enable);
    ui->actionImage->setEnabled(enable);
    ui->actionPuzzles->setEnabled(enable);
    ui->actionHint->setEnabled(enable);
    ui->actionBackground->setEnabled(enable);
}

QAction * MainWindow::getNewAction() {
    return ui->actionNew;
}

QAction * MainWindow::getOpenAction() {
    return ui->actionOpen;
}

QAction * MainWindow::getImageAction() {
    return ui->actionImage;
}

void MainWindow::on_actionAudio_triggered(bool checked) {
    playAudio = checked;
}

void MainWindow::on_actionAutoscroll_triggered(bool checked) {
    autoScroll = checked;
}

bool MainWindow::isAudioEnabled() {
    return playAudio;
}

bool MainWindow::isAutoScrollEnabled() {
    return autoScroll;
}
