#ifndef AXBOPENGLWIDGET_H
#define AXBOPENGLWIDGET_H

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QTime>
#include <QTimer>
#include <QOpenGLShaderProgram>
#include <QWheelEvent>
#include "camera.h"
#include <QOpenGLTexture>
#include "model.h"

class AXBOpenglWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit AXBOpenglWidget(QWidget *parent = nullptr);
    ~AXBOpenglWidget();
    void loadModel(string path);
protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
signals:

public slots:
    void on_timeout();
private:
    QTime m_time;
    QTimer m_timer;
    QOpenGLShaderProgram m_ShaderProgram;
    QOpenGLShaderProgram m_LightShaderProgram;
    Camera m_camera;
    QOpenGLTexture * m_diffuseTex;
    QOpenGLTexture * m_specularTex;
//    Mesh *m_mesh;
    //把现有的数据给mesh类，并在axbopenglwidget.cpp那边用一个mesh对象接住
//    Mesh* processMess();

    Model * m_model = NULL;
    QVector3D cameraPosInit(float maxY, float minY);
};

#endif // AXBOPENGLWIDGET_H
