#include "TaoItem.h"
#include "TaoRenderer.h"
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
//************TaoItemRender************//
class TaoItemRender : public QQuickFramebufferObject::Renderer
{
public:
    TaoItemRender();

    void render() override;
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;
    void synchronize(QQuickFramebufferObject *) override;
private:
    TaoRenderer m_render;
    QQuickWindow *m_window = nullptr;
};

TaoItemRender::TaoItemRender()
{
    m_render.init();
}

void TaoItemRender::render()
{
    m_render.paint();
    m_window->resetOpenGLState();
}

QOpenGLFramebufferObject *TaoItemRender::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    m_render.resize(size.width(), size.height());
    return new QOpenGLFramebufferObject(size, format);
}

void TaoItemRender::synchronize(QQuickFramebufferObject *item)
{
    TaoItem *pItem = qobject_cast<TaoItem *>(item);
    if (pItem)
    {
        if (!m_window)
        {
            m_window = pItem->window();
        }
        if (pItem->infoDirty())
        {
            m_render.updateTextureInfo(pItem->videoWidth(), pItem->videoHeght(), pItem->videoFormat());
            pItem->makeInfoDirty(false);
        }
        bool got = false;
        YUVData data = pItem->getFrame(got);
        if (got)
        {
            m_render.updateTextureData(data);
        }
    }
}

//************TaoItem************//
TaoItem::TaoItem(QQuickItem *parent) : QQuickFramebufferObject (parent)
{
    m_decoderController = std::make_unique<TaoDecoderController>();
    connect(m_decoderController.get(), &TaoDecoderController::videoInfoReady, this, &TaoItem::onVideoInfoReady);
    //按每秒60帧的帧率更新界面
    startTimer(1000 / 60);
}

void TaoItem::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    update();
}

YUVData TaoItem::getFrame(bool &got)
{
    return m_decoderController->getFrame(got);
}

void TaoItem::loadFile(const QUrl &url)
{
    m_decoderController->load(url.toLocalFile());
}

void TaoItem::onVideoInfoReady(int width, int height, int format)
{
    if (m_videoWidth != width)
    {
        m_videoWidth = width;
        makeInfoDirty(true);
    }
    if (m_videoHeight != height)
    {
        m_videoHeight = height;
        makeInfoDirty(true);
    }
    if (m_videoFormat != format)
    {
        m_videoFormat = format;
        makeInfoDirty(true);
    }
}

QQuickFramebufferObject::Renderer *TaoItem::createRenderer() const
{
    return new TaoItemRender;
}

