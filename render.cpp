#include "render.h"

// the camera’s position in its own coordinate system
int cam_x = 0;
int cam_y = -30;
int cam_z = 20;

extern city *sim_city;

// commented for now
// L3DS scene;

void setup_opengl(int width, int height) {
  float ratio = (float)width / (float)height;

  /* Our shading model--Gouraud (smooth). */
  glShadeModel(GL_SMOOTH); //(GL_FLAT);  // smoothing polygons

  /* Culling. */
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);

  glEnable(GL_DEPTH_TEST); // hide objects in the background

  // glEnable(GL_LIGHTING);// put a ligthting. It only works with text!!
  // glEnable(GL_LIGHT0);

  /* Set the clear color. */
  glClearColor(0, 0.2, 0.3, 0); // Background color
                                // red,green,blue, alpha

  /* Required if you want alpha-blended textures (for our fonts) */
  //	glBlendFunc(GL_ONE, GL_ONE);
  //	glEnable(GL_BLEND);

  /* Setup our viewport. */
  glViewport(0, 0, width, height);

  /*
   * Change to the projection matrix and set
   * our viewing volume.
   */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // gluPerspective( 60.0, ratio, 1.0, 1024.0 );
  gluPerspective(45.0, ratio, 1.0f, 1024.0f); // Same as in ReceiveObjectID

  //////////////////////////TESTING L3DS /////////////////////////////

  // glMatrixMode(GL_MODELVIEW);
  // glLoadIdentity();

  //  GLfloat glfLightAmbient[] = {0.0, 0.0, 0.0, 1.0};
  //  GLfloat glfLightDiffuse[] = {0.0, 1.0, 0.0, 1.0};
  //  GLfloat glfLightSpecular[] = {0.6f, 0.6f, 0.3f, 1.0f};

  //  glLightfv(GL_LIGHT0, GL_AMBIENT, glfLightAmbient);
  //  glLightfv(GL_LIGHT0, GL_DIFFUSE, glfLightDiffuse);
  //  glLightfv(GL_LIGHT0, GL_SPECULAR, glfLightSpecular);
  //  glEnable(GL_LIGHT0);

  // glClearColor(0.5, 0.5, 0.5, 0.0);
  // glColor3f(1.0, 1.0, 1.0);
  // glEnable(GL_DEPTH_TEST);
  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_FRONT_AND_BACK);
  // glEnable(GL_NORMALIZE);
  // glEnable(GL_LIGHTING);
  // glColor3f(0.5, 0, 0);

  // glEnableClientState(GL_VERTEX_ARRAY);
  // glEnableClientState(GL_NORMAL_ARRAY);
  // glEnableClientState(GL_COLOR_ARRAY);

  // if (!scene.LoadFile("3ds/house.3ds")) printf ("Can not load 3ds model\n");
  // else printf ("3ds loaded correctly\n");
}

// Given a clicked position, returns the identification (ID) of the object
// (square) located at that position.
int RetrieveObjectID(int x, int y) {
  int objectsFound = 0; // This will hold the amount of objects clicked
  int viewportCoords[4] = {
      0}; // We need an array to hold our view port coordinates

  int window_width = 640;
  int window_height = 480;

  // This will hold the ID's of the objects we click on.
  // We make it an arbitrary number of 32 because openGL also stores other
  // information that we don't care about.  There is about 4 slots of info for
  // every object ID taken up.
  unsigned int selectBuffer[32] = {0};

  // glSelectBuffer is what we register our selection buffer with.  The first
  // parameter is the size of our array.  The next parameter is the buffer to
  // store the information found. More information on the information that will
  // be stored in selectBuffer is further below.

  glSelectBuffer(
      32, selectBuffer); // Setup our selection buffer to accept object ID's

  // This function returns information about many things in OpenGL.  We pass in
  // GL_VIEWPORT to get the view port coordinates.  It saves it like a RECT with
  // {top, left, bottom, right}

  glGetIntegerv(GL_VIEWPORT,
                viewportCoords); // Get the current view port coordinates

  // Now we want to get out of our GL_MODELVIEW matrix and start effecting our
  // GL_PROJECTION matrix.  This allows us to check our X and Y coords against
  // 3D space.

  glMatrixMode(GL_PROJECTION); // We want to now effect our projection matrix

  glPushMatrix(); // We push on a new matrix so we don't effect our 3D
                  // projection

  // This makes it so it doesn't change the frame buffer if we render into it,
  // instead, a record of the names of primitives that would have been drawn if
  // the render mode was GL_RENDER are now stored in the selection array
  // (selectBuffer).

  glRenderMode(GL_SELECT); // Allows us to render the objects, but not change
                           // the frame buffer

  glLoadIdentity(); // Reset our projection matrix

  // gluPickMatrix allows us to create a projection matrix that is around our
  // cursor.  This basically only allows rendering in the region that we
  // specify. If an object is rendered into that region, then it saves that
  // objects ID for us (The magic). The first 2 parameters are the X and Y
  // position to start from, then the next 2 are the width and height of the
  // region from the starting point.  The last parameter is of course our view
  // port coordinates.  You will notice we subtract "y" from the BOTTOM view
  // port coordinate.  We do this to flip the Y coordinates around.  The 0 y
  // coordinate starts from the bottom, which is opposite to window's
  // coordinates. We also give a 2 by 2 region to look for an object in.  This
  // can be changed to preference.

  gluPickMatrix(x, viewportCoords[3] - y, 2, 2, viewportCoords);

  // Next, we just call our normal gluPerspective() function, exactly as we did
  // on startup. This is to multiply the perspective matrix by the pick matrix
  // we created up above.

  // Same as in setup_opengl
  gluPerspective(45.0f, (float)window_width / (float)window_height, 0.1f,
                 1024.0f);

  glMatrixMode(GL_MODELVIEW); // Go back into our model view matrix

  // RenderScene();
  // Now we render into our selective mode to pinpoint clicked objects
  draw_screen_lite();

  // If we return to our normal render mode from select mode, glRenderMode
  // returns the number of objects that were found in our specified region
  // (specified in gluPickMatrix())

  objectsFound = glRenderMode(
      GL_RENDER); // Return to render mode and get the number of objects found

  glMatrixMode(GL_PROJECTION); // Put our projection matrix back to normal.
  glPopMatrix();               // Stop effecting our projection matrix

  glMatrixMode(GL_MODELVIEW); // Go back to our normal model view matrix

  // PHEW!  That was some stuff confusing stuff.  Now we are out of the clear
  // and should have an ID of the object we clicked on.  objectsFound should be
  // at least 1 if we found an object.

  if (objectsFound > 0) {
    // If we found more than one object, we need to check the depth values
    // of all the objects found.  The object with the LEAST depth value is
    // the closest object that we clicked on.  Depending on what you are doing,
    // you might want ALL the objects that you clicked on (if some objects were
    // behind the closest one), but for this tutorial we just care about the one
    // in front.  So, how do we get the depth value?  Well, The selectionBuffer
    // holds it.  For every object there is 4 values.  The first value is
    // "the number of names in the name stack at the time of the event, followed
    // by the minimum and maximum depth values of all vertices that hit since
    // the previous event, then followed by the name stack contents, bottom name
    // first." - MSDN The only ones we care about are the minimum depth value
    // (the second value) and the object ID that was passed into glLoadName()
    // (This is the fourth value). So, [0 - 3] is the first object's data, [4 -
    // 7] is the second object's data, etc... Be carefull though, because if you
    // are displaying 2D text in front, it will always find that as the lowest
    // object.  So make sure you disable text when rendering the screen for the
    // object test.  I use a flag for RenderScene(). So, lets get the object
    // with the lowest depth!

    // Set the lowest depth to the first object to start it off.
    // 1 is the first object's minimum Z value.
    // We use an unsigned int so we don't get a warning with selectBuffer below.
    unsigned int lowestDepth = selectBuffer[1];

    // Set the selected object to the first object to start it off.
    // 3 is the first object's object ID we passed into glLoadName().
    int selectedObject = selectBuffer[3];

    // Go through all of the objects found, but start at the second one
    for (int i = 1; i < objectsFound; i++) {
      // Check if the current objects depth is lower than the current lowest
      // Notice we times i by 4 (4 values for each object) and add 1 for the
      // depth.
      if (selectBuffer[(i * 4) + 1] < lowestDepth) {
        // Set the current lowest depth
        lowestDepth = selectBuffer[(i * 4) + 1];

        // Set the current object ID
        selectedObject = selectBuffer[(i * 4) + 3];
      }
    }

    // Return the selected object
    return selectedObject;
  }

  // We didn't click on any objects so return 0
  return 0;
}

void draw_screen() {
  int i, j;
  float d, alt;

  /* Clear the color and depth buffers. */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* We don't want to modify the projection matrix. */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Set the camera
  gluLookAt(1.4142 * cam_x + (-cam_y - cam_x) * 0.7071, cam_z,
            (-cam_y - cam_x) * 0.7071,
            1.4142 * cam_x + (-cam_y - cam_x) * 0.7071 - 10, cam_z - 10,
            (-cam_y - cam_x) * 0.7071 - 10, 0.0, 1.0, 0.0);

  /* Send our quad data to the pipeline. */

  // Draw squares
  glBegin(GL_QUADS);

  for (i = 0; i < sim_city->xsize; i++)
    for (j = 0; j < sim_city->ysize; j++) {
      if (sim_city->color_layer[i][j] == NULL) {
        switch (sim_city->map[i][j].type) {
        case RES:
          glColor4ubv(dark_green);
          break;
        case COM:
          glColor4ubv(blue);
          break;
        case IND:
          glColor4ubv(yellow);
          break;
        case ROAD:
          glColor4ubv(black);
          break;
        case NONE:
          glColor4ubv(green);
          break;
        default:
          glColor4ubv(green);
          break;
        }
      } else
        glColor4ubv(sim_city->color_layer[i][j]);

      if (sim_city->cursor_x == i && sim_city->cursor_y == j)
        glColor4ubv(white);

      glVertex3f(i, sim_city->map[i][j].height, j);
      glVertex3f(i, sim_city->map[i][j + 1].height, j + 1);
      glVertex3f(i + 1, sim_city->map[i + 1][j + 1].height, j + 1);
      glVertex3f(i + 1, sim_city->map[i + 1][j].height, j);
    }
  glEnd();

  // Draw lines
  glLineWidth(2.0);

  glBegin(GL_LINES);

  glColor4ubv(black);

  for (i = 0; i < sim_city->xsize; i++)
    for (j = 0; j < sim_city->ysize; j++) {

      glVertex3f(i, sim_city->map[i][j].height, j);
      glVertex3f(i + 1, sim_city->map[i + 1][j].height, j);

      glVertex3f(i, sim_city->map[i][j].height, j);
      glVertex3f(i, sim_city->map[i][j + 1].height, j + 1);

      if (i == sim_city->xsize - 1) {
        glVertex3f(i + 1, sim_city->map[i + 1][j].height, j);
        glVertex3f(i + 1, sim_city->map[i + 1][j + 1].height, j + 1);
      }
      if (j == sim_city->ysize - 1) {
        glVertex3f(i, sim_city->map[i][j + 1].height, j + 1);
        glVertex3f(i + 1, sim_city->map[i + 1][j + 1].height, j + 1);
      }
    }

  glEnd();

  // draw buildings

  d = 0.15;
  glBegin(GL_QUADS);

  glColor4ubv(red);

  for (i = 0; i < sim_city->xsize; i++)
    for (j = 0; j < sim_city->ysize; j++) {
      if (sim_city->is_flat(i, j) && sim_city->map[i][j].building != NULL) {
        alt = sim_city->map[i][j].height;

        glVertex3f(i + d, alt, j + d); // cube face 1
        glVertex3f(i + d, alt, j + 1 - d);
        glVertex3f(i + d, alt + 1, j + 1 - d);
        glVertex3f(i + d, alt + 1, j + d);

        glVertex3f(i + d, alt, j + 1 - d); // cube face 2
        glVertex3f(i + 1 - d, alt, j + 1 - d);
        glVertex3f(i + 1 - d, alt + 1, j + 1 - d);
        glVertex3f(i + d, alt + 1, j + 1 - d);

        glVertex3f(i + 1 - d, alt, j + 1 - d); // cube face 3
        glVertex3f(i + 1 - d, alt, j + d);
        glVertex3f(i + 1 - d, alt + 1, j + d);
        glVertex3f(i + 1 - d, alt + 1, j + 1 - d);

        glVertex3f(i + 1 - d, alt, j + d); // cube face 4
        glVertex3f(i + d, alt, j + d);
        glVertex3f(i + d, alt + 1, j + d);
        glVertex3f(i + 1 - d, alt + 1, j + d);

        glVertex3f(i + d, alt + 1, j + d); // ceiling
        glVertex3f(i + d, alt + 1, j + 1 - d);
        glVertex3f(i + 1 - d, alt + 1, j + 1 - d);
        glVertex3f(i + 1 - d, alt + 1, j + d);
      }
    }

  glEnd();

  //////////////////////////TESTING L3DS /////////////////////////////
  /*
  for (uint i= 0; i<scene.GetMeshCount(); i++)
      {
          LMesh &mesh = scene.GetMesh(i);

          glVertexPointer(4, GL_FLOAT, 0, &mesh.GetVertex(0));
          glNormalPointer(GL_FLOAT, 0, &mesh.GetNormal(0));
          glColorPointer(3, GL_FLOAT, 0, &mesh.GetBinormal(0));
          glDrawElements(GL_TRIANGLES, mesh.GetTriangleCount()*3,
                          GL_UNSIGNED_SHORT, &mesh.GetTriangle(0));

      }
  */
  //////////////////////////END TESTING L3DS////////////////////////////

  /*
   * Swap the buffers. This this tells the driver to
   * render the next frame from the contents of the
   * back-buffer, and to set all rendering operations
   * to occur on what was the front-buffer.
   *
   * Double buffering prevents nasty visual tearing
   * from the application drawing on areas of the
   * screen that are being updated at the same time.
   */

  // SDL_GL_SwapBuffers( );
}

// simple scene draw, only used in RetrieveObjectID
void draw_screen_lite() {
  int i, j;
  float d, alt;

  /* Clear the color and depth buffers. */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* We don't want to modify the projection matrix. */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Put the camera
  gluLookAt(1.4142 * cam_x + (-cam_y - cam_x) * 0.7071, cam_z,
            (-cam_y - cam_x) * 0.7071,
            1.4142 * cam_x + (-cam_y - cam_x) * 0.7071 - 10, cam_z - 10,
            (-cam_y - cam_x) * 0.7071 - 10, 0.0, 1.0, 0.0);

  glInitNames();

  glPushName(0);

  /* Send our quad data to the pipeline. */

  // draw squares

  for (i = 0; i < sim_city->xsize; i++)
    for (j = 0; j < sim_city->ysize; j++) {
      glLoadName((i + 1) * 1000 + (j + 1));

      glBegin(GL_QUADS);

      glVertex3f(i, sim_city->map[i][j].height, j);
      glVertex3f(i, sim_city->map[i][j + 1].height, j + 1);
      glVertex3f(i + 1, sim_city->map[i + 1][j + 1].height, j + 1);
      glVertex3f(i + 1, sim_city->map[i + 1][j].height, j);

      glEnd();
    }

  glInitNames();

  // draw buildings

  d = 0.15;
  glBegin(GL_QUADS);

  for (i = 0; i < sim_city->xsize; i++)
    for (j = 0; j < sim_city->ysize; j++) {
      if (sim_city->is_flat(i, j) && sim_city->map[i][j].building != NULL) {
        alt = sim_city->map[i][j].height;

        glVertex3f(i + d, alt, j + d); // cube face 1
        glVertex3f(i + d, alt, j + 1 - d);
        glVertex3f(i + d, alt + 1, j + 1 - d);
        glVertex3f(i + d, alt + 1, j + d);

        glVertex3f(i + d, alt, j + 1 - d); // cube face 2
        glVertex3f(i + 1 - d, alt, j + 1 - d);
        glVertex3f(i + 1 - d, alt + 1, j + 1 - d);
        glVertex3f(i + d, alt + 1, j + 1 - d);

        glVertex3f(i + 1 - d, alt, j + 1 - d); // cube face 3
        glVertex3f(i + 1 - d, alt, j + d);
        glVertex3f(i + 1 - d, alt + 1, j + d);
        glVertex3f(i + 1 - d, alt + 1, j + 1 - d);

        glVertex3f(i + 1 - d, alt, j + d); // cube face 4
        glVertex3f(i + d, alt, j + d);
        glVertex3f(i + d, alt + 1, j + d);
        glVertex3f(i + 1 - d, alt + 1, j + d);

        glVertex3f(i + d, alt + 1, j + d); // ceiling
        glVertex3f(i + d, alt + 1, j + 1 - d);
        glVertex3f(i + 1 - d, alt + 1, j + 1 - d);
        glVertex3f(i + 1 - d, alt + 1, j + d);
      }
    }

  glEnd();
}
