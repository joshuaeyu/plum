#include "scene/scenenode.hpp"

#include "component/all.hpp"
#include "interface/widget.hpp"

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
        auto child = children.emplace_back(std::move(node));
        child->parent = this;
        return child;
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
    void SceneNode::Draw(Renderer::RenderModule& module, const glm::mat4& parent_transform) {
        const glm::mat4 model_matrix = parent_transform * transform.Matrix();
        if (component)
            component->Draw(module, model_matrix);
        for (auto& child : children) {
            child->Draw(module, model_matrix);
        }
    }

    bool SceneNode::DisplayWidget() {
        static std::string newName;
        
        std::string label;
        bool expanded;
        
        if (editingName) {
            label = std::string("##treenode") + dummyWidget.idString;
            ImGui::SetNextItemAllowOverlap();
        } else {
            label = name + "##treenode" + dummyWidget.idString;
        }

        ImVec4 headerColor = ImGui::GetStyleColorVec4(ImGuiCol_Header);
        headerColor.w /= 1.75f; 
        ImGui::PushStyleColor(ImGuiCol_Header, headerColor);
        expanded = ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_Selected);
        ImGui::PopStyleColor();
        
        if (editingName) {
            ImGui::SameLine();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
            if (ImGui::InputText(std::string("##inputtext" + dummyWidget.idString).c_str(), &newName, ImGuiInputTextFlags_EnterReturnsTrue)) {
                name = newName;
                editingName = false;
            }
            ImGui::PopStyleVar();
            if (activatedThisFrame) {
                ImGui::SetKeyboardFocusHere(-1);
                activatedThisFrame = false;
            } else if (!ImGui::IsItemActive()) {
                editingName = false;
            }
        } else if (ImGui::BeginPopupContextItem(std::string("##popup" + dummyWidget.idString).c_str())) {
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
            // if (ImGui::Button("Duplicate")) {
            //     if (parent) {
            //     }
            //     ImGui::CloseCurrentPopup();
            // }
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
            if (ImGui::TreeNodeEx("[Transform]", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen)) {
                bool pos = ImGui::DragFloat3("Position", glm::value_ptr(transform.position), 0.01f, 0.0f, 0.0f, "%.2f");
                bool rot = ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotationEuler), 0.1f, 0.0f, 0.0f, "%.1f");
                glm::vec3 scaleVal = transform.scale;
                bool scale = ImGui::DragFloat3("Scale", glm::value_ptr(scaleVal), 0.001f, 0.001f, 1e6f, "%.3f");
                ImGui::SameLine();
                ImGui::Checkbox("Link", &linkScale);
                if (scale) {
                    if (linkScale) {
                        if (scaleVal.x != transform.scale.x) {
                            transform.scale *= scaleVal.x / transform.scale.x;
                        } else if (scaleVal.y != transform.scale.y) {
                            transform.scale *= scaleVal.y / transform.scale.y;
                        } else if (scaleVal.z != transform.scale.z) {
                            transform.scale *= scaleVal.z / transform.scale.z;
                        }
                    } else {
                        transform.scale = scaleVal;
                    }
                }
                if (pos || rot || scale)
                    transform.Update();
                ImGui::TreePop();
            }
            if (!component) {
                if (ImGui::Button("Add Component")) {
                    showComponentCreationWidget = !showComponentCreationWidget;
                }
                if (showComponentCreationWidget) {
                    std::shared_ptr<Component::ComponentBase> comp;
                    if (componentCreationWidget.Display(&comp)) {
                        component = comp;
                        showComponentCreationWidget = false;
                    }
                }
            } else {
                if (ImGui::TreeNodeEx(("[" + component->name + "]").c_str(), ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen)) {
                    component->DisplayWidget();
                    if (ImGui::Button("Remove Component")) {
                        component.reset();
                    }
                    ImGui::TreePop();
                }
            }
            for (auto& child : children) {
                if (!child->DisplayWidget()) {
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