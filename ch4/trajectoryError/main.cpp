#include <iostream>     // 标准输入输出流库，用于 cout, cerr 等
using namespace std;

#include <fstream>      // 文件读写流库，用于 ifstream 读取文件
#include <unistd.h>     // 提供了系统调用的函数,如usleep(5000)表示延时5ms

#include <pangolin/pangolin.h>  //包含Pangolin库的头文件: 提供了可视化界面的创建和管理
#include <sophus/se3.hpp>       // 包含 Sophus 库的 SE3 头文件：用于表示 3D 刚体变换（旋转+平移）
using namespace Sophus;

string groundtruth_file = "../groundtruth.txt";
string estimated_file = "../estimated.txt";

typedef vector<Sophus::SE3d, Eigen::aligned_allocator<Sophus::SE3d>> TrajectoryType;    
// 定义轨迹类型：使用 vector 存储 Sophus::SE3d（双精度 3D 位姿）
// 注意：由于 Eigen 使用了 SIMD 指令加速（如 SSE/AVX），要求内存按 16 字节对齐。
// 因此，放在 STL 容器中必须显式指定 Eigen::aligned_allocator，否则运行时会报段错误（Segmentation fault）。

void RrawTrajectory(const TrajectoryType &gt, const TrajectoryType &esti);
TrajectoryType ReadTrajectory(const string &path);

int main(int argc, char **argv)
{
    TrajectoryType groundtruth = ReadTrajectory(groundtruth_file); //
    TrajectoryType estimated = ReadTrajectory(estimated_file);

    // 断言检查：确保文件不为空，且估计轨迹与真实轨迹的帧数（大小）一致
    assert(!groundtruth.empty() && !estimated.empty());
    assert(groundtruth.size() == estimated.size());

    // 计算均方根误差 (RMSE - Root Mean Square Error)
    double rmse = 0;
    for(size_t i=0;i<estimated.size();i++)      // 遍历每一帧
    {
        Sophus::SE3d p1 = estimated[i];
        Sophus::SE3d p2 = groundtruth[i];
        double error = (p2.inverse() * p1).log().norm();
            // 计算相对位姿误差： (p2.inverse() * p1) 即 T_gt^-1 * T_est
            // .log() 将 SE3 李群映射到 se3 李代数（6维向量：平移+旋转），方便计算欧氏距离
            // .norm() 计算该 6 维向量的模长（L2范数），即该帧的综合误差
        rmse += error * error;  // 累加误差的平方
    }
    rmse = rmse / double(estimated.size());     // 求均值（均方误差 MSE）
    rmse = sqrt(rmse);                          // 开根号得到均方根误差 (RMSE)

    cout << "RMSE = " << rmse << endl;
    RrawTrajectory(groundtruth, estimated); //画出真实与估计轨迹对比图

    return 0;
}   

TrajectoryType ReadTrajectory(const string &path)
{
    ifstream fin(path);             // 创建文件输入流，打开指定路径的文件
    TrajectoryType trajectory;       // 创建一个空的轨迹容器，用于存储读取的数据

    if(!fin)    // 检查文件是否成功打开
    {
        cerr << "trajectory" << path << "not found." << endl;   // 打印错误信息并返回空轨迹
        return trajectory;
    }

    // 循环读取文件直到文件末尾
    // 注意：这是一个常见的 C++ 陷阱，如果文件末尾有空行，可能会多读取一次导致数据重复。
    // 更严谨的写法是 while (fin >> time >> tx >> ty >> tz >> qx >> qy >> qz >> qw)
    while(!fin.eof())
    {
        double time, tx, ty, tz, qx, qy, qz, qw;
            // 定义变量存储读取的一行数据：时间、平移(tx,ty,tz)、四元数旋转(qx,qy,qz,qw)
        fin >> time >> tx >> ty >> tz >> qx >> qy >> qz >> qw;
            // 从文件中读取这些数据（TUM 数据集的格式通常为：time tx ty tz qx qy qz qw）
        Sophus::SE3d p1(Eigen::Quaterniond(qx, qy, qz, qw), Eigen::Vector3d(tx, ty, tz));
            // 使用四元数和位移向量构造一个 SE3 位姿对象
            // 注意：Eigen::Quaterniond 的构造函数参数顺序是 (w, x, y, z)，而文件中读取的顺序通常是 (x, y, z, w)
        trajectory.push_back(p1);   // 将构造好的位姿存入轨迹容器
    }

    return trajectory;
}

void RrawTrajectory(const TrajectoryType &gt, const TrajectoryType &esti)
{
    // 初始化 Pangolin窗口 与 OpenGL状态（以下是OpenGL原生函数）
    pangolin::CreateWindowAndBind("Trajectory", 1024, 768);
    glEnable(GL_DEPTH_TEST);    // 开启深度测试
    glEnable(GL_BLEND);         // 开启混合功能
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
                                // 设置混合函数为源Alpha和1-源Alpha  
                                // 用于透明度混合

    // 设置相机（视角）
    pangolin::OpenGlRenderState s_cam(  // 创建相机的渲染状态
        pangolin::ProjectionMatrix(1024, 768, 500, 500, 512, 389, 0.1, 1000),   // 设置投影矩阵（内参）
                                // 视口宽高：1024, 768
                                // 焦距fx,fy：500, 500
                                // 光心cx,cy（大约在画面中心）：512, 389
                                // 最近和最远可见距离：0.1, 1000
        pangolin::ModelViewLookAt(0, -0.1, -1.8, 0, 0, 0, 0.0, -1.0, 0.0) 
                                // 设置相机视图矩阵（外参），即相机在空间中的位置和朝向
    );                          // 相机在世界坐标系中的位置：(0, -0.1, -1.8)
                                // 相机在世界坐标系中的朝向：(0, 0, 0) 即向z轴正方向
                                // 相机在世界坐标系中的焦距：(0.0, -1.0, 0.0) 即向y轴负方向

    // 创建相机的显示对象
    pangolin::View &d_cam = pangolin::CreateDisplay()   
        .SetBounds(0.0, 1.0, 0.0, 1.0, -1024.0f / 768.0f)   // 设置相机的显示对象的边界
                                                            // -1024.0f/768.0f 是宽高比（负号表示这是以高度为基准的比例）
        .SetHandler(new pangolin::Handler3D(s_cam));        
                                // 绑定一个3D交互器（Handler3D）。允许用鼠标拖拽旋转、右键平移、滚轮缩放查看轨迹。

    //主渲染循环                                
    while (pangolin::ShouldQuit() == false)     // 只要窗口没有关闭，就不断循环（大约每秒60帧或更高）
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清空颜色缓冲区和深度缓冲区，准备画新一帧。
        d_cam.Activate(s_cam);                  // 激活刚刚设置的相机视角，后续的绘制都会基于这个视角
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);   // 设置背景颜色为纯白色（RGBA: 1,1,1,1）
        glLineWidth(2);                         // 设置接下来绘制的线条宽度为2像素

        for (size_t i = 0; i < gt.size() - 1; i++)
        {   
            glColor3f(0.0f, 0.0f, 1.0f); // groundtruth 蓝色
            glBegin(GL_LINES);                          // 用传统的OpenGL立即模式 画线
            auto p1 = gt[i], p2 = gt[i + 1];            // p1为该点，p2为下一个点
            glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);  // 在三维坐标系中画p1
            glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]);  // 在三维坐标系中画p2，p1与p2之间是直接连线
            glEnd();        // 结束画线
        }

        for (size_t i = 0; i < esti.size() - 1; i++)
        {
            glColor3f(1.0f, 0.0f, 0.0f); // estimated 红色
            glBegin(GL_LINES);                          // 用传统的OpenGL立即模式 画线
            auto p1 = esti[i], p2 = esti[i + 1];
            glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
            glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]);
            glEnd();
        }
        pangolin::FinishFrame();    // 告诉Pangolin当前帧画完了，交换前后缓冲区，把画面显示出来。
        usleep(5000); // sleep 5 ms，相当于限制了刷新率大约在 200 帧/秒
    }
}


