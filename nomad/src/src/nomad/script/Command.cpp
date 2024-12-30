//
// Created by jfbil on 2023-07-16.
//

#include "nomad/script/Command.hpp"

namespace nomad {

CommandParameterDefinition def_parameter(const NomadString& name, const Type* type, NomadDocArg) {
    return {
        name,
        type,
#if defined(NOMAD_SCRIPT_DOC)
        doc
#endif
    };
}

//CommandParameter def_parameter(CommandParameterType type, NomadDocArg) {
//    return {
//        type,
//        0
//#if defined(NOMAD_SCRIPT_DOC)
//        , doc
//#endif
//    };
//}
//
//CommandParameter def_callback(int parameter_count, NomadDocArg) {
//    return {
//        CommandParameterType::Callback,
//        parameter_count
//#if defined(NOMAD_SCRIPT_DOC)
//        , doc
//#endif
//    };
//}

} // namespace nomad