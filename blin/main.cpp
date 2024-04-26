
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include"glm/gtc/type_ptr.hpp"
#include"GL_camera.h"
#include"GL_Model.h"
#include"GL_Shader.h"
#include<algorithm>
#include<vector>
#include<random>
#include<array>
#include<cmath>
#include <chrono>
#include <thread>
#include<fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"
#include<string>


using namespace std;



vector<array<double,3>> getCameraPositions(int count, double radius);
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT =1024;
// camera
GL_camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool blinn = true;

int main(int argc,char* argv[])
{
    int targetFrame=1000;

    string ref_model_path,opt_model_path,save_path;
    

    if(argc==3){
        ref_model_path=argv[1];
        opt_model_path=argv[2];
    }else if(argc==4){
        ref_model_path=argv[1];
        opt_model_path=argv[2];
        save_path = argv[3];
    }

    cout<<"[Info]: ref model path:"<<ref_model_path<<endl;
    cout<<"[Info]: opt model path:"<<opt_model_path<<endl;
    cout<<"[Info]: opt model path:"<<save_path<<endl;
    cout<<"[Info]: sample counts:"<<targetFrame<<endl;
    
    // glfw: initialize and configure
    // ------------------------------`
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // glfw window creation
    // --------------------

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwHideWindow(window);
    glfwMakeContextCurrent(window);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);


    GL_shader ourShader("./normal_shader.vs", "./normal_shader.fs");
    // load models
    GL_Model refmodel(ref_model_path.c_str());
    GL_Model optmodel(opt_model_path.c_str());

    std::cout << "depth shader" << std::endl;
    GL_shader depshader("./depth.vs", "./depth.fs");

    std::cout << "four shader" << std::endl;
    GL_shader fourshader("./fourshader.vs", "./fourshader.fs");

    float near_plane = 0.1f;
    float far_plane = refmodel.getDiagnalLength() *15;

    int radius=refmodel.getDiagnalLength()*2;

    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    //������Ȳ���
    glEnable(GL_DEPTH_TEST);
    //�������޳�
    glEnable(GL_CULL_FACE);
    //����MSAA
    glEnable(GL_MULTISAMPLE);

    //��ʼ��Ļ��ɫ
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //��ճ�ʼ��ɫ�������Ȼ���
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    // don't forget to enable shader before setting uniforms
    ourShader.use();
    //light info 
    glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, near_plane, far_plane);
    glm::mat4 view = camera.GetViewMatrix();
    ourShader.setMat4f("projection", projection);
    ourShader.setMat4f("view", view);
    ourShader.setVec3("viewPos", camera.camera_position);
    //ourShader.setVec3("lightPos", camera.camera_position);
    ourShader.setBool("Blinn", blinn);

    depshader.setMat4f("projection", projection);
    depshader.setMat4f("view", view);
    depshader.setVec3("viewPos", camera.camera_position);

    fourshader.setInt("screenTexture1", 0);
    fourshader.setInt("screenTexture2", 1);
    fourshader.setInt("colorRamp", 2);
    fourshader.setInt("depth", 3);

    // FrameBUffer
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    //�����Դ棬�������ݵ��Դ�
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    //����ref֡����
    unsigned int framebuffer1;
    glGenFramebuffers(1, &framebuffer1);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer1);
    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH,SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    //���������ӵ�֡����
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    // ���������������������ģʽ�ͱ߽���Ϊ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // ���������洢�ռ䣬��ָ��Ϊ��������ʽ
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SCR_WIDTH,SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindRenderbuffer(GL_RENDERBUFFER, 0);


    //����opt֡����
    unsigned int framebuffer2;
    glGenFramebuffers(1, &framebuffer2);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);
    unsigned int texColorBuffer2;
    glGenTextures(1, &texColorBuffer2);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH,SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    //���������ӵ�֡����
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer2, 0);
    //��Ⱦ������󸽼�
    GLuint depthTexture2;
    glGenTextures(1, &depthTexture2);
    glBindTexture(GL_TEXTURE_2D, depthTexture2);
    // ���������������������ģʽ�ͱ߽���Ϊ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // ���������洢�ռ䣬��ָ��Ϊ��������ʽ
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SCR_WIDTH,SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture2, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindRenderbuffer(GL_RENDERBUFFER, 0);


    //����depth֡����
    unsigned int framebuffer3;
    glGenFramebuffers(1, &framebuffer3);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer3);
    unsigned int texColorBuffer3;
    glGenTextures(1, &texColorBuffer3);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer3);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH,SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    //���������ӵ�֡����
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer3, 0);
    //��Ⱦ������󸽼�
    unsigned int rbo3;
    glGenRenderbuffers(1, &rbo3);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo3);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH,SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    //����Ⱦ������󸽼ӵ�֡�������Ⱥ�ģ�帽����
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo3);


    unsigned int framebuffer4;
    glGenFramebuffers(1, &framebuffer4);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer4);
    unsigned int texColorBuffer4;
    glGenTextures(1, &texColorBuffer4);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH,SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer4, 0);
    unsigned int rbo4;
    glGenRenderbuffers(1, &rbo4);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo4);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH,SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo4);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glActiveTexture(GL_TEXTURE1);


    GLubyte* pixels1 = new GLubyte[SCR_WIDTH * SCR_HEIGHT * 4];
    GLubyte* pixels2 = new GLubyte[SCR_WIDTH * SCR_HEIGHT * 4];
    GLubyte* pixels3 = new GLubyte[SCR_WIDTH * SCR_HEIGHT * 4];
    GLubyte* pixels4= new GLubyte[SCR_WIDTH * SCR_HEIGHT * 4];

    // render loop
    // -----------
    glDisable(GL_CULL_FACE);
    auto samples=getCameraPositions(targetFrame,radius);
    std::cout << "render loop" << std::endl;
    int frame = 0;
    double total_mse=0.0;
    vector<double> rmse_vec(targetFrame,0);
    while (frame < targetFrame) {

        if(frame%100==0)
            std::cout << "Frame:"<< frame << std::endl;
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        auto cameraPosition=glm::vec3(samples[frame][0],samples[frame][1],samples[frame][2]);

        //set MVP matrix
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, near_plane, far_plane);
        camera.set_CameraPos(cameraPosition+refmodel.center);
        camera.setCameraVectors(refmodel.center-camera.camera_position);
        glm::mat4 view = camera.GetViewMatrix();
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

        //depth
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer3);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_TRUE); // ��������Ȼ�����д��
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        depshader.use();
        depshader.setMat4f("projection", projection);
        depshader.setMat4f("view", view);
        depshader.setMat4f("model", model);
        refmodel.Draw(depshader);
        
        glReadPixels(0, 0,SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels1);
          // 假设已经读取了像素到pixels1
        stbi_flip_vertically_on_write(1); // OpenGL的坐标系与许多图像文件相反，所以这里需要垂直翻转
        if(frame==targetFrame-1){
            string save_name=save_path+"/blin_output1.png";
            bool success = stbi_write_png(save_name.c_str(), SCR_WIDTH, SCR_HEIGHT, 4, pixels1, SCR_WIDTH * 4);
            if (!success) {
                std::cerr << "Failed to write PNG file!" << std::endl;
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer4);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClearDepth(1.0);
        glDisable(GL_CULL_FACE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE); // ��������Ȼ�����д��
        depshader.use();
        depshader.setMat4f("projection", projection);
        depshader.setMat4f("view", view);
        depshader.setMat4f("model", model);
        optmodel.Draw(depshader);


        glReadPixels(0, 0,SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels2);
        if(frame==targetFrame-1){
            string save_name=save_path+"/blin_output2.png";
            bool success = stbi_write_png(save_name.c_str(), SCR_WIDTH, SCR_HEIGHT, 4, pixels2, SCR_WIDTH * 4);
            if (!success) {
                std::cerr << "Failed to write PNG file!" << std::endl;
            }
        }

        
        //color
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer1);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_TRUE); // ��������Ȼ�����д��
        ourShader.use();
        ourShader.setMat4f("projection", projection);
        ourShader.setMat4f("view", view);
        ourShader.setVec3("light.position", camera.camera_position);
        ourShader.setVec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
        ourShader.setVec3("light.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        ourShader.setVec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        ourShader.setMat4f("model", model);
        refmodel.Draw(ourShader);

        glReadPixels(0, 0,SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels3);
        if(frame==targetFrame-1){
            string save_name=save_path+"/blin_output3.png";
            bool success = stbi_write_png(save_name.c_str(), SCR_WIDTH, SCR_HEIGHT, 4, pixels3, SCR_WIDTH * 4);
            if (!success) {
                std::cerr << "Failed to write PNG file!" << std::endl;
            }
        }


        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_TRUE); 
        ourShader.use();
        optmodel.Draw(ourShader);
   
        glReadPixels(0, 0,SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels4);
        if(frame==targetFrame-1){
            string save_name=save_path+"/blin_output4.png";
            bool success = stbi_write_png(save_name.c_str(), SCR_WIDTH, SCR_HEIGHT, 4, pixels4, SCR_WIDTH * 4);
            if (!success) {
                std::cerr << "Failed to write PNG file!" << std::endl;
            }
        }



        // glBindFramebuffer(GL_FRAMEBUFFER, 0); // ����Ĭ�ϻ�����
        // fourshader.use();
        
        // glClearColor(1.0f, 1.0f,1.0f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        // glBindVertexArray(quadVAO);
        // glDisable(GL_DEPTH_TEST);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, texColorBuffer);	// use the color attachment texture as the texture of the quad plane
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, texColorBuffer2);
        // glActiveTexture(GL_TEXTURE2);
        // glBindTexture(GL_TEXTURE_2D, texColorBuffer3);
        // glActiveTexture(GL_TEXTURE3);
        // glBindTexture(GL_TEXTURE_2D, texColorBuffer4);
        // fourshader.setInt("screenTexture1", 0);
        // fourshader.setInt("screenTexture2", 1);
        // fourshader.setInt("depth1", 2);
        // fourshader.setInt("depth2", 3);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // glFinish();

        double totalDiffSquared = 0.0;
        float count_pixel=0;
        
        for (int i = 0; i < SCR_WIDTH*SCR_HEIGHT; ++i) {
            // 假设RGBA都是无符号字节
            double r1 = static_cast<double>(pixels1[i * 4 + 0]);
            double g1= static_cast<double>(pixels1[i * 4 + 1]);
            double b1 =static_cast<double>(pixels1[i * 4 + 2]);
            
            double r2 = static_cast<double>(pixels2[i * 4 + 0]);
            double g2=  static_cast<double>(pixels2[i * 4 + 1]);
            double b2 =  static_cast<double>(pixels2[i * 4 + 2]);

            double r3 = static_cast<double>(pixels3[i * 4 + 0]);
            double g3= static_cast<double>(pixels3[i * 4 + 1]);
            double b3 =  static_cast<double>(pixels3[i * 4 + 2]);
            
            double r4 = static_cast<double>(pixels4[i * 4 + 0]);
            double g4=  static_cast<double>(pixels4[i * 4 + 1]);
            double b4= static_cast<double>(pixels4[i * 4 + 2]);
            
            double r_diff=r3-r4;
            double g_diff=g3-g4;
            double b_diff=b3-b4;

            if(r1<200||r2<200){
                totalDiffSquared += (r_diff * r_diff) + (g_diff * g_diff) + (b_diff * b_diff);
                count_pixel=count_pixel+1;
            }
        }
        double mse = totalDiffSquared / count_pixel;
        double rmse = std::sqrt(mse);
        rmse_vec[frame]=rmse;
        total_mse+=rmse;
        glfwSwapBuffers(window);
        glfwPollEvents();
        //sleep 2 second
        // std::this_thread::sleep_for(std::chrono::seconds(2));
        frame++;
    }
    delete[] pixels1;
    delete[] pixels2;
    delete[] pixels3;
    delete[] pixels4;
    cout<<"total rmse"<<total_mse/float(targetFrame)*4.0*M_PI<<endl;
    
    ofstream file(save_path+"/blin_output.txt");
    if (!file) { // 检查是否成功打开文件
        std::cerr << "file failed!" << std::endl;
        return 1;
    }
    file<<total_mse/float(targetFrame)*4.0*M_PI<<endl;
    for(auto data:rmse_vec){
        file<<data<<endl;
    }

    file.close();

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    // ESC���������˳�����
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // �������
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

vector<array<double,3>> getCameraPositions(int count, double radius){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist_phi(0, 2 * M_PI);
    std::uniform_real_distribution<> dist_theta(0, M_PI);
    vector<array<double,3>> ans;
    for (int i = 0; i < count; ++i) {
        double phi = dist_phi(gen);
        double theta = dist_theta(gen);
        double x = radius * sin(theta) * cos(phi);
        double y = abs(radius * sin(theta) * sin(phi));
        double z = radius * cos(theta);
        ans.push_back({x,y,z});

    }
    return ans;
}


