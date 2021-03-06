#ifndef VSRTL_LABEL_H
#define VSRTL_LABEL_H

#include <QGraphicsItem>

namespace vsrtl {

class Label : public QGraphicsItem {
public:
    Label(QString text, QGraphicsItem* parent);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget*) override;

private:
    QString m_text;
    QRectF m_textRect;
    const static QFont s_font;
};

}  // namespace vsrtl

#endif  // VSRTL_LABEL_H
