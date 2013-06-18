#include "app.h"

static Model* m;
static Model* background;

void AppStart()
{
    //Material* mat = MaterialLoad("test.mat");
    m = ModelNew(0);
    ModelLoadPLY(m, "TestAssets/monkey.ply");
   // ModelSetMaterial(m, mat);
    DrawOptimizeModel(m);

    Color col = {1.0f,0.0f,0.0f,1.0f};
    Model* rectM = GenRect(0, 0, 0, 10, 10, col);
    Rect wRect = {0,0,.2,.4};
    Widget* w = GUILayoutNewTextBox(rectM, &wRect, "HELLO ALL");
    GUILayoutSetRootWidget(w);

    Color bgCorner1 = {0.0f, 0.0f, 1.0f, 1.0f};
    Color bgCorner2 = {0.0f, 0.0f, 1.0f, 1.0f};
    Color bgCorner3 = {1.0f, 0.0f, 0.0f, 1.0f};
    Color bgCorner4 = {1.0f, 0.0f, 0.0f, 1.0f};
    background = GenRect(-4,-4,-20,8,8, bgCorner1);
    ModelSetAttribute(background, ATTRIBUTE_COLOR, 0, bgCorner1);
    ModelSetAttribute(background, ATTRIBUTE_COLOR, 1, bgCorner2);
    ModelSetAttribute(background, ATTRIBUTE_COLOR, 2, bgCorner3);
    ModelSetAttribute(background, ATTRIBUTE_COLOR, 3, bgCorner4);
    DrawOptimizeModel(background);
}

void AppUpdate()
{
    /* Render */
    DrawStartFrame();

    DrawModel(m);
    DrawModel(background);

    /* Render GUI */
    DrawGUI();
}

void AppQuit()
{
    ModelFree(m);
    ModelFree(background);
}
