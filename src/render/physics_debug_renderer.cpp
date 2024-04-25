//#include "physics_debug_renderer.h"
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//
//void PhysicsDebugRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
//    glColor4f(color.r, color.g, color.b, 0.5f);
//    glBegin(GL_LINE_LOOP);
//    for (int i = 0; i < vertexCount; i++) {
//        b2Vec2 v = vertices[i];
//        glVertex2f(v.x, v.y);
//    }
//    glEnd();
//}
//
//void PhysicsDebugRenderer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
//
//    glColor4f(color.r, color.g, color.b, 0.5f);
//    glBegin(GL_TRIANGLE_FAN);
//    for (int i = 0; i < vertexCount; i++) {
//        b2Vec2 v = vertices[i];
//        glVertex2f(v.x, v.y);
//    }
//    glEnd();
//}
//
//void PhysicsDebugRenderer::DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {
//    const float k_segments = 16.0f;
//    int vertexCount = 16;
//    const float k_increment = 2.0f * b2_pi / k_segments;
//    float theta = 0.0f;
//
//    glColor4f(color.r, color.g, color.b, 1);
//    glBegin(GL_LINE_LOOP);
//    GLfloat glVertices[vertexCount * 2];
//    for (int32 i = 0; i < k_segments; ++i) {
//        b2Vec2 v = center + radius * b2Vec2(cos(theta), sin(theta));
//        glVertex2f(v.x, v.y);
//        theta += k_increment;
//    }
//    glEnd();
//}
//
//void PhysicsDebugRenderer::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {
//    const float k_segments = 16.0f;
//    int vertexCount = 16;
//    const float k_increment = 2.0f * b2_pi / k_segments;
//    float theta = 0.0f;
//
//    glColor4f(color.r, color.g, color.b, 0.5f);
//    glBegin(GL_TRIANGLE_FAN);
//    GLfloat glVertices[vertexCount * 2];
//    for (int32 i = 0; i < k_segments; ++i) {
//        b2Vec2 v = center + radius * b2Vec2(cos(theta), sin(theta));
//        glVertex2f(v.x, v.y);
//        theta += k_increment;
//    }
//    glEnd();
//
//    DrawSegment(center, center + radius * axis, color);
//}
//
//void PhysicsDebugRenderer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
//    glColor4f(color.r, color.g, color.b, 1);
//    glBegin(GL_LINES);
//    glVertex2f(p1.x, p1.y);
//    glVertex2f(p2.x, p2.y);
//    glEnd();
//}
//
//void PhysicsDebugRenderer::DrawPoint(const b2Vec2& p, float size, const b2Color& color) {
//    glColor4f(color.r, color.g, color.b, 1);
//    glPointSize(1.0f);
//    glBegin(GL_POINTS);
//    glVertex2f(p.x, p.y);
//    glEnd();
//}
//
//void PhysicsDebugRenderer::DrawTransform(const b2Transform& xf) {
//
//    b2Vec2 p1 = xf.p, p2;
//    const float k_axisScale = 0.4f;
//
//    p2 = p1 + k_axisScale * xf.q.GetXAxis();
//    DrawSegment(p1, p2, b2Color(1, 0, 0));
//
//    p2 = p1 + k_axisScale * xf.q.GetYAxis();
//    DrawSegment(p1, p2, b2Color(0, 1, 0));
//}
