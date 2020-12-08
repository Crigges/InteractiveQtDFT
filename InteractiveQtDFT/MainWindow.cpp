#include "MainWindow.h"
#include <qfiledialog.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qdebug.h>
#include <qslider.h>


DFTMainWindow::DFTMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
     ui.setupUi(this);

     dftEditor = new DFTEditor();

     connect(ui.action_Open, &QAction::triggered, this, [this]() {
         QString fileName = QFileDialog::getOpenFileName(this,
             tr("Open Image"), "", "Image Files (*.png *.jpg *.bmp)");
         if (!fileName.isEmpty()) {
             dftEditor->openImage(fileName.toStdString().c_str());
             updateImages();
         }   
     });

     connect(ui.brushSizeSlider, &QSlider::valueChanged, this, [this]() {
         int value = ui.brushSizeSlider->value();
         value = value % 2 == 1 ? value : value + 1;
         dftEditor->brushSize = value;
         ui.label->setText(QString::number(value));
     });

     connect(ui.brushValueSlider, &QSlider::valueChanged, this, [this]() {
         dftEditor->color = (float)ui.brushValueSlider->value() / 255.f;
     });

     connect(ui.gaussBrush, &QCheckBox::stateChanged, this, [this]() {
         dftEditor->brushGauß = ui.gaussBrush->isChecked();
     });

     connect(ui.ringEdit, &QCheckBox::stateChanged, this, [this]() {
         dftEditor->frequencyEdits = ui.ringEdit->isChecked();
     });

     connect(ui.mirrorCheckBox, &QCheckBox::stateChanged, this, [this]() {
         dftEditor->mirrorDftEdits = ui.mirrorCheckBox->isChecked();
     });

     ui.dftImage->setCallback([this](int x, int y){
         double ratioX = (double)dftEditor->dftMat.cols / ui.dftImage->pixmap()->width();
         double ratioY = (double)dftEditor->dftMat.rows / ui.dftImage->pixmap()->height();
         dftEditor->drawDft(x * ratioX, y * ratioY);
         updateImages();
     });

     ui.orginalImage->setCallback([this](int x, int y) {
         double ratioX = (double)dftEditor->orginal.cols / ui.orginalImage->pixmap()->width();
         double ratioY = (double)dftEditor->orginal.rows / ui.orginalImage->pixmap()->height();
         dftEditor->drawOriginal(x * ratioX, y * ratioY);
         updateImages();
     });

     dftEditor->openImage("marlene.png");
     updateImages();
}

void DFTMainWindow::updateImages() {
    cv::Mat mat = dftEditor->orginal;
    QPixmap org = QPixmap::fromImage(QImage((unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8));
    ui.orginalImage->setPixmap(org);

    mat = dftEditor->dftMat;
    org = QPixmap::fromImage(QImage((unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8));
    ui.dftImage->setPixmap(org);
}
