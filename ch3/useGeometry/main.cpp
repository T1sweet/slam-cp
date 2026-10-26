#include <iostream>
#include <cmath>
using namespace std;

#include <Eigen/Core>   //包含Eigen库的Core头文件: 定义了矩阵和向量的基本操作
#include <Eigen/Geometry>  //包含Eigen库的Geometry头文件: 定义了矩阵和向量的实现
using namespace Eigen;


int main(int argc, char **argv)
{
    /*****************3D旋转矩阵、旋转向量定义与赋值********************/
    // 3D旋转矩阵
    Matrix3f rotation_matrix_3f = Matrix3f::Identity();
    Matrix3d rotation_matrix_3d = Matrix3d::Identity();
    cout << "定义 rotation_matrix_3f: " << endl << rotation_matrix_3f << endl;
    cout << "定义 rotation_matrix_3d: " << endl << rotation_matrix_3d << endl;
    // 3D旋转向量（或角轴/轴角）
    AngleAxisf angle_axis_3f(M_PI/4, Vector3f(0, 0, 1));    //数据类型为float
                        //Vector3f(0, 0, 1)表示z轴
                        //M_PI/4表示旋转角度为45度
                        //angle_axis_3f表示绕z轴逆时针旋转45度的旋转向量
    AngleAxisd angle_axis_3d(M_PI/4, Vector3d(0, 0, 1));    //数据类型为double
    cout.precision(3);  //设置输出精度为3位小数
    cout << "angle_axis_3f to rotation_matrix: " << endl << angle_axis_3f.matrix() << endl;
    cout << "angle_axis_3d to rotation_matrix: " << endl << angle_axis_3d.matrix() << endl;
                        //Eigen只为Matrix/Array 类提供了<<运算符的重载，其他类需要自己实现
                        //angle_axis_3d.matrix()表示将 旋转向量（角轴）转换为 旋转矩阵
    // 3D旋转矩阵赋值
    rotation_matrix_3f = angle_axis_3f.matrix();
    rotation_matrix_3d = angle_axis_3d.toRotationMatrix();
                    //两种将旋转向量转换为旋转矩阵的方法等价，但第二种方法含义更直接
    cout << "赋值 rotation_matrix_3f: " << endl << rotation_matrix_3f << endl;
    cout << "赋值 rotation_matrix_3d: " << endl << rotation_matrix_3d << endl;                

    /*****************坐标变换********************/
    //用角轴进行坐标变换
    Vector3f vector_3f(1, 0, 0);     //定义一个3D向量(1, 0, 0)
    Vector3f vector_rotated = angle_axis_3f * vector_3f;  //将向量(1, 0, 0)绕z轴旋转45度
                        // 旋转向量（角轴）*向量 = 旋转后的向量 之所以成立
                        // 是因为程序中做了运算符重载，所以可以将旋转向量（角轴）作为矩阵使用
    cout << "原向量: " << endl << vector_3f << endl;
    cout << "旋转后的向量（通过旋转向量（角轴）*向量）: " << endl << vector_rotated << endl;
   
    //用旋转矩阵进行坐标变换
    Vector3d vector_3d(1, 0, 0); 
    Vector3d vector_3d_rotated = rotation_matrix_3d * vector_3d;
    cout << "原向量: " << endl << vector_3d << endl;
    cout << "旋转后的向量（通过旋转矩阵*向量）: " << endl << vector_3d_rotated << endl;

    //欧拉角：将旋转矩阵转换为欧拉角
    //注意：Eigen里欧拉角不能直接做变换，必须先转成
    //旋转矩阵（Matrix3d） / （旋转向量（AngleAxisd） / 四元数（Quaternion3d） 中的一种。
    Vector3d euler_angles = rotation_matrix_3d.eulerAngles(2, 1, 0); 
             //2=z，1=y，0=x，表示三个轴的旋转顺序为z->y->x，
             //对应yaw（偏航）-> pitch（俯仰） -> roll（偏滚）
             //注：在ros中约定无人机朝向：前X、左Y、上Z
    cout << "欧拉角: " << endl << euler_angles << endl;

    //用欧式变换矩阵变换向量: 将旋转矩阵和平移向量组合起来，得到一个欧式变换矩阵
    Isometry3d isometry_3d_T = Isometry3d::Identity();  //定义一个欧式变换矩阵
    isometry_3d_T.rotate(rotation_matrix_3d);       
                                //将旋转矩阵赋值给欧式变换矩阵的旋转部分
                                //此外，传入旋转向量（角轴）、四元数都是可以的
    isometry_3d_T.pretranslate(Vector3d(1, 3, 4));  
                                //将平移向量赋值给欧式变换矩阵的位置部分
    cout << "欧式变换矩阵: " << endl << isometry_3d_T.matrix() << endl;
    cout << "欧式变换矩阵的旋转部分: " << endl << isometry_3d_T.rotation() << endl;
    cout << "欧式变换矩阵的位置部分: " << endl << isometry_3d_T.translation() << endl;
    Vector3d vector_3d_transformed = isometry_3d_T * vector_3d;
            //用欧式变换矩阵对向量进行变换
            //注意：欧式变换矩阵的尺寸为4x4，向量的尺寸为3，之所以乘法成立是因为程序中做了运算符重载
            //因为Isometry3d不是4×4矩阵，它内部存储的是 3×3旋转 + 3×1平移
            //.matrix() 输出的4×4矩阵只是它临时生成，用于查看的，不是它内部存储的矩阵
    cout << "变换后的向量: " << endl << vector_3d_transformed << endl;

    //用四元数变换向量
    Quaterniond quaternion_3d_1 = Quaterniond(angle_axis_3d); 
                    //将旋转向量（角轴）转换为四元数
    cout << "旋转向量（角轴）: " << endl << angle_axis_3d.axis() << endl;
    cout << "四元数: " << endl << quaternion_3d_1.coeffs() << endl;
                    //.coeffs()返回四元数的内部存储向量，类型是 Vector4d
                    //注意：四元数的定义是：(w, x, y, z)，其中w是标量部分，x、y、z是向量部分
    Quaterniond quaternion_3d_2 = Quaterniond(rotation_matrix_3d);
                    //将旋转矩阵转换为四元数
    cout << "旋转矩阵: " << endl << rotation_matrix_3d << endl;
    cout << "四元数: " << endl << quaternion_3d_2.coeffs() << endl;
    Vector3d vector_rotated_1 = quaternion_3d_1 * vector_3d;  
                    //Eigen内置的乘法运算符重载，等价于q * v * q^(-1)
    Vector3d vector_rotated_2 =
         (quaternion_3d_2 * Quaterniond(0, 1, 0, 0) * quaternion_3d_2.inverse()).vec();  
                    //用常规向量乘法表示：q * v * q^(-1)  
                    //vector_3d=[1, 0, 0]，四元数中用Quaterniond(0,1,0,0)表示
                    //.vec() 返回虚部 [x, y, z]
    cout << "原向量: " << endl << vector_3d << endl;
    cout << "四元数变换后的向量（通过四元数*向量）: " << endl << vector_rotated_1 << endl;
    cout << "四元数变换后的向量（通过四元数向量乘法）: " << endl << vector_rotated_2 << endl;



    //案例：方法1：在变换矩阵T中直接使用四元数
    // Quaterniond q1 = Quaterniond(0.35, 0.2, 0.3, 0.1);
    // Quaterniond q2 = Quaterniond(-0.5, 0.4, -0.1, 0.2);
    // Vector3d t1=Vector3d(0.3, 0.1, 0.1);
    // Vector3d t2=Vector3d(-0.1, 0.5, 0.3);
    Quaterniond q1(0.35, 0.2, 0.3, 0.1), q2(-0.5, 0.4, -0.1, 0.2);
    Vector3d t1(0.3, 0.1, 0.1), t2(-0.1, 0.5, 0.3);
    Isometry3d T_R1W = Isometry3d::Identity();  //定义一个欧式变换矩阵
    Isometry3d T_R2W = Isometry3d::Identity();  //定义一个欧式变换矩阵
    
    T_R1W.rotate(q1.normalized());  //将四元数归一化，确保模长为1,再赋值给旋转矩阵
    T_R2W.rotate(q2.normalized());  //Isometry3d的.rotate(q)方法要求传入的四元数必须是单位四元数
    T_R1W.pretranslate(t1);
    T_R2W.pretranslate(t2);

    cout.precision(6);  //恢复输出精度为6位小数           
    cout << "T_R1W: " << endl << T_R1W.matrix() << endl;
    cout << "T_R2W: " << endl << T_R2W.matrix() << endl;

    Vector3d p_R1(0.5, 0, 0.2);
    Vector3d p_W = T_R1W.inverse() * p_R1;
    Vector3d p_R2 = T_R2W * p_W;

    cout << "该向量在一号坐标系下的坐标p_R1: " << endl << p_R1 << endl;
    cout << "该向量在世界坐标系下的坐标p_W: " << endl << p_W << endl;
    cout << "该向量在二号坐标系下的坐标p_R2: " << endl << p_R2 << endl;


    //案例：方法2：用旋转矩阵计算
    Quaterniond qq1(0.35, 0.2, 0.3, 0.1), qq2(-0.5, 0.4, -0.1, 0.2);
    Vector3d tt1(0.3, 0.1, 0.1), tt2(-0.1, 0.5, 0.3);
    Vector3d pp_R1(0.5, 0, 0.2);
    Matrix3d R1 = T_R1W.rotation();
    Matrix3d R2 = T_R2W.rotation();

    Vector3d pp_R2 = R2*R1.transpose()*pp_R1 - R2*R1.transpose()*tt1 + tt2;        

    cout.precision(6);  //恢复输出精度为6位小数
    cout << "旋转矩阵R1: " << endl << R1 << endl;
    cout << "旋转矩阵R2: " << endl << R2 << endl;
    cout << "该向量在一号坐标系下的坐标pp_R1: " << endl << pp_R1 << endl;
    cout << "该向量在二号坐标系下的坐标pp_R2: " << endl << pp_R2 << endl;
    
    return 0;
}   