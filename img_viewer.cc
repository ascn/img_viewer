#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
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
#include <QPushButton>
#include <QSpacerItem>
#include <QMessageBox>
#include <QSpinBox>
#include <QProgressBar>
#include <string>
#include <cmath>
#include <float.h>
#include <iostream>
#include "ImageViewControls.h"
#include "img_viewer.h"
#include "tiny_obj_loader.h"
#include "im_op.h"
#include "mat4.h"
#include "vec4.h"

const double PI = 3.1415926;

ImageViewer::ImageViewer(QWidget *parent) :
    QMainWindow(parent) {

    this->setWindowTitle("Rasterizer and Image Viewer");

    imgLabel = new ImageViewControls(this);
    imgLabel->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *layout = new QVBoxLayout();
    // setLayout(layout); // doesn't work for main windows
    setCentralWidget(new QWidget());
    centralWidget()->setLayout(layout);

    layout->addWidget(imgLabel);
    imgLabel->show();
    connect(imgLabel, SIGNAL(rotateLeft()),
                      this, SLOT(activateRotateLeft()));
    connect(imgLabel, SIGNAL(rotateRight()),
                      this, SLOT(activateRotateRight()));
    connect(imgLabel, SIGNAL(rotateUp()),
                      this, SLOT(activateRotateUp()));
    connect(imgLabel, SIGNAL(rotateDown()),
                      this, SLOT(activateRotateDown()));
    connect(imgLabel, SIGNAL(increaseFOV()),
                      this, SLOT(activateIncreaseFOV()));
    connect(imgLabel, SIGNAL(decreaseFOV()),
                      this, SLOT(activateDecreaseFOV()));
    connect(imgLabel, SIGNAL(translateRight()),
                      this, SLOT(activateTranslateRight()));
    connect(imgLabel, SIGNAL(translateLeft()),
                      this, SLOT(activateTranslateLeft()));
    connect(imgLabel, SIGNAL(translateUp()),
                      this, SLOT(activateTranslateUp()));
    connect(imgLabel, SIGNAL(translateDown()),
                      this, SLOT(activateTranslateDown()));
    connect(imgLabel, SIGNAL(zoomIn()),
                      this, SLOT(activateZoomIn()));
    connect(imgLabel, SIGNAL(zoomOut()),
                      this, SLOT(activateZoomOut()));

    createCameraDock();
    createFilterDock();
    createActions();
    createMenus();

    img = QImage(512, 512, QImage::Format_RGB888);
    img.fill(qRgb(0, 0, 0));
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);

    objFileLabel = new QLabel(tr("No obj loaded"), this);
    layout->addWidget(objFileLabel);

    shadingGroup = new QGroupBox(tr(""));
    QHBoxLayout *h_layout = new QHBoxLayout;

    shadingOptionBox = new QComboBox(this);
    shadingOptionBox->addItem(tr("White"));
    shadingOptionBox->addItem(tr("Random"));
    shadingOptionBox->addItem(tr("Flat"));
    shadingOptionBox->addItem(tr("Gouraud"));
    shadingOptionBox->addItem(tr("Gouraud, perspective correct"));
    shadingOptionBox->addItem(tr("Barycentric"));
    shadingOptionBox->addItem(tr("Barycentric, perspective correct"));

    h_layout->addWidget(shadingOptionBox);

    rasterizeButton = new QPushButton("Rasterize", this);
    connect(rasterizeButton, SIGNAL(clicked()),
                             this, SLOT(rasterizeButtonClicked()));

    h_layout->addWidget(rasterizeButton);

    shadingGroup->setLayout(h_layout);

    layout->addWidget(shadingGroup);

    shadingOptionBox->show();
    shadingOption = WHITE;
    connect(shadingOptionBox, SIGNAL(activated(int)),
                              this, SLOT(shadingOptionChanged(int)));

    camera_init(&camera);
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

void ImageViewer::rasterizeButtonClicked() {
    if (obj_file == "") {
        QMessageBox errorBox;
        errorBox.setText("Please open an .obj file");
        errorBox.setIcon(QMessageBox::Warning);
        errorBox.exec();
    }
    addOperationForUndo();
    rasterize_wrapper();
}

void ImageViewer::cameraOptionsChanged() {
    camera.left = cam_left_box->value();
    camera.right = cam_right_box->value();
    camera.bottom = cam_bottom_box->value();
    camera.top = cam_top_box->value();
    camera.near = cam_near_box->value();
    camera.far = cam_far_box->value();
    camera.eye_x = cam_eye_x_box->value();
    camera.eye_y = cam_eye_y_box->value();
    camera.eye_z = cam_eye_z_box->value();
    camera.c_x = cam_cen_x_box->value();
    camera.c_y = cam_cen_y_box->value();
    camera.c_z = cam_cen_z_box->value();
    camera.up_x = cam_up_x_box->value();
    camera.up_y = cam_up_y_box->value();
    camera.up_z = cam_up_z_box->value();
    update_matrices(&camera);
    rasterize_wrapper();
}

void ImageViewer::saveCamera() {
    QString filename = QFileDialog::getSaveFileName(this,
            tr("Save image"), "./",
            tr("Text files (*.txt)"));
    if (filename == "") { return; }
    write_camera(filename.toStdString().c_str(), &camera);
}

void ImageViewer::open_obj() {
    obj_file = QFileDialog::getOpenFileName(this,
            tr("Open .obj..."), "./", tr("Object files (*.obj)"));
    qDebug() << obj_file;
    if (obj_file == "") { return; }
    QString tmp("Loaded .obj: ");
    tmp.append(obj_file.right(obj_file.size() - obj_file.lastIndexOf('/') - 1));
    objFileLabel->setText(tmp);
}

void ImageViewer::blockCameraOptionSignals(bool b) {
    cam_left_box->blockSignals(b);
    cam_right_box->blockSignals(b);
    cam_bottom_box->blockSignals(b);
    cam_top_box->blockSignals(b);
    cam_near_box->blockSignals(b);
    cam_far_box->blockSignals(b);
    cam_eye_x_box->blockSignals(b);
    cam_eye_y_box->blockSignals(b);
    cam_eye_z_box->blockSignals(b);
    cam_cen_x_box->blockSignals(b);
    cam_cen_y_box->blockSignals(b);
    cam_cen_z_box->blockSignals(b);
    cam_up_x_box->blockSignals(b);
    cam_up_y_box->blockSignals(b);
    cam_up_z_box->blockSignals(b);
}

void ImageViewer::open_cam() {
    QString filename = QFileDialog::getOpenFileName(this,
            tr("Open camera file"), "./", tr("Text files (*.txt)"));
    load_camera(filename.toStdString().c_str(), &camera);

    cameraChanged();
}

void ImageViewer::cameraChanged() {
    blockCameraOptionSignals(true);
    cam_left_box->setValue(camera.left);
    cam_right_box->setValue(camera.right);
    cam_bottom_box->setValue(camera.bottom);
    cam_top_box->setValue(camera.top);
    cam_near_box->setValue(camera.near);
    cam_far_box->setValue(camera.far);
    cam_eye_x_box->setValue(camera.eye_x);
    cam_eye_y_box->setValue(camera.eye_y);
    cam_eye_z_box->setValue(camera.eye_z);
    cam_cen_x_box->setValue(camera.c_x);
    cam_cen_y_box->setValue(camera.c_y);
    cam_cen_z_box->setValue(camera.c_z);
    cam_up_x_box->setValue(camera.up_x);
    cam_up_y_box->setValue(camera.up_y);
    cam_up_z_box->setValue(camera.up_z);
    blockCameraOptionSignals(false);
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
    if (filename == "") { return; }
    img.save(filename);
}

void ImageViewer::undo() {
    redoImageStack.push(img.copy());
    redoCameraStack.push(camera);
    redoAct->setEnabled(true);
    img = undoImageStack.pop();
    camera = undoCameraStack.pop();
    cameraChanged();
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);
    if (undoImageStack.isEmpty()) {
        undoAct->setEnabled(false);
    }
}

void ImageViewer::redo() {
    undoImageStack.push(img.copy());
    undoCameraStack.push(camera);
    undoAct->setEnabled(true);
    img = redoImageStack.pop();
    camera = redoCameraStack.pop();
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);
    cameraChanged();
    if (redoImageStack.isEmpty()) {
        redoAct->setEnabled(false);
    }
}

void ImageViewer::addOperationForUndo() {
    undoImageStack.push(img.copy());
    undoCameraStack.push(camera);
    undoAct->setEnabled(true);
    redoImageStack.clear();
    redoCameraStack.clear();
    redoAct->setEnabled(false);
}

void ImageViewer::rasterize_wrapper() {
    if (obj_file == "") { return; }
    img = rasterize(obj_file.toStdString().c_str(), &camera, 512, 512, shadingOption);
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);
}

void ImageViewer::grayscale_wrapper() {
    addOperationForUndo();
    grayscale(&img, filterProgress);
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);
}

void ImageViewer::flip_wrapper() {
    addOperationForUndo();
    flip(&img, filterProgress);
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);
}

void ImageViewer::flop_wrapper() {
    addOperationForUndo();
    flop(&img, filterProgress);
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);
}

void ImageViewer::transpose_wrapper() {
    addOperationForUndo();
    img = transpose(&img, filterProgress);
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);
}

void ImageViewer::boxBlur_wrapper() {
    addOperationForUndo();
    img = boxBlur(&img, boxBlurRadiusBox->value(), filterProgress);
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);
}

void ImageViewer::medianFilter_wrapper() {
    addOperationForUndo();
    img = medianFilter(&img, medianFilterRadiusBox->value(), filterProgress);
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);
}

void ImageViewer::gaussianBlur_wrapper() {
    addOperationForUndo();
    img = gaussianBlur(&img, gaussianBlurRadiusBox->value(),
                        gaussianBlurSigmaBox->value(), filterProgress);
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);
}

void ImageViewer::resize_wrapper() {
    QMessageBox errorBox;
    errorBox.setText("Not implemented");
    errorBox.setIcon(QMessageBox::Warning);
    errorBox.exec();
}

void ImageViewer::sobel_wrapper() {
    addOperationForUndo();
    img = sobel(&img, filterProgress);
    pixmap = QPixmap::fromImage(img);
    imgLabel->setPixmap(pixmap);
}

void ImageViewer::createCameraDock() {
    QGridLayout *cameraDockLayout = new QGridLayout;
    cameraDockLayout->setVerticalSpacing(5);

    cameraDockContents = new QGroupBox(tr(""));

    cam_left_box = new QDoubleSpinBox(cameraDockContents);
    cam_left_box->setRange(-1, 1);
    cam_left_box->setSingleStep(0.001);
    cam_left_box->setDecimals(6);
    cam_left_box->setKeyboardTracking(false);
    cam_right_box = new QDoubleSpinBox(cameraDockContents);
    cam_right_box->setRange(-1, 1);
    cam_right_box->setSingleStep(0.001);
    cam_right_box->setDecimals(6);
    cam_right_box->setKeyboardTracking(false);
    cam_bottom_box = new QDoubleSpinBox(cameraDockContents);
    cam_bottom_box->setRange(-1, 1);
    cam_bottom_box->setSingleStep(0.001);
    cam_bottom_box->setDecimals(6);
    cam_bottom_box->setKeyboardTracking(false);
    cam_top_box = new QDoubleSpinBox(cameraDockContents);
    cam_top_box->setRange(-1, 1);
    cam_top_box->setSingleStep(0.001);
    cam_top_box->setDecimals(6);
    cam_top_box->setKeyboardTracking(false);

    cam_near_box = new QDoubleSpinBox(cameraDockContents);
    cam_near_box->setRange(0, 100);
    cam_near_box->setSingleStep(0.01);
    cam_near_box->setKeyboardTracking(false);
    cam_far_box = new QDoubleSpinBox(cameraDockContents);
    cam_far_box->setRange(10, 1000);
    cam_far_box->setKeyboardTracking(false);

    cam_eye_x_box = new QDoubleSpinBox(cameraDockContents);
    cam_eye_x_box->setRange(-1000000000, 1000000000);
    cam_eye_x_box->setSingleStep(1);
    cam_eye_x_box->setDecimals(4);
    cam_eye_x_box->setKeyboardTracking(false);
    cam_eye_y_box = new QDoubleSpinBox(cameraDockContents);
    cam_eye_y_box->setRange(-1000000000, 1000000000);
    cam_eye_y_box->setSingleStep(1);
    cam_eye_y_box->setDecimals(4);
    cam_eye_y_box->setKeyboardTracking(false);
    cam_eye_z_box = new QDoubleSpinBox(cameraDockContents);
    cam_eye_z_box->setRange(-1000000000, 1000000000);
    cam_eye_z_box->setSingleStep(1);
    cam_eye_z_box->setDecimals(4);
    cam_eye_z_box->setKeyboardTracking(false);
    cam_cen_x_box = new QDoubleSpinBox(cameraDockContents);
    cam_cen_x_box->setRange(-1000000000, 1000000000);
    cam_cen_x_box->setSingleStep(1);
    cam_cen_x_box->setDecimals(4);
    cam_cen_x_box->setKeyboardTracking(false);
    cam_cen_y_box = new QDoubleSpinBox(cameraDockContents);
    cam_cen_y_box->setRange(-1000000000, 1000000000);
    cam_cen_y_box->setSingleStep(1);
    cam_cen_y_box->setDecimals(4);
    cam_cen_y_box->setKeyboardTracking(false);
    cam_cen_z_box = new QDoubleSpinBox(cameraDockContents);
    cam_cen_z_box->setRange(-1000000000, 1000000000);
    cam_cen_z_box->setSingleStep(1);
    cam_cen_z_box->setDecimals(4);
    cam_cen_z_box->setKeyboardTracking(false);
    cam_up_x_box = new QDoubleSpinBox(cameraDockContents);
    cam_up_x_box->setRange(-1000000000, 1000000000);
    cam_up_x_box->setSingleStep(1);
    cam_up_x_box->setDecimals(4);
    cam_up_x_box->setKeyboardTracking(false);
    cam_up_y_box = new QDoubleSpinBox(cameraDockContents);
    cam_up_y_box->setRange(-1000000000, 1000000000);
    cam_up_y_box->setSingleStep(1);
    cam_up_y_box->setDecimals(4);
    cam_up_y_box->setKeyboardTracking(false);
    cam_up_z_box = new QDoubleSpinBox(cameraDockContents);
    cam_up_z_box->setRange(-1000000000, 1000000000);
    cam_up_z_box->setSingleStep(1);
    cam_up_z_box->setDecimals(4);
    cam_up_z_box->setKeyboardTracking(false);

    cam_left_label = new QLabel(tr("Left: "), cameraDockContents);
    cam_right_label = new QLabel(tr("Right: "), cameraDockContents);
    cam_bottom_label = new QLabel(tr("Bottom: "), cameraDockContents);
    cam_top_label = new QLabel(tr("Top: "), cameraDockContents);
    cam_near_label = new QLabel(tr("Near: "), cameraDockContents);
    cam_far_label = new QLabel(tr("Far: "), cameraDockContents);
    cam_eye_x_label = new QLabel(tr("Eye (x): "), cameraDockContents);
    cam_eye_y_label = new QLabel(tr("Eye (y): "), cameraDockContents);
    cam_eye_z_label = new QLabel(tr("Eye (z): "), cameraDockContents);
    cam_cen_x_label = new QLabel(tr("Center (x): "), cameraDockContents);
    cam_cen_y_label = new QLabel(tr("Center (y): "), cameraDockContents);
    cam_cen_z_label = new QLabel(tr("Center (z): "), cameraDockContents);
    cam_up_x_label = new QLabel(tr("Up (x): "), cameraDockContents);
    cam_up_y_label = new QLabel(tr("Up (y): "), cameraDockContents);
    cam_up_z_label = new QLabel(tr("Up (z): "), cameraDockContents);

    saveCameraParamButton = new QPushButton(tr("Save camera"), cameraDockContents);

    cameraDock = new QDockWidget(tr("Camera options"), this);
    cameraDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, cameraDock);

    QSpacerItem *spacer = new QSpacerItem(
                    40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    cameraDockLayout->addWidget(cam_left_box, 0, 1);
    cameraDockLayout->addWidget(cam_right_box, 1, 1);
    cameraDockLayout->addWidget(cam_bottom_box, 2, 1);
    cameraDockLayout->addWidget(cam_top_box, 3, 1);
    cameraDockLayout->addWidget(cam_near_box, 4, 1);
    cameraDockLayout->addWidget(cam_far_box, 5, 1);
    cameraDockLayout->addWidget(cam_eye_x_box, 0, 3);
    cameraDockLayout->addWidget(cam_eye_y_box, 1, 3);
    cameraDockLayout->addWidget(cam_eye_z_box, 2, 3);
    cameraDockLayout->addWidget(cam_cen_x_box, 3, 3);
    cameraDockLayout->addWidget(cam_cen_y_box, 4, 3);
    cameraDockLayout->addWidget(cam_cen_z_box, 5, 3);
    cameraDockLayout->addWidget(cam_up_x_box, 6, 3);
    cameraDockLayout->addWidget(cam_up_y_box, 7, 3);
    cameraDockLayout->addWidget(cam_up_z_box, 8, 3);

    cameraDockLayout->addWidget(saveCameraParamButton, 8, 0, 1, 2);

    cameraDockLayout->addWidget(cam_left_label, 0, 0, Qt::AlignRight);
    cameraDockLayout->addWidget(cam_right_label, 1, 0, Qt::AlignRight);
    cameraDockLayout->addWidget(cam_bottom_label, 2, 0, Qt::AlignRight);
    cameraDockLayout->addWidget(cam_top_label, 3, 0, Qt::AlignRight);
    cameraDockLayout->addWidget(cam_near_label, 4, 0, Qt::AlignRight);
    cameraDockLayout->addWidget(cam_far_label, 5, 0, Qt::AlignRight);
    cameraDockLayout->addWidget(cam_eye_x_label, 0, 2, Qt::AlignRight);
    cameraDockLayout->addWidget(cam_eye_y_label, 1, 2, Qt::AlignRight);
    cameraDockLayout->addWidget(cam_eye_z_label, 2, 2, Qt::AlignRight);
    cameraDockLayout->addWidget(cam_cen_x_label, 3, 2, Qt::AlignRight);
    cameraDockLayout->addWidget(cam_cen_y_label, 4, 2, Qt::AlignRight);
    cameraDockLayout->addWidget(cam_cen_z_label, 5, 2, Qt::AlignRight);
    cameraDockLayout->addWidget(cam_up_x_label, 6, 2, Qt::AlignRight);
    cameraDockLayout->addWidget(cam_up_y_label, 7, 2, Qt::AlignRight);
    cameraDockLayout->addWidget(cam_up_z_label, 8, 2, Qt::AlignRight);

    cameraDockLayout->addItem(spacer, 9, 0, -1, -1, Qt::AlignTop);

    cameraDockContents->setLayout(cameraDockLayout);
    cameraDock->setWidget(cameraDockContents);

    connect(cam_left_box, SIGNAL(valueChanged(double)),
                          this, SLOT(cameraOptionsChanged()));
    connect(cam_right_box, SIGNAL(valueChanged(double)),
                          this, SLOT(cameraOptionsChanged()));
    connect(cam_bottom_box, SIGNAL(valueChanged(double)),
                          this, SLOT(cameraOptionsChanged()));
    connect(cam_top_box, SIGNAL(valueChanged(double)),
                          this, SLOT(cameraOptionsChanged()));
    connect(cam_near_box, SIGNAL(valueChanged(double)),
                          this, SLOT(cameraOptionsChanged()));
    connect(cam_far_box, SIGNAL(valueChanged(double)),
                          this, SLOT(cameraOptionsChanged()));
    connect(cam_eye_x_box, SIGNAL(valueChanged(double)),
                          this, SLOT(cameraOptionsChanged()));
    connect(cam_eye_y_box, SIGNAL(valueChanged(double)),
                          this, SLOT(cameraOptionsChanged()));
    connect(cam_eye_z_box, SIGNAL(valueChanged(double)),
                          this, SLOT(cameraOptionsChanged()));
    connect(cam_cen_x_box, SIGNAL(valueChanged(double)),
                          this, SLOT(cameraOptionsChanged()));
    connect(cam_cen_y_box, SIGNAL(valueChanged(double)),
                          this, SLOT(cameraOptionsChanged()));
    connect(cam_cen_z_box, SIGNAL(valueChanged(double)),
                          this, SLOT(cameraOptionsChanged()));
    connect(cam_up_x_box, SIGNAL(valueChanged(double)),
                          this, SLOT(cameraOptionsChanged()));
    connect(cam_up_y_box, SIGNAL(valueChanged(double)),
                          this, SLOT(cameraOptionsChanged()));
    connect(cam_up_z_box, SIGNAL(valueChanged(double)),
                          this, SLOT(cameraOptionsChanged()));
    connect(saveCameraParamButton, SIGNAL(clicked()),
                                   this, SLOT(saveCamera()));
}

void ImageViewer::createFilterDock() {
    QGridLayout *filterDockLayout = new QGridLayout;
    filterDockLayout->setVerticalSpacing(5);
    filterDockContents = new QGroupBox(tr(""));

    grayscaleButton = new QPushButton(tr("Grayscale"), filterDockContents);
    flipButton = new QPushButton(tr("Flip"), filterDockContents);
    flopButton = new QPushButton(tr("Flop"), filterDockContents);
    transposeButton = new QPushButton(tr("Transpose"), filterDockContents);
    boxBlurButton = new QPushButton(tr("Box blur"), filterDockContents);
    medianFilterButton = new QPushButton(tr("Median filter"), filterDockContents);
    gaussianBlurButton = new QPushButton(tr("Gaussian blur"), filterDockContents);
    sobelButton = new QPushButton(tr("Sobel"), filterDockContents);
    resizeButton = new QPushButton(tr("Resize"), filterDockContents);

    boxBlurRadiusBox = new QSpinBox(filterDockContents);
    boxBlurRadiusBox->setRange(1, 255);
    medianFilterRadiusBox = new QSpinBox(filterDockContents);
    medianFilterRadiusBox->setRange(1, 255);
    gaussianBlurRadiusBox = new QSpinBox(filterDockContents);
    gaussianBlurRadiusBox->setRange(1, 255);
    gaussianBlurSigmaBox = new QDoubleSpinBox(filterDockContents);
    gaussianBlurSigmaBox->setValue(1);
    resizeWidthBox = new QSpinBox(filterDockContents);
    resizeHeightBox = new QSpinBox(filterDockContents);

    boxBlurRadiusLabel = new QLabel(tr("Radius: "), filterDockContents);
    medianFilterRadiusLabel = new QLabel(tr("Radius: "), filterDockContents);
    gaussianBlurRadiusLabel = new QLabel(tr("Radius: "), filterDockContents);
    gaussianBlurSigmaLabel = new QLabel(tr("Sigma: "), filterDockContents);
    resizeWidthLabel = new QLabel(tr("Width: "), filterDockContents);
    resizeHeightLabel = new QLabel(tr("Height: "), filterDockContents);

    filterDockLayout->addWidget(grayscaleButton, 0, 0, 1, 2);
    filterDockLayout->addWidget(flipButton, 1, 0, 1, 2);
    filterDockLayout->addWidget(flopButton, 2, 0, 1, 2);
    filterDockLayout->addWidget(transposeButton, 3, 0, 1, 2);
    filterDockLayout->addWidget(boxBlurButton, 4, 0, 1, 2);
    filterDockLayout->addWidget(boxBlurRadiusLabel, 5, 0, 1, 1, Qt::AlignRight);
    filterDockLayout->addWidget(boxBlurRadiusBox, 5, 1, 1, 1);
    filterDockLayout->addWidget(sobelButton, 6, 0, 1, 2);
    filterDockLayout->addWidget(medianFilterButton, 0, 2, 1, 2);
    filterDockLayout->addWidget(medianFilterRadiusLabel, 1, 2, 1, 1, Qt::AlignRight);
    filterDockLayout->addWidget(medianFilterRadiusBox, 1, 3, 1, 1);
    filterDockLayout->addWidget(gaussianBlurButton, 2, 2, 1, 2);
    filterDockLayout->addWidget(gaussianBlurRadiusLabel, 3, 2, 1, 1, Qt::AlignRight);
    filterDockLayout->addWidget(gaussianBlurRadiusBox, 3, 3, 1, 1);
    filterDockLayout->addWidget(gaussianBlurSigmaLabel, 4, 2, 1, 1, Qt::AlignRight);
    filterDockLayout->addWidget(gaussianBlurSigmaBox, 4, 3, 1, 1);
    filterDockLayout->addWidget(resizeButton, 5, 2, 1, 2);
    filterDockLayout->addWidget(resizeWidthLabel, 6, 2, 1, 1, Qt::AlignRight);
    filterDockLayout->addWidget(resizeWidthBox, 6, 3, 1, 1);
    filterDockLayout->addWidget(resizeHeightLabel, 7, 2, 1, 1, Qt::AlignRight);
    filterDockLayout->addWidget(resizeHeightBox, 7, 3, 1, 1);

    filterProgress = new QProgressBar(filterDockContents);
    filterProgress->setValue(0);
    filterDockLayout->addWidget(filterProgress, 8, 0, 1, -1);

    QSpacerItem *spacer = new QSpacerItem(
                    40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    filterDockLayout->addItem(spacer, 9, 0, -1, -1, Qt::AlignTop);

    filterDock = new QDockWidget(tr("Filters"), this);
    filterDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, filterDock);
    filterDockContents->setLayout(filterDockLayout);
    filterDock->setWidget(filterDockContents);

    connect(grayscaleButton, SIGNAL(clicked()),
                             this, SLOT(grayscale_wrapper()));
    connect(flipButton, SIGNAL(clicked()),
                        this, SLOT(flip_wrapper()));
    connect(flopButton, SIGNAL(clicked()),
                        this, SLOT(flop_wrapper()));
    connect(transposeButton, SIGNAL(clicked()),
                             this, SLOT(transpose_wrapper()));
    connect(boxBlurButton, SIGNAL(clicked()),
                           this, SLOT(boxBlur_wrapper()));
    connect(medianFilterButton, SIGNAL(clicked()),
                                this, SLOT(medianFilter_wrapper()));
    connect(sobelButton, SIGNAL(clicked()),
                         this, SLOT(sobel_wrapper()));
    connect(gaussianBlurButton, SIGNAL(clicked()),
                                this, SLOT(gaussianBlur_wrapper()));
    connect(resizeButton, SIGNAL(clicked()),
                          this, SLOT(resize_wrapper()));
}

void ImageViewer::activateRotateLeft() {
    addOperationForUndo();
    vec4 cen = vec4(cam_cen_x_box->value(), cam_cen_y_box->value(), 
            cam_cen_z_box->value(), 0);
    vec4 eye = vec4(cam_eye_x_box->value(), cam_eye_y_box->value(),
            cam_eye_z_box->value(), 0);
    vec4 up = vec4(cam_up_x_box->value(), cam_up_y_box->value(),
            cam_up_z_box->value(), 0);
    vec4 lookAt = cen - eye;
    mat4 rotation = mat4::rot(2, up[0], up[1], up[2]);
    vec4 rotated = rotation * lookAt;
    blockCameraOptionSignals(true);
    cam_cen_x_box->setValue(rotated[0]);
    cam_cen_y_box->setValue(rotated[1]);
    cam_cen_z_box->setValue(rotated[2]);
    blockCameraOptionSignals(false);
    cameraOptionsChanged();
}

void ImageViewer::activateRotateRight() {
    addOperationForUndo();
    vec4 cen = vec4(cam_cen_x_box->value(), cam_cen_y_box->value(), 
            cam_cen_z_box->value(), 0);
    vec4 eye = vec4(cam_eye_x_box->value(), cam_eye_y_box->value(),
            cam_eye_z_box->value(), 0);
    vec4 up = vec4(cam_up_x_box->value(), cam_up_y_box->value(),
            cam_up_z_box->value(), 0);
    vec4 lookAt = cen - eye;
    mat4 rotation = mat4::rot(-2, up[0], up[1], up[2]);
    vec4 rotated = rotation * lookAt;
    blockCameraOptionSignals(true);
    cam_cen_x_box->setValue(rotated[0]);
    cam_cen_y_box->setValue(rotated[1]);
    cam_cen_z_box->setValue(rotated[2]);
    blockCameraOptionSignals(false);
    cameraOptionsChanged();
}

void ImageViewer::activateRotateUp() {
    addOperationForUndo();
    vec4 cen = vec4(cam_cen_x_box->value(), cam_cen_y_box->value(), 
            cam_cen_z_box->value(), 0);
    vec4 eye = vec4(cam_eye_x_box->value(), cam_eye_y_box->value(),
            cam_eye_z_box->value(), 0);
    vec4 up = vec4(cam_up_x_box->value(), cam_up_y_box->value(),
            cam_up_z_box->value(), 0);
    vec4 lookAt = cen - eye;
    vec4 right = cross(lookAt, up);
    mat4 rotation = mat4::rot(2, right[0], right[1], right[2]);
    vec4 rotated = rotation * lookAt;
    vec4 up_rot = rotation * up;
    up_rot.norm();
    blockCameraOptionSignals(true);
    cam_up_x_box->setValue(up_rot[0]);
    cam_up_y_box->setValue(up_rot[1]);
    cam_up_z_box->setValue(up_rot[2]);
    blockCameraOptionSignals(false);
    cameraOptionsChanged();
}

void ImageViewer::activateRotateDown() {
    addOperationForUndo();
    vec4 cen = vec4(cam_cen_x_box->value(), cam_cen_y_box->value(), 
            cam_cen_z_box->value(), 0);
    vec4 eye = vec4(cam_eye_x_box->value(), cam_eye_y_box->value(),
            cam_eye_z_box->value(), 0);
    vec4 up = vec4(cam_up_x_box->value(), cam_up_y_box->value(),
            cam_up_z_box->value(), 0);
    vec4 lookAt = cen - eye;
    vec4 right = cross(lookAt, up);
    mat4 rotation = mat4::rot(-2, right[0], right[1], right[2]);
    vec4 rotated = rotation * lookAt;
    vec4 up_rot = rotation * up;
    up_rot.norm();
    blockCameraOptionSignals(true);
    cam_up_x_box->setValue(up_rot[0]);
    cam_up_y_box->setValue(up_rot[1]);
    cam_up_z_box->setValue(up_rot[2]);
    blockCameraOptionSignals(false);
    cameraOptionsChanged();
}

void ImageViewer::activateIncreaseFOV() {

}

void ImageViewer::activateDecreaseFOV() {

}

void ImageViewer::activateTranslateRight() {
    addOperationForUndo();
    vec4 cen = vec4(cam_cen_x_box->value(), cam_cen_y_box->value(), 
            cam_cen_z_box->value(), 0);
    vec4 eye = vec4(cam_eye_x_box->value(), cam_eye_y_box->value(),
            cam_eye_z_box->value(), 0);
    vec4 up = vec4(cam_up_x_box->value(), cam_up_y_box->value(),
            cam_up_z_box->value(), 0);
    vec4 lookAt = cen - eye;
    vec4 right = cross(lookAt, up);
    right.norm();
    right /= 10;
    blockCameraOptionSignals(true);
    cam_cen_x_box->setValue(cam_cen_x_box->value() + right[0]);
    cam_cen_y_box->setValue(cam_cen_y_box->value() + right[1]);
    cam_cen_z_box->setValue(cam_cen_z_box->value() + right[2]);
    cam_eye_x_box->setValue(cam_eye_x_box->value() + right[0]);
    cam_eye_y_box->setValue(cam_eye_y_box->value() + right[1]);
    cam_eye_z_box->setValue(cam_eye_z_box->value() + right[2]);
    blockCameraOptionSignals(false);
    cameraOptionsChanged();
}

void ImageViewer::activateTranslateLeft() {
    addOperationForUndo();
    vec4 cen = vec4(cam_cen_x_box->value(), cam_cen_y_box->value(), 
            cam_cen_z_box->value(), 0);
    vec4 eye = vec4(cam_eye_x_box->value(), cam_eye_y_box->value(),
            cam_eye_z_box->value(), 0);
    vec4 up = vec4(cam_up_x_box->value(), cam_up_y_box->value(),
            cam_up_z_box->value(), 0);
    vec4 lookAt = cen - eye;
    vec4 right = cross(lookAt, up);
    right.norm();
    right /= 10;
    blockCameraOptionSignals(true);
    cam_cen_x_box->setValue(cam_cen_x_box->value() - right[0]);
    cam_cen_y_box->setValue(cam_cen_y_box->value() - right[1]);
    cam_cen_z_box->setValue(cam_cen_z_box->value() - right[2]);
    cam_eye_x_box->setValue(cam_eye_x_box->value() - right[0]);
    cam_eye_y_box->setValue(cam_eye_y_box->value() - right[1]);
    cam_eye_z_box->setValue(cam_eye_z_box->value() - right[2]);
    blockCameraOptionSignals(false);
    cameraOptionsChanged();
}

void ImageViewer::activateTranslateUp() {
    addOperationForUndo();
    vec4 up = vec4(cam_up_x_box->value(), cam_up_y_box->value(),
            cam_up_z_box->value(), 0);
    up.norm();
    up /= 10;
    blockCameraOptionSignals(true);
    cam_cen_x_box->setValue(cam_cen_x_box->value() + up[0]);
    cam_cen_y_box->setValue(cam_cen_y_box->value() + up[1]);
    cam_cen_z_box->setValue(cam_cen_z_box->value() + up[2]);
    cam_eye_x_box->setValue(cam_eye_x_box->value() + up[0]);
    cam_eye_y_box->setValue(cam_eye_y_box->value() + up[1]);
    cam_eye_z_box->setValue(cam_eye_z_box->value() + up[2]);
    blockCameraOptionSignals(false);
    cameraOptionsChanged();
}

void ImageViewer::activateTranslateDown() {
    addOperationForUndo();
    vec4 up = vec4(cam_up_x_box->value(), cam_up_y_box->value(),
            cam_up_z_box->value(), 0);
    up.norm();
    up /= 10;
    blockCameraOptionSignals(true);
    cam_cen_x_box->setValue(cam_cen_x_box->value() - up[0]);
    cam_cen_y_box->setValue(cam_cen_y_box->value() - up[1]);
    cam_cen_z_box->setValue(cam_cen_z_box->value() - up[2]);
    cam_eye_x_box->setValue(cam_eye_x_box->value() - up[0]);
    cam_eye_y_box->setValue(cam_eye_y_box->value() - up[1]);
    cam_eye_z_box->setValue(cam_eye_z_box->value() - up[2]);
    blockCameraOptionSignals(false);
    cameraOptionsChanged();
}

void ImageViewer::activateZoomIn() {
    addOperationForUndo();
    vec4 cen = vec4(cam_cen_x_box->value(), cam_cen_y_box->value(),
            cam_cen_z_box->value(), 0);
    vec4 eye = vec4(cam_eye_x_box->value(), cam_eye_y_box->value(),
            cam_eye_z_box->value(), 0);
    vec4 lookAt = cen - eye;
    lookAt.norm();
    lookAt /= 10;
    blockCameraOptionSignals(true);
    cam_cen_x_box->setValue(cam_cen_x_box->value() + lookAt[0]);
    cam_cen_y_box->setValue(cam_cen_y_box->value() + lookAt[1]);
    cam_cen_z_box->setValue(cam_cen_z_box->value() + lookAt[2]);
    cam_eye_x_box->setValue(cam_eye_x_box->value() + lookAt[0]);
    cam_eye_y_box->setValue(cam_eye_y_box->value() + lookAt[1]);
    cam_eye_z_box->setValue(cam_eye_z_box->value() + lookAt[2]);
    blockCameraOptionSignals(false);
    cameraOptionsChanged();
}

void ImageViewer::activateZoomOut() {
    addOperationForUndo();
    vec4 cen = vec4(cam_cen_x_box->value(), cam_cen_y_box->value(),
            cam_cen_z_box->value(), 0);
    vec4 eye = vec4(cam_eye_x_box->value(), cam_eye_y_box->value(),
            cam_eye_z_box->value(), 0);
    vec4 lookAt = cen - eye;
    lookAt.norm();
    lookAt /= 10;
    blockCameraOptionSignals(true);
    cam_cen_x_box->setValue(cam_cen_x_box->value() - lookAt[0]);
    cam_cen_y_box->setValue(cam_cen_y_box->value() - lookAt[1]);
    cam_cen_z_box->setValue(cam_cen_z_box->value() - lookAt[2]);
    cam_eye_x_box->setValue(cam_eye_x_box->value() - lookAt[0]);
    cam_eye_y_box->setValue(cam_eye_y_box->value() - lookAt[1]);
    cam_eye_z_box->setValue(cam_eye_z_box->value() - lookAt[2]);
    blockCameraOptionSignals(false);
    cameraOptionsChanged();
}

void ImageViewer::createActions() {
    openObjAct = new QAction(tr("&Open .obj..."), this);
    openObjAct->setShortcuts(QKeySequence::Open);
    openObjAct->setStatusTip(tr("Open an .obj file"));
    connect(openObjAct, &QAction::triggered, this, &ImageViewer::open_obj);

    openCamAct = new QAction(tr("Open camera file"), this);
    openCamAct->setStatusTip(tr("Open a camera file"));
    connect(openCamAct, &QAction::triggered, this, &ImageViewer::open_cam);

    openImgAct = new QAction(tr("Open image"), this);
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

    undoAct = new QAction(tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo the last image operation"));
    connect(undoAct, &QAction::triggered, this, &ImageViewer::undo);
    undoAct->setEnabled(false);

    redoAct = new QAction(tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo the last image operation"));
    connect(redoAct, &QAction::triggered, this, &ImageViewer::redo);
    redoAct->setEnabled(false);
}

void ImageViewer::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openObjAct);
    fileMenu->addAction(openCamAct);
    fileMenu->addAction(openImgAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveImgAct);
    fileMenu->addAction(rasterizeAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);    
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    ImageViewer *imgViewer = new ImageViewer();
    imgViewer->show();

    return app.exec();
}
