#include <map>
#include <vector>
#include <queue>
#include <random>
#include <string>

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>

#include <plum/engine.hpp>
#include <plum/interface.hpp>
#include <plum/light.hpp>
#include <plum/model.hpp>
#include <plum/resources.hpp>
#include <plum/scene.hpp>
#include <plum/scenenode.hpp>
#include <plum/shader.hpp>
#include <plum/shape.hpp>
#include <plum/texture.hpp>

#include <plum/context/context.hpp>
#include <plum/context/window.hpp>

// forward declarations
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void window_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// screen settings - macbook retina is 3024x1964
const unsigned int SCR_WIDTH_INIT = 1920;
const unsigned int SCR_HEIGHT_INIT = 1080;   // Depends on menu bar and dock

// globals
auto scene = std::make_shared<Scene>();    // contains camera
auto engine = std::make_shared<Engine>();
auto resources = std::make_shared<Resources>();
float lastX = SCR_WIDTH_INIT / 2.0f;
float lastY = SCR_HEIGHT_INIT / 2.0f;
bool firstMouse = true;
float deltaTime;

int main() {
    //          INITIALIZE - GLFW, GLAD
    Context::WindowCreator windowCreator;
    Context::Window windowObj = windowCreator.Create(SCR_WIDTH_INIT, SCR_HEIGHT_INIT, "Plum Engine");
    windowObj.MakeCurrent();
    
    // callbacks
    windowObj.SetFramebufferSizeCallback(framebuffer_size_callback);
    windowObj.SetCursorPosCallback(mouse_callback);
    windowObj.SetKeyCallback(key_callback);
    windowObj.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Context::GlContext context = Context::GlContext::GetGlContext();
    context.Initialize(windowObj);

    //          INITIALIZE - IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    ImGui_ImplGlfw_InitForOpenGL(windowObj.GetHandle(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    //          GL SETTINGS
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); // LEQUAL for skybox optimization, since depth buffer is cleared to 1.0 by default
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    // glad_glEnable(GL_STENCIL_TEST);
    // glad_glEnable(GL_BLEND);
    // glad_glBlendFunc((GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //          SHADERS
    std::cout << "Loading shaders..." << std::endl;;
    // Environment map
    Shader shader_skybox("shaders/shaderv_skybox.vs", "shaders/shaderf_skybox.fs");
    Shader shader_skyboxhdr("shaders/shaderv_skybox.vs", "shaders/shaderf_skyboxhdr.fs");
    Shader shader_equirect("shaders/shaderv_equirect.vs", "shaders/shaderf_equirect.fs");
    Shader shader_irradiance("shaders/shaderv_equirect.vs", "shaders/shaderf_irradiance.fs");
    Shader shader_prefilter("shaders/shaderv_equirect.vs", "shaders/shaderf_prefilter.fs");
    Shader shader_brdflut("shaders/shaderv_2d.vs", "shaders/shaderf_2dbrdflut.fs");
    // Shader shader_cube("shaders/shaderv_cube.vs", "shaders/shaderf_cube.fs");
    // Scene objects
    Shader shader_3mf("shaders/shaderv_gen.vs", "shaders/shaderf_3mf.fs");
    Shader shader_obj("shaders/shaderv_gen.vs", "shaders/shaderf_obj.fs");
    Shader shader_gltf("shaders/shaderv_gen.vs", "shaders/shaderf_gltf.fs");
    Shader shader_rawalbedo("shaders/shaderv_gen.vs", "shaders/shaderf_rawalbedo.fs");
    Shader shader_basiccolor("shaders/shaderv_gen.vs", "shaders/shaderf_basiccolor.fs");
    Shader shader_basictexture("shaders/shaderv_gen.vs", "shaders/shaderf_basictexture.fs");
    Shader shader_basichybrid("shaders/shaderv_gen.vs", "shaders/shaderf_basichybrid.fs");
    // // Shader shader_asteroids("shaders/shaderv_asteroids.vs", "shaders/shaderf_obj_tex.fs",   "shaderg_passthrough.gs");
    // Shader shader_normals("shaders/shaderv_normals.vs", "shaders/shaderf_normals.fs",   "shaderg_normals.gs");
    // Deferred lighting pass
    // Shader shader_lightingpass("shaders/shaderv_2d.vs", "shaders/shaderf_lightingpass.fs");
    Shader shader_lightingpasspbr("shaders/shaderv_2d.vs", "shaders/shaderf_lightingpasspbr.fs");
    // Shadows
    Shader shader_shadowmap_2d("shaders/shaderv_shadow2d.vs", "shaders/shaderf_shadow2d.fs");
    Shader shader_shadowmap_cube("shaders/shaderv_shadowcube.vs", "shaders/shaderf_shadowcube.fs", "shaders/shaderg_shadowcube.gs");
    Shader shader_2ddepth("shaders/shaderv_2d.vs", "shaders/shaderf_2ddepth.fs");
    // Post processing
    Shader shader_2dhdr("shaders/shaderv_2d.vs", "shaders/shaderf_2dhdr.fs");
    Shader shader_2dbloom("shaders/shaderv_2d.vs", "shaders/shaderf_2dbloom.fs");
    Shader shader_2dbloomblur("shaders/shaderv_2d.vs", "shaders/shaderf_2dgaussian.fs");
    Shader shader_2dfxaa("shaders/shaderv_2d.vs", "shaders/shaderf_2dfxaa.fs");
    Shader shader_2dssao("shaders/shaderv_2d.vs", "shaders/shaderf_2dssao.fs");
    Shader shader_2dssaoblur("shaders/shaderv_2d.vs", "shaders/shaderf_2dssaoblur.fs");
    // Set up shader uniform blocks
    shader_3mf.setUniformBlockBinding(Shader::Shader_Enum::SHADER_UBO_SCHEME_1);
    shader_obj.setUniformBlockBinding(Shader::Shader_Enum::SHADER_UBO_SCHEME_1);
    shader_rawalbedo.setUniformBlockBinding(Shader::Shader_Enum::SHADER_UBO_SCHEME_1);
    shader_basiccolor.setUniformBlockBinding(Shader::Shader_Enum::SHADER_UBO_SCHEME_1);
    shader_basictexture.setUniformBlockBinding(Shader::Shader_Enum::SHADER_UBO_SCHEME_1);
    shader_basichybrid.setUniformBlockBinding(Shader::Shader_Enum::SHADER_UBO_SCHEME_1);
    shader_lightingpasspbr.setUniformBlockBinding(Shader::Shader_Enum::SHADER_UBO_SCHEME_1);
    std::cout << "Done loading shaders!" << std::endl << std::endl;

    //          TEXTURES
    std::cout << "Loading standalone textures..." << std::endl;
    std::shared_ptr<Tex> tex_container = resources->LoadTexture2D("container", "assets/textures/container2.png", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR);
    std::shared_ptr<Tex> tex_cobblestone = resources->LoadTexture2D("cobblestone", "assets/textures/cobblestone.png", GL_TEXTURE_2D, GL_REPEAT, GL_NEAREST);
    std::shared_ptr<Tex> tex_wood = resources->LoadTexture2D("wood", "assets/textures/wood.png", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR);
    std::shared_ptr<Tex> tex_stonetiles = resources->LoadTexture2D("stonetiles", "assets/textures/stonetiles.jpg", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR);
    std::shared_ptr<Tex> tex_rustedmetal = resources->LoadTexture2D("rustedmetal", "assets/textures/rustedmetal.jpg", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR);
    std::shared_ptr<Tex> tex_brickdiffuse = resources->LoadTexture2D("brickdiffuse", "assets/textures/brickwall.jpg", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR);
    std::shared_ptr<Tex> tex_bricknormal = resources->LoadTexture2D("bricknormal", "assets/textures/brickwall_normal.jpg", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR, Tex::Tex_Type::TEX_HEIGHT);
    std::vector<std::string> ocean_faces = {
        "assets/textures/skybox/right.jpg",
        "assets/textures/skybox/left.jpg",
        "assets/textures/skybox/top.jpg",
        "assets/textures/skybox/bottom.jpg",
        "assets/textures/skybox/front.jpg",
        "assets/textures/skybox/back.jpg"
    };
    std::shared_ptr<Tex> tex_oceanskybox = resources->LoadTextureCube("ocean", ocean_faces, GL_TEXTURE_CUBE_MAP, GL_CLAMP_TO_EDGE, GL_LINEAR);
    std::shared_ptr<Tex> tex_nightsky = resources->LoadTexture2D("nightsky", "assets/textures/kloppenheim_4k.hdr", GL_TEXTURE_2D, GL_CLAMP_TO_EDGE, GL_LINEAR, Tex::Tex_Type::TEX_DIFFUSE, true);
    std::shared_ptr<Tex> tex_garden = resources->LoadTexture2D("garden", "assets/textures/studio_garden_4k.hdr", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR, Tex::Tex_Type::TEX_DIFFUSE, true);
    std::shared_ptr<Tex> tex_puppetstudio = resources->LoadTexture2D("puppetstudio", "assets/textures/puppet_studio_4k.hdr", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR, Tex::Tex_Type::TEX_DIFFUSE, true);
    std::shared_ptr<Tex> tex_trainstation = resources->LoadTexture2D("trainstation", "assets/textures/dresden_station_4k.hdr", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR, Tex::Tex_Type::TEX_DIFFUSE, true);
    std::shared_ptr<Tex> tex_newportloft = resources->LoadTexture2D("newportloft", "assets/textures/newport_loft.png", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR, Tex::Tex_Type::TEX_DIFFUSE, true);
    std::shared_ptr<Tex> tex_black = resources->LoadTexture2D("black", "assets/textures/black.png", GL_TEXTURE_2D, GL_REPEAT, GL_NEAREST);
    std::shared_ptr<Tex> tex_white = resources->LoadTexture2D("white", "assets/textures/white.png", GL_TEXTURE_2D, GL_REPEAT, GL_NEAREST);
    std::cout << "Done loading standalone textures!" << std::endl << std::endl;

    //          MODELS
    std::cout << "Loading models..." << std::endl;;
    auto backpack = resources->LoadModel("backpack", shader_obj, "assets/models/backpack/backpack.obj", 1.0f, true);
    auto toilet = resources->LoadModel("toilet", shader_3mf, "assets/models/toilet/toilet.3mf", 0.005f);
    auto plantpot = resources->LoadModel("plantpot", shader_obj, "assets/models/plantpot/indoor plant_02.obj", 0.01f);
    auto skull = resources->LoadModel("skull", shader_obj, "assets/models/skull/12140_Skull_v3_L2.obj", 0.1f);
    auto boat = resources->LoadModel("boat", shader_obj, "assets/models/boat/12219_boat_v2_L2.obj", 0.01f);
    auto adamHead = resources->LoadModel("adamhead", shader_gltf, "assets/models/adamHead/adamHead.gltf", 100.0f);
    auto lieutHead = resources->LoadModel("lieuthead", shader_gltf, "assets/models/lieutenantHead/lieutenantHead.gltf", 10.f);
    // auto sponza = resources->LoadModel("sponza", shader_gltf, "assets/models/sponza/glTF/Sponza.gltf", 1.f);
    // Model model_planet("assets/planet/planet.obj");
    // Model model_rock("assets/rock/rock.obj");
    std::cout << "Done loading models!" << std::endl << std::endl;
    
    //          SHAPE TEMPLATES
    std::cout << "Loading premade shape templates..." << std::endl;
    auto cobblestoneCube = resources->LoadCube("cobblestoneCube", shader_basictexture);
    cobblestoneCube->SetTexture(Tex::Tex_Type::TEX_DIFFUSE, tex_cobblestone);
    auto containerCube = resources->LoadCube("containerCube", shader_basictexture);
    containerCube->SetTexture(Tex::Tex_Type::TEX_DIFFUSE, tex_container);
    auto brickSphere = resources->LoadSphere("brickSphere", shader_basictexture, 40, 40);
    brickSphere->SetTexture(Tex::Tex_Type::TEX_DIFFUSE, tex_brickdiffuse);
    brickSphere->SetTexture(Tex::Tex_Type::TEX_HEIGHT, tex_bricknormal);
    auto brickFloor = resources->LoadRectangle("brickFloor", shader_basictexture, 10, 10);
    brickFloor->SetTexture(Tex::Tex_Type::TEX_DIFFUSE, tex_brickdiffuse);
    brickFloor->SetTexture(Tex::Tex_Type::TEX_HEIGHT, tex_bricknormal);
    std::cout << "Done loading premade shape templates!" << std::endl << std::endl;
    
    //          MISC INIT
    Shape::DefaultShaderColorOnly = &shader_basiccolor;
    Shape::DefaultShaderTextureOnly = &shader_basictexture;
    Shape::DefaultShaderHybrid = &shader_basichybrid;
    Light::DefaultShader = &shader_rawalbedo;
    scene->InitializeUniformBlocks();
    
    //          ENGINE
    std::cout << "Initializing engine..." << std::endl;
    engine->EquirectShader = &shader_equirect;
    engine->IrradianceShader = &shader_irradiance;
    engine->PrefilterShader = &shader_prefilter;
    scene->EnvironmentMap = tex_nightsky;
    engine->InitEnvironment(scene->EnvironmentMap);
    engine->SetScreenDimensions(SCR_WIDTH_INIT, SCR_HEIGHT_INIT);
    engine->InitGbuffer();
    engine->InitSsao();
    engine->InitHdr();
    engine->InitBloom();
    engine->InitFxaa();
    GLuint brdfLUT = engine->GenerateBrdfLut(shader_brdflut);
    Engine::Framebuffer shadowArrayFb2d = engine->GenerateShadowMapArray(GL_TEXTURE_2D_ARRAY);
    Engine::Framebuffer shadowArrayFbCube = engine->GenerateShadowMapArray(GL_TEXTURE_CUBE_MAP_ARRAY);
    std::cout << "Done initializing engine!" << std::endl << std::endl;

    //          DEFINE SCENE
    std::cout << "Defining scene..." << std::endl;
    int sceneIdx = 1;
    switch (sceneIdx) {
        // case 0: {
        //     // Sponza + four point lights
        //     auto sponza0 = scene->CreateNode(sponza);
        //     sponza0->SetName("sponza0");
        //     glm::vec3 pointlight_positions[] = { glm::vec3(9,5,3), glm::vec3(-10,5,3), glm::vec3(-10,5,-3.75),glm::vec3(9,5,-3.75) };
        //     for (auto& pos : pointlight_positions) {
        //         auto pl = scene->CreatePointLight();
        //         pl->Position = pos;
        //         pl->Color = glm::vec3(255/255.,222/255.,160/255.) * 5.0f;
        //         pl->EnableShadows();
        //     }
        //     break;
        // }
        case 1:
            // Random spheres and point lights over a floor
            auto floor0 = scene->CreateNode(brickFloor);
            floor0->SetPlacement(glm::vec3(0,-0.5,0), glm::vec3(10));
            floor0->SetName("brick floor");
            srand(40);
            for (int i = 0; i < 16; i++) {
                glm::vec3 col = glm::vec3(rand()%100/100.0, rand()%100/100.0, rand()%100/100.0);
                glm::vec3 pos = glm::vec3(rand()%15-7.5, rand()%7, rand()%15-7.5);
                if (i < 4) {
                    auto pl = scene->CreatePointLight();
                    glm::vec3 highlighted_col = glm::vec3(1) + 5.0f * col;
                    pl->Position = pos;
                    pl->Color = highlighted_col;
                    pl->EnableShadows();
                } else {
                    std::string name = "sphere"+std::to_string(i);
                    auto s = resources->LoadSphere(name);
                    s->SetColor(col);
                    s->SetMetallic(rand()%100/100.0);
                    s->SetRoughness(rand()%75/100.0);
                    auto sphereNode = scene->CreateNode(s);
                    sphereNode->SetName(name);
                    sphereNode->SetPlacement(pos, glm::vec3(0.2+rand()%50/100.0));
                }
            }
            break;
    }
    std::cout << "Done defining scene!" << std::endl << std::endl;
    // auto adamhead0 = scene->CreateNode(adamHead);
    // adamhead0->SetName("adamhead");
    // adamhead0->SetPlacement(glm::vec3(0,5,0), glm::vec3(1), glm::vec3(1,0,0), -90);
    // auto lieutHead0 = scene->CreateNode(lieutHead);
    // lieutHead0->SetName("lieuthead");
    // lieutHead0->SetPlacement(glm::vec3(5,5,0), glm::vec3(1), glm::vec3(0,1,0), 180);
    // std::shared_ptr<SceneNode> boat0 = scene->CreateNode(boat);
    // boat0->SetPlacement(glm::vec3(0,3,0), glm::vec3(0.01));
    // boat0->SetName("boat0");
    // std::shared_ptr<SceneNode> backpack0 = scene->CreateNode(resources->Models["backpack"]);
    // backpack0->SetName("backpack0");
    // backpack0->SetPlacement(glm::vec3(-5, 2, 0));
    // std::shared_ptr<SceneNode> skull0 = scene->CreateNode(resources->Models["skull"]);
    // skull0->SetName("skull0");
    // skull0->SetPlacement(glm::vec3(0, 10, 0), glm::vec3(0.25));
    // std::shared_ptr<SceneNode> toilet0 = scene->CreateNode(toilet);
    // toilet0->SetName("toilet0");
    // toilet0->SetPlacement(glm::vec3(-5,0,5));
    // std::shared_ptr<SceneNode> sphere0 = scene->CreateNode(brickSphere);
    // sphere0->SetName("brickSphere0");
    // sphere0->SetPlacement(glm::vec3(0,2,-5));
    // std::shared_ptr<SceneNode> cube0 = scene->CreateNode(cobblestoneCube);
    // cube0->SetName("cobblestoneCube0");
    // std::shared_ptr<SceneNode> cube1 = scene->CreateNode(containerCube);
    // cube1->SetName("containerCube0");

    
    // std::shared_ptr<DirectionalLight> dirlight0 = scene->CreateDirectionalLight();
    // dirlight0->SetColor(glm::vec3(1));
    // dirlight0->SetDirection(-glm::vec3(-0.45,0.42,-0.78));
    // dirlight0->GenerateLightspaceMatrix(60, 60, 0.1, 50);
    // dirlight0->SetShadows(false);
    // std::shared_ptr<DirectionalLight> dirlight1 = scene->CreateDirectionalLight();
    // dirlight1->Color = glm::vec3(2);
    // dirlight1->EnableShadows();
    
    // Mandatory
    Cube unitcube("cubeForSkybox");
    
    // Camera setup
    glm::mat4 projection = glm::perspective(glm::radians(72.5f), (float)engine->ScreenWidth / (float)engine->ScreenHeight, 0.1f, 80.0f);
    scene->SceneCamera.Projection = projection;

    //          RENDER LOOP
    float currentTime = glfwGetTime();
    float lastFpsTime = currentTime;
    float lastTime = currentTime;
    float msPerFrame = 0;
    unsigned int frameCount = 0;
    Interface interface(engine, resources, scene);
    while (!windowObj.ShouldClose()) {
        
        // calculate time
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        // fps
        frameCount++;
        if (currentTime - lastFpsTime >= 1.0) {
            msPerFrame = (currentTime-lastFpsTime)*1000/frameCount;
            if (interface.FramerateData.size() > 100)
                interface.FramerateData = std::vector<float>(interface.FramerateData.begin()+1, interface.FramerateData.end());
            interface.FramerateData.push_back(msPerFrame);
            std::cout << msPerFrame << " ms/frame" << std::endl;
            frameCount = 0;
            lastFpsTime = currentTime;
        }
        
        // inputs
        glfwPollEvents();
        processInput(windowObj.GetHandle());

        // ImGui
        interface.SetStyle(ImGui::GetStyle());
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        interface.ShowInterface();

        if (interface.Hdr)
            glDisable(GL_FRAMEBUFFER_SRGB);
        else
            glEnable(GL_FRAMEBUFFER_SRGB); // Gamma correction, only enable if not doing gamma correction in postprocessing framebuffer (e.g., HDR shader)

        // SET UNIFORMS
        scene->SetVertexMatricesUniformBlock();
        scene->SetFragmentMatricesUniformBlock();
        scene->SetDirectionalLightUniformBlock();
        scene->SetPointLightUniformBlock();

        // 1. GEOMETRY PASS
        glBindFramebuffer(GL_FRAMEBUFFER, engine->Gbuffer.fbo);
        glViewport(0, 0, engine->ScreenWidth, engine->ScreenHeight);
        glClearColor(0.,0.,0.,1.);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene->DrawNodes();

        // 2a. RENDER SHADOW MAPS
        // Generate orthographic shadow maps for directional lights
        glBindFramebuffer(GL_FRAMEBUFFER, shadowArrayFb2d.fbo);
        glViewport(0,0, engine->ShadowWidth2D, engine->ShadowHeight2D);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_shadowmap_2d.programID);
        for (int i = 0, shadow_count = 0; i < scene->DirLights.size(); i++) {
            std::shared_ptr<DirectionalLight> dirlight = scene->DirLights[i];
            if (!dirlight->HasShadows())
                continue;
            shader_shadowmap_2d.setMat4("lightSpaceMatrix", dirlight->GetLightspaceMatrix());
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowArrayFb2d.depth, 0, shadow_count++);
            // Draw objects
            // glCullFace(GL_FRONT);
            glDisable(GL_CULL_FACE);
            scene->DrawNodes(shader_shadowmap_2d);
            glEnable(GL_CULL_FACE);
            // glCullFace(GL_BACK);
        }
        // Generate omnidirectional shadow maps for point lights
        glBindFramebuffer(GL_FRAMEBUFFER, shadowArrayFbCube.fbo);
        glViewport(0,0, engine->ShadowWidthCube, engine->ShadowHeightCube);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_shadowmap_cube.programID);
        for (int i = 0, shadow_count = 0; i < scene->PointLights.size(); i++) {
            std::shared_ptr<PointLight> pointlight = scene->PointLights[i];
            if (!pointlight->HasShadows())
                continue;
            shader_shadowmap_cube.setInt("layer", shadow_count++);
            for (int j = 0; j < 6; j++) {
                shader_shadowmap_cube.setMat4("lightSpaceMatrices[" + std::to_string(j) + "]", (pointlight->GetLightspaceMatrices())[j]);
            }
            shader_shadowmap_cube.setVec3("lightPos", pointlight->Position);
            shader_shadowmap_cube.setFloat("far", pointlight->GetFarPlane());
            // Draw objects
            // glDisable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            scene->DrawNodes(shader_shadowmap_cube);
            glCullFace(GL_BACK);
            // glEnable(GL_CULL_FACE);
        }
        
        // 2b. SSAO
        // SSAO buffer as is
        if (interface.Ssao) {
            glViewport(0, 0, engine->ScreenWidth, engine->ScreenHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, engine->Ssao.fbo);
            glClearColor(0.,0.,0.,1.);
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(shader_2dssao.programID);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, engine->Gbuffer.colors[0]);    // Position
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, engine->Gbuffer.colors[1]);    // Normal
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, engine->SsaoNoiseTexture);
                shader_2dssao.setInt("gPosition", 0);
                shader_2dssao.setInt("gNormal", 1);
                shader_2dssao.setInt("noiseTexture", 2);
                shader_2dssao.setMat4("projection", projection);
                for (int i = 0; i < engine->SsaoKernel.size(); i++) {
                    shader_2dssao.setVec3("samples[" + std::to_string(i) + "]", engine->SsaoKernel[i]);
                }
                engine->RenderQuad();
        }
        // SSAO blurring
        glViewport(0, 0, engine->ScreenWidth, engine->ScreenHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, engine->SsaoBlur.fbo);
        glClearColor(1.,1.,1.,1.);  // So that no occlusion (occlusionfactor = 1) if ssao is disabled
        glClear(GL_COLOR_BUFFER_BIT);
        if (interface.Ssao) {
            glUseProgram(shader_2dssaoblur.programID);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, engine->Ssao.colors[0]);
                shader_2dssaoblur.setInt("ssaoInput", 0);
                engine->RenderQuad();
        }

        // Framebuffer queue
        std::queue<GLuint> framebuffers;
        if (interface.Bloom)
            framebuffers.push(engine->Bloom1.fbo);
        if (interface.Hdr)
            framebuffers.push(engine->Hdr.fbo);
        if (interface.Fxaa)
            framebuffers.push(engine->Fxaa.fbo);
        framebuffers.push(0);

        // 3. LIGHTING PASS
        glViewport(0, 0, engine->ScreenWidth, engine->ScreenHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.front());
        glClearColor(0.,0.,0.,1.);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_lightingpasspbr.programID);
            // G-buffer
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, engine->Gbuffer.colors[0]);    // position
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, engine->Gbuffer.colors[1]);    // normal
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, engine->Gbuffer.colors[2]);    // albedo, specular
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, engine->Gbuffer.colors[3]);    // metallic, roughness
            shader_lightingpasspbr.setInt("gPosition", 0);
            shader_lightingpasspbr.setInt("gNormal", 1);
            shader_lightingpasspbr.setInt("gAlbedoSpec", 2);
            shader_lightingpasspbr.setInt("gMetRouOcc", 3);
            // SSAO
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, engine->SsaoBlur.colors[0]);
            shader_lightingpasspbr.setInt("ssao", 4);
            // PBR IBL
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_CUBE_MAP, engine->CurrentEnvironment.irradiance);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_CUBE_MAP, engine->CurrentEnvironment.prefilter);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, brdfLUT);
            shader_lightingpasspbr.setInt("irradianceMap", 5);
            shader_lightingpasspbr.setInt("prefilterMap", 6);
            shader_lightingpasspbr.setInt("brdfLUT", 7);
            shader_lightingpasspbr.setFloat("ibl", interface.Ibl);
            // Shadow maps
            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_2D_ARRAY, shadowArrayFb2d.depth);
            glActiveTexture(GL_TEXTURE9);
            glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowArrayFbCube.depth);
            shader_lightingpasspbr.setInt("shadowmap_2d_array_shadow", 8);
            shader_lightingpasspbr.setInt("shadowmap_cube_array_shadow", 9);
            engine->RenderQuad();

        // 4. FORWARD RENDERING
        glBindFramebuffer(GL_READ_FRAMEBUFFER, engine->Gbuffer.fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers.front());
        glBlitFramebuffer(0, 0, engine->ScreenWidth, engine->ScreenHeight, 0, 0, engine->ScreenWidth, engine->ScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);   // Internal formats need to match!
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.front());
        framebuffers.pop();
        // Draw light sources
        scene->DrawLights();
        // Draw skybox
        glUseProgram(shader_skybox.programID);
        shader_skybox.setMat4("view", glm::mat4(glm::mat3(scene->SceneCamera.GetViewMatrix())));
        shader_skybox.setMat4("projection", projection);
        shader_skybox.setInt("cubemap", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, engine->CurrentEnvironment.environment);
        glCullFace(GL_FRONT);
        unitcube.Draw(shader_skybox);
        glCullFace(GL_BACK);

        // 5. POST-PROCESSING
        if (interface.Bloom) {
            bool horizontal = true;
            int amount = 3;
            glViewport(0, 0, engine->ScreenWidth, engine->ScreenHeight);
            glUseProgram(shader_2dbloomblur.programID);
            shader_2dbloomblur.setInt("image", 0);
            glActiveTexture(GL_TEXTURE0);
            // Render to Bloom2 using Bloom1 first, then to Bloom1 using Bloom2, swapping repeatedly
            for (int i = 0; i < amount*2; i++) {
                Engine::Framebuffer drawFb, readFb;
                if (!horizontal) {
                    drawFb = engine->Bloom1;
                    readFb = engine->Bloom2;
                } else {
                    drawFb = engine->Bloom2;
                    readFb = engine->Bloom1;
                }
                glBindFramebuffer(GL_FRAMEBUFFER, drawFb.fbo);
                shader_2dbloomblur.setInt("horizontal", horizontal);
                glBindTexture(GL_TEXTURE_2D, readFb.colors[1]);   // Use raw HDR scene highlights first, then ping pong
                engine->RenderQuad();
                horizontal = !horizontal;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.front());
            framebuffers.pop();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(shader_2dbloom.programID);
                shader_2dbloom.setInt("sceneRaw", 0);
                shader_2dbloom.setInt("sceneBloom", 1);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, engine->Bloom1.colors[0]);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, engine->Bloom1.colors[1]);
                engine->RenderQuad();
                glBindTexture(GL_TEXTURE_2D, 0);
        } 
        if (interface.Hdr) {
            glViewport(0, 0, engine->ScreenWidth, engine->ScreenHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.front());
            framebuffers.pop();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(shader_2dhdr.programID);
                shader_2dhdr.setFloat("exposure", interface.HdrExposure);
                shader_2dhdr.setInt("hdrBuffer", 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, engine->Hdr.colors[0]);
                engine->RenderQuad();
                glBindTexture(GL_TEXTURE_2D, 0);
        }
        if (interface.Fxaa) {
            glViewport(0, 0, engine->ScreenWidth, engine->ScreenHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.front());
            framebuffers.pop();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(shader_2dfxaa.programID);
                shader_2dfxaa.setInt("screenTexture", 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, engine->Fxaa.colors[0]);
                engine->RenderQuad();
                glBindTexture(GL_TEXTURE_2D, 0);
        }

        if (!interface.Hdr)
            glDisable(GL_FRAMEBUFFER_SRGB);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (!interface.Hdr)
            glEnable(GL_FRAMEBUFFER_SRGB);

        // Essential
        windowObj.SwapBuffers();
    }

    //          CLEAN UP
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    int inputmode = glfwGetInputMode(window, GLFW_CURSOR);
    if (inputmode == GLFW_CURSOR_NORMAL) 
        return;

    // Camera translation (needs to be here because GLFW key callback is only called on press, repeat, and release)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        scene->SceneCamera.ProcessKeyboard(Camera::Direction::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        scene->SceneCamera.ProcessKeyboard(Camera::Direction::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        scene->SceneCamera.ProcessKeyboard(Camera::Direction::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        scene->SceneCamera.ProcessKeyboard(Camera::Direction::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        scene->SceneCamera.ProcessKeyboard(Camera::Direction::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        scene->SceneCamera.ProcessKeyboard(Camera::Direction::DOWN, deltaTime);
}

// Engine needs to communicate with GLFWwindow
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    engine->SetScreenDimensions(width, height);
    engine->InitGbuffer();
    engine->InitSsao();
    engine->InitHdr();
    engine->InitBloom();
    engine->InitFxaa();
}
void window_size_callback(GLFWwindow *window, int width, int height) {
    glfwSetWindowSize(window, width, height);
}

// Controls (in scene?) need to communicate with window
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    int inputmode = glfwGetInputMode(window, GLFW_CURSOR);
    if (inputmode == GLFW_CURSOR_NORMAL)
        return;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = -(ypos - lastY);

    scene->SceneCamera.ProcessMouse(xoffset, yoffset);
    
    lastX = xpos;
    lastY = ypos;

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS)
        return;
    
    switch (key) {
        case GLFW_KEY_ESCAPE:   
            glfwSetWindowShouldClose(window, GLFW_TRUE); 
            break;
        case GLFW_KEY_GRAVE_ACCENT:
            int inputmode = glfwGetInputMode(window, GLFW_CURSOR);
            if (inputmode == GLFW_CURSOR_NORMAL) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                firstMouse = true;
            } else if (inputmode == GLFW_CURSOR_DISABLED) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            break;
    }
}