#include "ImageUtils.h"
#include "./ui_ImageUtils.h"
#include <QWheelEvent>
#include <QLineEdit>
#include <QValidator>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QImage>
#include <QScrollBar>
#include <QImageReader>
#include <QSharedPointer>

using namespace cv;
using namespace std;

ImageUtils::ImageUtils(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ImageUtils)
{
    ui->setupUi(this);

    QImageReader::setAllocationLimit(2048);
}

ImageUtils::~ImageUtils()
{
    delete ui;
}

void ImageUtils::imageRotate(cv::InputArray src, cv::OutputArray dst, const double &angle)
{
    double diagonal = std::sqrt(std::pow(src.cols(), 2) + std::pow(src.rows(), 2));
    double curAngle = std::atan2(src.rows(), src.cols());
    double angleRad = angle * CV_PI / 180.0f;
    double nw = std::max(std::abs(std::cos(curAngle + angleRad) * diagonal), std::abs(std::cos(curAngle - angleRad) * diagonal));
    double nh = std::max(std::abs(std::sin(curAngle + angleRad) * diagonal), std::abs(std::sin(curAngle - angleRad) * diagonal));

    cv::Mat R = cv::getRotationMatrix2D(cv::Point2f(src.cols() / 2, src.rows() / 2), angle, 1);
    R.at<double>(0,2) += (nw - src.cols()) / 2;
    R.at<double>(1,2) += (nh - src.rows()) / 2;
    cv::warpAffine(src, dst, R, cv::Size(nw , nh), cv::INTER_NEAREST);
}

void ImageUtils::imageResize(cv::InputArray src, cv::OutputArray dst, const int &x, const int &y)
{
    Mat srcImage = src.getMat();
    cv::resize(srcImage, srcImage, cv::Size(srcImage.cols + x, srcImage.rows + y), 0, 0, cv::INTER_NEAREST);
    srcImage.copyTo(dst);
}

void ImageUtils::imageRectify(cv::InputArray src, cv::OutputArray dst, const double &rate)
{
    Mat srcImage = src.getMat();
    cv::Mat xMapArray(srcImage.size(), CV_32FC1);
    cv::Mat yMapArray(srcImage.size(), CV_32FC1);
    int w = srcImage.cols;

    for (int i = 0; i < srcImage.rows; i++)
    {
        for (int j = 0; j < srcImage.cols; j++)
        {
            xMapArray.at<float>(i, j) = 1.0f / 3 * rate * std::pow(j, 3) - 1.0f / 2 * rate * w * std::pow(j, 2) + (1 + 1.0f / 6 * rate * std::pow(w, 2)) * j;
            yMapArray.at<float>(i, j) = i;
        }
    }
    cv::remap(srcImage, srcImage, xMapArray, yMapArray, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    srcImage.copyTo(dst);

}

cv::Mat ImageUtils::QImage2cvMat(const QImage &image)
{
    cv::Mat mat;
    switch(image.format())
    {
    case QImage::Format_Grayscale8: // 灰度图，每个像素点1个字节（8位）
        // Mat构造：行数，列数，存储结构，数据，step每行多少字节
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_ARGB32: // uint32存储0xAARRGGBB，pc一般小端存储低位在前，所以字节顺序就成了BGRA
    case QImage::Format_RGB32: // Alpha为FF
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888: // RR,GG,BB字节顺序存储
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        // opencv需要转为BGR的字节顺序
        cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
        break;
    case QImage::Format_RGBA64: // uint64存储，顺序和Format_ARGB32相反，RGBA
        mat = cv::Mat(image.height(), image.width(), CV_16UC4, (void*)image.constBits(), image.bytesPerLine());
        // opencv需要转为BGRA的字节顺序
        cv::cvtColor(mat, mat, cv::COLOR_RGBA2BGRA);
        break;

    default:
        std::cout << "ImageUtils::QImage2cvMat : cannot handle this format" << std::endl;
    }
    return mat;
}

QImage ImageUtils::cvMat2QImage(const cv::Mat &mat)
{
    QImage image;
    switch(mat.type())
    {
    case CV_8UC1:
        // QImage构造：数据，宽度，高度，每行多少字节，存储结构
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        break;
    case CV_8UC3:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        image = image.rgbSwapped(); // BRG转为RGB
        // Qt5.14增加了Format_BGR888
        // image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.cols * 3, QImage::Format_BGR888);
        break;
    case CV_8UC4:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        break;
    case CV_16UC4:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA64);
        image = image.rgbSwapped(); // BRG转为RGB
        break;
    }
    return image;
}

void ImageUtils::on_RectifyButton_clicked()
{
    double rectifyRate = ui->RectifyRateInput->text().toDouble();
    if(!TemplateImage.isNull()){
        cv::Mat tmp = QImage2cvMat(*TemplateImage);
        imageRectify(tmp, tmp, rectifyRate);
        *TemplateImage = cvMat2QImage(tmp);
    }
    else{
        std::cout << "NO IMAGE!" << std::endl;
    }
}

void ImageUtils::on_actionRead_Template_Image_triggered()
{
    QString textfile=QFileDialog::getOpenFileName(this);
    TemplateImage = QSharedPointer<QImage>(new QImage(textfile));
    ui->TemplateView->setPixmapItem(TemplateImage);

}



void ImageUtils::on_RotateButton_clicked()
{
    double rotateAngle = ui->RotateAngleInput->text().toDouble();
    if(!TemplateImage.isNull()){
        cv::Mat tmp = QImage2cvMat(*TemplateImage).clone();
        imageRotate(tmp, tmp, rotateAngle);
        cv::imwrite("C:/Users/user/Desktop/111.bmp", tmp);
        ui->TemplateView->setPixmapItem(cvMat2QImage(tmp));
    }
    else{
        std::cout << "NO IMAGE!" << std::endl;
    }
}

