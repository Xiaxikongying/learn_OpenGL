#ifndef PLANE_GROMETRY
#define PLANE_GROMETRY

#include <geometry/BufferGeometry.h>

// 平面几何体

using namespace std;

class PlaneGeometry : public BufferGeometry
{
public:
    /// @brief 用于生成一个平面几何体
    /// @param width_height  平面几何体的宽高
    /// @param wSegment_hSegment 平面在水平/垂直方向的分段数（即用几个顶点表示水平/垂直方向）
    PlaneGeometry(float width = 1.0, float height = 1.0, float wSegment = 1.0, float hSegment = 1.0)
    {

        // 获得平面的中心点(width_half，height_half)
        float width_half = width / 2.0f;
        float height_half = height / 2.0f;

        // 计算水平/垂直方向的顶点数量
        float gridX1 = wSegment + 1.0f; // 分为n段，顶点数量为n+1
        float gridY1 = hSegment + 1.0f;

        float segment_width = width / wSegment;   // 水平方向每一段的长度
        float segment_height = height / hSegment; // 垂直方向每一段的长度

        Vertex vertex;

        // 生成顶点位置、法向量、纹理坐标
        for (int iy = 0; iy < gridY1; iy++) // 遍历每个网格的每个顶点
        {
            // 以中心点为(0,0) 计算其他点的坐标 x指向朝右 y指向朝下
            float y = iy * segment_height - height_half;
            for (int ix = 0; ix < gridX1; ix++)
            {
                float x = ix * segment_width - width_half;
                vertex.Position = glm::vec3(x, -y, 0.0f);    // y指向朝下
                vertex.Normal = glm::vec3(0.0f, 0.0f, 1.0f); // 由于是2D平面，法向量固定为 0,0,1
                // 计算纹理坐标 u,v->[0,1]   y轴这样计算是因为  纹理是以左上角为(0,0)
                vertex.TexCoords = glm::vec2(ix / wSegment, 1.0f - (iy / hSegment));

                this->vertices.push_back(vertex);
            }
        }
        // 生成顶点索引
        for (int iy = 0; iy < hSegment; iy++)
        {
            for (int ix = 0; ix < wSegment; ix++)
            {
                // 计算当前四个顶点的索引
                float a = ix + gridX1 * iy;             // 左下角点的索引
                float b = ix + gridX1 * (iy + 1);       // 左上角点的索引
                float c = (ix + 1) + gridX1 * (iy + 1); // 右上角点的索引
                float d = (ix + 1) + gridX1 * iy;       // 右下角点的索引

                // 生成两个三角形的索引，构成矩形
                this->indices.push_back(a);
                this->indices.push_back(b);
                this->indices.push_back(d);

                this->indices.push_back(b);
                this->indices.push_back(c);
                this->indices.push_back(d);
            }
        }

        // 生成VAO VBO EBO
        this->setupBuffers();
    }
};

#endif