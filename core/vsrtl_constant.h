#ifndef CONSTANT_H
#define CONSTANT_H

#include "vsrtl_binutils.h"
#include "vsrtl_component.h"
#include "vsrtl_port.h"

namespace vsrtl {

namespace {
constexpr bool valueFitsInBitWidth(unsigned int width, int value) {
    const int v = value < 0 ? -value : value;
    // -1 to verify that there is space for the sign bit
    return ceillog2(v) <= width;
}
}  // namespace

/**
 * @param width Must be able to contain the signed bitfield of value
 *
 */
class Constant : public Component {
public:
    std::type_index getTypeId() const override { return std::type_index(typeid(Constant)); }
    Constant(std::string name, VSRTL_VT_U value, unsigned int width, Component* parent) : Component(name, parent) {
        if (!valueFitsInBitWidth(width, value)) {
            throw std::runtime_error("Value does not fit inside provided bit-width");
        }

        out << ([value] { return value; });
        out.setWidth(width);
    }

    OUTPUTPORT(out);

private:
    unsigned int m_width;
};

}  // namespace vsrtl

#endif  // CONSTANT_H
