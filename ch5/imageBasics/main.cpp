#include <iostream>     // 标准输入输出流库，用于 cout, cerr 等
#include <chrono>       // C++11 的时间库，用于后续的代码计时
using namespace std;

#include <opencv2/core/core.hpp>        // OpenCV 核心数据结构（如 cv::Mat）
#include <opencv2/highgui/highgui.hpp>  // OpenCV 图形界面模块（如 imshow, waitKey）

int main(int argc, char **argv)
{
    string image_path = "../ubuntu.png";    // 图片路径
    cv::Mat image;                          // 声明一个 OpenCV 的矩阵对象 image

    image = cv::imread(image_path);         // 读取图片，默认读取为三通道彩色图

    if(image.data ==nullptr)                // 核心判断：image.data 是图像数据的首地址
    {
        cerr << "文件" << image_path << "不存在。" << endl;
                    // 如果首地址为空指针（nullptr），说明读取失败
        return 0;
    }
    cout << "图像宽为" << image.cols << ", 高为" << image.rows << ", 通道数为" << image.channels() <<endl;
                    // cols是列数（宽），rows是行数（高）。channels()是通道数（彩色图通常是3，灰度图是1）。

    cv::imshow("image", image);     // 弹出一个名为 "image" 的窗口显示图片
    cv::waitKey(0);                 // 暂停程序，等待用户按下任意键。如果没有这句，窗口会一闪而过

    if(image.type() != CV_8UC1 && image.type()!= CV_8UC3)   //如果读入的图片既不是单通道也不是三通道（比如四通道的带透明度的PNG），就提示并退出。
    {                               //CV_8UC1 代表 8位无符号整数、1个通道（即灰度图）；CV_8UC3 代表 8位无符号整数、3个通道（即普通彩色图）。
        cout << "请输入一张彩色图或灰度图。" << endl;
        return 0;
    }

    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();  // 记录当前时间点 t1
    for(int y=0; y<image.rows; y++)     // 遍历每一行 (y 是行号)
    {
        unsigned char *row_ptr = image.ptr<unsigned char>(y);   // 获取第y行的行首指针。
        for(int x=0; x<image.cols; x++)     // 遍历每一列 (x 是列号)
        {
            unsigned char *data_ptr = &row_ptr[x * image.channels()];   // 计算第y行、第x列这个像素点数据块的起始地址
                                        
            for(int c=0; c!=image.channels();c++)   // 遍历该像素的每一个通道 (c 是通道索引，如B、G、R)
            {
                unsigned char data = data_ptr[c];   // 读取通道的值
            }
        }
    }

    // 记录当前时间点 t2，并计算差值 t2 - t1
    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();  // 记录当前时间点 t2
    chrono::duration<double> time_used = chrono::duration_cast<chrono::duration<double>> (t2 - t1);
    cout << "遍历图像用时：" << time_used.count() << "秒。" << endl;

    cv::Mat image_another = image;                  // 【浅拷贝】仅仅是引用计数的增加！
    image_another(cv::Rect(0,0,100,100)).setTo(0);  // 把 image_another 左上角100x100的区域设置为 0(黑色)
    cv::imshow("image1",image);          // 弹出一个名为 "image" 的窗口显示图片
    cv::waitKey(0);                     // 暂停程序，等待用户按下任意键。如果没有这句，窗口会一闪而过

    cv::Mat image_clone = image.clone();            // 【深拷贝】真正的内存复制！
    image_clone(cv::Rect(0,0,100,100)).setTo(255);  // 把克隆图的左上角100x100区域设置为 255(白色)
    cv::imshow("image2",image);              // 显示原图，此时原图左上角依然是刚才的黑色
    cv::imshow("image_clone", image_clone); // 显示克隆图，左上角是白色
    cv::waitKey(0);                     // 暂停程序，等待用户按下任意键。如果没有这句，窗口会一闪而过

    cv::destroyAllWindows();
    return 0;
}
   