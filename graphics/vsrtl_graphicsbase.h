#ifndef VSRTL_GRAPHICSBASE_H
#define VSRTL_GRAPHICSBASE_H

#include <QGraphicsItem>

namespace vsrtl {

class GraphicsBase : public QObject, public QGraphicsItem {
public:
    GraphicsBase();

    /**
     * @brief postSceneConstructionInitialize#
     * Some graphic components may need extra initialization steps after all graphics have been added to the scene (such
     * as wires). When overriding, overriding function must call GraphicsBase::postSceneConstructionInitialize#()
     *
     * Multiple passes may be used, allowing for staged initialization
     */
    virtual void postSceneConstructionInitialize1();
    virtual void postSceneConstructionInitialize2();

protected:
    bool m_initialized = false;
};
}  // namespace vsrtl

#endif  // VSRTL_GRAPHICSBASE_H
