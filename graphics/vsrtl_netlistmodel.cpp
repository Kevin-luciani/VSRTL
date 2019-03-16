/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QItemSelectionModel>
#include <QWidget>

#include <QDebug>
#include <QIcon>

#include "vsrtl_netlistmodel.h"

#include "vsrtl_design.h"

namespace vsrtl {

QVariant NetlistTreeItem::data(int column, int role) const {
    if (column == NetlistModel::IOColumn && role == Qt::DecorationRole) {
        return m_direction == PortDirection::Input ? QIcon(":/icons/input.svg") : QIcon(":/icons/output.svg");
    } else if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (column) {
            case NetlistModel::ComponentColumn: {
                return getName();
            }
            case NetlistModel::ValueColumn: {
                if (m_port) {
                    return m_port->value<VSRTL_VT_U>();
                }
                break;
            }
        }
    }
    return QVariant();
}
bool NetlistTreeItem::setData(int column, const QVariant& value, int role) {
    return false;
}

NetlistModel::NetlistModel(const Design& arch, QObject* parent)
    : NetlistModelBase({"Component", "I/O", "Value"}, arch, parent) {
    rootItem = new NetlistTreeItem(nullptr);

    loadDesign(rootItem, m_arch);
}

QVariant NetlistModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    NetlistTreeItem* item = getItem<NetlistTreeItem*>(index);
    return item->data(index.column(), role);
}

Qt::ItemFlags NetlistModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    // Output ports of register components are editable.
    // Check if parent component is a Register, and if the current port is an output port. If so, the port is editable
    if (indexIsRegisterOutputPortValue(index)) {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

bool NetlistModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (indexIsRegisterOutputPortValue(index)) {
        Register* reg = dynamic_cast<Register*>(getParentComponent(index));
        if (reg) {
            reg->forceValue(value.toInt());
            m_arch.propagateDesign();
            return true;
        }
    }
    return false;
}

QModelIndex NetlistModel::lookupIndexForComponent(Component* c) const {
    if (m_componentIndicies.find(c) != m_componentIndicies.end()) {
        NetlistTreeItem* item = m_componentIndicies.at(c);
        if (item->index.isValid()) {
            return item->index;
        }
    }
    return QModelIndex();
}

void NetlistModel::addPortToComponent(Port* port, NetlistTreeItem* parent, PortDirection dir) {
    auto* child = new NetlistTreeItem(parent);
    parent->insertChild(parent->childCount(), child);

    child->setName(QString::fromStdString(port->getName()));
    child->m_direction = dir;
    child->m_port = port;
}

bool NetlistModel::indexIsRegisterOutputPortValue(const QModelIndex& index) const {
    if (index.column() == ValueColumn) {
        auto* item = getItem<NetlistTreeItem*>(index);
        auto* parentItem = static_cast<NetlistTreeItem*>(item->parent());
        auto* parentComponent = parentItem->m_component;
        if (parentItem && parentComponent) {
            if (dynamic_cast<Register*>(parentComponent)) {
                // Parent is register, check if current index is an output port
                if (item->m_port && item->m_direction == PortDirection::Output) {
                    return true;
                }
            }
        }
    }
    return false;
}

void NetlistModel::loadDesignRecursive(NetlistTreeItem* parent, const Component& component) {
    auto& subComponents = component.getSubComponents();

    // Subcomponents
    for (const auto& subcomponent : subComponents) {
        auto* child = new NetlistTreeItem(parent);
        parent->insertChild(parent->childCount(), child);

        // Set component data (component pointer and name)
        m_componentIndicies[subcomponent.get()] = child;

        child->m_component = subcomponent.get();
        child->setName(QString::fromStdString(subcomponent->getName()));

        // Recurse into the child
        loadDesignRecursive(child, *subcomponent);
    }

    // I/O ports of component
    for (const auto& input : component.getInputs()) {
        addPortToComponent(input.get(), parent, PortDirection::Input);
    }
    for (const auto& output : component.getOutputs()) {
        addPortToComponent(output.get(), parent, PortDirection::Output);
    }
}

Component* NetlistModel::getParentComponent(const QModelIndex& index) const {
    auto* item = getItem<NetlistTreeItem*>(index);
    if (item) {
        item = static_cast<NetlistTreeItem*>(item->parent());
        if (item) {
            return item->m_component;
        }
    }
    return nullptr;
}

void NetlistModel::loadDesign(NetlistTreeItem* parent, const Design& design) {
    loadDesignRecursive(parent, design);
}
}  // namespace vsrtl
