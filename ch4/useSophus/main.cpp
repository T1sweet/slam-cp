#include <iostream>
using namespace std;

#include <Eigen/Core>   //包含Eigen库的Core头文件: 定义了矩阵和向量的基本操作
#include <Eigen/Geometry>  //包含Eigen库的Geometry头文件: 定义了旋转矩阵和四元数

#include <sophus/se3.hpp>

#include <cmath>  
using namespace Eigen;


int main(int argc, char **argv)
{
    /*****************李群SO(3)上定义、李代数so(3)上定义、在李群SO(3)上李代数扰动模型********************/
    // 旋转矩阵和四元数定义
    Matrix3d R = AngleAxisd(M_PI/2, Vector3d(0, 0, 1)).toRotationMatrix();  // R 是沿z轴旋转90度的旋转矩阵
    Quaterniond q(R);           // q 是 R 的四元数表示
    cout << "旋转矩阵R: " << endl << R << endl;

    // 李群SO(3)定义：传入旋转矩阵或四元数都是可以的
    Sophus::SO3d SO3_R(R);      // SO3_R 是 R的李群SO(3)表示
    Sophus::SO3d SO3_q(q);      // SO3_q 是用单位四元数 q 构造的李群SO(3)表示
    cout << "李群SO3_R: " << endl << SO3_R.matrix() << endl;
    cout << "李群SO3_q: " << endl << SO3_q.matrix() << endl;

    // 李代数so(3)定义
    Vector3d so3_vector  = SO3_R.log();    // 用对数映射获得李群SO3_R的李代数so3
    cout << "李代数so3_vector: " << endl << so3_vector << endl;

    // 李代数so(3)的反对称矩阵
    Matrix3d so3_vector_hat = Sophus::SO3d::hat(so3_vector);      // 李代数的反对称矩阵
    cout << "李代数so3_vector的反对称矩阵: " << endl << so3_vector_hat << endl;

    // 李代数so(3)的反对称矩阵 对应的 向量
    Vector3d so3_vector_hat_vee = Sophus::SO3d::vee(so3_vector_hat);  // 李代数的反对称矩阵 回到 向量表示
    cout << "李代数so3_vector反对称矩阵 对应的向量so3_vector_hat_vee: " << endl << so3_vector_hat_vee << endl;

    // 扰动模型：李代数so(3)左乘扰动更新
    Vector3d so3_delta_vector(1e-4, 0, 0);  // 定义扰动矩阵对应的向量
    Sophus::SO3d SO3_delta_R = Sophus::SO3d::exp(so3_delta_vector);     // 计算扰动矩阵SO3_delta_R
    Sophus::SO3d SO3_R_update = SO3_delta_R * SO3_R;                    // 左乘扰动矩阵更新
    cout << "SO3_R_update: " << endl << SO3_R_update.matrix() << endl;

    cout << "======================================================================"<< endl;
    /*****************李群SE(3)上定义、李代数se(3)上定义、在李群SE(3)上李代数扰动模型********************/
    // 李群SE(3)定义：传入旋转矩阵或四元数都是可以的
    Vector3d t(1, 0, 0);        // 定义平移向量
    Sophus::SE3d SE3_Rt(R,t);   // 用旋转矩阵R和平移向量t 构建 李群SE(3)
    Sophus::SE3d SE3_qt(q,t);   // 用四元数q和平移向量t 构建 李群SE(3)
    cout << "李群SE3_Rt: " << endl << SE3_Rt.matrix() << endl;
    cout << "李群SE3_qt: " << endl << SE3_qt.matrix() << endl;

    // 李代数se(3)定义
    typedef Matrix<double, 6, 1> Vector6d;  // Eigen库中没有定义Vector6d，所以需要自定义
    Vector6d se3_vector = SE3_Rt.log();     // 用对数映射获得李群SE3_Rt的李代数se3_vector();
    cout << "李代数se3_vector: " << endl << se3_vector << endl;

    // 李代数se(3)的反对称矩阵
    Matrix4d se3_vector_hat = Sophus::SE3d::hat(se3_vector);      // 李代数的反对称矩阵
    cout << "李代数se3_vector的反对称矩阵: " << endl << se3_vector_hat << endl;

    // 李代数se(3)的反对称矩阵 对应的 向量
    Vector6d se3_vector_hat_vee = Sophus::SE3d::vee(se3_vector_hat);  // 李代数的反对称矩阵 回到 向量表示
    cout << "李代数se3_vector反对称矩阵 对应的向量se3_vector_hat_vee: " << endl << se3_vector_hat_vee << endl;

    // 扰动模型：李代数se(3)左乘扰动更新
    // Vector6d se3_delta_vector(1e-4, 0, 0, 0, 0, 0);  // 定义扰动矩阵对应的向量
    Vector6d se3_delta_vector =  Vector6d::Zero();      //上面这种6个形参的写法编译是通不过的，只能先置0再赋值
    se3_delta_vector(0) = 1e-4;
       
    Sophus::SE3d SE3_delta_R = Sophus::SE3d::exp(se3_delta_vector);     // 计算扰动矩阵SE3_delta_R
    Sophus::SE3d SE3_R_update = SE3_delta_R * SE3_Rt;                    // 左乘扰动矩阵更新
    cout << "SE3_R_update: " << endl << SE3_R_update.matrix() << endl;
}


