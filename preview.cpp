#include "preview.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QDataStream>
#include <QFileInfo>
#include <QFile>

Preview::Preview(QWidget *parent) : QWidget(parent) {
   pImageLabel = new QLabel(this);
   pTextLabel = new QLabel(this);
   pTextLabel->setAlignment(Qt::AlignHCenter);
   layout = new QBoxLayout(QBoxLayout::TopToBottom);
   layout->addWidget(pImageLabel);
   layout->addWidget(pTextLabel);
   setLayout(layout);
}

Preview::~Preview() {
   delete layout;
   delete pImageLabel;
   delete pTextLabel;
}

QWidget * Preview::widget() {
    return this;
}

void Preview::previewUrl(const QUrl &u) {
   QString path = u.toLocalFile();
   QPixmap pix(path);
   QString stext = tr("Size");
   if (!pix.isNull()) {
       pImageLabel->setPixmap(pix.scaled(150,(150*pix.height())/pix.width()));
       pTextLabel->setText((stext + ": %1x%2").arg(pix.width()).arg(pix.height()));
   }
   else {
       if (QFileInfo(path).suffix() == "qsw") {
           QFile file(path);
           if (file.open(QIODevice::ReadOnly)) {
               QDataStream in(&file);
               QImage image;
               in >> image;
               if (!image.isNull()) {
                    pImageLabel->setPixmap(QPixmap::fromImage(image).scaled(150,(150*image.height())/image.width()));
                    pTextLabel->setText((stext + ": %1x%2").arg(image.width()).arg(image.height()));
                    return;
               }
           }
       }
       pTextLabel->setText(tr("No selected image"));
       pImageLabel->setPixmap(QPixmap());
   }
}
