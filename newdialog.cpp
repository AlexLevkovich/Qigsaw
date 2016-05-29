#include "newdialog.h"
#include "ui_NewDialog.h"
#include "Classic4Cutter.h"
#include "ClassicCutter.h"
#include "SquareCutter.h"
#include "RectCutter.h"
#include "QuadCutter.h"
#include "preview.h"
#include "qpreviewfiledialog.h"

#include <QAbstractButton>
#include <QDir>
#include <QColorDialog>
#include <QPalette>
#include <QPixmap>
#include <QShowEvent>
#include <QFileInfo>

const QColor NewDialog::defBackground = QColor(48,48,48);
QString NewDialog::prevName;

NewDialog::NewDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::NewDialog) {
    m_ui->setupUi(this);
    selectedColor = defBackground;

    cutters.append(new Classic4Cutter());
    cutters.append(new ClassicCutter());
    cutters.append(new SquareCutter());
    cutters.append(new RectCutter());
    cutters.append(new QuadCutter());

    for (int i=0;i<cutters.count();i++) {
        JigsawCutter * cutter = cutters.at(i);
        m_ui->cuttingCombo->addItem(cutter->getName());
    }
    m_ui->cuttingCombo->setCurrentIndex(0);
    on_cuttingCombo_activated(0);

    QPalette palette;
    palette.setColor(m_ui->backGroundButton->backgroundRole(), defBackground);
    m_ui->backGroundButton->setPalette(palette);

    m_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    m_ui->pathLineEdit->setText(prevName);
}

NewDialog::~NewDialog()
{
    for (int i=0;i<cutters.count();i++) {
        delete cutters.at(i);
    }
    delete m_ui;
}

void NewDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void NewDialog::on_pathButton_clicked() {
    QPreviewFileDialog fd(new Preview(),this);
    fd.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    fd.setWindowTitle(tr("Please select the image..."));
    QFileInfo fileInfo(m_ui->pathLineEdit->text());
    fd.setDirectory(fileInfo.exists()?fileInfo.dir():QDir::home());
    if (fd.exec() == QDialog::Rejected) return;
    QString name = fd.selectedFiles().at(0);
    if (name.isNull()) return;

    prevName = name;
    m_ui->pathLineEdit->setText(name);
}

void NewDialog::on_backGroundButton_clicked() {
    QColor color = QColorDialog::getColor (defBackground,this);
    if (!color.isValid()) return;

    selectedColor = color;
    QPalette palette;
    palette.setColor(m_ui->backGroundButton->backgroundRole(), selectedColor);
    m_ui->backGroundButton->setPalette(palette);
}

void NewDialog::on_cuttingCombo_activated(int index) {
    m_ui->cuttingLabel->setText(cutters.at(index)->getDescription());
}

void NewDialog::on_buttonBox_rejected() {
    reject();
}

void NewDialog::on_buttonBox_accepted() {
    accept();
}

QColor NewDialog::getSelectedColor() const {
    return selectedColor;
}

QString NewDialog::getSelectedPath() const {
    return m_ui->pathLineEdit->text();
}

JigsawCutter * NewDialog::getSelectedCutter() {
    return cutters.at(m_ui->cuttingCombo->currentIndex());
}

int NewDialog::getPieceCount() {
    return m_ui->pieceSpin->value();
}


void NewDialog::on_pathLineEdit_textChanged(QString text) {
    m_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!QImage(text).isNull());
}
