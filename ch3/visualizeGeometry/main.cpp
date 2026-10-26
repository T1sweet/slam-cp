#include <iostream>
using namespace std;

#include <Eigen/Core>   //包含Eigen库的Core头文件: 定义了矩阵和向量的基本操作
#include <Eigen/Geometry>  //包含Eigen库的Geometry头文件: 定义了旋转矩阵和四元数
#include <pangolin/pangolin.h>  //包含Pangolin库的头文件: 提供了可视化界面的创建和管理

#include <unistd.h>  // 提供了系统调用的函数,如usleep(5000)表示延时5ms
using namespace Eigen;


void RrawTrajectory(vector<Isometry3d, aligned_allocator<Isometry3d>> poses);

int main(int argc, char **argv)
{
    /*****************读取轨迹文件********************/
    string trajectory_file_path = "../trajectory.txt";
    ifstream trajectory_file_in;   //定义输入文件流对象ifstream（底层会管理操作系统的文件句柄），用于读取文件
    trajectory_file_in.open(trajectory_file_path, ios::in);    //打开轨迹文件, 以只读模式打开
    if (!trajectory_file_in.is_open())     //如果文件打开失败   
    {
        cout << "文件打开失败" << endl;
        return 1;
    }
    cout << "文件打开成功" << endl;


    double time, tx, ty, tz, qx, qy, qz, qw;   
                    // 定义时间time、平移tx、ty、tz、四元数qx、qy、qz、qw
    vector<Isometry3d, aligned_allocator<Isometry3d>> poses;    
                    // 定义一个顺序容器对象poses，用于存储所有位姿
                    // Isometry3d表示数据类型为3D变换矩阵
                    // aligned_allocator<Isometry3d>表示使用对齐分配器分配内存
    while (!trajectory_file_in.eof())   //eof()函数判断是否到文件结束位置
    {                                 
        trajectory_file_in >> time >> tx >> ty >> tz >> qx >> qy >> qz >> qw;

        Quaterniond q(qw, qx, qy, qz);
        Vector3d t(tx, ty, tz);

        // Isometry3d T_RW = Isometry3d::Identity();
        // T_RW.rotate(q.normalized());
        // Isometry3d T_RW(q.normalized());    //将上2句合二为一
        Isometry3d T_RW(q);    //一般情况，保存好的四元数都是单位四元数，不需要归一化
        T_RW.pretranslate(t);    

        poses.push_back(T_RW);    //将当前位姿T_RW添加到顺序容器poses中
    }
    trajectory_file_in.close();    //关闭轨迹文件
    cout << "成功读取轨迹文件" << poses.size() <<"个位姿"<<endl;

    /*****************可视化轨迹********************/
    RrawTrajectory(poses);  //轨迹可视化

    return 0;
}   

void RrawTrajectory(vector<Isometry3d, aligned_allocator<Isometry3d>> poses)
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
    while (pangolin::ShouldQuit() == false) {   // 只要窗口没有关闭，就不断循环（大约每秒60帧或更高）
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清空颜色缓冲区和深度缓冲区，准备画新一帧。
        d_cam.Activate(s_cam);                  // 激活刚刚设置的相机视角，后续的绘制都会基于这个视角
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);   // 设置背景颜色为纯白色（RGBA: 1,1,1,1）
        glLineWidth(2);                         // 设置接下来绘制的线条宽度为2像素
        for (size_t i = 0; i < poses.size(); i++) {
            // 画每个位姿的三个坐标轴，即RGB坐标系
            Vector3d Ow = poses[i].translation();   // 获取该位姿的平移部分（即原点在世界坐标系的位置）
            Vector3d Xw = poses[i] * (0.1 * Vector3d(1, 0, 0)); 
            Vector3d Yw = poses[i] * (0.1 * Vector3d(0, 1, 0));
            Vector3d Zw = poses[i] * (0.1 * Vector3d(0, 0, 1));
                                                // 将单位向量乘以 0.1（缩短长度），再变换到了世界坐标系下的位置
            glBegin(GL_LINES);      // 用传统的OpenGL立即模式 画线
            glColor3f(1.0, 0.0, 0.0);           // 红色
            glVertex3d(Ow[0], Ow[1], Ow[2]);    // 从 原点Ow 画线到 Xw（红色，代表X轴）
            glVertex3d(Xw[0], Xw[1], Xw[2]);
            glColor3f(0.0, 1.0, 0.0);           // 绿色
            glVertex3d(Ow[0], Ow[1], Ow[2]);    // 从 原点Ow 画线到 Yw（绿色，代表Y轴）
            glVertex3d(Yw[0], Yw[1], Yw[2]);
            glColor3f(0.0, 0.0, 1.0);           // 蓝色
            glVertex3d(Ow[0], Ow[1], Ow[2]);    // 从 原点Ow 画线到 Zw（蓝色，代表Z轴）
            glVertex3d(Zw[0], Zw[1], Zw[2]);
            glEnd();     // 结束画线
        }
        // 画出连线：把相邻的两个位姿（平移部分）连成一条黑线，形成轨迹线
        for (size_t i = 0; i < poses.size()-1; i++) {
            glColor3f(0.0, 0.0, 0.0);   
            glBegin(GL_LINES);              // 用传统的OpenGL立即模式 画线
            auto p1 = poses[i], p2 = poses[i+1];
            glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]); //上一位姿的平移部分
            glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]); //下一位姿的平移部分
            glEnd();     // 结束画线
        }
        pangolin::FinishFrame();    // 告诉Pangolin当前帧画完了，交换前后缓冲区，把画面显示出来。
        usleep(5000);               // sleep 5 ms，相当于限制了刷新率大约在 200 帧/秒
    }
}


