////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Contains main function to create a window and run engine
///
/// Basic support for directional light, depth buffer, setting up 3d projection,
/// setting up a 3d camera, drawing a dragon model, and running a set fixed frame
/// rate.
///
/// Bonus functionality: Support textures + normals. 
/// Support OBJ files with quadrilateral/triangular face format, with optional
/// normal and texture information. 
/// Change color of model, or change drawing style with popup menu
/// 
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes

// STL
#include <cmath>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Simulation/Simulator.h"

// OPEN GL Includes linked from "Model.h"
#ifndef __MAC__
#define __MAC__ 1
#endif

////////////////////////////////////////////////////////////////////////////////
// Global variables - avoid these

// Models
shared_ptr<Simulator> simulator;

// Window
int g_width{1360};
int g_height{768};
int g_window{0};

// Menu
static int submenuStyle_id;
static int submenuColor_id;
static int submenuModel_id;

// Frame rate
const unsigned int FPS = 60;
float g_frameRate{0.f};
std::chrono::high_resolution_clock::time_point g_frameTime{
  std::chrono::high_resolution_clock::now()};
float g_delay{0.f};
float g_framesPerSecond{0.f};

////////////////////////////////////////////////////////////////////////////////
// Functions

////////////////////////////////////////////////////////////////////////////////
/// @brief Initialize GL settings
void
initialize() {
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Based on user's mouse click, program changes
void menu(int num){
  switch(num){
    case 0: 
      glutDestroyWindow(g_window);
      exit(0);
      break;
    default:
      break;
  }
  
  
  glutPostRedisplay();
} 

////////////////////////////////////////////////////////////////////////////////
/// @brief Callback for resize of window
///
/// Responsible for setting window size (viewport) and projection matrix.
void
resize(GLint _w, GLint _h) {
  g_width = _w;
  g_height = _h;

  // Viewport
  glViewport(0, 0, g_width, g_height);

  // Projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.f, g_width/g_height, 0.01f, 150.f);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Timer function to fix framerate in a GLUT application
/// @param _v Value (not used here)
///
/// Note, this is rudametary and fragile.
void
timer(int _v) {
  if(g_window != 0) {
    /// Might need to change where to call update
    simulator->update();
    glutPostRedisplay();

    g_delay = std::max(0.f, 1.f/FPS - g_frameRate);
    glutTimerFunc((unsigned int)(1000.f*g_delay), timer, 0);
  }
  else
    exit(0);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Create menu
// Create Menus And SubMenus in OpenGL
// Sources: https://www.youtube.com/watch?v=cfiGI6l5sDU
void createMenu(void){ 
    submenuColor_id = glutCreateMenu(menu);
    glutAddMenuEntry("Red", 1);
    glutCreateMenu(menu); 
    submenuStyle_id = glutCreateMenu(menu);
    glutAddMenuEntry("Fill", 6);
    glutAddMenuEntry("Point", 7);
    glutAddMenuEntry("Line", 8);
    glutCreateMenu(menu);
    submenuModel_id = glutCreateMenu(menu);
    glutAddMenuEntry("Terrain", 14);
    glutAddMenuEntry("All", 15);
    glutCreateMenu(menu);
    glutAddSubMenu("Model", submenuModel_id);
    glutAddSubMenu("Draw", submenuStyle_id);
    glutAddSubMenu("Color", submenuColor_id);
    glutAddMenuEntry("Quit", 0);     
    glutAttachMenu(GLUT_RIGHT_BUTTON);
} 

////////////////////////////////////////////////////////////////////////////////
/// @brief Draw function for single frame
void
draw() {
  using namespace std::chrono;

  //////////////////////////////////////////////////////////////////////////////
  // Clear
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //////////////////////////////////////////////////////////////////////////////
  // Draw

  // Single directional light
  static GLfloat lightPosition[] = { 0.5f, 1.0f, 1.5f, 0.0f };
  static GLfloat whiteLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
  static GLfloat darkLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  glLightfv(GL_LIGHT0, GL_AMBIENT, darkLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);

  // Camera
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable( GL_BLEND); 
  simulator->draw();
  glDisable(GL_CULL_FACE);
  glDisable (GL_BLEND);
  ///////////////////////////////////////////////////////////////////////
  // Show
  glutSwapBuffers();

  //////////////////////////////////////////////////////////////////////////////
  // Record frame time
  high_resolution_clock::time_point time = high_resolution_clock::now();
  g_frameRate = duration_cast<duration<float>>(time - g_frameTime).count();
  g_frameTime = time;
  g_framesPerSecond = 1.f/(g_delay + g_frameRate);
  printf("FPS: %6.2f\n", g_framesPerSecond);

}

////////////////////////////////////////////////////////////////////////////////
/// @brief Callback function for keyboard presses
/// @param _key Key
/// @param _x X position of mouse
/// @param _y Y position of mouse
void
keyPressed(GLubyte _key, GLint _x, GLint _y) {
  switch(_key) {
    // Escape key : quit application
    case 27:
      std::cout << "Destroying window: " << g_window << std::endl;
      glutDestroyWindow(g_window);
      g_window = 0;
      break;
    case 'w':
      simulator->getCamera().rotateUp();
      cout << simulator->getCamera().getEyePos().x << " " << simulator->getCamera().getEyePos().y << " " <<simulator->getCamera().getEyePos().z << endl;
      cout << simulator->getCamera().getDir().x << " " << simulator->getCamera().getDir().y << " " <<simulator->getCamera().getDir().z << endl;
      break;
    case 's':
      simulator->getCamera().rotateDown();
      cout << simulator->getCamera().getEyePos().x << " " << simulator->getCamera().getEyePos().y << " " <<simulator->getCamera().getEyePos().z << endl;
      cout << simulator->getCamera().getDir().x << " " << simulator->getCamera().getDir().y << " " <<simulator->getCamera().getDir().z << endl;
      break;
    case 'a':
      simulator->getCamera().rotateLeft();
      cout << simulator->getCamera().getEyePos().x << " " << simulator->getCamera().getEyePos().y << " " <<simulator->getCamera().getEyePos().z << endl;
      cout << simulator->getCamera().getDir().x << " " << simulator->getCamera().getDir().y << " " <<simulator->getCamera().getDir().z << endl;
      break;
    case 'd':
      simulator->getCamera().rotateRight();
      cout << simulator->getCamera().getEyePos().x << " " << simulator->getCamera().getEyePos().y << " " <<simulator->getCamera().getEyePos().z << endl;
      cout << simulator->getCamera().getDir().x << " " << simulator->getCamera().getDir().y << " " <<simulator->getCamera().getDir().z << endl;
      break;
    case 'l':
      simulator->getCamera().panRight();
      break;
    case 'j':
      simulator->getCamera().panLeft();
      break;
    case '1':
      simulator->getCamera().setPosition(glm::vec3(89,20,-6), glm::vec3(-1.0,0.03,0));
      break;
    case '2': // 'waterfall'
      simulator->getCamera().setPosition(glm::vec3(84,20,-2), glm::vec3(-0.97,0.05,-0.24));
      break;
    case '3': // 'fighting scene'
      simulator->getCamera().setPosition(glm::vec3(46,20,70), glm::vec3(-0.42,0.03,-0.9));
      break;
    case '4': // 'fighting scene'
      simulator->getCamera().setPosition(glm::vec3(-41,20,20), glm::vec3(0.65,-0.074,-0.76));
      break;
    case '9':
      for(auto& repeller : simulator->getRepellers())
        repeller->toggleActivate();
      break;
    case '0':
      // activate
      for(auto& attractor : simulator->getAttractors()){
        attractor->toggleActivate();
      }
      break;
    default:
      std::cout << "Unhandled key: " << (int)(_key) << std::endl;
      break;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Callback function for keyboard presses of special keys
/// @param _key Key
/// @param _x X position of mouse
/// @param _y Y position of mouse
void
specialKeyPressed(GLint _key, GLint _x, GLint _y) {
  switch(_key) {
    // Arrow keys
    case GLUT_KEY_LEFT:
      simulator->getCamera().moveLeft();
      break;
    case GLUT_KEY_RIGHT:
      simulator->getCamera().moveRight();
      break;
    case GLUT_KEY_UP:
      simulator->getCamera().moveForward();
      break;
    case GLUT_KEY_DOWN:
      simulator->getCamera().moveBackwards();
      break;
    // Unhandled
    default:
      std::cout << "Unhandled special key: " << _key << std::endl;
      break;
  }
}

//////////////////////////////////////////////////////////////////////////////
// @brief: Assign callback functions
void assignCallbackFuncs(){
  std::cout << "Assigning Callback functions" << std::endl;
  glutReshapeFunc(resize);
  glutDisplayFunc(draw);
  glutKeyboardFunc(keyPressed);
  glutSpecialFunc(specialKeyPressed);
  glutTimerFunc(1000/FPS, timer, 0);
}


////////////////////////////////////////////////////////////////////////////////
// Main

////////////////////////////////////////////////////////////////////////////////
/// @brief main
/// @param _argc Count of command line arguments
/// @param _argv Command line arguments
/// @return Application success status
int
main(int _argc, char** _argv) {

  if (_argc < 2 ){
        cerr << "Usage: " << _argv[0] << " [input file for model]" << endl;
        exit(0);
  }

  simulator = make_shared<Simulator>(_argv[1]);

  //////////////////////////////////////////////////////////////////////////////
  // Initialize GLUT Window
  std::cout << "Initializing GLUTWindow" << std::endl;
  // GLUT
  glutInit(&_argc, _argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowPosition(50, 100);
  glutInitWindowSize(g_width, g_height); // HD size
  g_window = glutCreateWindow("Graphics Project 2");
  createMenu();

  // GL
  initialize();

  assignCallbackFuncs();

  // Start application
  std::cout << "Starting Application" << std::endl;
  glutMainLoop();
  return 0;
}
