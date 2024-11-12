#include <plum/interface.hpp>

#include <string>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <plum/engine.hpp>
#include <plum/light.hpp>
#include <plum/model.hpp>
#include <plum/resources.hpp>
#include <plum/scene.hpp>
#include <plum/scenenode.hpp>
#include <plum/shader.hpp>
#include <plum/shape.hpp>
#include <plum/texture.hpp>

Interface::Interface(std::shared_ptr<Engine> e, std::shared_ptr<Resources> r) {
    engine = e;
    resources = r;
}

void Interface::ShowInterface() {

    // "New" dialogues
    static bool _showNewShapeChild = false;
    static bool _showNewLightChild = false;
    static bool _showNewNodeChild = false;
    // Properties structs
    static NodeProperties  _nodeProps(0);
    static ShapeProperties _shapeProps(0);
    static LightProperties _lightProps(0);
    // New shape template options
    static int _shapeSelection = 0;
    static std::string _textureSelectionStr = "";
    // New light options
    static const char* _lightOptions = "Directional\0Point\0Spot(future)\0";
    static int _lightSelection = 0;
    // New node options
    static int _nodeTemplateTypeSelection = 0;
    static std::string _nodeTemplateSelectionStr = "";
    static std::shared_ptr<Drawable> _nodeTemplate;
    // Skybox options
    static std::string _skyboxSelectionStr = "";

    int i = 0;
    ImGui::Begin("Plum Engine v0.01", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
    ImGui::SetWindowPos(ImVec2(0,0));
    ImGui::TextColored(ImVec4(0.3,1,1,1), "Press ` to capture/release mouse.");
    ImGui::TextColored(ImVec4(0.3,1,1,1), "Use WASD, Shift, and Spacebar to move camera.");
    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (!FramerateData.empty())
            ImGui::Text("%.2f ms/frame (%.1f fps)", FramerateData.back(), 1000./FramerateData.back());
        ImGui::PlotLines("ms/frame", FramerateData.data(), FramerateData.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(200,50));
    }
    if (ImGui::CollapsingHeader("Render Options", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("IBL", &Ibl, 0.0f, 1.0f);
        ImGui::Checkbox("SSAO", &Ssao); ImGui::SameLine();
        ImGui::Checkbox("FXAA", &Fxaa);
        ImGui::Checkbox("HDR", &Hdr); ImGui::SameLine();
        ImGui::Checkbox("Bloom", &Bloom);
        if (Hdr)
            ImGui::SliderFloat("HDR Exposure", &HdrExposure, 0.0, 5.0);
    }
    if (ImGui::CollapsingHeader("Assets", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::TreeNode("Textures")) {
            for (auto it = resources->Textures.begin(); it != resources->Textures.end(); it++) {
                std::shared_ptr<Tex> texture = it->second;
                if (ImGui::TreeNode(texture->Name.c_str())) {
                    if (ImGui::InputText(("Name##"+std::to_string(i++)).c_str(), texture->NameTemp.data(), ImGuiInputTextFlags_EnterReturnsTrue))
                        texture->Name = texture->NameTemp;
                    std::string format;
                    if (texture->Target == GL_TEXTURE_2D) {
                        ImGui::Text("Type: %s, %s", "2D", Tex::TexTypeToString(texture->Type).c_str());
                        ImGui::Text("Path: %s", texture->Paths[0].c_str());
                    } else if (texture->Target == GL_TEXTURE_CUBE_MAP) {
                        ImGui::Text("Type: %s, %s", "cubemap", Tex::TexTypeToString(texture->Type).c_str());
                        ImGui::Text("Paths:");
                        ImGui::Indent();
                        for (auto& path : it->second->Paths)
                            ImGui::Text("%s", path.c_str());
                        ImGui::Unindent();
                    }
                    if (ImGui::Button("Delete")) {
                        resources->DeleteTexture(texture->Name);
                        ImGui::TreePop();
                        break;
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Models")) {
            for (auto it = resources->Models.begin(); it != resources->Models.end(); it++) {
                std::shared_ptr<Model> model = it->second;
                if (ImGui::TreeNode(model->Name.c_str())) {
                    if (ImGui::InputText(("Name##"+std::to_string(i++)).c_str(), model->NameTemp.data(), ImGuiInputTextFlags_EnterReturnsTrue))
                        model->Name = model->NameTemp;
                    ImGui::Text("Path: %s", model->Path.c_str());
                    if (ImGui::Button("Delete")) {
                        resources->DeleteModel(model->Name);
                        ImGui::TreePop();
                        break;
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }
    if (ImGui::CollapsingHeader("Shape Templates", ImGuiTreeNodeFlags_DefaultOpen)) {
        // === "New" buttons ===
        if (ImGui::Button("New Shape Template")) {
            _showNewShapeChild = !_showNewShapeChild;
            _shapeProps = ShapeProperties(shapeCount++);
        }
        if (_showNewShapeChild) {
            if (ImGui::BeginChild("New Shape Template", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY)) {
                ImGui::InputText(("Template Name##"+std::to_string(i++)).c_str(), _shapeProps.name.data(), ImGuiInputTextFlags_EnterReturnsTrue);
                ImGui::RadioButton("Sphere", &_shapeSelection, 0); ImGui::SameLine();
                ImGui::RadioButton("Cube", &_shapeSelection, 1); ImGui::SameLine();
                ImGui::RadioButton("Rectangle", &_shapeSelection, 2);
                switch (_shapeSelection) {
                    case 0:
                        ImGui::SliderInt("stacks", &_shapeProps.stacks, 2, 50);
                        ImGui::SliderInt("sectors", &_shapeProps.sectors, 2, 50);
                        break;
                    case 2:
                        ImGui::SliderInt("x sections", &_shapeProps.x_sections, 1, 50);
                        ImGui::SliderInt("z sections", &_shapeProps.z_sections, 1, 50);
                        break;
                }
                if (ImGui::TreeNode("Appearance")) {
                    int surface = _shapeProps.surfacetype;
                    ImGui::RadioButton("Colored", &surface, Shape::Shape_Surface::SHAPE_SURFACE_COLOR); ImGui::SameLine();
                    ImGui::RadioButton("Textured", &surface, Shape::Shape_Surface::SHAPE_SURFACE_TEXTURE); ImGui::SameLine();
                    ImGui::RadioButton("Hybrid", &surface, Shape::Shape_Surface::SHAPE_SURFACE_HYBRID);
                    _shapeProps.surfacetype = Shape::Shape_Surface(surface);
                    if (_shapeProps.surfacetype == Shape::Shape_Surface::SHAPE_SURFACE_COLOR || _shapeProps.surfacetype == Shape::Shape_Surface::SHAPE_SURFACE_HYBRID) {
                        ImGui::ColorEdit3(("Albedo##"+std::to_string(i++)).c_str(), glm::value_ptr(_shapeProps.albedo), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
                        ImGui::SliderFloat(("Metallic##"+std::to_string(i)).c_str(), &_shapeProps.metallic, 0.0f, 1.0f);
                        ImGui::SliderFloat(("Roughness##"+std::to_string(i)).c_str(), &_shapeProps.roughness, 0.0f, 1.0f);
                    }
                    if (_shapeProps.surfacetype == Shape::Shape_Surface::SHAPE_SURFACE_TEXTURE || _shapeProps.surfacetype == Shape::Shape_Surface::SHAPE_SURFACE_HYBRID) {
                        for (int i = 0; i < 8; i++) {
                            Tex::Tex_Type textype = Tex::Tex_Type(i);
                            std::string label = Tex::TexTypeToString(textype) + " map";
                            std::string preview = _shapeProps.textures.count(textype) ? _shapeProps.textures[textype] : "";
                            if (ImGui::BeginCombo(label.c_str(), preview.c_str())) {
                                bool isSelected = (_textureSelectionStr == "(none)"); 
                                if (ImGui::Selectable("(none)", isSelected)) {
                                    _textureSelectionStr = "(none)";
                                    ImGui::SetItemDefaultFocus();
                                }
                                for (auto tex_it = resources->Textures.begin(); tex_it != resources->Textures.end(); tex_it++) {
                                    if (tex_it->second->Type != textype)
                                        continue;
                                    isSelected = (_textureSelectionStr == _shapeProps.textures[textype]); 
                                    if (ImGui::Selectable(tex_it->first.c_str(), isSelected)) {
                                        _textureSelectionStr = tex_it->first;
                                        _shapeProps.textures[textype] = tex_it->second->Name;
                                        ImGui::SetItemDefaultFocus();
                                    }
                                }
                                ImGui::EndCombo();
                            }
                        }
                    }
                    ImGui::TreePop();
                }
                if (ImGui::Button("Create Template")) {
                    switch (_shapeSelection) {
                        case 0: {
                            auto s = resources->LoadSphere(_shapeProps.name, *Shape::DefaultShaderColorOnly, _shapeProps.stacks, _shapeProps.sectors);
                            s->SetSurfaceType(_shapeProps.surfacetype);
                            s->SetColor(_shapeProps.albedo);
                            s->SetMetallic(_shapeProps.metallic);
                            s->SetRoughness(_shapeProps.roughness);
                            for (auto& texture : _shapeProps.textures)
                                s->SetTexture(texture.first, resources->Textures[texture.second]);
                            break;
                        }
                        case 1: {
                            auto c = resources->LoadCube(_shapeProps.name, *Shape::DefaultShaderColorOnly);
                            c->SetSurfaceType(_shapeProps.surfacetype);
                            c->SetColor(_shapeProps.albedo);
                            c->SetMetallic(_shapeProps.metallic);
                            c->SetRoughness(_shapeProps.roughness);
                            for (auto& texture : _shapeProps.textures)
                                c->SetTexture(texture.first, resources->Textures[texture.second]);
                            break;
                        }
                        case 2: 
                            auto r = resources->LoadRectangle(_shapeProps.name, *Shape::DefaultShaderColorOnly, _shapeProps.x_sections, _shapeProps.z_sections);
                            r->SetSurfaceType(_shapeProps.surfacetype);
                            r->SetColor(_shapeProps.albedo);
                            r->SetMetallic(_shapeProps.metallic);
                            r->SetRoughness(_shapeProps.roughness);
                            for (auto& texture : _shapeProps.textures)
                                r->SetTexture(texture.first, resources->Textures[texture.second]);
                            break;
                    }
                    _showNewShapeChild = false;
                    shapeCount++;
                }
                ImGui::EndChild();
            }
        }
        if (ImGui::TreeNode("Shape Templates##1")) {
            for (auto it = resources->Shapes.begin(); it != resources->Shapes.end(); it++) {
                std::shared_ptr<Shape> shape = it->second;
                if (ImGui::TreeNode(shape->Name.c_str())) {
                    if (ImGui::InputText(("Node Name##"+std::to_string(i++)).c_str(), shape->NameTemp.data(), ImGuiInputTextFlags_EnterReturnsTrue))
                        shape->Name = shape->NameTemp;
                    if (ImGui::TreeNode("Appearance")) {
                        int surface = shape->GetSurfaceType();
                        ImGui::RadioButton("Colored", &surface, Shape::Shape_Surface::SHAPE_SURFACE_COLOR); ImGui::SameLine();
                        ImGui::RadioButton("Textured", &surface, Shape::Shape_Surface::SHAPE_SURFACE_TEXTURE); ImGui::SameLine();
                        ImGui::RadioButton("Hybrid", &surface, Shape::Shape_Surface::SHAPE_SURFACE_HYBRID);
                        shape->SetSurfaceType(Shape::Shape_Surface(surface));
                        if (shape->GetSurfaceType() == Shape::Shape_Surface::SHAPE_SURFACE_COLOR || shape->GetSurfaceType() == Shape::Shape_Surface::SHAPE_SURFACE_HYBRID) {
                            ImGui::ColorEdit3(("Albedo##"+std::to_string(i++)).c_str(), glm::value_ptr(*shape->GetColorPtr()), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
                            ImGui::SliderFloat(("Metallic##"+std::to_string(i)).c_str(), shape->GetMetallicPtr(), 0.0f, 1.0f);
                            ImGui::SliderFloat(("Roughness##"+std::to_string(i)).c_str(), shape->GetRoughnessPtr(), 0.0f, 1.0f);
                        }
                        if (shape->GetSurfaceType() == Shape::Shape_Surface::SHAPE_SURFACE_TEXTURE || shape->GetSurfaceType() == Shape::Shape_Surface::SHAPE_SURFACE_HYBRID) {
                            for (int i = 0; i < 8; i++) {
                                Tex::Tex_Type textype = Tex::Tex_Type(i);
                                std::string label = Tex::TexTypeToString(textype) + " map";
                                std::string preview = shape->Textures.count(textype) ? shape->Textures[textype]->Name : "";
                                if (ImGui::BeginCombo(label.c_str(), preview.c_str())) {
                                    bool isSelected = (_textureSelectionStr == "(remove)"); 
                                    if (ImGui::Selectable("(remove)", isSelected)) {
                                        _textureSelectionStr = "(remove)";
                                        shape->Textures.erase(textype);
                                        ImGui::SetItemDefaultFocus();
                                    }
                                    for (auto tex_it = resources->Textures.begin(); tex_it != resources->Textures.end(); tex_it++) {
                                        if (tex_it->second->Type != textype)
                                            continue;
                                        isSelected = (_textureSelectionStr == it->first); 
                                        if (ImGui::Selectable(tex_it->first.c_str(), isSelected)) {
                                            _textureSelectionStr = tex_it->first;
                                            shape->SetTexture(textype, tex_it->second);
                                            ImGui::SetItemDefaultFocus();
                                        }
                                    }
                                    ImGui::EndCombo();
                                }

                            }
                        }
                        ImGui::TreePop();
                    }
                    if (ImGui::Button("Delete")) {
                        resources->DeleteShape(shape->Name);
                        ImGui::TreePop();
                        break;
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }
    if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
        // === New Light and New Node dialogues ===
        // === "New" buttons ===
        if (ImGui::Button("New Light")) {
            _showNewLightChild = !_showNewLightChild;
            _showNewNodeChild = false;
            _nodeProps = NodeProperties(nodeCount++);
            _lightProps = LightProperties(lightCount++);
        }
        ImGui::SameLine();
        if (ImGui::Button("New Node")) {
            _showNewLightChild = false;
            _showNewNodeChild = !_showNewNodeChild;
            _nodeProps = NodeProperties(nodeCount++);
            _lightProps = LightProperties(lightCount++);
        }
        if (_showNewLightChild) {
            if (ImGui::BeginChild("New Light", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY)) {
                ImGui::InputText("Name", _lightProps.name.data(), ImGuiInputTextFlags_AutoSelectAll);
                ImGui::Combo("Light Selection", &_lightSelection, _lightOptions);
                ImGui::ColorEdit3("Color", glm::value_ptr(_lightProps.color), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
                switch (_lightSelection) {
                    case 0: 
                        ImGui::DragFloat3("Direction", glm::value_ptr(_lightProps.direction), 0.01, 0.0f, 0.0f, "%.2f");
                        break;
                    case 1:
                        ImGui::DragFloat3("Position", glm::value_ptr(_lightProps.position), 0.01, 0.0f, 0.0f, "%.2f");
                        ImGui::DragFloat3(("Size##"+std::to_string(i++)).c_str(), glm::value_ptr(_lightProps.size), 0.01, 0.0f, 0.0f, "%.2f");
                        break;
                    case 2: 
                        ImGui::Text("(future)");
                        ImGui::DragFloat3("Direction", glm::value_ptr(_lightProps.direction), 0.01, 0.0f, 0.0f, "%.2f");
                        ImGui::DragFloat3("Position", glm::value_ptr(_lightProps.position), 0.01, 0.0f, 0.0f, "%.2f");
                        break;
                }
                ImGui::Checkbox("Cast Shadows", &_lightProps.shadows);
                if (ImGui::Button("Create Light")) {
                    switch (_lightSelection) {
                        case 0: {
                            std::shared_ptr<DirectionalLight> dl = MainScene->CreateDirectionalLight();
                            dl->SetName(_lightProps.name);
                            dl->Color = _lightProps.color;
                            dl->Direction = _lightProps.direction;
                            if (_lightProps.shadows)
                                dl->EnableShadows();
                            break;
                        }
                        case 1: {
                            std::shared_ptr<PointLight> pl = MainScene->CreatePointLight();
                            pl->SetName(_lightProps.name);
                            pl->Color = _lightProps.color;
                            pl->Position = _lightProps.position;
                            if (_lightProps.shadows)
                                pl->EnableShadows(float(engine->ShadowWidthCube)/float(engine->ShadowHeightCube));
                            break;
                        }
                        case 2: 
                            ImGui::Text("(future)");
                            break;
                    }
                    _showNewLightChild = false;
                    lightCount++;
                }
                ImGui::EndChild();
            }
        }
        if (_showNewNodeChild) {
            if (ImGui::BeginChild("New Node", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY)) {
                ImGui::InputText("Name", _nodeProps.name.data(), ImGuiInputTextFlags_AutoSelectAll);
                ImGui::RadioButton("From Model", &_nodeTemplateTypeSelection, 0);
                ImGui::RadioButton("From Shape Template", &_nodeTemplateTypeSelection, 1);
                switch (_nodeTemplateTypeSelection) {
                    case 0:
                        if (ImGui::BeginCombo("Model", _nodeTemplateSelectionStr.c_str())) {
                            for (auto it = resources->Models.begin(); it != resources->Models.end(); it++) {
                                bool isSelected = (_nodeTemplateSelectionStr == it->first); 
                                if (ImGui::Selectable(it->first.c_str(), isSelected)) {
                                    _nodeTemplateSelectionStr = it->first;
                                    _nodeTemplate = it->second;
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }
                        break;
                    case 1:
                        if (ImGui::BeginCombo("Shape Template", _nodeTemplateSelectionStr.c_str())) {
                            for (auto it = resources->Shapes.begin(); it != resources->Shapes.end(); it++) {
                                bool isSelected = (_nodeTemplateSelectionStr == it->first); 
                                if (ImGui::Selectable(it->first.c_str(), isSelected)) {
                                    _nodeTemplateSelectionStr = it->first;
                                    _nodeTemplate = it->second;
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }
                        break;
                }
                if (ImGui::TreeNode("Model Matrix")) {
                    ImGui::DragFloat3("Position", glm::value_ptr(_nodeProps.position), 0.01, 0.0f, 0.0f, "%.2f");
                    ImGui::DragFloat3("Scale", glm::value_ptr(_nodeProps.scale), 0.01, 0.0f, 100.0f, "%.3f");
                    ImGui::DragFloat3("Rotation", glm::value_ptr(_nodeProps.rotation), 0.01, -360.0f, 360.0f, "%.1f");
                    ImGui::TreePop();
                }
                ImGui::Checkbox("Bypass Lighting", &_nodeProps.bypasslighting);
                if (ImGui::Button("Create Node") && _nodeTemplate) {
                    std::shared_ptr<SceneNode> node = MainScene->CreateNode(_nodeTemplate);
                    node->SetName(_nodeProps.name);
                    node->SetPlacement(_nodeProps.position, _nodeProps.scale, _nodeProps.rotation);
                    node->BypassLighting = _nodeProps.bypasslighting;
                    _showNewNodeChild = false;
                    nodeCount++;
                }
                ImGui::EndChild();
            }
        }
        // === Scene components ===
        if (ImGui::TreeNode("Skybox")) {
            std::string preview = MainScene->EnvironmentMap->Name;
            if (ImGui::BeginCombo("Diffuse Texture", preview.c_str())) {
                for (auto it = resources->Textures.begin(); it != resources->Textures.end(); it++) {
                    if (it->second->Type != Tex::Tex_Type::TEX_DIFFUSE)
                        continue;
                    bool isSelected = (_skyboxSelectionStr == it->first); 
                    if (ImGui::Selectable(it->first.c_str(), isSelected)) {
                        _skyboxSelectionStr = it->first;
                        MainScene->EnvironmentMap = it->second;
                        engine->InitEnvironment(MainScene->EnvironmentMap);
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Lights")) {
            for (auto it = MainScene->DirLights.begin(); it != MainScene->DirLights.end(); it++) {
                std::shared_ptr<DirectionalLight> dirlight = *it;
                if (ImGui::TreeNode(dirlight->Name.c_str())) {
                    if (ImGui::InputText(("Name##"+std::to_string(i++)).c_str(), dirlight->NameTemp.data(), ImGuiInputTextFlags_EnterReturnsTrue))
                        dirlight->Name = dirlight->NameTemp;
                    ImGui::ColorEdit3(("Color##"+std::to_string(i++)).c_str(), glm::value_ptr(dirlight->Color), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
                    ImGui::DragFloat3(("Direction##"+std::to_string(i++)).c_str(), glm::value_ptr(dirlight->Direction), 0.01, 0.0f, 0.0f, "%.2f");
                    bool hasShadows = dirlight->HasShadows();
                    ImGui::Checkbox(("Cast Shadows##"+std::to_string(i++)).c_str(), &hasShadows);
                    if (hasShadows != dirlight->HasShadows()) {
                        if (hasShadows)
                            dirlight->EnableShadows();
                        else
                            dirlight->DisableShadows();
                    }
                    if (ImGui::Button("Delete")) {
                        MainScene->Delete(dirlight);
                        ImGui::TreePop();
                        break;
                    }
                    ImGui::TreePop();
                }
            }
            for (auto it = MainScene->PointLights.begin(); it != MainScene->PointLights.end(); it++) {
                std::shared_ptr<PointLight> pointlight = *it;
                if (ImGui::TreeNode(pointlight->Name.c_str())) {
                    if (ImGui::InputText(("Name##"+std::to_string(i++)).c_str(), pointlight->NameTemp.data(), ImGuiInputTextFlags_EnterReturnsTrue))
                        pointlight->Name = pointlight->NameTemp;
                    ImGui::ColorEdit3(("Color##"+std::to_string(i++)).c_str(), glm::value_ptr(pointlight->Color), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
                    ImGui::DragFloat3(("Position##"+std::to_string(i++)).c_str(), glm::value_ptr(pointlight->Position), 0.01, 0.0f, 0.0f, "%.2f");
                    ImGui::DragFloat3(("Size##"+std::to_string(i++)).c_str(), glm::value_ptr(pointlight->Size), 0.01, 0.0f, 0.0f, "%.2f");
                    bool hasShadows = pointlight->HasShadows();
                    ImGui::Checkbox(("Cast Shadows##"+std::to_string(i++)).c_str(), &hasShadows);
                    if (hasShadows != pointlight->HasShadows()) {
                        if (hasShadows)
                            pointlight->EnableShadows();
                        else
                            pointlight->DisableShadows();
                    }
                    if (ImGui::Button("Delete")) {
                        MainScene->Delete(pointlight);
                        ImGui::TreePop();
                        break;
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Nodes")) {
            for (auto it = MainScene->Nodes.begin(); it != MainScene->Nodes.end(); it++) {
                std::shared_ptr<SceneNode> node = *it;
                if (ImGui::TreeNode(node->Name.c_str())) {
                    if (ImGui::InputText(("Name##"+std::to_string(i++)).c_str(), node->NameTemp.data(), ImGuiInputTextFlags_EnterReturnsTrue))
                        node->Name = node->NameTemp;
                    ImGui::Text("Model/Template: %s", node->NodeTemplate->Name.c_str());
                    if (ImGui::TreeNode("Model Matrix")) {
                        ImGui::DragFloat3("Position", glm::value_ptr(node->Position), 0.01, 0.0f, 0.0f, "%.2f");
                        ImGui::DragFloat3("Scale", glm::value_ptr(node->Scale), 0.01, 0.0f, 100.0f, "%.3f");
                        ImGui::DragFloat3("Rotation", glm::value_ptr(node->Rotation), 0.01, -360.0f, 360.0f, "%.1f");
                        ImGui::TreePop();
                    }
                    ImGui::Checkbox("Bypass Lighting", &node->BypassLighting);
                    if (ImGui::Button("Delete")) {
                        MainScene->Delete(node);
                        ImGui::TreePop();
                        break;
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

void Interface::SetStyle(ImGuiStyle& style) {
    float r = 103./255.;
    float g = 49./255.;
    float b = 71./255.;
    float a = 0.5;
    
    float r_hov = r*1.5;
    float g_hov = g*1.5;
    float b_hov = b*1.5;

    ImGui::StyleColorsClassic(&style);
    
    style.FrameRounding = 1.f;
    style.ChildRounding = 1.f;
    style.WindowRounding = 1.f;

    style.Colors[ImGuiCol_TitleBg]              = ImVec4(r,g,b,a);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(r,g,b,a+0.25);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0,0,0,a);
    style.Colors[ImGuiCol_Header]               = ImVec4(r*1.2,g*1.2,b*1.2,a+0.15);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(r_hov, g_hov, b_hov, a+0.15);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(r_hov, g_hov, b_hov, a+0.15);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(0.5, 0.5, 0.5, a/2);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(r_hov, g_hov, b_hov, a/1.5);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(r_hov, g_hov, b_hov, a/1.5);
    style.Colors[ImGuiCol_Button]               = ImVec4(r,g,b,a);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(r_hov, g_hov, b_hov, a);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(r_hov, g_hov, b_hov, a);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(r, g, b, a);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(1,1,1,0.6);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.25,0.25,0.25,0.6);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(r,g,b,0.5);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(r,g,b,0.7);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(r,g,b,0.8);
    style.Colors[ImGuiCol_NavHighlight]         = ImVec4(r_hov*1.5, g_hov*1.5, b_hov*1.5, a);
}