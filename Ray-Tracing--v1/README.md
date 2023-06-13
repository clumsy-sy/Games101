# Ray Tracing V1

本项目是 [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html#diffusematerials/fixingshadowacne) 的学习记录，大致顺序按照书本，但是会有细微变化。

## 准备

与书中使用的 `PPM` 格式不同，本项目使用 `BMP` 格式（代码详细见 `BMP.hpp` 文件），有一下几个原因
1.  `PPM` 格式打开有些慢
2.  书本使用重定向命令将输出到文件，这也比较慢，并且如果想在程序中输出些调试信息也不方便

打算使用进度条，由于本项目最后的程序需要运行很久，所有加了进度条显示进度（代码详见 `global.hpp` 文件）
![Alt](images/progress.png)

项目使用 cmake 工具，提供了 `build py` 脚本，默认开启 `release`
```shell
$ python build.py [argvs] # 支持任意数量参数，会嵌入最后可执行文件后
```
还有 `justfile` 工具
```shell
just fmt # 格式化
just clean # 清空 build 文件夹
just build # 编译运行
just move # 讲生成的图片移到 images 文件夹
```
测试运行第一份代码
```cpp
#include "BMP.hpp"
#include "global.hpp"
auto main() -> int {
  // Image
  const int image_width = 256;
  const int image_height = 256;

  // Render
  bmp::bitmap photo(image_width, image_height);
  for (int j = 0; j < image_height; ++j) {
    for (int i = 0; i < image_width; ++i) {
      auto r = double(i) / (image_width - 1);
      auto g = double(j) / (image_height - 1);
      auto b = 0.25;
      photo.set(i, j, {r, g, b});
    }
    UpdateProgress(j, image_height - 1);
  }
  photo.generate("image1.bmp");

  return 0;
}
```
如图所示

![Alt](images/image1.bmp)


## 路径追踪

### 封装 Vec3d 类

与原文不同，多加了字母 `d` 代表 `double` 以表示存储的变量类型。
既可以用来表示点，又可以用来表示颜色

### 封装射线类

把射线看作一个函数, A 是原点，B 是射线方向
$$
P(t) = A + t \cdot B (t \ge 0)
$$

将摄像机放在坐标原点，向 -z 方向看，y 轴朝上，z 轴朝右，从左下角（书上说左上角，但是实际上是左下角，这与 bmp 格式非常符合）开始遍历像素，从原点作为光线原点，向像素点方向发出射线，得到返回值。

单线性插值
$$
lerp = (1 - t) \cdot startValue + t \cdot endValue (0.0 \leqslant t \leqslant 1.0)
$$

![Alt](images/image2.bmp)

