#include <QApplication>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QPixmap>
#include <QComboBox>
#include <QDockWidget>
#include <QDebug>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <string>

#include "img_viewer.h"
#include "tiny_obj_loader.h"

ImageViewer::ImageViewer(QWidget *parent) :
    QMainWindow(parent), channels(0) {
    imgLabel = new QLabel(this);

    QVBoxLayout *layout = new QVBoxLayout();
    // setLayout(layout); // doesn't work for main windows
    setCentralWidget(new QWidget());
    centralWidget()->setLayout(layout);

    layout->addWidget(imgLabel);

    imgLabel->show();

    cameraDockContents = new QGroupBox(tr("Camera options"));

    cam_left_box = new QDoubleSpinBox(cameraDockContents);
    cam_left_box->setRange(-1, 1);
    cam_left_box->setSingleStep(0.01);
    cam_left_box->setDecimals(6);
    cam_right_box = new QDoubleSpinBox(cameraDockContents);
    cam_right_box->setRange(-1, 1);
    cam_right_box->setSingleStep(0.01);
    cam_right_box->setDecimals(6);
    cam_bottom_box = new QDoubleSpinBox(cameraDockContents);
    cam_bottom_box->setRange(-1, 1);
    cam_bottom_box->setSingleStep(0.01);
    cam_bottom_box->setDecimals(6);
    cam_top_box = new QDoubleSpinBox(cameraDockContents);
    cam_top_box->setRange(-1, 1);
    cam_top_box->setSingleStep(0.01);
    cam_top_box->setDecimals(6);

    cam_near_box = new QDoubleSpinBox(cameraDockContents);
    cam_near_box->setRange(0.1, 100);
    cam_far_box = new QDoubleSpinBox(cameraDockContents);
    cam_far_box->setRange(10, 1000);

    cam_eye_x_box = new QDoubleSpinBox(cameraDockContents);
    cam_eye_x_box->setRange(-100, 100);
    cam_eye_x_box->setSingleStep(1);


    cameraDock->addWidget(cameraDockContents);

    cameraDock = new QDockWidget(tr("Camera options"), this);
    cameraDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, cameraDock);

    createActions();
    createMenus();

    img.load(QCoreApplication::arguments()[1]);
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);

    shadingOptionBox = new QComboBox(this);
    shadingOptionBox->addItem(tr("White"));
    shadingOptionBox->addItem(tr("Random"));
    shadingOptionBox->addItem(tr("Flat"));
    shadingOptionBox->addItem(tr("Gouraud"));
    shadingOptionBox->addItem(tr("Gouraud, perspective correct"));
    shadingOptionBox->addItem(tr("Barycentric"));
    shadingOptionBox->addItem(tr("Barycentric, perspective correct"));

    layout->addWidget(shadingOptionBox);

    shadingOptionBox->show();
    shadingOption = WHITE;
    connect(shadingOptionBox, SIGNAL(activated(int)),
                              this, SLOT(shadingOptionChanged(int)));
}

ImageViewer::~ImageViewer() {
}

void ImageViewer::shadingOptionChanged(int index) {
    switch (index) {
    case 0:
        shadingOption = WHITE;
        break;
    case 1:
        shadingOption = RANDOM;
        break;
    case 2:
        shadingOption = NORM_FLAT;
        break;
    case 3:
        shadingOption = NORM_GOURAUD;
        break;
    case 4:
        shadingOption = NORM_GOURAUD_Z;
        break;
    case 5:
        shadingOption = NORM_BARY;
        break;
    case 6:
        shadingOption = NORM_BARY_Z;
        break;
    default:
        break;
    }
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
    img = rasterize(obj_file.toStdString().c_str(), camera, 512, 512, shadingOption);
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
