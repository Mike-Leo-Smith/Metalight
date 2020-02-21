//
// Created by Mike Smith on 2020/2/10.
//

#include "shape_replica.h"

namespace luisa {

LUISA_REGISTER_NODE_CREATOR("Replica", ShapeReplica)

void ShapeReplica::load(GeometryEncoder &encoder) {
    LUISA_ERROR_IF(_reference->is_instance(), "cannot make shape replica from instance");
    LUISA_ERROR_IF_NOT(_reference->transform().is_static(), "cannot make shape replica from shapes with non-static transforms");
    if (!_reference->loaded()) {
        _reference->load(encoder);
    }
    _entity_index = encoder.replicate(_reference->entity_index(), _transform->static_matrix());
}

ShapeReplica::ShapeReplica(Device *device, const ParameterSet &parameter_set)
    : Shape{device, parameter_set} { _reference = parameter_set["reference"].parse<Shape>(); }
    
}
