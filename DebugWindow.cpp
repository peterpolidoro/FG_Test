#include "DebugWindow.h"
#include "Version.h"
#include "ui_DebugWindow.h"
#include "Util.h"
#include <QTimer>
#include <QDesktopWidget>
#include <QApplication>
#include <QPixmap>
#include <QGraphicsView>
#include <QStandardPaths>
#include <QFileDialog>
#include <QSettings>
#include <QTextBrowser>
#include "Settings.h"
#include "App.h"

DebugWindow::DebugWindow(QWidget *parent) :
    QMainWindow(parent, Qt::Dialog/*|Qt::MSWindowsFixedSizeDialogHint*/),
    ui(new Ui::DebugWindow)
{
    using Util::Connect;
    using Util::app;

    ui->setupUi(this);
    ui->verboseChk->setChecked(app() && app()->isVerboseDebugMode());
    Connect(ui->verboseChk, SIGNAL(toggled(bool)), app(), SLOT(setVerboseDebugMode(bool)));
    Connect(ui->clearBut, SIGNAL(clicked(bool)), this, SLOT(clearLog()));

#ifdef Q_OS_WIN
    setWindowTitle("Debug Console");
#else
    setWindowTitle(QString(APPNAME) + " - Debug Console");
#endif
    setWindowIcon(QIcon(":/Img/app_icon.png"));
    ui->tb->setFontPointSize(8.0);
}


DebugWindow::~DebugWindow()
{
    delete ui; ui = nullptr;
}

QTextBrowser *DebugWindow::console() { return ui->tb; }

void DebugWindow::printSettings(const Settings &s)
{
    ui->settingsTB->clear();
    ui->settingsTB->append(s.toPrettyString());
    auto tc = ui->settingsTB->textCursor();
    tc.setPosition(0);
    ui->settingsTB->setTextCursor(tc);
}

void DebugWindow::clearLog()
{
    ui->tb->clear();
}
