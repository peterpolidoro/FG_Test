#include "Settings.h"
#include "Version.h"
#include "Frame.h"
#include <QSettings>
#include <QTextStream>
#include <QStandardPaths>
Settings::Settings()
{
    load(All);
}

Settings::~Settings()
{
}


static const QString fmtStrings[] =
{
    "RAW", "PNG", "JPG", "Mpeg2", "Mpeg4", "H.264", "ProRes4444", "ProRes422", "MJPEG", "FFV1", "LJPEG", "APNG",  "GIF", QString()
};

static const QString fmtPrettyStrings[] =
{
    "RAW", "PNG", "JPEG", "Mpeg2", "Mpeg4", "H.264", "Apple ProRes4444", "Apple ProRes422", "MJPEG (Motion JPEG)", "FFV1 (lossless)", "LJPEG (lossless JPEG)", "APNG (lossless)", "GIF (animated)", QString()
};

const std::set<Settings::Fmt> Settings::EnabledFormats = {
    Fmt_RAW, Fmt_PNG, Fmt_JPG, Fmt_FFV1, Fmt_MJPEG, Fmt_LJPEG, Fmt_Mpeg2, Fmt_Mpeg4, Fmt_H264
};

const std::set<Settings::Fmt> Settings::ZipableFormats = {
    Fmt_RAW, Fmt_PNG, Fmt_JPG
};

const std::set<Settings::Fmt> Settings::FFmpegFormats = {
    Fmt_FFV1, Fmt_MJPEG, Fmt_LJPEG, Fmt_Mpeg2, Fmt_Mpeg4, Fmt_H264
};


/*static*/ QString Settings::fmt2String(Fmt f, bool pretty) {
    QString ret;
    if (f < Fmt_N) ret = pretty ? fmtPrettyStrings[f] : fmtStrings[f];
    return ret;
}

/* static */ Settings::Fmt Settings::string2Fmt(const QString & s, bool pretty) {
    const QString *fmts = pretty ? fmtPrettyStrings : fmtStrings;
    for (int i = 0; i < int(Fmt_N); ++i)
        if (s.startsWith(fmts[i],Qt::CaseInsensitive) || fmts[i].startsWith(s,Qt::CaseInsensitive))
            return Fmt(i);
    return Fmt_N;
}


void Settings::load(int scope)
{
    QSettings s(APPDOMAIN, QString(APPNAME).split(' ').join(""));

//    Debug() << "Settings file: " << s.fileName();

    if (scope & Main) {
        format = string2Fmt(s.value("format",fmt2String(Fmt_MJPEG)).toString());
        if (!EnabledFormats.count(format)) format = defaultFormat;
        zipEmbed = s.value("zipEmbed", true).toBool();
        saveDir = s.value("saveDir", QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)).toString();
        savePrefix = s.value("savePrefix", "Recording").toString();
        fps = s.value("fps", Frame::DefaultFPS()).toDouble();
    }
    if (scope & UART) {
        // uart related
        uart.portName = s.value("uart_portName", "xx").toString();
        uart.baud = s.value("uart_baud", 115200).toInt();
        uart.flowControl = s.value("uart_flowControl", 0).toInt();
        uart.bpsEncoded = s.value("uart_bpsEncoded", int((8&0xff)<<16)|((0&0xff)<<8)|(1&0xff)).toInt();
    }
    if (scope & Other) {
        other.verbosity = s.value("verbosity", 2).toInt();
    }
    if (scope & Appearance) {
        appearance.useDarkStyle = s.value("useDarkStyle", true).toBool();
    }
}

void Settings::save(int scope)
{
    QSettings s(APPDOMAIN, QString(APPNAME).split(' ').join(""));

    if (scope & Main) {
        s.setValue("format", fmt2String(format));
        s.setValue("saveDir", saveDir);
        s.setValue("savePrefix", savePrefix);
        s.setValue("zipEmbed", zipEmbed);
        s.setValue("fps", fps);
    }
    if (scope & UART) {
        s.setValue("uart_portName", uart.portName);
        s.setValue("uart_baud", uart.baud);
        s.setValue("uart_flowControl", uart.flowControl);
        s.setValue("uart_bpsEncoded", uart.bpsEncoded);
    }
    if (scope & Other) {
        s.setValue("verbosity", other.verbosity);
    }
    if (scope & Appearance) {
        s.setValue("useDarkStyle", appearance.useDarkStyle);
    }
}

QString Settings::toPrettyString() const
{
    QString ret;
    {
        QTextStream ts(&ret,QIODevice::WriteOnly);
        ts << "saveDir = " << saveDir << "\n";
        ts << "savePrefix = " << savePrefix << "\n";
        ts << "format = " << fmt2String(format, false) << "\n";
        ts << "verbosity = " << other.verbosity << "\n";
        ts << "useDarkStyle = " << appearance.useDarkStyle << "\n";
        ts.flush();
    }
    return ret;
}
