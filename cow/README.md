# 可爱的牛牛

## 简介

正如整个项目所说，这并不能算是某次的作业，而是我对某几次作业的重构与合并，对于前面的三次作业，最合适作为主体的便是作业三，因为它差不多包含了前两次的所有内容，整体结构上也差不多。

这是 GAMES101提到的经典的光栅化过程。

## 光栅化

### MVP 变换

MVP变换有顺序
1. 缩放、再旋转整个空间
2. 将摄像机移到坐标原点
3. 透视投影到正交投影，空间中心移到原点，缩放到 1 Cube 空间。

### rasterizer::draw

### rasterizer::rasterize_triangle

### fragment_shader_payload

## 链接

[课程链接](https://www.bilibili.com/video/BV1X7411F744/?spm_id_from=333.999.0.0&vd_source=2755a8e2f7027d4806f7514ef9c3663a)

[作业链接](https://games-cn.org/forums/topic/s2021-games101-zuoyehuizong/)