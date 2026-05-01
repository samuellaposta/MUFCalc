#pragma once
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

// ─────────────────────────────────────────────────────────────────
//  IAEAEmblem  –  pure QPainter, zero external Qt-module dependency
// ─────────────────────────────────────────────────────────────────
namespace IAEAEmblem {

inline void draw(QPainter& p, const QRectF& bounds, qreal opacity)
{
    p.save();
    p.setOpacity(opacity);
    p.setRenderHint(QPainter::Antialiasing);

    const QColor col("#1a3a5c");
    const qreal cx = bounds.center().x();
    const qreal cy = bounds.top() + bounds.height() * 0.44;
    const qreal R  = qMin(bounds.width(), bounds.height()) * 0.40;

    // Outer double ring
    p.setPen(QPen(col, R * 0.040)); p.setBrush(Qt::NoBrush);
    p.drawEllipse(QPointF(cx, cy), R, R);
    p.setPen(QPen(col, R * 0.014));
    p.drawEllipse(QPointF(cx, cy), R * 0.92, R * 0.92);

    // Three electron orbits (60° apart)
    p.setPen(QPen(col, R * 0.034));
    for (int deg = 0; deg < 180; deg += 60) {
        p.save();
        p.translate(cx, cy);
        p.rotate(deg);
        p.drawEllipse(QPointF(0, 0), R * 0.76, R * 0.26);
        p.restore();
    }

    // Nucleus
    p.setPen(Qt::NoPen);
    p.setBrush(col);   p.drawEllipse(QPointF(cx,cy), R*0.14, R*0.14);
    p.setBrush(Qt::white); p.drawEllipse(QPointF(cx,cy), R*0.085, R*0.085);
    p.setBrush(col);   p.drawEllipse(QPointF(cx,cy), R*0.048, R*0.048);

    // Olive branches (5 leaves each side)
    for (int side = -1; side <= 1; side += 2) {
        QPainterPath stem;
        stem.moveTo(cx + side*R*0.17, cy + R*0.78);
        stem.cubicTo(cx + side*R*0.44, cy + R*0.66,
                     cx + side*R*0.62, cy + R*0.48,
                     cx + side*R*0.64, cy + R*0.12);
        p.setPen(QPen(col, R*0.022, Qt::SolidLine, Qt::RoundCap));
        p.setBrush(Qt::NoBrush);
        p.drawPath(stem);
        p.setPen(Qt::NoPen); p.setBrush(col);
        for (int i = 0; i < 5; ++i) {
            qreal t = (qreal)(i+1) / 6.0, mt = 1.0-t;
            qreal x0=cx+side*R*0.17, y0=cy+R*0.78;
            qreal x1=cx+side*R*0.44, y1=cy+R*0.66;
            qreal x2=cx+side*R*0.62, y2=cy+R*0.48;
            qreal x3=cx+side*R*0.64, y3=cy+R*0.12;
            qreal lx = mt*mt*mt*x0+3*mt*mt*t*x1+3*mt*t*t*x2+t*t*t*x3;
            qreal ly = mt*mt*mt*y0+3*mt*mt*t*y1+3*mt*t*t*y2+t*t*t*y3;
            qreal tx_ = 3*(mt*mt*(x1-x0)+2*mt*t*(x2-x1)+t*t*(x3-x2));
            qreal ty_ = 3*(mt*mt*(y1-y0)+2*mt*t*(y2-y1)+t*t*(y3-y2));
            qreal ang = qAtan2(ty_, tx_) * 180.0 / M_PI;
            qreal lw = R*(0.130 - i*0.012), lh = R*(0.046 - i*0.004);
            p.save();
            p.translate(lx, ly); p.rotate(ang + side*42.0);
            p.drawEllipse(QPointF(0,0), lw, lh);
            p.restore();
        }
    }

    // "IAEA" wordmark
    p.setPen(col);
    QFont f; f.setBold(true);
    f.setPixelSize(qMax(8.0, R * 0.30));
    f.setLetterSpacing(QFont::AbsoluteSpacing, R * 0.07);
    p.setFont(f);
    p.drawText(QRectF(bounds.left(), cy + R*1.04, bounds.width(), R*0.44),
               Qt::AlignHCenter | Qt::AlignTop, "IAEA");

    // Sub-caption
    QFont sf; sf.setPixelSize(qMax(5.0, R * 0.092));
    sf.setLetterSpacing(QFont::AbsoluteSpacing, R * 0.022);
    p.setFont(sf);
    p.drawText(QRectF(bounds.left(), cy + R*1.44, bounds.width(), R*0.24),
               Qt::AlignHCenter | Qt::AlignTop, "INTERNATIONAL ATOMIC ENERGY AGENCY");

    p.restore();
}

} // namespace IAEAEmblem

// ─────────────────────────────────────────────────────────────────
//  WatermarkWidget – use as a container widget; paints IAEA backdrop
// ─────────────────────────────────────────────────────────────────
class WatermarkWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WatermarkWidget(qreal opacity = 0.15,
                             QColor bg = Qt::white,
                             QWidget* parent = nullptr)
        : QWidget(parent), m_opacity(opacity), m_bg(bg) {}

protected:
    void paintEvent(QPaintEvent* e) override {
        Q_UNUSED(e)
        QPainter p(this);
        p.fillRect(rect(), m_bg);
        int side = qMin(width(), height()) * 4 / 5;
        if (side < 60) side = 60;
        QRectF r((width()-side)/2.0, (height()-side)/2.0, side, side);
        IAEAEmblem::draw(p, r, m_opacity);
    }
private:
    qreal  m_opacity;
    QColor m_bg;
};

// ─────────────────────────────────────────────────────────────────
//  IAEAWatermark::paint  –  call inside any QWidget/QDialog paintEvent
// ─────────────────────────────────────────────────────────────────
namespace IAEAWatermark {
inline void paint(QPainter& p, const QRect& r, qreal opacity = 0.15) {
    int side = qMin(r.width(), r.height()) * 4 / 5;
    if (side < 60) side = 60;
    QRectF b(r.x()+(r.width()-side)/2.0, r.y()+(r.height()-side)/2.0, side, side);
    IAEAEmblem::draw(p, b, opacity);
}
}
