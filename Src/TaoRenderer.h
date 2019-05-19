#pragma once

#include <QOpenGLFunctions>
#include <QQuickFramebufferObject>
#include <QOpenGLTexture>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <memory>

#include "TaoDecoder.h"

class TaoRenderer : public QOpenGLFunctions
{
public:
    TaoRenderer();
    ~TaoRenderer();
    void init();
    void paint();
    void resize(int width, int height);
    void updateTextureInfo(int width, int height, int format);
    void updateTextureData(const YUVData &data);
protected:
    void initTexture();
    void initShader();
    void initGeometry();
private:

    QOpenGLShaderProgram mProgram;
    QOpenGLTexture *mTexY = nullptr;
    QOpenGLTexture *mTexU = nullptr;
    QOpenGLTexture *mTexV = nullptr;
    QVector<QVector3D> mVertices;
    QVector<QVector2D> mTexcoords;
    int mModelMatHandle, mViewMatHandle, mProjectMatHandle;
    int mVerticesHandle;
    int mTexCoordHandle;

    QMatrix4x4 mModelMatrix;
    QMatrix4x4 mViewMatrix;
    QMatrix4x4 mProjectionMatrix;
    GLint mPixFmt = 0;
    bool mTextureAlloced = false;
};

