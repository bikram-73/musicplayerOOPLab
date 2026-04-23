#include "marqueelabel.h"
#include <QPainter>
#include <QFontMetrics>

MarqueeLabel::MarqueeLabel(QWidget* parent) : QLabel(parent), px(0), textWidth(0), needsScrolling(false), scrollPause(0) {
    connect(&timer, &QTimer::timeout, this, &MarqueeLabel::updateScroll);
    setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

void MarqueeLabel::setText(const QString& text) {
    fullText = text;
    px = 0;
    scrollPause = 30; // Pause ticks
    QFontMetrics fm(font());
    textWidth = fm.horizontalAdvance(fullText);
    needsScrolling = textWidth > width() && width() > 0;
    
    if (needsScrolling) {
        timer.start(30); // roughly 30 fps
    } else {
        timer.stop();
        QLabel::setText(text); // Fallback to normal rendering
    }
    update();
}

void MarqueeLabel::resizeEvent(QResizeEvent* event) {
    QLabel::resizeEvent(event);
    needsScrolling = textWidth > width();
    if (needsScrolling && !timer.isActive()) {
        timer.start(30);
    } else if (!needsScrolling && timer.isActive()) {
        timer.stop();
        px = 0;
        QLabel::setText(fullText);
    }
}

void MarqueeLabel::paintEvent(QPaintEvent* event) {
    if (!needsScrolling) {
        QLabel::paintEvent(event);
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(palette().color(QPalette::WindowText));
    
    int y = (height() + fontMetrics().ascent() - fontMetrics().descent()) / 2;
    painter.drawText(-px, y, fullText);
}

void MarqueeLabel::updateScroll() {
    if (scrollPause > 0) {
        scrollPause--;
        return;
    }
    
    px += 1;
    if (px > textWidth) {
        px = -width(); // Restart from right side pushing left
    }
    update();
}
