#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QColorTransform>
#include <QScrollBar>

#include "GprData.hpp"
#include <ios>

#define CL_HPP_MINIMUM_OPENCL_VERSION 200
#define CL_HPP_TARGET_OPENCL_VERSION 200
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-declarations"
#include <CL/cl2.hpp>
#pragma GCC diagnostic pop

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , imageLabel(new ImageLabel)
    , scrollArea(new QScrollArea)
{
    ui->setupUi(this);
    ui->actionOpen->setShortcut(QKeySequence::Open);

    connect(ui->actionOpen,      SIGNAL(triggered(bool)),        this, SLOT(on_actionOpenTriggered(bool))        );
    connect(ui->actionGrayscale, SIGNAL(triggered(bool)),        this, SLOT(on_actionGrayscaleTriggered(bool))   );
    connect(imageLabel,          SIGNAL(mouseWheelUsed(QPoint)), this, SLOT(on_mouseWheelUsed(QPoint))           );
    connect(imageLabel,          SIGNAL(mouseMoved(int,int)),    this, SLOT(on_mouseMoved(int,int))              );

    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);
    setCentralWidget(scrollArea);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::scaleImage(float factor)
{
    const float newScaleFactor = scaleFactor * factor;

    if (newScaleFactor >= maxScaleFactor or newScaleFactor <= minScaleFactor)
        return;

    scaleFactor = newScaleFactor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap(Qt::ReturnByValue).size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, float factor)
{
    scrollBar->setValue(static_cast<int>(factor * scrollBar->value() + ((factor -1) * scrollBar->pageStep()/2)));
}

void MainWindow::on_actionOpenTriggered(bool)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose file"), "./", tr("Profiles (*.asc);;Images (*.jpg *.png *.bmp)"));

    qDebug() << fileName;

    if (fileName.isNull())
        return;

    QFile file(fileName);
    try
    {
        GprData gprData{file};
        qDebug() << gprData;
    }
    catch(std::exception& e)
    {
        qDebug() << e.what();
    }

    /*QImage newImage(fileName);
    if (newImage.isNull())
    {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(), tr("Cannot open the image: %1").arg(QDir::toNativeSeparators(fileName)));
        return;
    }

    image = std::move(newImage);
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();
    scrollArea->setVisible(true);*/
}
#include <iostream>

void MainWindow::on_actionGrayscaleTriggered(bool)
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    std::vector<cl::Device> devices;
    platforms.front().getDevices(CL_DEVICE_TYPE_GPU, &devices);

    cl::Context context(devices.front());
    cl_device_id device = devices.front().get();
    cl_context myctx = context.get();

    cl_int err;

    //cl::CommandQueue commandQueue(context, devices.front());
    //cl_command_queue myqueue = commandQueue.get();

    cl_command_queue myqueue = clCreateCommandQueue(myctx, device, 0, &err);
    if (err != 0)
    {
        qDebug () << "clCreateCommandQueue err: " << err;
    }

    uchar* src = image.bits();
    cl_mem input = clCreateBuffer(myctx, CL_MEM_READ_ONLY, image.sizeInBytes(), NULL, &err);
    if (err != 0)
    {
        qDebug() << "input clCreateBuffer err: " << err;
    }

    cl_mem output = clCreateBuffer(myctx, CL_MEM_WRITE_ONLY, image.sizeInBytes(), NULL, &err);
    if (err != 0)
    {
        qDebug() << "output clCreateBuffer err: " << err;
    }

    err = clEnqueueWriteBuffer(myqueue, input, CL_TRUE, 0, image.sizeInBytes(), (void*)src, 0, NULL, NULL);
    if (err != 0)
    {
        qDebug() << "clEnqueueWriteBuffer err: " << err;
    }

    QFile file("D:\\praca_magisterska\\GeoViewer\\GeoViewer\\kernels\\test2.cl");
    file.open(QFile::ReadOnly);
    std::string fileContentStr = file.readAll().toStdString();
    const char* fileContent = fileContentStr.c_str();
    cl_program myprog = clCreateProgramWithSource(myctx, 1, (const char**)&fileContent, 0, &err);
    if (err != 0)
    {
        qDebug() << "clCreateProgramWithSource err: " << err;
    }

    err = clBuildProgram(myprog, 0, NULL, NULL, NULL, NULL);
    if (err != 0)
    {
        qDebug() << "clBuildProgram err: " << err;
    }

    cl_kernel mykernel = clCreateKernel(myprog, "rgb2gray", &err);
    if (err != 0)
    {
        qDebug() << "clCreateKernel err: " << err;
    }

    size_t width = static_cast<size_t>(image.width());
    size_t height = static_cast<size_t>(image.height());
    clSetKernelArg(mykernel, 0, sizeof(cl_mem), (void*)&input);
    clSetKernelArg(mykernel, 1, sizeof(cl_mem), (void*)&output);
    clSetKernelArg(mykernel, 2, sizeof(cl_int), (void*)&width);
    clSetKernelArg(mykernel, 3, sizeof(cl_int), (void*)&height);

    size_t localws[2] = {8, 8};
    size_t globalws[2] = {width, height};
    qDebug() << "width: " << width << " height: " << height;

    err = clEnqueueNDRangeKernel(myqueue, mykernel, 2, 0, globalws, localws, 0, NULL, NULL);
    if (err != 0)
    {
        qDebug() << "clEnqueueNDRangeKernel err: " << err;
    }

    uchar* outputBits = new uchar[image.sizeInBytes()]();
    err = clEnqueueReadBuffer(myqueue, output, CL_TRUE, 0, image.sizeInBytes(), (void*)outputBits, NULL, NULL, NULL);
    if (err != 0)
    {
        qDebug() << "clEnqueueReadBuffer err: " << err;
    }

    QImage newImage(outputBits, image.width(), image.height(), image.bytesPerLine(), image.format());
    image = std::move(newImage);
    imageLabel->setPixmap(QPixmap::fromImage(image));
}

/*void MainWindow::on_actionGrayscaleTriggered(bool)
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    std::vector<cl::Device> devices;
    platforms.front().getDevices(CL_DEVICE_TYPE_GPU, &devices);

    cl::Context context(devices.front());
    qDebug() << devices.front().getInfo<CL_DEVICE_NAME>().data();

    QFile file("D:\\praca_magisterska\\GeoViewer\\GeoViewer\\kernels\\test.cl");
    file.open(QFile::ReadOnly);
    std::string fileContent = file.readAll().toStdString();

    cl_int err;
    cl::Program program(context, fileContent, CL_TRUE, &err);
    qDebug() << "Program err: " << err;

    cl::Kernel kernel(program, "rgbToGray", &err);
    qDebug() << "Kernel err: " << err;

    cl::KernelFunctor<cl_mem> kernelFunctor(kernel);
    cl::CommandQueue commandQueue(context, devices.front());
    cl::EnqueueArgs enqueueArgs(commandQueue, cl::NDRange(image.width(), image.height()));

    cl_image_format imageFormat = {CL_sRGBA, CL_UNORM_INT8};
    uchar* tmp = image.bits();
    uchar* buffer = new uchar[image.width()*image.height()*4];
    std::memcpy(buffer, tmp, image.width()*image.height()*4);
    cl_mem srcImg = clCreateImage2D(context.get(), CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, &imageFormat, image.width(), image.height(), 0, buffer, &err);
    qDebug() << "clCreateImage2D err: " << err;

    kernelFunctor(enqueueArgs, srcImg);

    print_image(image.bits());
    //std::memcpy(buffer, srcImg, image.width()*image.height()*4);
    print_image(buffer);
    //QImage newImage(buffer, image.width(), image.height(), image.bytesPerLine(), image.format());

    //image = std::move(newImage);
    //print_image(newImage.bits());

    if (image.isNull())
    {
        qDebug() << "isNull";
        return;
    }

    imageLabel->setPixmap(QPixmap::fromImage(image));

}*/

void MainWindow::on_mouseWheelUsed(QPoint angleDelta)
{
    float factor = (angleDelta.y() > 0) ? (1.0f + scaleFactorStep) : (1.0f - scaleFactorStep);
    scaleImage(factor);
}

void MainWindow::on_mouseMoved(int x, int y)
{
    if (x not_eq 0)
    {
        QScrollBar* horizontalScrollBar = scrollArea->horizontalScrollBar();
        int currentValue = horizontalScrollBar->value();
        horizontalScrollBar->setValue(currentValue + x);
    }

    if (y != 0)
    {
        QScrollBar* verticallScrollBar = scrollArea->verticalScrollBar();
        int currentValue = verticallScrollBar->value();
        verticallScrollBar->setValue(currentValue + y);
    };
}