#pragma once

extern "C"
{
#define __STDC_CONSTANT_MACROS
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
}

#include <QContiguousCache>
#include <QThread>
#include <QObject>
const static int cDataCount = 4;
const static int cBufferSize = 1024 * 768;
struct YUVData {
    YUVData()
    {
        Y.reserve(cBufferSize);
        U.reserve(cBufferSize);
        V.reserve(cBufferSize);
    }
    QByteArray Y;
    QByteArray U;
    QByteArray V;
    int yLineSize;
    int uLineSize;
    int vLineSize;
    int height;
};
Q_DECLARE_METATYPE(YUVData);

class TaoDecoder : public QObject
{
    Q_OBJECT
public:

public slots:
    void init();
    void uninit();
    void load(const QString &file);
signals:
    void videoFrameDataReady(YUVData data);
    void videoInfoReady(int width, int height, int format);
protected:
    void demuxing();
    void decodeFrame();
private:
    AVFormatContext *m_fmtCtx = nullptr;
    AVCodecContext *m_videoCodecCtx = nullptr;
    AVStream *m_videoStream = nullptr;
    AVFrame *m_frame = nullptr;
    AVPacket m_packet;
    int m_videoStreamIndex = 0;

    AVPixelFormat m_pixFmt;
    int m_width, m_height;

    YUVData m_yuvData;
};

class TaoDecoderController : public QObject
{
    Q_OBJECT
public:
    TaoDecoderController(QObject *parent = nullptr);
    ~TaoDecoderController();

    YUVData getFrame(bool &got);
signals:
    void init();
    void uninit();
    void pause(bool );
    void load(const QString &file);

    void videoInfoReady(int width, int height, int format);
protected slots:
    void onVideoFrameDataReady(YUVData data);
private:
    TaoDecoder *m_decoder = nullptr;
    QThread m_thread;
    QContiguousCache<YUVData> m_videoDataCache;
};

