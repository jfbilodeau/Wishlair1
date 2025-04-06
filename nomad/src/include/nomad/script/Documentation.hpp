//
// Created by jfbil on 2023-06-26.
//

#pragma once

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

void generateDocumentation(Runtime* runtime, std::ostream& out);
void generateKeywords(Runtime* runtime, std::ostream& out);

#else

#define NomadDocField NomadString doc;
#define NomadDocParamField NomadString doc;
#define NomadDocArg const NomadString& doc
#define NomadDoc(documentation) ""
#define NomadParamDoc(param_doc) ""

#endif // NOMAD_SCRIPT_DOC

} // namespace nomad

