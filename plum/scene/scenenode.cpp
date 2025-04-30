#include <plum/scene/scenenode.hpp>

#include <plum/component/all.hpp>
#include <plum/interface/widget.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui_stdlib.h>

#include <iostream>
#include <string>

namespace Scene {
    
    SceneNode::SceneNode() {}
    SceneNode::SceneNode(const std::string& name)
        : name(name)
    {}
    SceneNode::SceneNode(std::shared_ptr<Component::ComponentBase> component) 
        : name(component->name),
        component(std::move(component))
        
    {}
    SceneNode::SceneNode(std::shared_ptr<Component::ComponentBase> component, const std::string& name) 
        : component(std::move(component)),
        name(name)
    {}
    SceneNode::~SceneNode() {
        // std::clog << "destroying SceneNode " << name << std::endl;
    }

    std::shared_ptr<SceneNode> SceneNode::AddChild(std::shared_ptr<SceneNode> node) {
        children.emplace_back(std::move(node));
        return children.back();
    }
    
    void SceneNode::RemoveChild(std::shared_ptr<SceneNode> node) {
        auto it = std::find(children.begin(), children.end(), node);
        children.erase(it);
    }

    void SceneNode::Draw(const glm::mat4& parent_transform) {
        const glm::mat4 model_matrix = parent_transform * transform.Matrix();
        if (component)
            component->Draw(model_matrix);
        for (auto& child : children) {
            child->Draw(model_matrix);
        }
    }
    void SceneNode::Draw(Material::MaterialBase& material, const glm::mat4& parent_transform) {
        const glm::mat4 model_matrix = parent_transform * transform.Matrix();
        if (component)
            component->Draw(material, model_matrix);
        for (auto& child : children) {
            child->Draw(material, model_matrix);
        }
    }
    void SceneNode::Draw(Renderer::Module& module, const glm::mat4& parent_transform) {
        const glm::mat4 model_matrix = parent_transform * transform.Matrix();
        if (component)
            component->Draw(module, model_matrix);
        for (auto& child : children) {
            child->Draw(module, model_matrix);
        }
    }

    bool SceneNode::DisplayWidget(std::set<std::shared_ptr<Material::MaterialBase>> materials) {
        static std::string newName;
        static bool activatedThisFrame;
        
        std::string label;
        bool expanded;
        
        if (editingName) {
            label = "##treenode" + std::to_string(widgetId);
            ImGui::SetNextItemAllowOverlap();
        } else {
            label = name + "##treenode" + std::to_string(widgetId);
        }

        ImVec4 headerColor = ImGui::GetStyleColorVec4(ImGuiCol_Header);
        headerColor.w /= 1.75f; 
        ImGui::PushStyleColor(ImGuiCol_Header, headerColor);
        expanded = ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_Selected);
        ImGui::PopStyleColor();
        
        if (editingName) {
            ImGui::SameLine();
            if (ImGui::InputText(("##inputtext" + std::to_string(widgetId)).c_str(), &newName, ImGuiInputTextFlags_EnterReturnsTrue)) {
                name = newName;
                editingName = false;
            }
            if (activatedThisFrame) {
                ImGui::SetKeyboardFocusHere(-1);
                activatedThisFrame = false;
            } else if (!ImGui::IsItemActive()) {
                editingName = false;
            }
        } else if (ImGui::BeginPopupContextItem()) {
            if (ImGui::Button("Rename")) {
                activatedThisFrame = true;
                editingName = true;
                newName = name;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("Add Empty Child")) {
                EmplaceChild();
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("Duplicate")) {
                parent->EmplaceChild(component);
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("Delete")) {
                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
                if (expanded)
                    ImGui::TreePop();
                return false;
            }
            ImGui::EndPopup();
        }
        // Node Transform and children
        if (expanded) {
            ImGui::Unindent(5);
            if (ImGui::TreeNodeEx("[Transform]", ImGuiTreeNodeFlags_Bullet)) {
                bool pos = ImGui::DragFloat3("Position", glm::value_ptr(transform.position), 0.01f, 0.0f, 0.0f, "%.2f");
                bool rot = ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotationEuler), 0.1f, 0.0f, 0.0f, "%.1f");
                bool scale = ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.001f, 0.001f, 1e6f, "%.3f");
                if (pos || rot || scale)
                    transform.Update();
                ImGui::TreePop();
            }
            if (!component) {
                static bool showComponentCreationWidget = false;
                if (ImGui::Button("Add Component")) {
                    showComponentCreationWidget = !showComponentCreationWidget;
                }
                if (showComponentCreationWidget) {
                    auto component = Interface::ComponentCreationWidget(&showComponentCreationWidget);
                    if (component) {
                        component = component;
                    }
                }
            } else {
                if (ImGui::TreeNodeEx(("[" + component->name + "]").c_str(), ImGuiTreeNodeFlags_Bullet)) {
                    if (component->IsMesh()) {
                        std::static_pointer_cast<Component::Mesh>(component)->DisplayWidget(materials);
                    } else {
                        component->DisplayWidget();
                    }
                    if (ImGui::Button("Remove Component")) {
                        component.reset();
                    }
                    ImGui::TreePop();
                }
            }
            for (auto& child : children) {
                if (!child->DisplayWidget(materials)) {
                    RemoveChild(child);
                    break;
                }
            }
            ImGui::Indent(5);
            ImGui::TreePop();
        }
        return true;
    }

}