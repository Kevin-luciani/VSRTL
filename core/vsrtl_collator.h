#ifndef VSRTL_COLLATOR_H
#define VSRTL_COLLATOR_H

#include "vsrtl_component.h"

namespace vsrtl {

/**
 * @brief The Collator class
 * Collates binary signals into a compound signal
 * ie:
 *           _____
 * a[0:0] -> |   |
 * b[0:0] -> |   | -> [3:0] a&b&c&d
 * c[0:0] -> |   |
 * d[0:0] -> |___|
 */
class Collator : public Component {
public:
    Collator(std::string name, unsigned int width, Component* parent) : Component(name, parent), m_width(width) {
        in = this->createInputPorts("in", width, 1);
        out.setWidth(width);

        out << [=] {
            VSRTL_VT_U value = 0;
            for (int i = 0; i < width; i++) {
                value |= static_cast<bool>(*in[i]) << i;
            }
            return value;
        };
    }
    OUTPUTPORT(out);
    INPUTPORTS(in);

protected:
    unsigned int m_width;
};

}  // namespace vsrtl

#endif  // VSRTL_COLLATOR_H
