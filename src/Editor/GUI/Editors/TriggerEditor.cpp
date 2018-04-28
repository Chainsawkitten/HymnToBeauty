#include "TriggerEditor.hpp"

#include <array>
#include <algorithm>
#include <Engine/Component/Trigger.hpp>
#include <Engine/Component/Script.hpp>
#include <Engine/Component/Shape.hpp>
#include <Engine/Component/RigidBody.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/TriggerManager.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Trigger/TriggerRepeat.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Script/ScriptFile.hpp>
#include <imgui.h>

// This is necessary to use std::string in ImGui::Combo(). Taken from https://github.com/ocornut/imgui/issues/1180
namespace ImGui {
    static auto vector_getter = [](void* vec, int idx, const char** out_text) {
        auto& vector = *static_cast<std::vector<std::string>*>(vec);
        if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
        *out_text = vector.at(idx).c_str();
        return true;
    };

    bool Combo(const char* label, int* currIndex, std::vector<std::string>& values) {
        if (values.empty()) { return false; }
        return Combo(label, currIndex, vector_getter,
            static_cast<void*>(&values), static_cast<int>(values.size()));
    }
}

namespace GUI {
    void TriggerEditor::Open() {
        ImGui::SetNextWindowPosCenter();
        ImGui::OpenPopup("trigger-edit");
    }

    void TriggerEditor::Show(Component::Trigger& comp) {
        if (ImGui::BeginPopupModal("trigger-edit", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar)) {
            if (ImGui::RadioButton("Properties", selectedTab == PROPERTIES))
                selectedTab = PROPERTIES;

            ImGui::SameLine();

            if (ImGui::RadioButton("Subjects", selectedTab == SUBJECTS))
                selectedTab = SUBJECTS;

            ImGui::SameLine();

            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();

            ImGui::Separator();

            // Working under the assumption that the internal trigger
            // is set and is indeed a repeat trigger.
            auto repeat = Managers().triggerManager->GetTriggerRepeat(comp);
            assert(repeat);
            repeat->SetOwningEntity(comp.entity);

            switch (selectedTab) {
                case PROPERTIES: {
                    std::array<char, 100> name;
                    float delay = repeat->GetDelay();
                    float cooldown = repeat->GetCooldown();
                    int charges = repeat->GetTriggerCharges();
                    int startActive = repeat->GetStartActive();


                    memcpy(name.data(), repeat->GetName().c_str(), std::min(repeat->GetName().size(), name.size()));
                    name[std::min(repeat->GetName().size(), name.size() - 1)] = '\0';

                    // NAME
                    if (ImGui::InputText("Name", name.data(), name.size()))
                        repeat->SetName(name.data());

                    // DELAY
                    if (ImGui::InputFloat("Delay", &delay, 0.1f, 1.0f, 1)) {
                        if (delay < 0.0f)
                            delay = 0.0f;

                        repeat->SetDelay(delay);
                    }
                    ImGui::SameLine();
                    ImGui::Text("[?]");
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Time in seconds to delay.\nHold button to increase speed.");

                    // COOLDOWN
                    if (ImGui::InputFloat("Cooldown", &cooldown, 0.1f, 1.0f, 1)) {
                        if (cooldown < 0.0f)
                            cooldown = 0.0f;

                        repeat->SetCooldown(cooldown);
                    }

                    ImGui::SameLine();
                    ImGui::Text("[?]");
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Time in seconds to cooldown.\nHold button to increase speed.");

                    // CHARGES
                    if (ImGui::InputInt("Charges", &charges)) {
                        if (charges < 0)
                            charges = 0;

                        repeat->SetTriggerCharges(charges);
                    }
                    ImGui::SameLine();

                    ImGui::Text("[?]");
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("How many times a trigger can execute before automatically going inactive.\n0 = INFINITE.\nHold button to increase speed.");

                    // START ACTIVE
                    ImGui::Text("Start active? ");

                    ImGui::SameLine();

                    if (ImGui::RadioButton("Yes", &startActive, 1))
                        repeat->SetStartActive(true);

                    ImGui::SameLine();

                    if (ImGui::RadioButton("No", &startActive, 0))
                        repeat->SetStartActive(false);

                    break;
                }

                case SUBJECTS: {
                    // Hardcoded single event for demonstration purposes
                    if (ImGui::Button("New event", ImVec2(100, 25))) {
                        triggerEvent::EventStruct newStruct;
                        repeat->GetEventVector()->push_back(newStruct);
                    }

                    ImGui::NewLine();
                    ImGui::Columns(4);

                    ImGui::Text("Event");
                    ImGui::NextColumn();
                    ImGui::Text("Shape");
                    ImGui::SameLine();
                    ImGui::Text("[?]");
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("When this shape collide with trigger volume the trigger will be executed.");

                    ImGui::NextColumn();
                    ImGui::Text("Target entity");
                    ImGui::SameLine();
                    ImGui::Text("[?]");
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Entity to gather script from.");

                    ImGui::NextColumn();
                    ImGui::Text("Script method");
                    ImGui::Separator();

                    for (std::size_t i = 0; i < repeat->GetEventVector()->size(); ++i) {
                        ImGui::Columns(4);

                        std::array<const char*, 3> events = {
                            "OnEnter",
                            "OnRetain",
                            "OnLeave",
                        };

                        std::string labelEvent = "Type ";
                        labelEvent.append(std::to_string(i));
                        std::string labelShape = "Shape ";
                        labelShape.append(std::to_string(i));
                        std::string labelTarget = "Entity ";
                        labelTarget.append(std::to_string(i));
                        std::string labelScript = "Script ";
                        labelScript.append(std::to_string(i));

                        // Event type
                        int eventType = repeat->GetEventVector()->at(i).m_eventID;

                        if (ImGui::Combo(labelEvent.c_str(), &eventType, events.data(), static_cast<int>(events.size()))) {
                            repeat->GetEventVector()->at(i).m_eventID = eventType;
                            repeat->GetEventVector()->at(i).check[0] = true;
                        }

                        // Subject
                        ImGui::NextColumn();
                        int collidedEntityUID = repeat->GetCollidedEntityUID();
                        if (ImGui::InputInt("Input UID: ", &collidedEntityUID)) {
                            repeat->SetCollidedEntityUID(collidedEntityUID);
                            repeat->GetEventVector()->at(i).check[1] = true;
                        }

                        // Entity target
                        ImGui::NextColumn();
                        int targetID = repeat->GetEventVector()->at(i).m_targetID;
                        std::vector<std::string> entityName;

                        for (std::size_t i = 0; i < Hymn().world.GetEntities().size(); ++i) {
                            if (Hymn().world.GetEntities().at(i)->GetComponent<Component::Script>() != nullptr && Hymn().world.GetEntities().at(i)->name != comp.entity->name)
                                entityName.push_back(Hymn().world.GetEntities().at(i)->name);
                        }

                        if (ImGui::Combo(labelTarget.c_str(), &targetID, entityName)) {
                            repeat->GetEventVector()->at(i).m_targetID = targetID;

                            for (std::size_t j = 0; j < Hymn().world.GetEntities().size(); ++j) {
                                if (Hymn().world.GetEntities().at(j)->name == entityName.at(targetID)) {
                                    repeat->GetTargetEntity()->push_back(Hymn().world.GetEntities().at(j));
                                    repeat->GetEventVector()->at(i).check[2] = true;
                                }
                            }
                        }

                        // Script name
                        ImGui::NextColumn();
                        int scriptID = repeat->GetEventVector()->at(i).m_scriptID;

                        std::vector<std::string> scriptVector;

                        if (!repeat->GetTargetEntity()->empty()) {
                            for (std::size_t x = 0; x < repeat->GetTargetEntity()->size(); ++x) {
                                if (repeat->GetTargetEntity()->at(x)->GetComponent<Component::Script>() != nullptr && repeat->GetTargetEntity()->at(x)->name == entityName.at(targetID)) {
                                    for (std::size_t j = 0; j < repeat->GetTargetEntity()->at(x)->GetComponent<Component::Script>()->scriptFile->functionList.size(); ++j) {
                                        if (std::find(scriptVector.begin(), scriptVector.end(), repeat->GetTargetEntity()->at(x)->GetComponent<Component::Script>()->scriptFile->functionList.at(j)) == scriptVector.end())
                                            scriptVector.push_back(repeat->GetTargetEntity()->at(x)->GetComponent<Component::Script>()->scriptFile->functionList.at(j));
                                    }
                                }
                            }
                        }

                        if (!scriptVector.empty()) {
                            if (ImGui::Combo(labelScript.c_str(), &scriptID, scriptVector)) {
                                repeat->GetEventVector()->at(i).m_scriptID = scriptID;
                                repeat->GetTargetFunction()->push_back(scriptVector.at(scriptID));
                                repeat->GetEventVector()->at(i).check[3] = true;
                            }
                        }

                        ImGui::Separator();
                    }
                    break;
                }
            }

            ImGui::EndPopup();
        }
    }
}
