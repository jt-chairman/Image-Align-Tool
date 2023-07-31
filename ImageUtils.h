#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QDebug>


QT_BEGIN_NAMESPACE
namespace Ui { class ImageUtils; }
QT_END_NAMESPACE

class ImageUtils : public QMainWindow
{
    Q_OBJECT

public:
    ImageUtils(QWidget *parent = nullptr);
    ~ImageUtils();

private:
    void imageRotate(cv::InputArray src, cv::OutputArray dst, const double& angle);
    void imageResize(cv::InputArray src, cv::OutputArray dst, const int& x, const int& y);
    void imageRectify(cv::InputArray src, cv::OutputArray dst, const double& rate);
    QImage cvMat2QImage(const cv::Mat &mat);
    cv::Mat QImage2cvMat(const QImage &image);

private slots:


    void on_RectifyButton_clicked();

    void on_actionRead_Template_Image_triggered();

    void on_RotateButton_clicked();

signals:


private:
    QSharedPointer<QImage> TemplateImage;
    QSharedPointer<QImage> SourceImage;

private:
    Ui::ImageUtils *ui;
};
#endif // IMAGEUTILS_H
