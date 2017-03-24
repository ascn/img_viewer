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
#include <string>

#include "img_viewer.h"
#include "tiny_obj_loader.h"

ImageViewer::ImageViewer(QWidget *parent) :
    QMainWindow(parent) {
    imgLabel = new QLabel(this);

    QVBoxLayout *layout = new QVBoxLayout();
    // setLayout(layout); // doesn't work for main windows
    setCentralWidget(new QWidget());
    centralWidget()->setLayout(layout);

    layout->addWidget(imgLabel);

    imgLabel->show();

    QGridLayout *g_layout = new QGridLayout;
    qDebug() << g_layout->verticalSpacing();
    g_layout->setVerticalSpacing(5);
    qDebug() << g_layout->verticalSpacing();

    cameraDockContents = new QGroupBox(tr(""));

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
    cam_eye_y_box = new QDoubleSpinBox(cameraDockContents);
    cam_eye_y_box->setRange(-100, 100);
    cam_eye_y_box->setSingleStep(1);
    cam_eye_z_box = new QDoubleSpinBox(cameraDockContents);
    cam_eye_z_box->setRange(-100, 100);
    cam_eye_z_box->setSingleStep(1);
    cam_cen_x_box = new QDoubleSpinBox(cameraDockContents);
    cam_cen_x_box->setRange(-100, 100);
    cam_cen_x_box->setSingleStep(1);
    cam_cen_y_box = new QDoubleSpinBox(cameraDockContents);
    cam_cen_y_box->setRange(-100, 100);
    cam_cen_y_box->setSingleStep(1);
    cam_cen_z_box = new QDoubleSpinBox(cameraDockContents);
    cam_cen_z_box->setRange(-100, 100);
    cam_cen_z_box->setSingleStep(1);
    cam_up_x_box = new QDoubleSpinBox(cameraDockContents);
    cam_up_x_box->setRange(-100, 100);
    cam_up_x_box->setSingleStep(1);
    cam_up_y_box = new QDoubleSpinBox(cameraDockContents);
    cam_up_y_box->setRange(-100, 100);
    cam_up_y_box->setSingleStep(1);
    cam_up_z_box = new QDoubleSpinBox(cameraDockContents);
    cam_up_z_box->setRange(-100, 100);
    cam_up_z_box->setSingleStep(1);

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

    cameraDock = new QDockWidget(tr("Camera options"), this);
    cameraDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, cameraDock);

    QSpacerItem *spacer = new QSpacerItem(
                    40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    g_layout->addWidget(cam_left_box, 0, 1);
    g_layout->addWidget(cam_right_box, 1, 1);
    g_layout->addWidget(cam_bottom_box, 2, 1);
    g_layout->addWidget(cam_top_box, 3, 1);
    g_layout->addWidget(cam_near_box, 4, 1);
    g_layout->addWidget(cam_far_box, 5, 1);
    g_layout->addWidget(cam_eye_x_box, 0, 3);
    g_layout->addWidget(cam_eye_y_box, 1, 3);
    g_layout->addWidget(cam_eye_z_box, 2, 3);
    g_layout->addWidget(cam_cen_x_box, 3, 3);
    g_layout->addWidget(cam_cen_y_box, 4, 3);
    g_layout->addWidget(cam_cen_z_box, 5, 3);
    g_layout->addWidget(cam_up_x_box, 6, 3);
    g_layout->addWidget(cam_up_y_box, 7, 3);
    g_layout->addWidget(cam_up_z_box, 8, 3);

    g_layout->addWidget(cam_left_label, 0, 0, Qt::AlignRight);
    g_layout->addWidget(cam_right_label, 1, 0, Qt::AlignRight);
    g_layout->addWidget(cam_bottom_label, 2, 0, Qt::AlignRight);
    g_layout->addWidget(cam_top_label, 3, 0, Qt::AlignRight);
    g_layout->addWidget(cam_near_label, 4, 0, Qt::AlignRight);
    g_layout->addWidget(cam_far_label, 5, 0, Qt::AlignRight);
    g_layout->addWidget(cam_eye_x_label, 0, 2, Qt::AlignRight);
    g_layout->addWidget(cam_eye_y_label, 1, 2, Qt::AlignRight);
    g_layout->addWidget(cam_eye_z_label, 2, 2, Qt::AlignRight);
    g_layout->addWidget(cam_cen_x_label, 3, 2, Qt::AlignRight);
    g_layout->addWidget(cam_cen_y_label, 4, 2, Qt::AlignRight);
    g_layout->addWidget(cam_cen_z_label, 5, 2, Qt::AlignRight);
    g_layout->addWidget(cam_up_x_label, 6, 2, Qt::AlignRight);
    g_layout->addWidget(cam_up_y_label, 7, 2, Qt::AlignRight);
    g_layout->addWidget(cam_up_z_label, 8, 2, Qt::AlignRight);

    g_layout->addItem(spacer, 9, 0, -1, -1, Qt::AlignTop);

    cameraDockContents->setLayout(g_layout);
    cameraDock->setWidget(cameraDockContents);

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
    rasterize_wrapper();
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

void ImageViewer::open_cam() {
    QString filename = QFileDialog::getOpenFileName(this,
            tr("Open camera file"), "./", tr("Text files (*.txt)"));
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
