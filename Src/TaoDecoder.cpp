#include "TaoDecoder.h"
#include <QDebug>
/* Enable or disable frame reference counting. You are not supposed to support
 * both paths in your application but pick the one most appropriate to your
 * needs. Look for the use of refcount in this example to see what are the
 * differences of API usage between them. */
static int gRefCount = 0;
static void pgm_save(unsigned char* buf, int wrap, int xsize, int ysize, char* filename)
{
    FILE* f;
    int i;

    f = fopen(filename, "w");
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}
static void outError(int num)
{
    char errorStr[1024];
    av_strerror(num, errorStr, sizeof errorStr);
    qDebug() << "FFMPEG ERROR:" << QString(errorStr);
}
static int open_codec_context(int& streamIndex, AVCodecContext** decCtx, AVFormatContext* fmtCtx, enum AVMediaType type)
{
    int ret;
    int index;
    AVStream* st;
    AVCodec* codec = nullptr;
    AVDictionary* opts = nullptr;
    ret = av_find_best_stream(fmtCtx, type, -1, -1, nullptr, 0);
    if (ret < 0)
    {
        qWarning() << "Could not find stream " << av_get_media_type_string(type);
        return ret;
    }
    index = ret;
    st = fmtCtx->streams[index];
    codec = avcodec_find_decoder(st->codecpar->codec_id);
    if (!codec)
    {
        qWarning() << "Cound not find codec " << av_get_media_type_string(type);
        return AVERROR(EINVAL);
    }
    *decCtx = avcodec_alloc_context3(codec);
    if (!*decCtx)
    {
        qWarning() << "Failed to allocate codec context " << av_get_media_type_string(type);
        return AVERROR(ENOMEM);
    }
    ret = avcodec_parameters_to_context(*decCtx, st->codecpar);
    if (ret < 0)
    {
        qWarning() << "Failed to copy codec parameters to decoder context" << av_get_media_type_string(type);
        return ret;
    }
    av_dict_set(&opts, "refcounted_frames", gRefCount ? "1" : "0", 0);

    ret = avcodec_open2(*decCtx, codec, &opts);
    if (ret < 0)
    {
        qWarning() << "Failed to open codec " << av_get_media_type_string(type);
        return ret;
    }
    streamIndex = index;
    return 0;
}
void TaoDecoder::init()
{
    avformat_network_init();
}

void TaoDecoder::uninit()
{
    avformat_network_deinit();
}

void TaoDecoder::load(const QString& file)
{
    int ret = 0;
    ret = avformat_open_input(&m_fmtCtx, file.toStdString().data(), nullptr, nullptr);
    if (0 > ret)
    {
        qWarning() << "open url error";
        outError(ret);
        return;
    }
    ret = avformat_find_stream_info(m_fmtCtx, nullptr);
    if (0 > ret)
    {
        qWarning() << "find stream failed";
        outError(ret);
        return;
    }
    ret = open_codec_context(m_videoStreamIndex, &m_videoCodecCtx, m_fmtCtx, AVMEDIA_TYPE_VIDEO);
    if (ret < 0)
    {
        qWarning() << "open_codec_context failed";
        return;
    }
    m_videoStream = m_fmtCtx->streams[m_videoStreamIndex];
    m_width = m_videoCodecCtx->width;
    m_height = m_videoCodecCtx->height;
    m_pixFmt = m_videoCodecCtx->pix_fmt;

    emit videoInfoReady(m_width, m_height, m_pixFmt);

    av_dump_format(m_fmtCtx, 0, file.toStdString().data(), 0);
    do
    {
        if (!m_videoStream)
        {
            qWarning() << "Could not find audio or video stream in the input, aborting";
            break;
        }
        m_frame = av_frame_alloc();
        if (!m_frame)
        {
            qWarning() << "Could not allocate frame";
            break;
        }
        demuxing();
    } while (0);
    avcodec_free_context(&m_videoCodecCtx);
    avformat_close_input(&m_fmtCtx);
    av_frame_free(&m_frame);
}

void TaoDecoder::demuxing()
{
    av_init_packet(&m_packet);
    m_packet.data = nullptr;
    m_packet.size = 0;
    while (av_read_frame(m_fmtCtx, &m_packet) >= 0)
    {
        if (m_packet.stream_index == m_videoStreamIndex)
        {
            if (avcodec_send_packet(m_videoCodecCtx, &m_packet) == 0)
            {
                while (avcodec_receive_frame(m_videoCodecCtx, m_frame) == 0)
                {
                    decodeFrame();
                }
            }
        }
    }
    if (avcodec_send_packet(m_videoCodecCtx, nullptr) == 0)
    {
        while (avcodec_receive_frame(m_videoCodecCtx, m_frame) == 0)
        {
            decodeFrame();
        }
    }
}

void TaoDecoder::decodeFrame()
{

    switch (m_frame->format)
    {
    case AV_PIX_FMT_YUV420P: {
        m_yuvData.Y.resize(m_frame->linesize[0] * m_frame->height);
        memcpy(m_yuvData.Y.data(), m_frame->data[0], static_cast<size_t>(m_yuvData.Y.size()));
        m_yuvData.U.resize(m_frame->linesize[1] * m_frame->height / 2);
        memcpy(m_yuvData.U.data(), m_frame->data[1], static_cast<size_t>(m_yuvData.U.size()));
        m_yuvData.V.resize(m_frame->linesize[2] * m_frame->height / 2);
        memcpy(m_yuvData.V.data(), m_frame->data[2], static_cast<size_t>(m_yuvData.V.size()));
        m_yuvData.yLineSize = m_frame->linesize[0];
        m_yuvData.uLineSize = m_frame->linesize[1];
        m_yuvData.vLineSize = m_frame->linesize[2];
        m_yuvData.height = m_frame->height;
        emit videoFrameDataReady(m_yuvData);

        //            char buf[1024];
        //            snprintf(buf, sizeof(buf), "%d%s", m_videoCodecCtx->frame_number, ".pgm");
        //            pgm_save(m_frame->data[0], m_frame->linesize[0],
        //                     m_frame->width, m_frame->height, buf);

        break;
    }
        //                        case AV_PIX_FMT_NV12:
        //                        {
        //                            break;
        //                        }
    default:
        // TODO support other format
        break;
    }
}

//--------------------------------------------------------------------------------//
TaoDecoderController::TaoDecoderController(QObject* parent)
    : QObject(parent)
{
    m_videoDataCache.setCapacity(1024);

    m_decoder = new TaoDecoder;
    m_decoder->moveToThread(&m_thread);
    connect(&m_thread, &QThread::finished, m_decoder, &TaoDecoder::deleteLater);
    connect(this, &TaoDecoderController::init, m_decoder, &TaoDecoder::init);
    connect(this, &TaoDecoderController::uninit, m_decoder, &TaoDecoder::uninit);
    connect(this, &TaoDecoderController::load, m_decoder, &TaoDecoder::load);

    qRegisterMetaType<YUVData>();
    connect(m_decoder, &TaoDecoder::videoInfoReady, this, &TaoDecoderController::videoInfoReady);
    connect(m_decoder, &TaoDecoder::videoFrameDataReady, this, &TaoDecoderController::onVideoFrameDataReady);

    m_thread.start();
    emit init();
}

TaoDecoderController::~TaoDecoderController()
{
    if (m_thread.isRunning())
    {
        emit uninit();
        m_thread.quit();
        m_thread.wait();
    }
}

YUVData TaoDecoderController::getFrame(bool& got)
{
    if (m_videoDataCache.isEmpty())
    {
        got = false;
        return {};
    }
    got = true;
    return m_videoDataCache.takeFirst();
}

void TaoDecoderController::onVideoFrameDataReady(YUVData data)
{
    m_videoDataCache.append(data);
}
