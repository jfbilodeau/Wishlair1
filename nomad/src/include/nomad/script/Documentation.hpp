//
// Created by jfbil on 2023-06-26.
//

#ifndef NOMAD_DOCUMENTATION_HPP
#define NOMAD_DOCUMENTATION_HPP

#include <ostream>

namespace nomad {

// Forward declarations
class Runtime;

// Helpers to document script
#if defined(NOMAD_SCRIPT_DOC)

#define NomadDocField \
    NomadString doc;

#define NomadDocParamField \
    NomadString doc;

#define NomadDocArg \
    const NomadString& doc

#define NomadDoc(doc_text) \
    NomadString(doc_text)

#define NomadParamDoc(param_doc) \
    param_doc

void generate_documentation(Runtime* runtime, std::ostream& out);
void generate_keywords(Runtime* runtime, std::ostream& out);

#else

#define NomadDocField NomadString doc;
#define NomadDocParamField NomadString doc;
#define NomadDocArg const NomadString& doc
#define NomadDoc(documentation) ""
#define NomadParamDoc(param_doc) ""

#endif // NOMAD_SCRIPT_DOC

} // namespace nomad

#endif //NOMAD_DOCUMENTATION_HPP
