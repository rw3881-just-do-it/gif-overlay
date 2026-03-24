#pragma once

#include <QWidget>
#include <QMovie>
#include <QLabel>
#include <QPoint>
#include <QSize>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <QPushButton>
#include <QMouseEvent>
#include <QResizeEvent>

class GifOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit GifOverlay(QWidget *parent = nullptr);
    ~GifOverlay();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void toggleVisibility();
    void toggleMirror();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void browseForGif();

private:
    void setupUI();
    void setupTrayIcon();
    void loadGif(const QString &path);
    void saveSettings();
    void loadSettings();
    void repositionOverlays();
    void applyScaledFrame();

    // UI
    QLabel          *m_gifLabel;
    QPushButton     *m_closeBtn;
    QPushButton     *m_mirrorBtn;
    QLabel          *m_resizeHandle;
    QMovie          *m_movie;

    // Tray
    QSystemTrayIcon *m_trayIcon;
    QMenu           *m_trayMenu;
    QAction         *m_toggleAction;
    QAction         *m_loadAction;
    QAction         *m_quitAction;

    // Drag state
    bool             m_dragging;
    QPoint           m_dragOffset;

    // Resize state
    bool             m_resizing;
    QPoint           m_resizeStart;
    QSize            m_sizeAtResize;
    QSize            m_gifSize;

    // Mirror state
    bool             m_mirrored;

    // Persistence
    QSettings        m_settings;
    QString          m_gifPath;
};