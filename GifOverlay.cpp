#include "GifOverlay.h"

#include <QApplication>
#include <QStyle>
#include <QFileDialog>
#include <QScreen>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include <QResizeEvent>
#include <QPainter>
#include <QTransform>

// ─────────────────────────────────────────────
//  Constructor / Destructor
// ─────────────────────────────────────────────

GifOverlay::GifOverlay(QWidget *parent)
    : QWidget(parent)
    , m_gifLabel(nullptr)
    , m_closeBtn(nullptr)
    , m_mirrorBtn(nullptr)
    , m_resizeHandle(nullptr)
    , m_movie(nullptr)
    , m_trayIcon(nullptr)
    , m_dragging(false)
    , m_resizing(false)
    , m_gifSize(150, 150)
    , m_mirrored(false)
    , m_settings("GifOverlay", "GifOverlay")
{
    setupUI();
    setupTrayIcon();
    loadSettings();
}

GifOverlay::~GifOverlay()
{
    saveSettings();
}

// ─────────────────────────────────────────────
//  Paint — fully transparent
// ─────────────────────────────────────────────

void GifOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(rect(), Qt::transparent);
}

// ─────────────────────────────────────────────
//  UI Setup
// ─────────────────────────────────────────────

void GifOverlay::setupUI()
{
    setWindowFlags(
        Qt::FramelessWindowHint  |
        Qt::WindowStaysOnTopHint |
        Qt::Tool
        );
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoFillBackground(false);
    setContentsMargins(0, 0, 0, 0);
    setStyleSheet("QWidget { background: transparent; border: none; }");
    setMouseTracking(true);

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // GIF label
    m_gifLabel = new QLabel(this);
    m_gifLabel->setAlignment(Qt::AlignCenter);
    m_gifLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_gifLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_gifLabel->setAttribute(Qt::WA_NoSystemBackground);
    m_gifLabel->setAutoFillBackground(false);
    m_gifLabel->setStyleSheet(
        "QLabel { background: transparent; border: none; margin: 0px; padding: 0px; }"
        );

    // Close button — top-right
    m_closeBtn = new QPushButton("x", this);
    m_closeBtn->setFixedSize(22, 22);
    m_closeBtn->setCursor(Qt::PointingHandCursor);
    m_closeBtn->setToolTip("Hide");
    m_closeBtn->setStyleSheet(
        "QPushButton {"
        "  background: rgba(40,40,40,200);"
        "  color: #ffffff;"
        "  border: none;"
        "  border-radius: 11px;"
        "  font-size: 11px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background: rgba(220,50,50,230); }"
        );
    connect(m_closeBtn, &QPushButton::clicked, this, &GifOverlay::toggleVisibility);

    // Mirror button — top-left
    m_mirrorBtn = new QPushButton("⇆", this);
    m_mirrorBtn->setFixedSize(22, 22);
    m_mirrorBtn->setCursor(Qt::PointingHandCursor);
    m_mirrorBtn->setToolTip("Mirror GIF");
    m_mirrorBtn->setStyleSheet(
        "QPushButton {"
        "  background: rgba(40,40,40,200);"
        "  color: #ffffff;"
        "  border: none;"
        "  border-radius: 11px;"
        "  font-size: 11px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background: rgba(80,80,200,230); }"
        );
    connect(m_mirrorBtn, &QPushButton::clicked, this, &GifOverlay::toggleMirror);

    // Resize handle — bottom-right
    m_resizeHandle = new QLabel(this);
    m_resizeHandle->setFixedSize(18, 18);
    m_resizeHandle->setCursor(Qt::SizeFDiagCursor);
    m_resizeHandle->setAlignment(Qt::AlignCenter);
    m_resizeHandle->setStyleSheet(
        "QLabel {"
        "  background: rgba(40,40,40,180);"
        "  border-radius: 4px;"
        "  color: white;"
        "  font-size: 10px;"
        "}"
        );
    m_resizeHandle->installEventFilter(this);

    root->addWidget(m_gifLabel);
    resize(150, 150);
}

void GifOverlay::setupTrayIcon()
{
    QIcon icon = QApplication::style()->standardIcon(QStyle::SP_MediaPlay);
    m_trayIcon = new QSystemTrayIcon(icon, this);
    m_trayMenu = new QMenu();

    m_toggleAction = new QAction("Hide GIF", this);
    m_loadAction   = new QAction("Load GIF file...", this);
    m_quitAction   = new QAction("Quit", this);

    m_trayMenu->addAction(m_toggleAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(m_loadAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(m_quitAction);

    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->setToolTip("GIF Overlay");
    m_trayIcon->show();

    connect(m_toggleAction, &QAction::triggered, this, &GifOverlay::toggleVisibility);
    connect(m_loadAction,   &QAction::triggered, this, &GifOverlay::browseForGif);
    connect(m_quitAction,   &QAction::triggered, qApp, &QApplication::quit);
    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, &GifOverlay::onTrayIconActivated);
}

// ─────────────────────────────────────────────
//  GIF Loading
// ─────────────────────────────────────────────

void GifOverlay::browseForGif()
{
    QString path = QFileDialog::getOpenFileName(
        nullptr, "Select a GIF", QDir::homePath(), "GIF Images (*.gif)"
        );
    if (!path.isEmpty()) {
        loadGif(path);
        saveSettings();
    }
}

void GifOverlay::loadGif(const QString &path)
{
    if (path.isEmpty() || !QFileInfo::exists(path)) return;

    if (m_movie) {
        m_movie->stop();
        delete m_movie;
        m_movie = nullptr;
    }

    m_gifPath = path;
    m_movie   = new QMovie(path, QByteArray(), this);

    if (!m_movie->isValid()) {
        QMessageBox::warning(nullptr, "GIF Overlay",
                             "Could not load GIF:\n" + path);
        delete m_movie;
        m_movie = nullptr;
        return;
    }

    // Get natural size from first frame
    m_movie->jumpToFrame(0);
    QSize natural = m_movie->currentPixmap().size();
    m_gifSize = (natural.isValid() && !natural.isEmpty()) ? natural : QSize(150, 150);

    // Connect frame updates
    connect(m_movie, &QMovie::frameChanged, this, [this]() {
        applyScaledFrame();
    });

    m_gifLabel->setGeometry(0, 0, width(), height());
    m_movie->start();
    repositionOverlays();

    if (!isVisible()) {
        show();
        m_toggleAction->setText("Hide GIF");
    }

    // Resize window to natural GIF size (capped at 400px wide)
    int w = qMin(m_gifSize.width(), 400);
    int h = (m_gifSize.height() * w) / qMax(m_gifSize.width(), 1);
    resize(w, h);
}

void GifOverlay::applyScaledFrame()
{
    if (!m_movie) return;
    QPixmap frame = m_movie->currentPixmap();
    if (frame.isNull()) return;

    QPixmap scaled = frame.scaled(m_gifLabel->size(),
                                  Qt::IgnoreAspectRatio,
                                  Qt::SmoothTransformation);
    if (m_mirrored) {
        scaled = scaled.transformed(QTransform().scale(-1, 1));
    }

    m_gifLabel->setPixmap(scaled);
}

// ─────────────────────────────────────────────
//  Show / Hide
// ─────────────────────────────────────────────

void GifOverlay::toggleVisibility()
{
    if (isVisible()) {
        hide();
        m_toggleAction->setText("Show GIF");
    } else {
        show();
        raise();
        activateWindow();
        m_toggleAction->setText("Hide GIF");
    }
    saveSettings();
}

void GifOverlay::toggleMirror()
{
    m_mirrored = !m_mirrored;

    if (m_mirrored) {
        m_mirrorBtn->setStyleSheet(
            "QPushButton {"
            "  background: rgba(80,80,200,230);"
            "  color: #ffffff;"
            "  border: none;"
            "  border-radius: 11px;"
            "  font-size: 11px;"
            "  font-weight: bold;"
            "}"
            "QPushButton:hover { background: rgba(60,60,180,230); }"
            );
    } else {
        m_mirrorBtn->setStyleSheet(
            "QPushButton {"
            "  background: rgba(40,40,40,200);"
            "  color: #ffffff;"
            "  border: none;"
            "  border-radius: 11px;"
            "  font-size: 11px;"
            "  font-weight: bold;"
            "}"
            "QPushButton:hover { background: rgba(80,80,200,230); }"
            );
    }

    applyScaledFrame();
    saveSettings();
}

void GifOverlay::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        toggleVisibility();
    }
    if (reason == QSystemTrayIcon::Context) {
        this->show();
        this->raise();
        this->activateWindow();
        m_trayMenu->popup(QCursor::pos());
    }
}

// ─────────────────────────────────────────────
//  Drag
// ─────────────────────────────────────────────

void GifOverlay::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging   = true;
        m_resizing   = false;
        m_dragOffset = event->globalPosition().toPoint() - frameGeometry().topLeft();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    }
}

void GifOverlay::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = event->globalPosition().toPoint() - m_dragOffset;
        QRect  screen = QApplication::primaryScreen()->availableGeometry();
        newPos.setX(qBound(screen.left(), newPos.x(), screen.right()  - width()));
        newPos.setY(qBound(screen.top(),  newPos.y(), screen.bottom() - height()));
        move(newPos);
        event->accept();
    }
}

void GifOverlay::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        m_resizing = false;
        setCursor(Qt::OpenHandCursor);
        saveSettings();
        event->accept();
    }
}

// ─────────────────────────────────────────────
//  Resize handle
// ─────────────────────────────────────────────

bool GifOverlay::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_resizeHandle) {
        QMouseEvent *me = dynamic_cast<QMouseEvent*>(event);
        if (!me) return false;

        if (event->type() == QEvent::MouseButtonPress
            && me->button() == Qt::LeftButton) {
            m_resizing     = true;
            m_dragging     = false;
            m_resizeStart  = me->globalPosition().toPoint();
            m_sizeAtResize = size();
            return true;
        }
        if (event->type() == QEvent::MouseMove && m_resizing) {
            QPoint delta = me->globalPosition().toPoint() - m_resizeStart;
            int newW = qMax(30, m_sizeAtResize.width()  + delta.x());
            int newH = qMax(30, m_sizeAtResize.height() + delta.y());
            resize(newW, newH);
            return true;
        }
        if (event->type() == QEvent::MouseButtonRelease
            && me->button() == Qt::LeftButton) {
            m_resizing = false;
            saveSettings();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

// ─────────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────────

void GifOverlay::repositionOverlays()
{
    m_closeBtn->move(width() - m_closeBtn->width() - 2, 2);
    m_closeBtn->raise();
    m_mirrorBtn->move(2, 2);
    m_mirrorBtn->raise();
    m_resizeHandle->move(width()  - m_resizeHandle->width()  - 2,
                         height() - m_resizeHandle->height() - 2);
    m_resizeHandle->raise();
}

void GifOverlay::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_gifLabel->setGeometry(0, 0, event->size().width(), event->size().height());
    applyScaledFrame();
    repositionOverlays();
}

// ─────────────────────────────────────────────
//  Settings
// ─────────────────────────────────────────────

void GifOverlay::saveSettings()
{
    m_settings.setValue("pos",      pos());
    m_settings.setValue("size",     size());
    m_settings.setValue("visible",  isVisible());
    m_settings.setValue("gifPath",  m_gifPath);
    m_settings.setValue("mirrored", m_mirrored);
}

void GifOverlay::loadSettings()
{
    QPoint savedPos  = m_settings.value("pos",  QPoint(100, 100)).toPoint();
    QSize  savedSize = m_settings.value("size", QSize(150, 150)).toSize();
    QRect  screen    = QApplication::primaryScreen()->availableGeometry();

    resize(savedSize);
    move(screen.contains(savedPos) ? savedPos : QPoint(100, 100));

    QString savedPath = m_settings.value("gifPath", "").toString();
    if (!savedPath.isEmpty()) loadGif(savedPath);

    m_mirrored = m_settings.value("mirrored", false).toBool();
    if (m_mirrored) {
        m_mirrorBtn->setStyleSheet(
            "QPushButton {"
            "  background: rgba(80,80,200,230);"
            "  color: #ffffff;"
            "  border: none;"
            "  border-radius: 11px;"
            "  font-size: 11px;"
            "  font-weight: bold;"
            "}"
            "QPushButton:hover { background: rgba(60,60,180,230); }"
            );
    }

    bool visible = m_settings.value("visible", true).toBool();
    if (visible) {
        show();
        m_toggleAction->setText("Hide GIF");
    } else {
        hide();
        m_toggleAction->setText("Show GIF");
    }

    setCursor(Qt::OpenHandCursor);
    repositionOverlays();
}