#include "../include/lvis.hpp"

void Lvis::testInit() {
    inputPaths.push_back(new LightPath());
    for (int i=0;i<5;i++) {
        inputPaths[0]->pushVertex(new PathVertex());
    }
    singlePath=new LightPath();
    previousPath=new LightPath();
    for (int i=0;i<5;i++) {
        PathVertex* pv=new PathVertex();
        PathVertex* pv2;
        if (i==2) pv2=new PathVertex(); else pv2=new PathVertex(pv->getPos());
        singlePath->pushVertex(pv);
        previousPath->pushVertex(pv2);
    }
    singlePath->finalize();
    previousPath->finalize();
    inputPaths[0]->finalize();
}

void Lvis::run() {
	init();
	flagRunning=true;
    lvisThread=std::thread(&Lvis::threadFunc,this);
}

void Lvis::finish() {
	flagRunning=false;
}

void Lvis::join() {
	lvisThread.join();
}

void Lvis::wait() {
    {
        std::unique_lock<std::mutex> lock(mut);
        flagWaiting=true;
        cv.wait(lock, [=]{ return ((!flagWaiting)||(!flagRunning)); });
    }
}

void Lvis::pushSinglePath(LightPath* _path) {
    {
        std::unique_lock<std::mutex> lock(mut);
        if (_path==NULL) return;
        inputPaths.clear();
        delete previousPath;
        previousPath=singlePath;
        singlePath=_path;
        std::cout << singlePath->getInfo() << std::endl << std::flush;
    }
}

void Lvis::pushPath(LightPath* _path) {
	while ((inputPaths.size()>limit)&&(isRunning())) {
        std::cout << "All required paths drawn, waiting..." << std::endl << std::flush;
        wait();
        if (flagSingle) {
            pushSinglePath(_path); //in case of redirect during waiting
            return;
        }
    }
    {
        std::unique_lock<std::mutex> lock(mut);
        if (_path==NULL) return;
        inputPaths.push_back(_path);
    }
}

void Lvis::clearSingle() {
    delete previousPath;
    delete singlePath;
    previousPath=NULL;
    singlePath=NULL;
}

void Lvis::clearDrawnPaths() {
    for (std::map<GLuint,LightPath*>::iterator it=drawnPaths.begin();it!=drawnPaths.end();it++) {
        delete it->second;
    }
    for (std::vector<LightPath*>::iterator it=inputPaths.begin();it != inputPaths.end();it++) {
        delete *it;
    }
    activePath=NULL;
    selectedPath=NULL;
    drawnPaths.clear();
    inputPaths.clear();
}

void Lvis::clearAllSpheres() {
    for (std::map<GLuint,SphereSelect*>::iterator it=allSpheres.begin();it!=allSpheres.end();it++) {
        delete it->second;
    }
    activeSphere=NULL;
    allSpheres.clear();    
}

void Lvis::setLimit(GLuint _limit) {
	limit=_limit;
}

void Lvis::setDrawLimit(GLuint _limit) {
    drawLimit=_limit;
}

void Lvis::setThreshold(GLuint _thresh) {
    threshold=_thresh;
}

LightPath* Lvis::getSelected() {
    {
        std::unique_lock<std::mutex> lock(mut);
        flagSelected=false;
        return selectedPath;
    }
}

void Lvis::setupVirtualCamera(glm::vec3 from, glm::vec3 to, glm::vec3 up) {
    virtCam->lookAt(from,to,up);
    showCamera=true;
}

void Lvis::setupLight(glm::vec3 lpos) {
    artificalLight=lpos;
}

bool Lvis::isRunning() {
    return flagRunning;
}

//*******************************************
//********** PRIVATE FUNCTIONS **************
//*******************************************

void Lvis::threadFunc() {
	while (flagRunning) {
        {
            std::unique_lock<std::mutex> lock(mut);
            if (inputPaths.size()>=threshold) {
                if ((!flagWaiting)||(drawnPaths.size()<drawLimit)) processPaths();
            }
            handleEvents(window);
            draw();    
            window->display();
        }
        cv.notify_one(); //if the second threard is waiting for mutex, this is the time to take it
    }
}

void Lvis::counterUp() {
    counter++;
    if (counter>MAXCOUNT) {
        counter=1;
        countOverflow=true;
    }
    if (countOverflow) {
        while ((allSpheres.find(counter)!=allSpheres.end())&&(drawnPaths.find(counter)!=drawnPaths.end())) {
        counter++; //limits of sizes ensure we'll find one
        std::cout << "Counter " << counter << std::endl << std::flush;
        }
    }
}

void Lvis::initShaders(GLuint &program,const char* vsf,const char* fsf) {
    std::ifstream vfs(vsf);
    std::ifstream ffs(fsf);
    std::string vs,fs;
    if (!vfs || !ffs ) {
        printf("Shader files not found, exiting...\n");
        exit(1);    
    }
    vs.assign((std::istreambuf_iterator<char> (vfs) ), std::istreambuf_iterator<char>());
    fs.assign((std::istreambuf_iterator<char> (ffs) ), std::istreambuf_iterator<char>());
    const GLchar* vch = vs.c_str() + '\0';
    const GLchar* fch = fs.c_str() + '\0';

    char buffer[512];
    GLint status;
    GLuint vShader=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader,1,(const GLchar**) &vch,NULL);
    glCompileShader(vShader);
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &status);
    glGetShaderInfoLog(vShader, 512, NULL, buffer);
    std::cout << buffer;
    if (status!=GL_TRUE) {
        printf("Vertex shader failed to compile, exiting...\n");
        exit(1); 
    }

    GLuint fShader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader,1,(const GLchar**) &fch,NULL);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &status);
    glGetShaderInfoLog(fShader, 512, NULL, buffer);
    std::cout << buffer;
    if (status!=GL_TRUE) {
        printf("Fragment shader failed to compile, exiting...\n");
        exit(1); 
    }

    program=glCreateProgram();
    glAttachShader(program,vShader);
    glAttachShader(program,fShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    glGetProgramInfoLog(program, 512, NULL, buffer);
    std::cout << buffer;
    if (status!=GL_TRUE) {
        printf("Shader linking unsuccessfull, exiting...\n");
        exit(1); 
    }
}

void Lvis::init() {
    //SFML
    window = new sf::Window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
    window->setVerticalSyncEnabled(true);

    //GLEW & OpenGL
    glewExperimental= GL_TRUE; //allow for modern ogl stuff
    glewInit();
    if (glewIsSupported("GL_VERSION_3_0")) printf("OpenGL 3.0 supported\n");
    else {
        printf("OpenGL 3.0 not supported, shamefully exiting...\n");
        exit(1);
    }
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0,0.0,0.0,0.0);
    
    //Objects
    scene = new meshLoader(scene_file,scene_textures_folder);
    sphere = new meshLoader(sphere_file);
    arrow = new meshLoader(arrow_file);
    camModel = new meshLoader(camera_file,camera_textures_folder);
    virtCam=new VirtualCamera(camModel);
    cam = new Camera();

    //Shaders
    initShaders(basicProgram, basic_vertex_shader_file, basic_fragment_shader_file);
    initShaders(noTextProgram, basic_vertex_shader_file, notext_fragment_shader_file);
    initShaders(textOnlyProgram, basic_vertex_shader_file, textonly_fragment_shader_file);
    initShaders(lineProgram, line_vertex_shader_file, line_fragment_shader_file);
    glGenVertexArrays(1,&basicVao); //also used for notext and textonly, since they have same data layout
    glGenVertexArrays(1,&lineVao);
    
    glUseProgram(lineProgram);
    glBindVertexArray(lineVao);
    uniLMVP = glGetUniformLocation(lineProgram, "uniMVP");
    uniLcolor= glGetUniformLocation(lineProgram, "uniColor");
    positionAttrib = glGetAttribLocation(lineProgram,"vertex");
    
    glUseProgram(basicProgram);
    glBindVertexArray(basicVao);
    uniBMVP = glGetUniformLocation(basicProgram, "uniMVP");
}

bool Lvis::filterPath(LightPath* path) {
    std::vector<PathVertex*>* vertices=path->getVertices();
    bool startExists=false,start=false,endExists=false,end=false,midExists=false,mid=false,optionalExists=false,optional=false,midError=false;
    for (std::map<GLuint,SphereSelect*>::iterator it=allSpheres.begin();it!=allSpheres.end();it++) {
        switch (it->second->getType()) {
            case 0:
                midExists=true;
                mid=false;
                break;
            case 1:
                startExists=true;
                if (it->second->pointInside(vertices->at(0)->getPos())) start=true;
                break;
            case 2:
                endExists=true;
                if (it->second->pointInside(vertices->at(vertices->size()-1)->getPos())) end=true;
                break;
            case 3:
                optionalExists=true;
                break;
            default:
                break;
        }
        if ((it->second->getType()==1)||(it->second->getType()==2)) continue;
        for (int i=0;i<vertices->size();i++) {
            if ((it->second->getType()==0)&&(it->second->pointInside(vertices->at(i)->getPos()))) {
                mid=true;
            } 
            if ((it->second->getType()==3)&&(it->second->pointInside(vertices->at(i)->getPos()))) {
                optional=true;
            }
        }
        if (midExists!=mid) break;
    }
    if ((startExists==start)&&(endExists==end)&&(optionalExists==optional)&&(midExists==mid)) return true;
    return false;
}

void Lvis::processPaths() {
    if (inputPaths.empty()) return;
    GLint size=0;
    for (std::vector<LightPath*>::iterator it=inputPaths.begin();it != inputPaths.end();) {
        if (drawnPaths.size()>=drawLimit) break;
        if (filterPath(*it)) {
            (*it)->setIndex(counter);
            (*it)->finalize();
            drawnPaths.emplace(counter,*it);
            counterUp();
            it = inputPaths.erase(it);
        } else {
            it++;
            size++;
        }
    }
    //erase the rest of LightPaths in input
    for (int i=0;i<size;i++) {
        delete inputPaths[i];
    }
    inputPaths.clear();
}

void Lvis::filterDrawn() {
    for (std::map<GLuint,LightPath*>::iterator it=drawnPaths.begin();it!=drawnPaths.end();) {
        if (!filterPath(it->second)) {
            if (it->second->getIndex()==activePath->getIndex()) activePath=NULL;
            delete it->second;
            it = drawnPaths.erase(it);
        } else it++;
    }
}

void Lvis::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    switch (shaderType) {
        case 0:
            glUseProgram(basicProgram);
            break;        
        case 1:
            glUseProgram(textOnlyProgram);
            break;        
        case 2:
            glUseProgram(noTextProgram);
            break;        
    }
    glBindVertexArray(basicVao); 
    glUniformMatrix4fv(uniBMVP, 1, GL_FALSE, glm::value_ptr(cam->getVP()));
    GLint uniCamPos= glGetUniformLocation(basicProgram, "cameraPosition");
    glUniform3fv(uniCamPos, 1, glm::value_ptr(cam->getPos()));
    GLint uniLightPos= glGetUniformLocation(basicProgram, "lightPos");
    glUniform3fv(uniLightPos, 1, glm::value_ptr(artificalLight));
    scene->draw(basicProgram,false);
    if ((virtCam!=NULL)&&(showCamera)) {
        virtCam->draw(basicProgram,cam->getVP());
    }
    glUseProgram(lineProgram);
    glBindVertexArray(lineVao);
    if (singlePath!=NULL) {
        singlePath->drawDiff(previousPath,lineProgram,cam->getVP());
        //previousPath->draw(lineProgram,cam->getVP());
    }
    for (std::map<GLuint,LightPath*>::iterator it=drawnPaths.begin();it!=drawnPaths.end();it++) {
        if ((activePath!=NULL)&&(activePath->getIndex()==it->second->getIndex())) {
            //it->second->draw(lineProgram,cam->getVP(),true,true);
            //do nothing, draw path later to override any non-active
        } else {
            it->second->draw(lineProgram,cam->getVP(),true,false);
        }
    }
    if (activePath!=NULL) activePath->draw(lineProgram,cam->getVP(),true,true);
    for (std::map<GLuint,SphereSelect*>::iterator it=allSpheres.begin();it!=allSpheres.end();it++) {
        it->second->draw(lineProgram,cam->getVP(),true);
    }
    if (activeSphere!=NULL) {
        activeSphere->drawAxes(lineProgram,cam->getVP(),scaleSwitch,cam->getPos());
    }
}

GLint Lvis::pickAxes(GLint x,GLint y) {
    if (activeSphere==NULL) return(0);
    GLubyte tmp[3] = {0,0,0}; 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(lineProgram);
    glBindVertexArray(lineVao);
    activeSphere->drawAxes(lineProgram,cam->getVP(),scaleSwitch,cam->getPos());
    glReadPixels(x, window_h-y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &tmp[0]);
    if (tmp[0]) return (scaleSwitch) ? 4:1;//{ (scaleSwitch) ? return 4; : return 1;}
    if (tmp[1]) return 2;
    if (tmp[2]) return 3;
    return 0;
}

void Lvis::pickSpheres(GLint x,GLint y) {
    GLubyte tmp[3] = {0,0,0};
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(lineProgram);
    glBindVertexArray(lineVao);
    for (std::map<GLuint,SphereSelect*>::iterator it=allSpheres.begin();it!=allSpheres.end();it++) {
        it->second->draw(lineProgram,cam->getVP(),false);
    }
    glReadPixels(x, window_h-y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &tmp[0]);
    GLuint id=SphereSelect::getIndexByColor((GLuint)tmp[0],(GLuint)tmp[1],(GLuint)tmp[2]);
    std::map<GLuint,SphereSelect*>::iterator it=allSpheres.find(id);
    if (it!=allSpheres.end()) {
        activePath=NULL;
        activeSphere=allSpheres[id];
    }
}

void Lvis::pickPaths(GLint x,GLint y) {
    GLubyte tmp[3] = {0,0,0};
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(lineProgram);
    glBindVertexArray(lineVao);
    for (std::map<GLuint,LightPath*>::iterator it=drawnPaths.begin();it!=drawnPaths.end();it++) {
        it->second->draw(lineProgram,cam->getVP(),false,false);
    }
    glReadPixels(x, window_h-y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &tmp[0]);
    GLuint id=LightPath::getIndexByColor((GLuint)tmp[0],(GLuint)tmp[1],(GLuint)tmp[2]);
    std::map<GLuint,LightPath*>::iterator it=drawnPaths.find(id);
    if (it!=drawnPaths.end()) {
        activeSphere=NULL;
        activePath=drawnPaths[id];
        std::string ts=activePath->getInfo();
        std::cout << ts << std::endl << std::flush;
    }
}

void Lvis::selectPath() {
    if (activePath==NULL) return;
    if (selectedPath!=NULL) drawnPaths.emplace(selectedPath->getIndex(),selectedPath);
    selectedPath=activePath;
    flagSelected=true;
}

void Lvis::handleEvents(sf::Window* win) {
    sf::Event event;
    while (win->pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            flagRunning = false;
            break;
        case sf::Event::Resized:
            glViewport(0, 0, event.size.width, event.size.height);
            cam->setProjection(event.size.width,event.size.height);
            window_w=event.size.width;
            window_h=event.size.height;
            break;
        case sf::Event::GainedFocus:
            focus=true;
            break;
        case sf::Event::LostFocus:
            focus=false;
            break;
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Right) {
                mouseDownX=event.mouseButton.x;
                mouseDownY=event.mouseButton.y;
                cam->memorizeSpherical();
                mouseRotating=true;
            } else if (event.mouseButton.button == sf::Mouse::Left) {
                GLint axisId=pickAxes(event.mouseButton.x,event.mouseButton.y);
                glm::mat4 tempMat = cam->getVP();
                glm::vec4 tmp;
                switch(axisId) {
                    case 1:
                        mouseDownX=event.mouseButton.x;
                        mouseDownY=event.mouseButton.y;
                        dragX=true;
                        tmp=cam->getVP()*glm::vec4(1.0f,0.0f,0.0f,0.0f);
                        dragModX=-1*sgn(tmp.x);
                        dragModY=sgn(tmp.y);
                        break;
                    case 2:
                        mouseDownX=event.mouseButton.x;
                        mouseDownY=event.mouseButton.y;
                        dragY=true;
                        tmp=cam->getVP()*glm::vec4(0.0f,1.0f,0.0f,0.0f);
                        dragModX=-1*sgn(tmp.x);
                        dragModY=sgn(tmp.y);
                        break;
                    case 3:
                        mouseDownX=event.mouseButton.x;
                        mouseDownY=event.mouseButton.y;
                        dragZ=true;
                        tmp=cam->getVP()*glm::vec4(0.0f,0.0f,1.0f,0.0f);
                        dragModX=-1*sgn(tmp.x);
                        dragModY=sgn(tmp.y);
                        break;
                    case 4:
                        mouseDownX=event.mouseButton.x;
                        mouseDownY=event.mouseButton.y;
                        scaleXYZ=true;
                        tmp=cam->getVP()*glm::vec4(0.0f,1.0f,0.0f,0.0f);
                        dragModX=-1*sgn(tmp.x);
                        dragModY=sgn(tmp.y);
                    default:
                        pickSpheres(event.mouseButton.x,event.mouseButton.y);
                        pickPaths(event.mouseButton.x,event.mouseButton.y); //paths will override selected spheres
                        break;
                }
            }
            break;
        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Right) {
                mouseRotating=false;
            } else {
                dragX=false;
                dragY=false;
                dragZ=false;
                scaleXYZ=false;
                if (activeSphere!=NULL) {
                    activeSphere->finishTranslate();
                    activeSphere->finishScaling();
                }
            }
            break;
        case sf::Event::MouseWheelMoved:
            if (!cam->isFree()) cam->setDeltaDistance((GLfloat)(-1*event.mouseWheel.delta));
            break;
        case sf::Event::MouseMoved:
            if (mouseRotating) {
                cam->setMemSpherical((mouseDownY-event.mouseMove.y)/500.0f,(mouseDownX-event.mouseMove.x)/500.0f);
            }
            if (activeSphere!=NULL) {
                if (dragX) {
                    activeSphere->translate((dragModX*(mouseDownX-event.mouseMove.x))/100.0f+(dragModY*(mouseDownY-event.mouseMove.y))/100.0f,0.0f,0.0f);
                }
                if (dragY) {
                    activeSphere->translate(0.0f,(dragModX*(mouseDownX-event.mouseMove.x))/100.0f+(dragModY*(mouseDownY-event.mouseMove.y))/100.0f,0.0f);
                }
                if (dragZ) {
                    activeSphere->translate(0.0f,0.0f,(dragModX*(mouseDownX-event.mouseMove.x))/100.0f+(dragModY*(mouseDownY-event.mouseMove.y))/100.0f);
                }
                if (scaleXYZ) {
                    activeSphere->scaling((dragModX*(mouseDownX-event.mouseMove.x))/100.0f+(dragModY*(mouseDownY-event.mouseMove.y))/100.0f);
                }
            }
            break;
        case sf::Event::KeyPressed:
            switch (event.key.code) {
                case sf::Keyboard::F:
                    cam->switchFree();
                    break;
                case sf::Keyboard::Z:
                    if (allSpheres.size()>=47) {
                        std::cout << "Limit on allowed number of selectors reached." << std::endl << std::flush;
                        break;
                    }
                    allSpheres.emplace(counter,new SphereSelect(counter,1,sphere,arrow,virtCam->getPos()));
                    counterUp();
                    break;
                case sf::Keyboard::X:
                    if (allSpheres.size()>=47) {
                        std::cout << "Limit on allowed number of selectors reached." << std::endl << std::flush;
                        break;
                    }
                    allSpheres.emplace(counter,new SphereSelect(counter,2,sphere,arrow,virtCam->getPos()));
                    counterUp();
                    break;
                case sf::Keyboard::C:
                    if (allSpheres.size()>=47) {
                        std::cout << "Limit on allowed number of selectors reached." << std::endl << std::flush;
                        break;
                    }
                    allSpheres.emplace(counter,new SphereSelect(counter,0,sphere,arrow,virtCam->getPos()));
                    counterUp();
                    break;
                case sf::Keyboard::V:
                    if (allSpheres.size()>=47) {
                        std::cout << "Limit on allowed number of selectors reached." << std::endl << std::flush;
                        break;
                    }
                    allSpheres.emplace(counter,new SphereSelect(counter,3,sphere,arrow,virtCam->getPos()));
                    counterUp();
                    break;
                case sf::Keyboard::A:
                    scaleSwitch=!scaleSwitch;
                    break;
                case sf::Keyboard::Delete:
                    if (activeSphere!=NULL) {
                        allSpheres.erase(activeSphere->getIndex());
                        delete activeSphere;
                        activeSphere=NULL;
                    }
                    break;
                case sf::Keyboard::Home:
                    clearAllSpheres();
                    break;
                case sf::Keyboard::BackSpace:
                    clearDrawnPaths();
                    clearSingle();
                    break;
                case sf::Keyboard::Return:
                    if (activePath!=NULL) {
                        selectedPath=activePath;
                        std::cout << "Path selected." << std::endl << std::flush;
                    }
                    break;
                case sf::Keyboard::F12:
                    shaderType=(shaderType+1)%3;
                    std::cout << "Shader changed." << std::endl << std::flush;
                    break;
                case sf::Keyboard::F11:
                    filterDrawn();
                    break;
                case sf::Keyboard::F10:
                    flagWaiting=!flagWaiting;
                    std::cout << "Waiting flag: " << flagWaiting << std::endl << std::flush;
                    break;
                case sf::Keyboard::F9:
                    flagPause=!flagPause;
                    std::cout << "Pause flag: " << flagPause << std::endl << std::flush;
                    break;
                case sf::Keyboard::F8:
                    flagSingle=!flagSingle;
                    std::cout << "Single path flag: " << flagSingle << std::endl << std::flush;
                    break;
                case sf::Keyboard::F2:
                    limit+=10;
                    std::cout << "Input buffer limit set to: " << limit <<std::endl << std::flush;
                    break;
                case sf::Keyboard::F1:
                    if (limit>10) limit-=10;
                    std::cout << "Input buffer limit set to: " << limit <<std::endl << std::flush;
                    break;
                case sf::Keyboard::F4:
                    if (drawLimit<MAXCOUNT-47) drawLimit+=10;
                    std::cout << "Limit of drawn paths set to: " << drawLimit <<std::endl << std::flush;
                    break;
                case sf::Keyboard::F3:
                    if (drawLimit>10) drawLimit-=10;
                    std::cout << "Limit of drawn paths set to: " << drawLimit <<std::endl << std::flush;
                    break;
                case sf::Keyboard::F6:
                    threshold+=10;
                    std::cout << "Threshold set to: " << threshold <<std::endl << std::flush;
                    break;
                case sf::Keyboard::F5:
                    if (threshold>10) threshold-=10; else threshold=1;
                    std::cout << "Threshold set to: " << threshold <<std::endl << std::flush;
                    break;
                default:
                    break;
            }
        default:
            break;
        }
    }
    //realtime keyboard feedback
    if (focus) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            cam->move(glm::vec3(0.0f,0.0f,-0.2f));
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            cam->move(glm::vec3(0.0f,0.0f,0.2f));
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            cam->move(glm::vec3(0.2f,0.0f,0.0f));
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            cam->move(glm::vec3(-0.2f,0.0f,0.0f));
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            cam->move(glm::vec3(0.0f,0.2f,0.0f));
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            cam->move(glm::vec3(0.0f,-0.2f,0.0f));
        }
    }
} 