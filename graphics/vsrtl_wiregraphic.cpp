#include "vsrtl_wiregraphic.h"
#include "vsrtl_componentgraphic.h"
#include "vsrtl_port.h"
#include "vsrtl_portgraphic.h"

#include "vsrtl_graphics_defines.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QPolygon>

namespace vsrtl {

WireGraphic::WireGraphic(PortGraphic* from, const std::vector<Port*>& to, QGraphicsItem* parent)
    : m_fromPort(from), m_toPorts(to) {
    setParentItem(parent);
}

QRectF WireGraphic::boundingRect() const {
    QPolygonF p;
    p.append(mapFromItem(m_fromPort, m_fromPort->getInputPoint()));
    for (const auto& to : m_toGraphicPorts) {
        p.append(mapFromItem(to, to->getInputPoint()));
    }
    QRectF br = p.boundingRect();
    br.adjust(-WIRE_WIDTH, -WIRE_WIDTH, WIRE_WIDTH, WIRE_WIDTH);

    // HACK HACK HACK
    // To ensure that input ports are redrawn when this wire changes (ie. gets selected), we overlap the bounding rect
    // of this item onto both of its ports, ensuring redraws
    br.adjust(-parentItem()->boundingRect().width(), 0, parentItem()->boundingRect().width(), 0);
    // HACK HACK HACK

    return br;
}

/**
 * @brief WireGraphic::postSceneConstructionInitialize1
 * With all ports and components created during circuit construction, wires may now register themselves with their
 * attached input- and output ports
 */
void WireGraphic::postSceneConstructionInitialize1() {
    for (const auto& item : scene()->items()) {
        PortGraphic* portItem = dynamic_cast<PortGraphic*>(item);
        if (portItem) {
            if (std::find(m_toPorts.begin(), m_toPorts.end(), portItem->getPort()) != m_toPorts.end()) {
                m_toGraphicPorts.push_back(portItem);
            }
        }
        if (m_toGraphicPorts.size() == m_toPorts.size()) {
            break;
        }
    }

    // Assert that all ports were found in the scene
    Q_ASSERT(m_toGraphicPorts.size() == m_toPorts.size());

    // Make the wire destination ports aware of this wire
    for (const auto& port : m_toGraphicPorts) {
        port->setInputWire(this);
    }

    GraphicsBase::postSceneConstructionInitialize1();
}

const QPen& WireGraphic::getPen() {
    // propagate the source port pen to callers
    return m_fromPort->getPen();
}

void WireGraphic::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->save();
    painter->setPen(getPen());
    for (const auto& toPort : m_toGraphicPorts) {
        const auto* portParent = dynamic_cast<ComponentGraphic*>(toPort->parentItem());
        if (portParent->isVisible()) {
            painter->drawLine(mapFromItem(m_fromPort, m_fromPort->getOutputPoint()),
                              mapFromItem(toPort, toPort->getInputPoint()));
        }
    }

    painter->restore();
}
}  // namespace vsrtl
