#ifndef _CSCI441_RENDERER_H_
#define _CSCI441_RENDERER_H_

#include <csci441/matrix3.h>

class Renderer {
    Matrix3 itModel_tmp;
public:

    void render(const Camera& camera, Model& m, const Vector4& light) {
        itModel_tmp.inverse_transpose(m.model);

        m.shader.use();
        Uniform::set(m.shader.ID, "model", m.model);
        Uniform::set(m.shader.ID, "projection", camera.projection);
        Uniform::set(m.shader.ID, "camera", camera.look_at());
        Uniform::set(m.shader.ID, "eye", camera.eye);
        Uniform::set(m.shader.ID, "lightPos", light);
        Uniform::set(m.shader.ID, "itModel", itModel_tmp);

        if (m.flat)
        {
            std::cout << "render flat" << std::endl;

        }
        // render the cube
        glBindVertexArray(m.vao);
        glDrawArrays(GL_TRIANGLES, 0, m.size);
        
    }
};

#endif
