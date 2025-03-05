#ifndef PLANE_GROMETRY
#define PLANE_GROMETRY

#include <geometry/BufferGeometry.h>

// ƽ�漸����

using namespace std;

class PlaneGeometry : public BufferGeometry
{
public:
    /// @brief ��������һ��ƽ�漸����
    /// @param width_height  ƽ�漸����Ŀ��
    /// @param wSegment_hSegment ƽ����ˮƽ/��ֱ����ķֶ��������ü��������ʾˮƽ/��ֱ����
    PlaneGeometry(float width = 1.0, float height = 1.0, float wSegment = 1.0, float hSegment = 1.0)
    {

        // ���ƽ������ĵ�(width_half��height_half)
        float width_half = width / 2.0f;
        float height_half = height / 2.0f;

        // ����ˮƽ/��ֱ����Ķ�������
        float gridX1 = wSegment + 1.0f; // ��Ϊn�Σ���������Ϊn+1
        float gridY1 = hSegment + 1.0f;

        float segment_width = width / wSegment;   // ˮƽ����ÿһ�εĳ���
        float segment_height = height / hSegment; // ��ֱ����ÿһ�εĳ���

        Vertex vertex;

        // ���ɶ���λ�á�����������������
        for (int iy = 0; iy < gridY1; iy++) // ����ÿ�������ÿ������
        {
            // �����ĵ�Ϊ(0,0) ��������������� xָ���� yָ����
            float y = iy * segment_height - height_half;
            for (int ix = 0; ix < gridX1; ix++)
            {
                float x = ix * segment_width - width_half;
                vertex.Position = glm::vec3(x, -y, 0.0f);    // yָ����
                vertex.Normal = glm::vec3(0.0f, 0.0f, 1.0f); // ������2Dƽ�棬�������̶�Ϊ 0,0,1
                // ������������ u,v->[0,1]   y��������������Ϊ  �����������Ͻ�Ϊ(0,0)
                vertex.TexCoords = glm::vec2(ix / wSegment, 1.0f - (iy / hSegment));

                this->vertices.push_back(vertex);
            }
        }
        // ���ɶ�������
        for (int iy = 0; iy < hSegment; iy++)
        {
            for (int ix = 0; ix < wSegment; ix++)
            {
                // ���㵱ǰ�ĸ����������
                float a = ix + gridX1 * iy;             // ���½ǵ������
                float b = ix + gridX1 * (iy + 1);       // ���Ͻǵ������
                float c = (ix + 1) + gridX1 * (iy + 1); // ���Ͻǵ������
                float d = (ix + 1) + gridX1 * iy;       // ���½ǵ������

                // �������������ε����������ɾ���
                this->indices.push_back(a);
                this->indices.push_back(b);
                this->indices.push_back(d);

                this->indices.push_back(b);
                this->indices.push_back(c);
                this->indices.push_back(d);
            }
        }

        // ����VAO VBO EBO
        this->setupBuffers();
    }
};

#endif