#include <QApplication>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QPixmap>
#include <QDebug>
#include <string>

#include "img_viewer.h"
#include "tiny_obj_loader.h"

ImageViewer::ImageViewer(QWidget *parent) :
    QMainWindow(parent), channels(0) {
    rCheck = new QCheckBox("&Red", this);
    gCheck = new QCheckBox("&Green", this);
    bCheck = new QCheckBox("&Blue", this);
    imgLabel = new QLabel(this);

    QVBoxLayout *layout = new QVBoxLayout();
    // setLayout(layout); // doesn't work for main windows
    setCentralWidget(new QWidget());
    centralWidget()->setLayout(layout);

    layout->addWidget(rCheck);
    layout->addWidget(gCheck);
    layout->addWidget(bCheck);

    layout->addWidget(imgLabel);

    rCheck->show();
    gCheck->show();
    bCheck->show();

    imgLabel->show();

    createActions();
    createMenus();

    img.load(QCoreApplication::arguments()[1]);
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);
    
    connect(rCheck, SIGNAL(stateChanged(int)),
                    this, SLOT(redStateChanged(int)));
    connect(gCheck, SIGNAL(stateChanged(int)),
                    this, SLOT(greenStateChanged(int)));
    connect(bCheck, SIGNAL(stateChanged(int)),
                    this, SLOT(blueStateChanged(int)));
}

ImageViewer::~ImageViewer() {
    // delete rCheck;
    // delete gCheck;
    // delete bCheck;

    // delete imgLabel;
}

void ImageViewer::redStateChanged(int state) {
    if (state == Qt::Unchecked) {
        channels &= ~RED;
    } else {
        channels |=  RED;
    }

    qDebug() << channels;
    
    updateLabel();
}

void ImageViewer::greenStateChanged(int state) {
    if (state == Qt::Unchecked) {
        channels &= ~GREEN;
    } else {
        channels |=  GREEN;
    }

    qDebug() << channels;
    
    updateLabel();
}

void ImageViewer::blueStateChanged(int state) {
    if (state == Qt::Unchecked) {
        channels &= ~BLUE;
    } else {
        channels |=  BLUE;
    }

    qDebug() << channels;
    
    updateLabel();
}

void ImageViewer::updateLabel() {
    imgLabel->setText(QString::number(channels));
}

void ImageViewer::open_obj() {
    obj_file = QFileDialog::getOpenFileName(this,
            tr("Open .obj..."), "./", tr("Object files (*.obj)"));
    qDebug() << obj_file;
    if (obj_file == "") { return; }
}

void ImageViewer::open_cam() {
    QString filename = QFileDialog::getOpenFileName(this,
            tr("Open camera file"), "./", tr("Text files (*.txt)"));
    qDebug() << filename;
    camera = load_camera(filename.toStdString().c_str());
}

void ImageViewer::open_img() {
    QString filename = QFileDialog::getOpenFileName(this,
            tr("Open image"), "./", tr("Image files (*.ppm *.png *.jpg *.bmp)"));
    if (filename == "") { return; }
    img.load(filename);
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);
}

void ImageViewer::save() {
    QString filename = QFileDialog::getSaveFileName(this,
            tr("Save image"), "./",
            tr("Image files (*.ppm *.png *.jpg *.bmp)"));
    qDebug() << filename;
    if (filename == "") { return; }
    img.save(filename);

}

void ImageViewer::rasterize_wrapper() {
    img = rasterize(obj_file.toStdString().c_str(), camera, 512, 512, WHITE);
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);
}

void ImageViewer::createActions() {
    openObjAct = new QAction(tr("&Open .obj..."), this);
    openObjAct->setShortcuts(QKeySequence::Open);
    openObjAct->setStatusTip(tr("Open an .obj file"));
    connect(openObjAct, &QAction::triggered, this, &ImageViewer::open_obj);

    openCamAct = new QAction(tr("&Open camera file"), this);
    openCamAct->setStatusTip(tr("Open a camera file"));
    connect(openCamAct, &QAction::triggered, this, &ImageViewer::open_cam);

    openImgAct = new QAction(tr("&Open image"), this);
    openImgAct->setStatusTip(tr("Open an image"));
    connect(openImgAct, &QAction::triggered, this, &ImageViewer::open_img);

    saveImgAct = new QAction(tr("&Save image"), this);
    saveImgAct->setShortcuts(QKeySequence::Save);
    saveImgAct->setStatusTip(tr("Save image to disk"));
    connect(saveImgAct, &QAction::triggered, this, &ImageViewer::save);

    rasterizeAct = new QAction(tr("&Rasterize"), this);
    rasterizeAct->setShortcut(tr("Ctrl+R"));
    rasterizeAct->setStatusTip(tr("Rasterize .obj"));
    connect(rasterizeAct, &QAction::triggered, this, &ImageViewer::rasterize_wrapper);

}

void ImageViewer::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openObjAct);
    fileMenu->addAction(openCamAct);
    fileMenu->addAction(openImgAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveImgAct);
    fileMenu->addAction(rasterizeAct);
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    // do your own argument parsing

    ImageViewer *imgViewer = new ImageViewer();
    imgViewer->show();

    return app.exec();
}
