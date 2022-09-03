#include "axbopenglwidget.h"


unsigned int VAO,VBO,lightVAO;
QVector3D lightPos(1.2f, 1.0f, 2.0f);
QVector3D lightColor(1.0f, 1.0f, 1.0f);
QVector3D objectColor(1.0f, 0.5f, 0.31f);
QVector3D viewInitPos(0.0, 0.0, 5.0);

const unsigned int timeOutmSec = 50;

AXBOpenglWidget::AXBOpenglWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(on_timeout()));
    m_timer.start(timeOutmSec);
    m_time.start();
    m_camera.Position = viewInitPos;
    setFocusPolicy(Qt::StrongFocus);
    //setMouseTracking(true);

}

AXBOpenglWidget::~AXBOpenglWidget()
{
    makeCurrent();
    glDeleteBuffers(1,&VBO);
    glDeleteVertexArrays(1,&VAO);
    glDeleteVertexArrays(1,&lightVAO);

    doneCurrent();

}

void AXBOpenglWidget::loadModel(string path)
{
    if(m_model !=NULL)
        delete m_model;

    m_model=NULL;


    makeCurrent();
    //如果不是在initializeGL里面，你是拿不到当前的状态的。所以要加上面的makeCurrent等语句
    m_model=new Model(QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>()
                    ,path.c_str());

    m_camera.Position = cameraPosInit(m_model->m_maxY, m_model->m_minY);
    doneCurrent();
}

void AXBOpenglWidget::initializeGL()
{
    initializeOpenGLFunctions(); //创建VBO和VAO对象，并赋予ID
    //创建lightVAO对象
    //着色以及缓冲的创建都放到了mesh的构造函数里了，构造函数的最后一句就是setupMesh

    bool success;
    m_ShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/shaders/shapes.vert");
    m_ShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shaders/shaders/shapes.frag");
    success=m_ShaderProgram.link();
    if(!success) qDebug()<<"ERR:"<<m_ShaderProgram.log();

    //纹理都通过model加载
//    m_diffuseTex = new QOpenGLTexture(QImage(":/images/images/container2.png").mirrored());
//    m_specularTex = new QOpenGLTexture(QImage(":/images/images/container2_specular.png").mirrored());
    m_ShaderProgram.bind();
    //这里的纹理设置也封装掉好了

    m_LightShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/shaders/light.vert");
    m_LightShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shaders/shaders/light.frag");
    success=m_LightShaderProgram.link();
    if(!success) qDebug()<<"ERR:"<<m_LightShaderProgram.log();


}

void AXBOpenglWidget::resizeGL(int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h);
}

void AXBOpenglWidget::paintGL()
{
    if(m_model == NULL) return;
    QMatrix4x4 model;
    QMatrix4x4 view;
    QMatrix4x4 projection;
//    float time=m_time.elapsed()/50.0;
    projection.perspective(m_camera.Zoom,(float)width()/height(),0.1,100); view=m_camera.GetViewMatrix(); glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_ShaderProgram.bind();
    m_ShaderProgram.setUniformValue("projection", projection);
    m_ShaderProgram.setUniformValue("view", view);
//    model.rotate(time, 1.0f, 1.0f, 0.5f);
    m_ShaderProgram.setUniformValue("model", model);

    m_ShaderProgram.setUniformValue("viewPos", m_camera.Position);

    //设置材质
    // light properties, note that all light colors are set at full intensity
    m_ShaderProgram.setUniformValue("light.ambient", 0.3f, 0.3f, 0.3f);
    m_ShaderProgram.setUniformValue("light.diffuse", 0.9f, 0.9f, 0.9f);
    m_ShaderProgram.setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);
    // material properties
    m_ShaderProgram.setUniformValue("material.shininess", 32.0f);
    m_ShaderProgram.setUniformValue("light.direction", -0.2f, -1.0f, -0.3f);


    //纹理绑定不在此处进行，是为了应对以后加载纹理更复杂的情况
//    m_diffuseTex->bind(0);
//    m_specularTex->bind(1);

//    glDrawArrays(GL_TRIANGLES, 0, 36);
    // positions all containers

    m_ShaderProgram.setUniformValue("model", model);
//    glDrawArrays(GL_TRIANGLES,0,36);
    m_model->Draw(m_ShaderProgram);

}

void AXBOpenglWidget::wheelEvent(QWheelEvent *event)
{
    m_camera.ProcessMouseScroll(event->angleDelta().y()/120);

}

void AXBOpenglWidget::keyPressEvent(QKeyEvent *event)
{
    float deltaTime=timeOutmSec/1000.0;
    switch (event->key()) {
    case Qt::Key_W: m_camera.ProcessKeyboard(FORWARD,deltaTime);break;
    case Qt::Key_S: m_camera.ProcessKeyboard(BACKWARD,deltaTime);break;
    case Qt::Key_D: m_camera.ProcessKeyboard(RIGHT,deltaTime);break;
    case Qt::Key_A: m_camera.ProcessKeyboard(LEFT,deltaTime);break;
    case Qt::Key_Q: m_camera.ProcessKeyboard(DOWN,deltaTime);break;
    case Qt::Key_E: m_camera.ProcessKeyboard(UP,deltaTime);break;
    case Qt::Key_Space: m_camera.Position=viewInitPos;break;
    default: break;
    }
}

void AXBOpenglWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::RightButton){
        static QPoint lastPos(width()/2,height()/2);
        auto currentPos=event->pos();
        QPoint deltaPos=currentPos-lastPos;
        lastPos=currentPos;

        m_camera.ProcessMouseMovement(deltaPos.x(),-deltaPos.y());
    }

}



void AXBOpenglWidget::on_timeout()
{
    update();
}

QVector3D AXBOpenglWidget::cameraPosInit(float maxY, float minY)
{
    QVector3D temp={0,0,0};
    float height = maxY - minY;
    temp.setZ(1.5 * height);

    if(minY >= 0)
        temp.setY(height/2.0);
    viewInitPos = temp;

    return temp;

}

