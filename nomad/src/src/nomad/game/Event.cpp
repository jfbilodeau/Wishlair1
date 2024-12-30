//
// Created by jfbil on 2023-07-06.
//

#include "nomad/game/Event.hpp"

namespace nomad {

Event::Event(const NomadString& name, NomadId script_id) :
    m_name(name),
    m_script_id(script_id) {

}

NomadString Event::get_name() const {
    return m_name;
}

NomadId Event::get_script_id() const {
    return m_script_id;
}

void EventManager::add_event(const NomadString& name, NomadId script_id) {
    // If the event handler already exists, replace it
    for (auto i = 0; i < m_events.size(); i++) {
        if (m_events[i].get_name() == name) {
            m_events[i] = Event(name, script_id);
            return;
        }
    }

    // Event does not exist, add it
    m_events.emplace_back(name, script_id);
}

void EventManager::remove_event(const NomadString& name) {
    for (auto i = 0; i < m_events.size(); i++) {
        if (m_events[i].get_name() == name) {
            m_events.erase(m_events.begin() + i);

            return;
        }
    }
}

NomadId EventManager::get_script_id_for_event(const NomadString& name) const {
    for (auto& event : m_events) {
        if (event.get_name() == name) {
            return event.get_script_id();
        }
    }

    return NOMAD_INVALID_ID;
}

} // nomad