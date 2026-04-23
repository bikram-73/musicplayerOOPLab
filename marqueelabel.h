#ifndef MARQUEELABEL_H
#define MARQUEELABEL_H

#include <QLabel>
#include <QTimer>

class MarqueeLabel : public QLabel {
    Q_OBJECT
public:
    explicit MarqueeLabel(QWidget* parent = nullptr);
    void setText(const QString& text);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void updateScroll();

private:
    int px;       
    QTimer timer; 
    QString fullText;
    int textWidth;
    bool needsScrolling;
    int scrollPause; 
};

#endif // MARQUEELABEL_H
