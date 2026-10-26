#include <iostream>
using namespace std;

#include <ctime>        //包含时间函数
#include <Eigen/Core>   //包含Eigen库的Core头文件: 定义了矩阵和向量的基本操作
#include <Eigen/Dense>  //包含Eigen库的Dense头文件: 定义了矩阵和向量的实现
using namespace Eigen;

#define MATRIX_SIZE 50

int main(int argc, char **argv)
{
    /*****************向量/矩阵定义********************/
    // Eigen是名字空间，matrix是名字空间Eigen下的一个类模板，用于表示矩阵和向量
    // matrix的前三个参数：数据类型、矩阵的行数、矩阵的列数
    //定义向量
    Matrix<float, 3, 1> matrix_f31;  // 定义一个3×1的float类型的矩阵
    Vector3f vector_f3;              // 定义一个3维float类型的向量,与Matrix<float, 3, 1>等价
    Matrix<double, 3, 1> matrix_d31; // 定义一个3×1的double类型的矩阵
    Vector3d vector_d3;              // 定义一个3维double类型的向量,与Matrix<double, 3, 1>等价

    // 定义矩阵
    Matrix<float, 2, 3> matrix_f23;     // 类模板Matrix 实例化为 模板类Matrix<float, 2, 3>，
                                        // 再按该类定义一个2×3的float类型的矩阵对象:matrix_f23
    Matrix<double, 3, 3> matrix_d33;    // 定义一个3×3的double类型的矩阵

    //定义特殊矩阵
    Matrix3d matrix_d33_I = Matrix3d::Identity(); // 定义一个3×3的double类型的矩阵, 并初始化为单位矩阵
    Matrix3d matrix_d33_0 = Matrix3d::Zero();     // 定义一个3×3的double类型的矩阵, 并初始化为零矩阵
    Matrix3d matrix_d33_R = Matrix3d::Random();     // 定义一个3×3的double类型的矩阵, 并初始化为随机矩阵, 元素在[0, 1)范围内
    Matrix3d matrix_d33_1 = Matrix3d::Ones();       // 定义一个3×3的double类型的矩阵, 并初始化为矩阵, 元素全为1
    cout << "单位矩阵:" << endl;
    cout << matrix_d33_I << endl;
    cout << "零矩阵:" << endl;
    cout << matrix_d33_0 << endl;
    cout << "随机矩阵:" << endl;
    cout << matrix_d33_R << endl;
    cout << "全1矩阵:" << endl;
    cout << matrix_d33_1 << endl;

    // 定义动态矩阵
    Matrix<float, Dynamic, Dynamic> matrix_dynamic_f; // 定义一个float类型的动态矩阵, 可以改变行数和列数
    MatrixXd matrix_dynamic_d;          // 定义一个double类型的动态矩阵, 可以改变行数和列数

    /*******************矩阵操作**********************/
    //矩阵输入输出
    matrix_f23 << 1, 2, 3, 4, 5, 6;  // 矩阵元素输入：手动初始化一个2×3的float类型的矩阵
    cout << "输出矩阵:" << endl;
    cout << matrix_f23 << endl;      // 矩阵元素输出

    //用()访问矩阵元素
    cout << "依次输出矩阵元素:" << endl;
    for (int i = 0; i < matrix_f23.rows(); i++)      // 遍历矩阵的行
    {
        for (int j = 0; j < matrix_f23.cols(); j++)  // 遍历矩阵的列
        {
            cout << matrix_f23(i, j) << " ";
        }   
    }
    cout << endl;

    /*******************矩阵运算**********************/
    //矩阵与向量的乘法(本质上是矩阵与矩阵的乘法)
    vector_f3 << 1, 2, 3;
    matrix_f23 << 1, 2, 3, 4, 5, 6;
    cout << "矩阵与向量的乘法:" << endl;
    cout << matrix_f23 * vector_f3 << endl;

    //类型转换后在做运算：矩阵与矩阵的乘法
    matrix_d33 << 1, 2, 3, 4, 5, 6, 7, 8, 9;
    Matrix<float, 2, 3> matrix_f23_result = matrix_f23 * matrix_d33.cast<float>();
    cout << "矩阵与矩阵的乘法:" << endl;
    cout << matrix_f23_result << endl;

    //矩阵转置
    cout << "矩阵转置:" << endl;
    cout << matrix_f23_result.transpose() << endl;

    //各元素之和
    Matrix<float, 3, 3> matrix_f23_test;
    matrix_f23_test << 1, 2, 3, 6, 5, 6, 7, 8, 2;
    cout << "原矩阵:" << endl;
    cout << matrix_f23_test << endl;
    cout << "各元素之和:" << endl;
    cout << matrix_f23_test.sum() << endl;

    //迹
    cout << "迹:" << endl;
    cout << matrix_f23_test.trace() << endl;

    //数乘
    cout << "数乘:" << endl;
    cout << matrix_f23_test * 2 << endl;

    //逆
    cout << "逆:" << endl;
    cout << matrix_f23_test.inverse() << endl;

    //行列式
    cout << "行列式:" << endl;
    cout << matrix_f23_test.determinant() << endl;

    /*******************矩阵求解**********************/
    //求特征值和特征向量
    Matrix<float, 3, 3> A = Matrix3f::Random();         // 3*3随机实数矩阵
    Matrix<float, 3, 3> matrix_f33 = A.transpose() * A; // A是任意实数矩阵，A^T * A 必是实对称矩阵
                                                        // 实对称矩阵一定可以对角化
    SelfAdjointEigenSolver<Matrix<float, 3, 3>> eigen_solver(matrix_f33); 
        // 类模板SelfAdjointEigenSolver 实例化为 模板类SelfAdjointEigenSolver<Matrix<float, 3, 3>>，
        // 再按该类定义一个对象:eigen_solver
        // 构造对象时传入matrix_f33，构造函数会立即进行特征分解
        // 对矩阵实对称矩阵matrix_f33进行特征分解和对角化，得到特征值和特征向量
    cout << "特征值:" << endl;
    cout << eigen_solver.eigenvalues() << endl;
    cout << "特征向量:" << endl;
    cout << eigen_solver.eigenvectors() << endl;
        

    //解方程（分析求解时间）: matrix_A * x = vector_b
    Matrix<float, MATRIX_SIZE, MATRIX_SIZE> matrix_A = MatrixXf::Random(MATRIX_SIZE, MATRIX_SIZE);  
                        // 定义一个float类型的随机矩阵, 元素在[0, 1)范围内
    matrix_A = matrix_A.transpose() * matrix_A; 
                        //matrix_A是实对称矩阵，一定可以对角化，且是半正定矩阵（所有特征值都大于等于0）
    Matrix<float, MATRIX_SIZE, 1> vector_b =  MatrixXf::Random(MATRIX_SIZE, 1);

    //////通过直接求逆解方程：x = matrix_A^{-1} * vector_b
    clock_t start = clock();    // 计时开始
    Matrix<float, MATRIX_SIZE, 1> x = matrix_A.inverse() * vector_b;
    clock_t end = clock();      // 计时结束
    cout << "解方程得x的前5行:" << endl;
    cout << x.head(5) << endl;
    cout << "通过直接求逆解方程耗时:" << 1000 * (double)(end - start) / CLOCKS_PER_SEC << "ms" << endl;

    //////通过矩阵QR分解求逆解方程：x = matrix_A^{-1} * vector_b
    clock_t start1 = clock();    // 计时开始
    Matrix<float, MATRIX_SIZE, 1> x1 = matrix_A.colPivHouseholderQr().solve(vector_b);
    clock_t end1 = clock();      // 计时结束
    cout << "解方程得x1的前5行:" << endl;
    cout << x1.head(5) << endl;
    cout << "通过矩阵QR分解求逆解方程耗时:" << 1000 * (double)(end1 - start1) / CLOCKS_PER_SEC << "ms" << endl;
    
    //////对于正定矩阵，通过Cholesky分解求逆解方程
    clock_t start2 = clock();    // 计时开始
    Matrix<float, MATRIX_SIZE, 1> x2 = matrix_A.ldlt().solve(vector_b);
    clock_t end2 = clock();      // 计时结束
    cout << "解方程得x2的前5行:" << endl;
    cout << x2.head(5) << endl;
    cout << "通过Cholesky分解求逆解方程耗时:" << 1000 * (double)(end2 - start2) / CLOCKS_PER_SEC << "ms" << endl;
    
    return 0;
}