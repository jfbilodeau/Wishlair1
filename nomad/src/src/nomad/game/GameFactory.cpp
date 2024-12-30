//
// Created by jfbil on 2023-06-26.
//

#include "nomad/game/GameFactory.hpp"

#include "nomad/log/ConsoleSink.hpp"
#include "nomad/log/Logger.hpp"

#include "nomad/game/Game.hpp"

#include "nomad/script/Documentation.hpp"

#include <boost/program_options.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>

namespace nomad {

enum class ParseCommandLineResult {
    Unknown = 1, // Unknown result
    Run,         // Run game
#if defined(NOMAD_SCRIPT_DOC)
    ScriptDoc,   // Write documentation and exit
    Keywords,    // Write keywords and exit
#endif
};

struct Configuration {
    ParseCommandLineResult result = ParseCommandLineResult::Unknown;  // Result of parsing command line
    NomadString resource_path;
    bool debug;
};

void parse_command_line(const std::shared_ptr<Logger>& logger, int argc, char** argv, Configuration* configuration) {
    configuration->result = ParseCommandLineResult::Run;

    boost::program_options::options_description desc("Allowed options");

    desc.add_options()
        ("help", "Display help and exit")
#if defined(NOMAD_SCRIPT_DOC)
        ("doc", "Write documentation to 'nomad-doc.json' and exit")
        ("keywords", "Write keywords to 'nomad-keywords.txt' and exit")
#endif
        ("debug", "Enable debug mode")
        ("resource-path", boost::program_options::value<NomadString>(&configuration->resource_path), "Path to resource directory");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        std::exit(EXIT_SUCCESS);
    }

#if defined(NOMAD_SCRIPT_DOC)
    if (vm.count("doc")) {
        configuration->result = ParseCommandLineResult::ScriptDoc;
    }
    if (vm.count("keywords")) {
        configuration->result = ParseCommandLineResult::Keywords;
    }
#endif
    if (vm.count("debug")) {
        logger->info("Debug mode enabled");
        logger->set_log_level(LogLevel::Debug);
        configuration->debug = true;
    } else {
        logger->set_log_level(LogLevel::Info);
        configuration->debug = false;
    }

    if (configuration->resource_path.empty()) {
        configuration->resource_path = NomadString(SDL_GetBasePath());
    }
}

int run(int argc, char** argv) {
    ConsoleSink console_sink;
    auto logger = std::make_shared<Logger>(&console_sink);

    try {
        Configuration configuration;

        parse_command_line(logger, argc, argv, &configuration);

        Game game(configuration.resource_path, configuration.debug);

        switch (configuration.result) {
            case ParseCommandLineResult::Run:
                game.run();
                break;
#if defined(NOMAD_SCRIPT_DOC)
            case ParseCommandLineResult::ScriptDoc: {
                auto doc_out = std::ofstream("nomad-doc.json");
                generate_documentation(game.get_runtime(), doc_out);
                break;
            }
            case ParseCommandLineResult::Keywords: {
                auto keywords_out = std::ofstream("nomad-keywords.txt");
                generate_keywords(game.get_runtime(), keywords_out);
                break;
            }
#endif
            default:
                throw NomadException("Internal error: unknown command line result");
        }
    } catch (std::exception& e) {
        logger->error(e.what());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", e.what(), nullptr);

        return EXIT_FAILURE;
    } catch (...) {
        logger->error("Unknown error");
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "An unknown error occurred", nullptr);

        return EXIT_FAILURE;
    }

    logger->debug("Removing console sink");
    logger->remove_sink(&console_sink);

    logger->flush();

    return EXIT_SUCCESS;
}

} // nomad