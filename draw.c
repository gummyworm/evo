#include "draw.h"


/************************* Rendering variables *******************************/
/* Matrices for the shaders */
static Mat4x4 ModelMat;
static Mat4x4 ProjectionMat;
static Mat4x4 ViewMat;
/* ID's for the matrix uniforms */
static GLuint ModelMatID;
static GLuint ProjectionMatID;
static GLuint ViewMatID;

/* Matrices for GUI */
static Mat4x4 GUIModelMat;
static Mat4x4 GUIProjectionMat;
static Mat4x4 GUIViewMat;
/* ID's for the GUI matrix uniforms */
static GLuint GUIModelMatID;
static GLuint GUIProjectionMatID;
static GLuint GUIViewMatID;
 
/* Screen surface. */
static SDL_Surface *screen;

/* The current camera. */
static Camera cam;

/* The current vertex and fragment shaders. */
static Material* MaterialMain;
static Material* MaterialGUISelect;


/************************* Shader/Material tables ****************************/
/* IDs of loaded programs (materials) */
static GHashTable* materials;
/* IDs of all loaded shaders */
static GHashTable* fragShaders;
static GHashTable* vertShaders;
static GHashTable* geomShaders;
/* translation table from material names -> IDs */
static GHashTable* fragShaderNames;
static GHashTable* vertShaderNames;
static GHashTable* geomShaderNames;


/************************** Material loading *********************************/
int MaterialInit();

/* load a material using the given shader files and attributes */
Material* MaterialLoad(const char* vertFile, const char* fragFile, 
        const char* geomFile, char** attributes, int numAttributes);
/* get the material associated with the given material ID */
Material* GetMaterial(int id);
/* compile a program consisting of the given shaders and attributes */
GLuint CompileProgram(GLuint vertShader, GLuint fragShader, GLuint geomShader, 
        char **attributes, int numAttributes);
/* read the file "filename" into buffer */
int MaterialReadFile(const char* filename, char** buffer);
/* associate the given material to id in the internal material hash table */
void AddMaterial(int id, Material *mat);
/* compile the given shader of the given type */
GLuint CompileShader(const GLchar* shader, GLuint type);

/************************** helper functions *********************************/
void DrawWidgetRecursive(gpointer data, gpointer user_data);

/* The current level. */
// static Map world;


/*****************************************************************************/
/* Rendering subsystem                                                       */
/*****************************************************************************/
int DrawInit()
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Error: Unable to initialize SDL: %s", SDL_GetError());
        return -1;
    }
    puts("Initializing draw");

    /* Turn on double buffering. */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);   
    screen = SDL_SetVideoMode(640, 480, 32, SDL_OPENGL);
    
    /* Initialize OpenGL. */
    if(glewInit() == GLEW_OK) {
        int success; 

        printf("OpenGL extensions availiable\n");
        printf("Shader version %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        glClearColor(1.0, 1.0, 1.0, 0.0);
        

        /* Initialize material subsystem */
        if(MaterialInit() != 0) {
            fprintf(stderr, "Error: could not initialize the material subsytem\n");
        }

        /* compile shader programs */
        char attr1[] = "in_Position"; char attr2[] = "in_Color"; 
        char *attrs[2] = {attr1, attr2};
        char guiAttr1[] = "in_Position"; char guiAttr2[] = "in_Color"; 
        char *guiAttrs[2] = {guiAttr1, guiAttr2};
        MaterialMain = MaterialLoad("test.vert", "test.frag", "test.geom", attrs, 2);
        MaterialGUISelect = MaterialLoad("gui.vert", "gui.frag", "test.geom", guiAttrs, 2);
        puts("loaded materials successfully");

        glUseProgram(MaterialMain->program);
        /* Initialize the camera. */
        cam.pos[0] = 0.0f; cam.pos[1] = 0.0f; cam.pos[2] = -4.0f;
        cam.rot[0] = 0.0f; cam.rot[1] = 0.0f; cam.rot[2] = 0.0f;
        cam.fov = 60.0f;
        cam.aspect = 640.0f / 480.0f;
        cam.near = 0.1f;
        cam.far = 100.0f;
        
        /* Initialize matrices */
        Mat4x4LoadIdentity(ModelMat);
        Mat4x4LoadIdentity(ViewMat);
        Mat4x4LoadIdentity(ProjectionMat);
        Mat4x4PerspMat(ProjectionMat, cam.fov, cam.aspect, cam.near, cam.far);
        /* get uniforms for all model/view/projection matrices */
        ModelMatID = glGetUniformLocation(MaterialMain->program, "Model");
        ViewMatID = glGetUniformLocation(MaterialMain->program, "View");
        ProjectionMatID = glGetUniformLocation(MaterialMain->program, "Projection");

        /* Initialize GUI matrices */
        Mat4x4LoadIdentity(GUIModelMat);
        Mat4x4LoadIdentity(GUIViewMat);
        Mat4x4LoadIdentity(GUIProjectionMat);
        Mat4x4OrthoMat(GUIProjectionMat, 0.0f, (float)screen->w, (float)screen->h, 0.0f, 0.01f, 100.0f);
        /* get uniforms for GUI matrices */
        GUIModelMatID = glGetUniformLocation(MaterialGUISelect->program, "Model");
        GUIViewMatID = glGetUniformLocation(MaterialGUISelect->program, "View");
        GUIProjectionMatID = glGetUniformLocation(MaterialGUISelect->program, "Projection");
    }
    /* Initialize legacy OpenGL for older hardware. */
    else {
        printf("OpenGL extensions not supported, going old-skool\n");
        printf("But seriously, this is probably going to be a miserable failure\n");
        glEnable(GL_TEXTURE_2D);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glViewport(0, 0, 640, 480);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    /* wider lines */
    glLineWidth(3);

    /* Done. */
    puts("Draw initialization complete");
    return 0;
}

void DrawQuit()
{
        glDetachShader(MaterialMain->program, MaterialMain->vert);
        glDetachShader(MaterialMain->program, MaterialMain->frag);
        glDeleteShader(MaterialMain->vert);
        glDeleteShader(MaterialMain->frag);
}

void DrawStartFrame()
{
    /* clear GL buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* position the camera */
    Mat4x4LoadIdentity(ViewMat);
    Mat4x4Translate(ViewMat, -cam.pos[0], -cam.pos[1], cam.pos[2]);
    glUniformMatrix4fv(ViewMatID, 1, 0, ViewMat);
    /* use scene projection matrix */
    glUniformMatrix4fv(ModelMatID, 1, 0, ModelMat);
    glUniformMatrix4fv(ViewMatID, 1, 0, ViewMat);
    glUniformMatrix4fv(ProjectionMatID, 1, 0, ProjectionMat);

}

void DrawGUI()
{
    Widget* w;

    /* set up GUI "camera" */
    Mat4x4LoadIdentity(GUIViewMat);
    Mat4x4Translate(GUIViewMat, 0, 0, -1);
    glUniformMatrix4fv(GUIViewMatID, 1, 0, GUIViewMat);
    /* do GUI */
    glUniformMatrix4fv(GUIModelMatID, 1, 0, GUIModelMat);
    glUniformMatrix4fv(GUIProjectionMatID, 1, 0, GUIProjectionMat);

    glEnable(GL_SCISSOR_TEST);
    DrawWidgetRecursive((gpointer)GUILayoutGetRootWidget(), NULL);
    glDisable(GL_SCISSOR_TEST);
}

void DrawWidgetRecursive(gpointer data, gpointer user_data)
{
    Widget* w = (Widget*)data;
    DrawModel(w->background);
    DrawModel(w->contents);
    glScissor(w->rect.x * screen->w, w->rect.y * screen->h, 
            w->rect.w * screen->w, w->rect.h * screen->h);
    g_slist_foreach(w->children, DrawWidgetRecursive, NULL);
}

void DrawOptimizeModel(Model* m) {
    int i;
    
    /* create a VAO for the model */
    glGenVertexArrays(1, &m->vao);
    glBindVertexArray(m->vao);

    /* buffer all the attributes of the model into VBO's */
    m->vboIDs = (GLuint*)malloc(m->numAttributes * sizeof(GLuint));
    glGenBuffers(m->numAttributes, m->vboIDs);
    for(i = 0; i < m->numAttributes; ++i) {
        int attrSize = ModelGetAttributeSize(m->attributeTable[i]);
        glBindBuffer(GL_ARRAY_BUFFER, m->vboIDs[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * attrSize * m->numVertices,
                m->attributes[i], GL_STATIC_DRAW);
        glVertexAttribPointer((GLuint)i, attrSize, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(i);
    }

    /* Unbind. */
    glBindVertexArray(0);
}

void DrawModel(Model *m)
{
    /* Bind the models' vertex attribute object. */
    glBindVertexArray(m->vao);

    /* Draw the model. */
    glDrawArrays(m->primitive, 0, m->numVertices);
    
    /* Unbind. */
    glBindVertexArray(0);
}

void DrawMoveCamera(float x, float y, float z)
{
    cam.pos[0] += x;
    cam.pos[1] += y;
    cam.pos[2] += z;
}

/*****************************************************************************/
/*  Shader/Material loading subsystem                                        */
/*****************************************************************************/
int MaterialInit()
{
    materials = g_hash_table_new(g_int_hash, g_direct_equal);
    fragShaders = g_hash_table_new(g_int_hash, g_int_equal);
    fragShaderNames = g_hash_table_new(g_str_hash, g_int_equal);
    vertShaders = g_hash_table_new(g_int_hash, g_int_equal);
    vertShaderNames = g_hash_table_new(g_str_hash, g_int_equal);
    geomShaders = g_hash_table_new(g_int_hash, g_int_equal);
    geomShaderNames = g_hash_table_new(g_str_hash, g_int_equal);
    return 0;
}

Material* GetMaterial(int id)
{
    return (Material*)g_hash_table_lookup(materials, (gconstpointer)id);
}

void AddMaterial(int id, Material *mat)
{
    g_hash_table_insert(materials, (gpointer)id, (gpointer)mat);
}

Material* MaterialLoad(const char* vertFile, const char* fragFile, 
        const char* geomFile, char** attributes, int numAttributes)
{
    Material* m;

    char* frag;
    char* vert;
    char* geom;
    GLuint v, f, g;
    gpointer lup;

    m = (Material*)malloc(sizeof(Material));

    /* has this shader already been loaded? */
    lup = (char*)g_hash_table_lookup(vertShaderNames, vertFile);
    if(lup == NULL) {
        /* no, load it */
        MaterialReadFile(vertFile, &vert);
        v = CompileShader(vert, GL_VERTEX_SHADER);
        /* insert into hash tables */
        g_hash_table_insert(vertShaderNames, (gpointer)vertFile, (gpointer)v);
    }
    else {
        /* yes, use saved ID */
        v = (GLuint)lup;
    }
    m->vert = v;

    /* get/compile fragment shader */
    lup = (char*)g_hash_table_lookup(fragShaderNames, fragFile);
    if(lup == NULL) {
        MaterialReadFile(fragFile, &frag);
        f = CompileShader(frag, GL_FRAGMENT_SHADER);
        g_hash_table_insert(fragShaderNames, (gpointer)fragFile, (gpointer)f);
    }
    else {
        f = (GLuint)lup;
    }
    m->frag = f;

    /* get/compile geometry shader */
    lup = (char*)g_hash_table_lookup(geomShaderNames, geomFile);
    if(lup == NULL) {
        MaterialReadFile(geomFile, &geom);
        g = CompileShader(geom, GL_GEOMETRY_SHADER);
        g_hash_table_insert(geomShaderNames, (gpointer)geomFile, (gpointer)g);
    }
    else {
        g = (GLuint)lup;
    }
    m->geom = g;

    m->program = CompileProgram(v, f, g, attributes, numAttributes);
    puts("program compiled");
    return m;
}

int MaterialReadFile(const char* filename, char** buffer)
{
    FILE* fp = NULL;
    char* data = NULL;
    long lSize;

    fp = fopen(filename, "rb");
    if(fp == NULL) {
        fprintf(stderr, "Error: could not open file %s for reading\n", filename);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);
    data = (char*)malloc(sizeof(char) * (lSize+1));
    if(data == NULL) {
        fclose(fp);
        fprintf(stderr, "Failed to allocate buffer for material load\n");
        return -2;
    }
    if(lSize != fread(data, sizeof(char), lSize, fp)) {
        fclose(fp);
        free(data);
        fprintf(stderr, "Failed to read material file %s\n", filename);
        return -3;
    }
    data[lSize] = '\0';
    *buffer = data;
    fclose(fp);
}

GLuint CompileShader(const GLchar* shader, GLuint type) 
{
    GLuint s;
    GLsizei len;
    int success;
    GLchar* log;

    s = glCreateShader(type);
    glShaderSource(s, 1, &shader, NULL);
    glCompileShader(s);
    glGetShaderiv(s, GL_COMPILE_STATUS, &success);
    glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
    log = malloc(sizeof(GLchar) * len);
    glGetShaderInfoLog(s, len, &len, log);
    puts(log);
    free(log);

    if(success == GL_FALSE) {
        if(type == GL_VERTEX_SHADER) {
            fprintf(stderr, "Error: vertex shader was not compiled successfully.\n");
        }
        else if(type == GL_FRAGMENT_SHADER) {
            fprintf(stderr, "Error: fragment shader was not compiled successfully.\n");
        }
        else if(type == GL_GEOMETRY_SHADER) {
            fprintf(stderr, "Error: geometry shader was not compiled successfully.\n");
        }
        else {
            fprintf(stderr, "Error: shader was not compiled successfully.\n");
        }
        exit(EXIT_FAILURE);
    }
    return s;
}

GLuint CompileProgram(GLuint vertShader, GLuint fragShader, GLuint geomShader,
      char **attributes, int numAttributes)
{
    int i;
    int success;
    char* log;
    int len;
    GLuint temp;
    GLuint program;

    program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glAttachShader(program, geomShader);

    for(i = 0; i < numAttributes; i++) {
        glBindAttribLocation(program, i, attributes[i]);
    }
    printf("linking shader program. log:\n");
    glLinkProgram(program);

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    log = malloc(sizeof(GLchar) * len);
    glGetProgramInfoLog(program, len, &len, log);
    puts(log);
    free(log);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(success == GL_FALSE) {
        fprintf(stderr, "Error: shader could not be linked successfully.\n");
        exit(EXIT_FAILURE);
    }
    return program;
}

