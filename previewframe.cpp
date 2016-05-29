#include "previewframe.h"
#include "ui_previewframe.h"

#include <QLabel>
#include <QPixmap>
#include <QCloseEvent>
#include <QResizeEvent>

PreviewFrame::PreviewFrame(const QImage & image,QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::PreviewFrame) {
    m_ui->setupUi(this);
    this->image = image;
    label = new QLabel();
    label->setPixmap(QPixmap::fromImage(image));
    m_ui->scrollArea->setWidget(label);
    resize(640,400);
}

PreviewFrame::~PreviewFrame()
{
    delete m_ui;
}

void PreviewFrame::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PreviewFrame::on_actionClose_triggered() {
    close();
}

void PreviewFrame::closeEvent(QCloseEvent *event) {
   emit closed();
   event->accept();
}

void PreviewFrame::on_actionFitToPage_triggered(bool checked) {
    if (checked) {
        QSize scrollSize = m_ui->scrollArea->size();
        QSize imageSize = image.size();

        int newWidth = (imageSize.width()*scrollSize.height())/imageSize.height();
        int newHeight = scrollSize.height();
        if (newWidth > scrollSize.width()) {
            newHeight = (imageSize.height()*scrollSize.width())/imageSize.width();
            newWidth = scrollSize.width();
        }

        label->setPixmap(QPixmap::fromImage(image.scaled(newWidth,newHeight)));
    }
    else {
        label->setPixmap(QPixmap::fromImage(image));
    }
}

void PreviewFrame::resizeEvent(QResizeEvent * event) {
    QMainWindow::resizeEvent(event);
    if (!m_ui->actionFitToPage->isChecked()) return;

    on_actionFitToPage_triggered(true);
}
