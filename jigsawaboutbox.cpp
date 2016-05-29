#include "jigsawaboutbox.h"
#include "ui_jigsawaboutbox.h"

#include <QTextCursor>
#include <QFile>
#include <QTextStream>

extern QString _locale;

JigsawAboutBox::JigsawAboutBox(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::JigsawAboutBox) {
    m_ui->setupUi(this);
    QFile file(QString(":/txt/copying_")+_locale);
    if (!file.open(QIODevice::ReadOnly)) {
        file.setFileName(":/txt/copying_en");
        if (!file.open(QIODevice::ReadOnly)) return;
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    m_ui->plainText->appendPlainText(in.readAll());
    QTextCursor cursor = m_ui->plainText->textCursor();
    cursor.movePosition(QTextCursor::Start);
    m_ui->plainText->setTextCursor(cursor);
}

JigsawAboutBox::~JigsawAboutBox() {
    delete m_ui;
}

void JigsawAboutBox::changeEvent(QEvent *e) {
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void JigsawAboutBox::on_closeButton_clicked() {
    reject();
}
