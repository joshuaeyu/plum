#include <map>
#include <vector>
#include <queue>
#include <random>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <external/imgui/imgui.h>
#include <external/imgui/imgui_impl_glfw.h>
#include <external/imgui/imgui_impl_opengl3.h>
#include <external/imgui/imgui_stdlib.h>

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

using namespace std;

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
Scene scene, scene1;    // contains camera
float lastX = SCR_WIDTH_INIT / 2.0f;
float lastY = SCR_HEIGHT_INIT / 2.0f;
bool firstMouse = true;
float deltaTime;

int main() {
    //          INITIALIZE - GLFW, GLAD
    if (!glfwInit()) return -1;
    // window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH_INIT, SCR_HEIGHT_INIT, "Plum Engine", NULL, NULL);
    if (window == nullptr) {
        std::cerr << "glfwCreateWindow failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "gladLoadGLLoader failed" << std::endl;
        glfwTerminate();
        return -1;
    }

    //          INITIALIZE - IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    GLint n;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &n); // 2048
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &n);  // 16
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &n); // 80
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &n); // 65536
    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &n); // 16384
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &n); // 2048
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &n); // 16384

    //          GL SETTINGS
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); // LEQUAL for skybox optimization, since depth buffer is cleared to 1.0 by default
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    // glad_glEnable(GL_STENCIL_TEST);
    // glad_glEnable(GL_BLEND);
    // glad_glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //          SHADERS
    cout << "Loading shaders..." << endl;;
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
    shader_3mf.setUniformBlockBinding(SHADER_UBO_SCHEME_1);
    shader_obj.setUniformBlockBinding(SHADER_UBO_SCHEME_1);
    shader_rawalbedo.setUniformBlockBinding(SHADER_UBO_SCHEME_1);
    shader_basiccolor.setUniformBlockBinding(SHADER_UBO_SCHEME_1);
    shader_basictexture.setUniformBlockBinding(SHADER_UBO_SCHEME_1);
    shader_basichybrid.setUniformBlockBinding(SHADER_UBO_SCHEME_1);
    shader_lightingpasspbr.setUniformBlockBinding(SHADER_UBO_SCHEME_1);
    // shader_obj_explode.setUniformBlockBinding(SHADER_UBO_SCHEME_1);
    // shader_normals.setUniformBlockBinding(SHADER_UBO_SCHEME_1);
    // shader_asteroids.setUniformBlockBinding(SHADER_UBO_SCHEME_1);
    cout << "Done loading shaders!" << endl << endl;

    //          TEXTURES
    cout << "Loading standalone textures..." << endl;
    shared_ptr<Tex> tex_container = Resources::LoadTexture2D("container", "assets/container2.png", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR);
    shared_ptr<Tex> tex_cobblestone = Resources::LoadTexture2D("cobblestone", "assets/cobblestone.png", GL_TEXTURE_2D, GL_REPEAT, GL_NEAREST);
    shared_ptr<Tex> tex_wood = Resources::LoadTexture2D("wood", "assets/wood.png", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR);
    shared_ptr<Tex> tex_stonetiles = Resources::LoadTexture2D("stonetiles", "assets/stonetiles.jpg", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR);
    shared_ptr<Tex> tex_rustedmetal = Resources::LoadTexture2D("rustedmetal", "assets/rustedmetal.jpg", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR);
    shared_ptr<Tex> tex_brickdiffuse = Resources::LoadTexture2D("brickdiffuse", "assets/brickwall.jpg", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR);
    shared_ptr<Tex> tex_bricknormal = Resources::LoadTexture2D("bricknormal", "assets/brickwall_normal.jpg", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR, TEX_HEIGHT);
    vector<string> ocean_faces = {
        "assets/skybox/right.jpg",
        "assets/skybox/left.jpg",
        "assets/skybox/top.jpg",
        "assets/skybox/bottom.jpg",
        "assets/skybox/front.jpg",
        "assets/skybox/back.jpg"
    };
    shared_ptr<Tex> tex_oceanskybox = Resources::LoadTextureCube("ocean", ocean_faces, GL_TEXTURE_CUBE_MAP, GL_CLAMP_TO_EDGE, GL_LINEAR);
    shared_ptr<Tex> tex_nightsky = Resources::LoadTexture2D("nightsky", "assets/kloppenheim_4k.hdr", GL_TEXTURE_2D, GL_CLAMP_TO_EDGE, GL_LINEAR);
    shared_ptr<Tex> tex_garden = Resources::LoadTexture2D("garden", "assets/studio_garden_4k.hdr", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR, TEX_DIFFUSE, true);
    shared_ptr<Tex> tex_puppetstudio = Resources::LoadTexture2D("puppetstudio", "assets/puppet_studio_4k.hdr", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR, TEX_DIFFUSE, true);
    shared_ptr<Tex> tex_trainstation = Resources::LoadTexture2D("trainstation", "assets/dresden_station_4k.hdr", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR, TEX_DIFFUSE, true);
    shared_ptr<Tex> tex_newportloft = Resources::LoadTexture2D("newportloft", "assets/newport_loft.png", GL_TEXTURE_2D, GL_REPEAT, GL_LINEAR, TEX_DIFFUSE, true);
    shared_ptr<Tex> tex_black = Resources::LoadTexture2D("black", "assets/black.png", GL_TEXTURE_2D, GL_REPEAT, GL_NEAREST);
    shared_ptr<Tex> tex_white = Resources::LoadTexture2D("white", "assets/white.png", GL_TEXTURE_2D, GL_REPEAT, GL_NEAREST);
    cout << "Done loading standalone textures!" << endl << endl;

    //          MODELS
    cout << "Loading models..." << endl;;
    auto backpack = Resources::LoadModel("backpack", shader_obj, "assets/backpack/backpack.obj", 1.0f, true);
    auto toilet = Resources::LoadModel("toilet", shader_3mf, "assets/toilet/toilet.3mf", 0.005f);
    auto plantpot = Resources::LoadModel("plantpot", shader_obj, "assets/plantpot/indoor plant_02.obj", 0.01f);
    auto skull = Resources::LoadModel("skull", shader_obj, "assets/skull/12140_Skull_v3_L2.obj", 0.1f);
    auto boat = Resources::LoadModel("boat", shader_obj, "assets/boat/12219_boat_v2_L2.obj", 0.01f);
    auto adamHead = Resources::LoadModel("adamhead", shader_gltf, "assets/adamHead/adamHead.gltf", 100.0f);
    auto lieutHead = Resources::LoadModel("lieuthead", shader_gltf, "assets/lieutenantHead/lieutenantHead.gltf", 10.f);
    auto sponza = Resources::LoadModel("sponza", shader_gltf, "assets/sponza/glTF/Sponza.gltf", 1.f);
    // Model model_planet("assets/planet/planet.obj");
    // Model model_rock("assets/rock/rock.obj");
    cout << "Done loading models!" << endl << endl;
    
    //          SHAPE TEMPLATES
    cout << "Loading premade shape templates..." << endl;
    auto cobblestoneCube = Resources::LoadCube("cobblestoneCube", shader_basictexture);
    cobblestoneCube->SetTexture(TEX_DIFFUSE, tex_cobblestone);
    auto containerCube = Resources::LoadCube("containerCube", shader_basictexture);
    containerCube->SetTexture(TEX_DIFFUSE, tex_container);
    auto brickSphere = Resources::LoadSphere("brickSphere", shader_basictexture, 40, 40);
    brickSphere->SetTexture(TEX_DIFFUSE, tex_brickdiffuse);
    brickSphere->SetTexture(TEX_HEIGHT, tex_bricknormal);
    auto brickFloor = Resources::LoadRectangle("brickFloor", shader_basictexture, 10, 10);
    brickFloor->SetTexture(TEX_DIFFUSE, tex_brickdiffuse);
    brickFloor->SetTexture(TEX_HEIGHT, tex_bricknormal);
    cout << "Done loading premade shape templates!" << endl << endl;
    
    //          MISC INIT
    Shape::DefaultShaderColorOnly = &shader_basiccolor;
    Shape::DefaultShaderTextureOnly = &shader_basictexture;
    Shape::DefaultShaderHybrid = &shader_basichybrid;
    Light::DefaultShader = &shader_rawalbedo;
    scene.InitializeUniformBlocks();
    
    //          ENGINE
    Engine::EquirectShader = &shader_equirect;
    Engine::IrradianceShader = &shader_irradiance;
    Engine::PrefilterShader = &shader_prefilter;
    scene.EnvironmentMap = tex_nightsky;
    Engine::InitEnvironment(scene.EnvironmentMap);
    Engine::SetScreenDimensions(SCR_WIDTH_INIT, SCR_HEIGHT_INIT);
    Engine::InitGbuffer();
    Engine::InitSsao();
    Engine::InitHdr();
    Engine::InitBloom();
    Engine::InitFxaa();
    GLuint brdfLUT = Engine::GenerateBrdfLut(shader_brdflut);
    Engine::Framebuffer shadowArrayFb2d = Engine::GenerateShadowMapArray(GL_TEXTURE_2D_ARRAY);
    Engine::Framebuffer shadowArrayFbCube = Engine::GenerateShadowMapArray(GL_TEXTURE_CUBE_MAP_ARRAY);

    //          DEFINE SCENE
    // shared_ptr<SceneNode> floor0 = scene.CreateNode(brickFloor);
    // floor0->SetPlacement(glm::vec3(0,-0.5,0), glm::vec3(20));
    // floor0->SetName("brick floor");
    // auto adamhead0 = scene.CreateNode(adamHead);
    // adamhead0->SetName("adamhead");
    // adamhead0->SetPlacement(glm::vec3(0,5,0), glm::vec3(1), glm::vec3(1,0,0), -90);
    // auto lieutHead0 = scene.CreateNode(lieutHead);
    // lieutHead0->SetName("lieuthead");
    // lieutHead0->SetPlacement(glm::vec3(5,5,0), glm::vec3(1), glm::vec3(0,1,0), 180);
    auto sponza0 = scene.CreateNode(sponza);
    sponza0->SetName("sponza0");
    // shared_ptr<SceneNode> boat0 = scene.CreateNode(boat);
    // boat0->SetPlacement(glm::vec3(0,3,0), glm::vec3(0.01));
    // boat0->SetName("boat0");
    // shared_ptr<SceneNode> backpack0 = scene.CreateNode(Resources::Models["backpack"]);
    // backpack0->SetName("backpack0");
    // backpack0->SetPlacement(glm::vec3(-5, 2, 0));
    // shared_ptr<SceneNode> skull0 = scene.CreateNode(Resources::Models["skull"]);
    // skull0->SetName("skull0");
    // skull0->SetPlacement(glm::vec3(0, 10, 0), glm::vec3(0.25));
    // shared_ptr<SceneNode> toilet0 = scene.CreateNode(toilet);
    // toilet0->SetName("toilet0");
    // toilet0->SetPlacement(glm::vec3(-5,0,5));
    // shared_ptr<SceneNode> sphere0 = scene.CreateNode(brickSphere);
    // sphere0->SetName("brickSphere0");
    // sphere0->SetPlacement(glm::vec3(0,2,-5));
    // shared_ptr<SceneNode> cube0 = scene.CreateNode(cobblestoneCube);
    // cube0->SetName("cobblestoneCube0");
    // shared_ptr<SceneNode> cube1 = scene.CreateNode(containerCube);
    // cube1->SetName("containerCube0");

    Cube unitcube("cubeForSkybox");
    
    // shared_ptr<DirectionalLight> dirlight0 = scene.CreateDirectionalLight();
    // dirlight0->SetColor(glm::vec3(1));
    // dirlight0->SetDirection(-glm::vec3(-0.45,0.42,-0.78));
    // dirlight0->GenerateLightspaceMatrix(60, 60, 0.1, 50);
    // dirlight0->SetShadows(false);
    shared_ptr<DirectionalLight> dirlight1 = scene.CreateDirectionalLight();
    dirlight1->Color = glm::vec3(2);
    dirlight1->EnableShadows();
    glm::vec3 pointlight_positions[] = { glm::vec3(9,5,3), glm::vec3(-10,5,3), glm::vec3(-10,5,-3.75),glm::vec3(9,5,-3.75) };
    for (auto& pos : pointlight_positions) {
        auto pl = scene.CreatePointLight();
        pl->Position = pos;
        pl->Color = glm::vec3(255/255.,222/255.,160/255.) * 5.0f;
        pl->EnableShadows();
    }
    
    srand(40);
    // for (int i = 0; i < 16; i++) {
    //     glm::vec3 col = glm::vec3(rand()%100/100.0, rand()%100/100.0, rand()%100/100.0);
    //     glm::vec3 pos = glm::vec3(rand()%30-15, rand()%10, rand()%30-15);
    //     shared_ptr<Sphere> s = scene.CreateSphere(30,30);
    //     s->SetName("sphere"+to_string(i));
    //     s->SetColor(col);
    //     s->SetMetallic(rand()%100/100.0);
    //     s->SetRoughness(rand()%75/100.0);
    //     s->SetPlacement(pos, glm::vec3(0.2+rand()%50/100.0));
    //     s->SetDefaultShader(&shader_basiccolor);
    //     if (i < 0) {
    //         shared_ptr<PointLight> pl = scene.CreatePointLight();
    //         glm::vec3 highlighted_col = glm::vec3(1)+5.0f*col;
    //         pl->Color = highlighted_col;
    //         pl->HasShadows = true;
    //         pl->Position = pos;
    //         pl->GenerateLightspaceMatrices(float(Engine::ShadowWidthCube)/float(Engine::ShadowHeightCube), 0.1, 75);
    //         s->SetPipelineBypass(true);
    //         s->SetColor(highlighted_col);
    //     }
    // }

    // Camera setup
    glm::mat4 projection = glm::perspective(glm::radians(72.5f), (float)Engine::ScreenWidth / (float)Engine::ScreenHeight, 0.1f, 80.0f);
    scene.SceneCamera.SetProjectionMatrix(projection);

    //          RENDER LOOP
    float currentTime = glfwGetTime();
    float lastFpsTime = currentTime;
    float lastTime = currentTime;
    float msPerFrame = 0;
    unsigned int frameCount = 0;
    Interface::MainScene = &scene;
    while (!glfwWindowShouldClose(window)) {
        
        // calculate time
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        // fps
        frameCount++;
        if (currentTime - lastFpsTime >= 1.0) {
            msPerFrame = (currentTime-lastFpsTime)*1000/frameCount;
            if (Interface::FramerateData.size() > 100)
                Interface::FramerateData = vector<float>(Interface::FramerateData.begin()+1, Interface::FramerateData.end());
            Interface::FramerateData.push_back(msPerFrame);
            cout << msPerFrame << " ms/frame" << endl;
            frameCount = 0;
            lastFpsTime = currentTime;
        }
        
        // inputs
        glfwPollEvents();
        processInput(window);

        // ImGui
        Interface::SetStyle(ImGui::GetStyle());
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        Interface::ShowInterface();

        if (Interface::Hdr)
            glDisable(GL_FRAMEBUFFER_SRGB);
        else
            glEnable(GL_FRAMEBUFFER_SRGB); // Gamma correction, only enable if not doing gamma correction in postprocessing framebuffer (e.g., HDR shader)

        // SET UNIFORMS
        scene.SetVertexMatricesUniformBlock();
        scene.SetFragmentMatricesUniformBlock();
        scene.SetDirectionalLightUniformBlock();
        scene.SetPointLightUniformBlock();

        // 1. GEOMETRY PASS
        glBindFramebuffer(GL_FRAMEBUFFER, Engine::Gbuffer.fbo);
        glViewport(0, 0, Engine::ScreenWidth, Engine::ScreenHeight);
        glClearColor(0.,0.,0.,1.);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene.DrawNodes();

        // 2a. RENDER SHADOW MAPS
        // Generate orthographic shadow maps for directional lights
        glBindFramebuffer(GL_FRAMEBUFFER, shadowArrayFb2d.fbo);
        glViewport(0,0, Engine::ShadowWidth2D, Engine::ShadowHeight2D);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_shadowmap_2d.programID);
        for (int i = 0, shadow_count = 0; i < scene.DirLights.size(); i++) {
            shared_ptr<DirectionalLight> dirlight = scene.DirLights[i];
            if (!dirlight->HasShadows())
                continue;
            shader_shadowmap_2d.setMat4("lightSpaceMatrix", dirlight->GetLightspaceMatrix());
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowArrayFb2d.depth, 0, shadow_count++);
            // Draw objects
            // glCullFace(GL_FRONT);
            glDisable(GL_CULL_FACE);
            scene.DrawNodes(shader_shadowmap_2d);
            glEnable(GL_CULL_FACE);
            // glCullFace(GL_BACK);
        }
        // Generate omnidirectional shadow maps for point lights
        glBindFramebuffer(GL_FRAMEBUFFER, shadowArrayFbCube.fbo);
        glViewport(0,0, Engine::ShadowWidthCube, Engine::ShadowHeightCube);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_shadowmap_cube.programID);
        for (int i = 0, shadow_count = 0; i < scene.PointLights.size(); i++) {
            shared_ptr<PointLight> pointlight = scene.PointLights[i];
            if (!pointlight->HasShadows())
                continue;
            shader_shadowmap_cube.setInt("layer", shadow_count++);
            for (int j = 0; j < 6; j++) {
                shader_shadowmap_cube.setMat4("lightSpaceMatrices[" + to_string(j) + "]", (pointlight->GetLightspaceMatrices())[j]);
            }
            shader_shadowmap_cube.setVec3("lightPos", pointlight->Position);
            shader_shadowmap_cube.setFloat("far", pointlight->GetFarPlane());
            // Draw objects
            // glDisable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            scene.DrawNodes(shader_shadowmap_cube);
            glCullFace(GL_BACK);
            // glEnable(GL_CULL_FACE);
        }
        
        // 2b. SSAO
        // SSAO buffer as is
        if (Interface::Ssao) {
            glViewport(0, 0, Engine::ScreenWidth, Engine::ScreenHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, Engine::Ssao.fbo);
            glClearColor(0.,0.,0.,1.);
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(shader_2dssao.programID);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Engine::Gbuffer.colors[0]);    // Position
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, Engine::Gbuffer.colors[1]);    // Normal
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, Engine::SsaoNoiseTexture);
                shader_2dssao.setInt("gPosition", 0);
                shader_2dssao.setInt("gNormal", 1);
                shader_2dssao.setInt("noiseTexture", 2);
                shader_2dssao.setMat4("projection", projection);
                for (int i = 0; i < Engine::SsaoKernel.size(); i++) {
                    shader_2dssao.setVec3("samples[" + to_string(i) + "]", Engine::SsaoKernel[i]);
                }
                Engine::RenderQuad();
        }
        // SSAO blurring
        glViewport(0, 0, Engine::ScreenWidth, Engine::ScreenHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, Engine::SsaoBlur.fbo);
        glClearColor(1.,1.,1.,1.);  // So that no occlusion (occlusionfactor = 1) if ssao is disabled
        glClear(GL_COLOR_BUFFER_BIT);
        if (Interface::Ssao) {
            glUseProgram(shader_2dssaoblur.programID);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Engine::Ssao.colors[0]);
                shader_2dssaoblur.setInt("ssaoInput", 0);
                Engine::RenderQuad();
        }

        // Framebuffer queue
        queue<GLuint> framebuffers;
        if (Interface::Bloom)
            framebuffers.push(Engine::Bloom1.fbo);
        if (Interface::Hdr)
            framebuffers.push(Engine::Hdr.fbo);
        if (Interface::Fxaa)
            framebuffers.push(Engine::Fxaa.fbo);
        framebuffers.push(0);

        // 3. LIGHTING PASS
        glViewport(0, 0, Engine::ScreenWidth, Engine::ScreenHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.front());
        glClearColor(0.,0.,0.,1.);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_lightingpasspbr.programID);
            // G-buffer
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Engine::Gbuffer.colors[0]);    // position
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, Engine::Gbuffer.colors[1]);    // normal
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, Engine::Gbuffer.colors[2]);    // albedo, specular
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, Engine::Gbuffer.colors[3]);    // metallic, roughness
            shader_lightingpasspbr.setInt("gPosition", 0);
            shader_lightingpasspbr.setInt("gNormal", 1);
            shader_lightingpasspbr.setInt("gAlbedoSpec", 2);
            shader_lightingpasspbr.setInt("gMetRouOcc", 3);
            // SSAO
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, Engine::SsaoBlur.colors[0]);
            shader_lightingpasspbr.setInt("ssao", 4);
            // PBR IBL
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_CUBE_MAP, Engine::CurrentEnvironment.irradiance);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_CUBE_MAP, Engine::CurrentEnvironment.prefilter);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, brdfLUT);
            shader_lightingpasspbr.setInt("irradianceMap", 5);
            shader_lightingpasspbr.setInt("prefilterMap", 6);
            shader_lightingpasspbr.setInt("brdfLUT", 7);
            shader_lightingpasspbr.setFloat("ibl", Interface::Ibl);
            // Shadow maps
            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_2D_ARRAY, shadowArrayFb2d.depth);
            glActiveTexture(GL_TEXTURE9);
            glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowArrayFbCube.depth);
            shader_lightingpasspbr.setInt("shadowmap_2d_array_shadow", 8);
            shader_lightingpasspbr.setInt("shadowmap_cube_array_shadow", 9);
            Engine::RenderQuad();

        // 4. FORWARD RENDERING
        glBindFramebuffer(GL_READ_FRAMEBUFFER, Engine::Gbuffer.fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers.front());
        glBlitFramebuffer(0, 0, Engine::ScreenWidth, Engine::ScreenHeight, 0, 0, Engine::ScreenWidth, Engine::ScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);   // Internal formats need to match!
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.front());
        framebuffers.pop();
        // Draw light sources
        scene.DrawLights();
        // Draw skybox
        glUseProgram(shader_skybox.programID);
        shader_skybox.setMat4("view", glm::mat4(glm::mat3(scene.SceneCamera.GetViewMatrix())));
        shader_skybox.setMat4("projection", projection);
        shader_skybox.setInt("cubemap", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Engine::CurrentEnvironment.environment);
        glCullFace(GL_FRONT);
        unitcube.Draw(shader_skybox);
        glCullFace(GL_BACK);

        // 5. POST-PROCESSING
        if (Interface::Bloom) {
            bool horizontal = true;
            int amount = 3;
            glViewport(0, 0, Engine::ScreenWidth, Engine::ScreenHeight);
            glUseProgram(shader_2dbloomblur.programID);
            shader_2dbloomblur.setInt("image", 0);
            glActiveTexture(GL_TEXTURE0);
            // Render to Bloom2 using Bloom1 first, then to Bloom1 using Bloom2, swapping repeatedly
            for (int i = 0; i < amount*2; i++) {
                Engine::Framebuffer drawFb, readFb;
                if (!horizontal) {
                    drawFb = Engine::Bloom1;
                    readFb = Engine::Bloom2;
                } else {
                    drawFb = Engine::Bloom2;
                    readFb = Engine::Bloom1;
                }
                glBindFramebuffer(GL_FRAMEBUFFER, drawFb.fbo);
                shader_2dbloomblur.setInt("horizontal", horizontal);
                glBindTexture(GL_TEXTURE_2D, readFb.colors[1]);   // Use raw HDR scene highlights first, then ping pong
                Engine::RenderQuad();
                horizontal = !horizontal;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.front());
            framebuffers.pop();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(shader_2dbloom.programID);
                shader_2dbloom.setInt("sceneRaw", 0);
                shader_2dbloom.setInt("sceneBloom", 1);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Engine::Bloom1.colors[0]);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, Engine::Bloom1.colors[1]);
                Engine::RenderQuad();
                glBindTexture(GL_TEXTURE_2D, 0);
        } 
        if (Interface::Hdr) {
            glViewport(0, 0, Engine::ScreenWidth, Engine::ScreenHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.front());
            framebuffers.pop();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(shader_2dhdr.programID);
                shader_2dhdr.setFloat("exposure", Interface::HdrExposure);
                shader_2dhdr.setInt("hdrBuffer", 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Engine::Hdr.colors[0]);
                Engine::RenderQuad();
                glBindTexture(GL_TEXTURE_2D, 0);
        }
        if (Interface::Fxaa) {
            glViewport(0, 0, Engine::ScreenWidth, Engine::ScreenHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.front());
            framebuffers.pop();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(shader_2dfxaa.programID);
                shader_2dfxaa.setInt("screenTexture", 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Engine::Fxaa.colors[0]);
                Engine::RenderQuad();
                glBindTexture(GL_TEXTURE_2D, 0);
        }

        if (!Interface::Hdr)
            glDisable(GL_FRAMEBUFFER_SRGB);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (!Interface::Hdr)
            glEnable(GL_FRAMEBUFFER_SRGB);

        // Essential
        glfwSwapBuffers(window);
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

    // Camera translation
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        scene.SceneCamera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        scene.SceneCamera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        scene.SceneCamera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        scene.SceneCamera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        scene.SceneCamera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        scene.SceneCamera.ProcessKeyboard(DOWN, deltaTime);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    Engine::SetScreenDimensions(width, height);
    Engine::InitGbuffer();
    Engine::InitSsao();
    Engine::InitHdr();
    Engine::InitBloom();
    Engine::InitFxaa();
}
void window_size_callback(GLFWwindow *window, int width, int height) {
    glfwSetWindowSize(window, width, height);
}

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

    scene.SceneCamera.ProcessMouse(xoffset, yoffset);
    
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