#pragma once
#include "TaoDecoder.h"
#include <QQuickFramebufferObject>
#include <QQuickItem>
#include <memory>
class TaoItem : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    TaoItem(QQuickItem* parent = nullptr);
    void timerEvent(QTimerEvent* event) override;

    YUVData getFrame(bool& got);
    bool infoDirty() const
    {
        return m_infoChanged;
    }
    void makeInfoDirty(bool dirty)
    {
        m_infoChanged = dirty;
    }
    int videoWidth() const
    {
        return m_videoWidth;
    }
    int videoHeght() const
    {
        return m_videoHeight;
    }
    int videoFormat() const
    {
        return m_videoFormat;
    }
public slots:
    void loadFile(const QUrl& url);

protected slots:
    void onVideoInfoReady(int width, int height, int format);

public:
    Renderer* createRenderer() const override;
    std::unique_ptr<TaoDecoderController> m_decoderController = nullptr;
    int m_videoWidth;
    int m_videoHeight;
    int m_videoFormat;
    bool m_infoChanged = false;
};
