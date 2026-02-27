#include "render.h"

// Camera position in world space
int cam_x = 0;
int cam_y = -30;
int cam_z = 20;

extern city *sim_city;

L3DS scene;

// L3DS test variables (used in the TESTING L3DS block below)
float angle;
float udistance;
uint ticks;
uint frame;
float speed;

// Computed once after LoadFile — used every frame to position the model.
// Non-uniform scaling so the footprint fills the full 1×1 cell in both
// world X and world Z, with the height scaled by the average of the two.
float model_sx = 1.0f; // scale for 3DS X axis  (→ world X)
float model_sy = 1.0f; // scale for 3DS Y axis  (→ world Z after rotation)
float model_sz = 1.0f; // scale for 3DS Z axis  (→ world Y / height)
float model_cx = 0.0f; // 3DS-space X centre (for horizontal centering)
float model_cy = 0.0f; // 3DS-space Y centre (for horizontal centering)
float model_cz = 0.0f; // 3DS-space Z minimum (bottom sits on the ground)

void setup_opengl(int width, int height) {
  float ratio = (float)width / (float)height;

  /* Gouraud (smooth) shading */
  glShadeModel(GL_SMOOTH);

  /* Back-face culling */
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);

  /* Hide objects behind closer ones */
  glEnable(GL_DEPTH_TEST);

  /* Background color (dark teal) */
  glClearColor(0, 0.2f, 0.3f, 0);

  glViewport(0, 0, width, height);

  /* Set up the perspective projection */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // Same FOV used in RetrieveObjectID — must stay in sync
  gluPerspective(45.0, ratio, 1.0f, 1024.0f);

  // Set up Light 0 for the 3DS model.
  // GL_LIGHTING is toggled per-frame around the model draw only — terrain
  // uses glColor4ubv (direct colors) which require lighting to be off.
  GLfloat glfLightAmbient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
  GLfloat glfLightDiffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat glfLightSpecular[] = {0.6f, 0.6f, 0.3f, 1.0f};
  glLightfv(GL_LIGHT0, GL_AMBIENT, glfLightAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, glfLightDiffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, glfLightSpecular);
  glEnable(GL_LIGHT0);

  if (!scene.LoadFile("3ds/house.3ds")) {
    printf("Can not load 3ds model\n");
  } else {
    printf("3ds loaded correctly\n");

    // Compute the axis-aligned bounding box over all meshes so we can
    // scale and position the model at runtime without hard-coding values.
    float minX = 1e30f, maxX = -1e30f;
    float minY = 1e30f, maxY = -1e30f;
    float minZ = 1e30f, maxZ = -1e30f;

    for (uint m = 0; m < scene.GetMeshCount(); m++) {
      LMesh &mesh = scene.GetMesh(m);
      for (uint v = 0; v < mesh.GetVertexCount(); v++) {
        const LVector4 &vert = mesh.GetVertex(v);
        if (vert.x < minX)
          minX = vert.x;
        if (vert.x > maxX)
          maxX = vert.x;
        if (vert.y < minY)
          minY = vert.y;
        if (vert.y > maxY)
          maxY = vert.y;
        if (vert.z < minZ)
          minZ = vert.z;
        if (vert.z > maxZ)
          maxZ = vert.z;
      }
    }

    // After glRotatef(-90, X): 3DS-X → world-X, 3DS-Y → world-(-Z), 3DS-Z →
    // world-Y. Scale each footprint axis independently so the model fills the
    // full 1×1 cell. The vertical axis uses the average to keep proportions
    // natural.
    float extX = maxX - minX;
    float extY = maxY - minY;
    model_sx   = 1.0f / extX;
    model_sy   = 1.0f / extY;
    model_sz   = (model_sx + model_sy) * 0.5f; // natural height
    model_cx   = (minX + maxX) * 0.5f;         // horizontal centre X
    model_cy   = (minY + maxY) * 0.5f; // horizontal centre Y (→ world Z)
    model_cz   = minZ;                 // bottom of model (→ world Y = 0)

    printf("Model bbox  X[%.2f..%.2f] Y[%.2f..%.2f] Z[%.2f..%.2f]\n", minX,
           maxX, minY, maxY, minZ, maxZ);
    printf("Model sx=%.6f sy=%.6f sz=%.6f  centre (%.2f, %.2f)  bottom %.2f\n",
           model_sx, model_sy, model_sz, model_cx, model_cy, model_cz);
  }
}

// Given a clicked screen position (x, y), returns the GL name (ID) of the
// terrain cell under the cursor, or 0 if nothing was hit.
// Uses OpenGL selection mode: renders the scene without touching the frame
// buffer and collects the names of any primitives inside a 2x2 pick region.
int RetrieveObjectID(int x, int y) {
  int objectsFound      = 0;
  int viewportCoords[4] = {0};
  // 4 slots per hit record: name_count, min_depth, max_depth, name
  unsigned int selectBuffer[32] = {0};

  // Register the selection buffer
  glSelectBuffer(32, selectBuffer);
  // Read current viewport (top, left, bottom, right)
  glGetIntegerv(GL_VIEWPORT, viewportCoords);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix(); // save normal projection matrix

  // Switch to selection mode — renders normally but writes hit records instead
  // of pixels
  glRenderMode(GL_SELECT);
  glLoadIdentity();

  // Build a pick matrix: a small projection window centered on the cursor.
  // Y is flipped because OpenGL origin is bottom-left, SDL is top-left.
  gluPickMatrix(x, viewportCoords[3] - y, 2, 2, viewportCoords);
  // Multiply by the same perspective as setup_opengl
  gluPerspective(45.0f, (float)SCREENWIDTH / (float)SCREENHEIGHT, 1.0f,
                 1024.0f);

  glMatrixMode(GL_MODELVIEW);

  // Render the simplified scene to populate the selection buffer
  draw_screen_lite();

  // Exit selection mode; returns the number of hit records written
  objectsFound = glRenderMode(GL_RENDER);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix(); // restore normal projection matrix
  glMatrixMode(GL_MODELVIEW);

  if (objectsFound > 0) {
    // Each hit record: [name_count, min_depth, max_depth, name].
    // Pick the record with the lowest min_depth (closest to the camera).
    unsigned int lowestDepth = selectBuffer[1];
    int selectedObject       = selectBuffer[3];

    for (int i = 1; i < objectsFound; i++) {
      if (selectBuffer[(i * 4) + 1] < lowestDepth) {
        lowestDepth    = selectBuffer[(i * 4) + 1];
        selectedObject = selectBuffer[(i * 4) + 3];
      }
    }

    return selectedObject;
  }

  // No object under the cursor
  return 0;
}

// Convert cam_x/cam_y/cam_z into a gluLookAt call for the isometric view.
// sqrt(2) ≈ 1.4142, sqrt(2)/2 ≈ 0.7071 — standard 45° isometric math.
static void set_camera() {
  float eye_x = 1.4142f * cam_x + (-cam_y - cam_x) * 0.7071f;
  float eye_z = (-cam_y - cam_x) * 0.7071f;
  gluLookAt(eye_x, cam_z, eye_z, eye_x - 10, cam_z - 10, eye_z - 10, 0.0, 1.0,
            0.0);
}

void draw_screen() {
  int i, j;

  /* Clear color and depth buffers */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* We don't want to modify the projection matrix */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  set_camera();

  /* Send terrain quad data to the pipeline */

  // Polygon offset pushes filled quads slightly deeper so the grid lines
  // drawn at the same vertices always win the depth test (no z-fighting).
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glBegin(GL_QUADS);
  for (i = 0; i < sim_city->xsize; i++) {
    for (j = 0; j < sim_city->ysize; j++) {
      // Pick color from the color overlay, zone type, or cursor highlight
      if (sim_city->color_layer[i][j] == nullptr) {
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
        default:
          glColor4ubv(green);
          break;
        }
      } else {
        glColor4ubv(sim_city->color_layer[i][j]);
      }

      if (sim_city->cursor_x == i && sim_city->cursor_y == j)
        glColor4ubv(white);

      glVertex3f(i, sim_city->map[i][j].height, j);
      glVertex3f(i, sim_city->map[i][j + 1].height, j + 1);
      glVertex3f(i + 1, sim_city->map[i + 1][j + 1].height, j + 1);
      glVertex3f(i + 1, sim_city->map[i + 1][j].height, j);
    }
  }
  glEnd();
  glDisable(GL_POLYGON_OFFSET_FILL);

  // Draw grid lines on top of the quads
  glLineWidth(2.0f);
  glBegin(GL_LINES);
  glColor4ubv(black);
  for (i = 0; i < sim_city->xsize; i++) {
    for (j = 0; j < sim_city->ysize; j++) {
      // Left and bottom edges of each cell
      glVertex3f(i, sim_city->map[i][j].height, j);
      glVertex3f(i + 1, sim_city->map[i + 1][j].height, j);

      glVertex3f(i, sim_city->map[i][j].height, j);
      glVertex3f(i, sim_city->map[i][j + 1].height, j + 1);

      // Right border (last column only)
      if (i == sim_city->xsize - 1) {
        glVertex3f(i + 1, sim_city->map[i + 1][j].height, j);
        glVertex3f(i + 1, sim_city->map[i + 1][j + 1].height, j + 1);
      }
      // Top border (last row only)
      if (j == sim_city->ysize - 1) {
        glVertex3f(i, sim_city->map[i][j + 1].height, j + 1);
        glVertex3f(i + 1, sim_city->map[i + 1][j + 1].height, j + 1);
      }
    }
  }
  glEnd();

  // Draw one 3DS house instance for every cell that carries a HOUSE structure.
  // Lighting is enabled once around the whole loop and restored after.
  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  for (int ci = 0; ci < sim_city->xsize; ci++) {
    for (int cj = 0; cj < sim_city->ysize; cj++) {
      structure *bld = sim_city->map[ci][cj].building;
      if (bld == nullptr || bld->struct_type != HOUSE)
        continue;

      // Transform order (OpenGL right-to-left):
      //   1. Shift model so its bottom-centre is at the 3DS-space origin.
      //   2. Scale so the footprint fills the full 1×1 cell.
      //   3. Rotate from 3DS Z-up to OpenGL Y-up.
      //   4. Translate to the world-space centre of the target cell.
      float h = sim_city->map[ci][cj].height;
      glPushMatrix();
      glTranslatef(ci + 0.5f, h, cj + 0.5f);         // 4. cell centre
      glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);           // 3. Z-up → Y-up
      glScalef(model_sx, model_sy, model_sz);        // 2. fill cell
      glTranslatef(-model_cx, -model_cy, -model_cz); // 1. centre model

      for (uint m = 0; m < scene.GetMeshCount(); m++) {
        LMesh &mesh = scene.GetMesh(m);
        glVertexPointer(4, GL_FLOAT, 0, &mesh.GetVertex(0));
        glNormalPointer(GL_FLOAT, 0, &mesh.GetNormal(0));
        glColor3f(0.6f, 0.4f, 0.2f);
        glDrawElements(GL_TRIANGLES, mesh.GetTriangleCount() * 3,
                       GL_UNSIGNED_SHORT, &mesh.GetTriangle(0));
      }

      glPopMatrix();
    }
  }

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisable(GL_LIGHTING);
  glDisable(GL_NORMALIZE);
}

// Simplified scene used only by RetrieveObjectID for hit-testing.
// Each terrain quad is assigned a GL name: (i+1)*1000 + (j+1).
// No colors, no grid lines — just geometry for the selection buffer.
void draw_screen_lite() {
  int i, j;

  /* Clear color and depth buffers */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* We don't want to modify the projection matrix */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  set_camera();

  glInitNames();
  glPushName(0);

  /* Assign a unique GL name to each terrain quad for picking */
  for (i = 0; i < sim_city->xsize; i++) {
    for (j = 0; j < sim_city->ysize; j++) {
      glLoadName((i + 1) * 1000 + (j + 1));

      glBegin(GL_QUADS);
      glVertex3f(i, sim_city->map[i][j].height, j);
      glVertex3f(i, sim_city->map[i][j + 1].height, j + 1);
      glVertex3f(i + 1, sim_city->map[i + 1][j + 1].height, j + 1);
      glVertex3f(i + 1, sim_city->map[i + 1][j].height, j);
      glEnd();
    }
  }

  glInitNames(); // clear name stack before drawing buildings (no ID needed)

  // Also draw buildings so they can occlude terrain in the hit test
  const float d = 0.15f;
  glBegin(GL_QUADS);
  for (i = 0; i < sim_city->xsize; i++) {
    for (j = 0; j < sim_city->ysize; j++) {
      if (!sim_city->is_flat(i, j) || sim_city->map[i][j].building == nullptr)
        continue;

      float alt = sim_city->map[i][j].height;

      glVertex3f(i + d, alt, j + d);
      glVertex3f(i + d, alt, j + 1 - d);
      glVertex3f(i + d, alt + 1, j + 1 - d);
      glVertex3f(i + d, alt + 1, j + d);

      glVertex3f(i + d, alt, j + 1 - d);
      glVertex3f(i + 1 - d, alt, j + 1 - d);
      glVertex3f(i + 1 - d, alt + 1, j + 1 - d);
      glVertex3f(i + d, alt + 1, j + 1 - d);

      glVertex3f(i + 1 - d, alt, j + 1 - d);
      glVertex3f(i + 1 - d, alt, j + d);
      glVertex3f(i + 1 - d, alt + 1, j + d);
      glVertex3f(i + 1 - d, alt + 1, j + 1 - d);

      glVertex3f(i + 1 - d, alt, j + d);
      glVertex3f(i + d, alt, j + d);
      glVertex3f(i + d, alt + 1, j + d);
      glVertex3f(i + 1 - d, alt + 1, j + d);

      glVertex3f(i + d, alt + 1, j + d);
      glVertex3f(i + d, alt + 1, j + 1 - d);
      glVertex3f(i + 1 - d, alt + 1, j + 1 - d);
      glVertex3f(i + 1 - d, alt + 1, j + d);
    }
  }
  glEnd();
}
