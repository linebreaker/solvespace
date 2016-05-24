//-----------------------------------------------------------------------------
// The toolbar that appears at the top left of the graphics window, where the
// user can select icons with the mouse, to perform operations equivalent to
// selecting a menu item or using a keyboard shortcut.
//
// Copyright 2008-2013 Jonathan Westhues.
//-----------------------------------------------------------------------------
#include "solvespace.h"

static const char *SPACER = "";
static struct {
    const char  *iconName;
    int          menu;
    const char  *tip;
    Pixmap       icon;
} Toolbar[] = {
    { "line",            GraphicsWindow::MNU_LINE_SEGMENT,   "Sketch line segment",                              {} },
    { "rectangle",       GraphicsWindow::MNU_RECTANGLE,      "Sketch rectangle",                                 {} },
    { "circle",          GraphicsWindow::MNU_CIRCLE,         "Sketch circle",                                    {} },
    { "arc",             GraphicsWindow::MNU_ARC,            "Sketch arc of a circle",                           {} },
    { "text",            GraphicsWindow::MNU_TTF_TEXT,       "Sketch curves from text in a TrueType font",       {} },
    { "tangent-arc",     GraphicsWindow::MNU_TANGENT_ARC,    "Create tangent arc at selected point",             {} },
    { "bezier",          GraphicsWindow::MNU_CUBIC,          "Sketch cubic Bezier spline",                       {} },
    { "point",           GraphicsWindow::MNU_DATUM_POINT,    "Sketch datum point",                               {} },
    { "construction",    GraphicsWindow::MNU_CONSTRUCTION,   "Toggle construction",                              {} },
    { "trim",            GraphicsWindow::MNU_SPLIT_CURVES,   "Split lines / curves where they intersect",        {} },
    { SPACER, 0, 0, {} },

    { "length",          GraphicsWindow::MNU_DISTANCE_DIA,   "Constrain distance / diameter / length",           {} },
    { "angle",           GraphicsWindow::MNU_ANGLE,          "Constrain angle",                                  {} },
    { "horiz",           GraphicsWindow::MNU_HORIZONTAL,     "Constrain to be horizontal",                       {} },
    { "vert",            GraphicsWindow::MNU_VERTICAL,       "Constrain to be vertical",                         {} },
    { "parallel",        GraphicsWindow::MNU_PARALLEL,       "Constrain to be parallel or tangent",              {} },
    { "perpendicular",   GraphicsWindow::MNU_PERPENDICULAR,  "Constrain to be perpendicular",                    {} },
    { "pointonx",        GraphicsWindow::MNU_ON_ENTITY,      "Constrain point on line / curve / plane / point",  {} },
    { "symmetric",       GraphicsWindow::MNU_SYMMETRIC,      "Constrain symmetric",                              {} },
    { "equal",           GraphicsWindow::MNU_EQUAL,          "Constrain equal length / radius / angle",          {} },
    { "same-orientation",GraphicsWindow::MNU_ORIENTED_SAME,  "Constrain normals in same orientation",            {} },
    { "other-supp",      GraphicsWindow::MNU_OTHER_ANGLE,    "Other supplementary angle",                        {} },
    { "ref",             GraphicsWindow::MNU_REFERENCE,      "Toggle reference dimension",                       {} },
    { SPACER, 0, 0, {} },

    { "extrude",         GraphicsWindow::MNU_GROUP_EXTRUDE,  "New group extruding active sketch",                {} },
    { "lathe",           GraphicsWindow::MNU_GROUP_LATHE,    "New group rotating active sketch",                 {} },
    { "step-rotate",     GraphicsWindow::MNU_GROUP_ROT,      "New group step and repeat rotating",               {} },
    { "step-translate",  GraphicsWindow::MNU_GROUP_TRANS,    "New group step and repeat translating",            {} },
    { "sketch-in-plane", GraphicsWindow::MNU_GROUP_WRKPL,    "New group in new workplane (thru given entities)", {} },
    { "sketch-in-3d",    GraphicsWindow::MNU_GROUP_3D,       "New group in 3d",                                  {} },
    { "assemble",        GraphicsWindow::MNU_GROUP_LINK,     "New group linking / assembling file",              {} },
    { SPACER, 0, 0, {} },

    { "in3d",            GraphicsWindow::MNU_NEAREST_ISO,    "Nearest isometric view",                           {} },
    { "ontoworkplane",   GraphicsWindow::MNU_ONTO_WORKPLANE, "Align view to active workplane",                   {} },
    { NULL, 0, 0, {} }
};

void GraphicsWindow::ToolbarDraw() {
    ToolbarDrawOrHitTest(0, 0, true, NULL);
}

bool GraphicsWindow::ToolbarMouseMoved(int x, int y) {
    x += ((int)width/2);
    y += ((int)height/2);

    int nh = 0;
    bool withinToolbar = ToolbarDrawOrHitTest(x, y, false, &nh);
    if(!withinToolbar) nh = 0;

    if(nh != toolbarTooltipped) {
        // Don't let the tool tip move around if the mouse moves within the
        // same item.
        toolbarMouseX = x;
        toolbarMouseY = y;
        toolbarTooltipped = 0;
    }

    if(nh != toolbarHovered) {
        toolbarHovered = nh;
        SetTimerFor(1000);
        PaintGraphics();
    }
    // So if we moved off the toolbar, then toolbarHovered is now equal to
    // zero, so it doesn't matter if the tool tip timer expires. And if
    // we moved from one item to another, we reset the timer, so also okay.
    return withinToolbar;
}

bool GraphicsWindow::ToolbarMouseDown(int x, int y) {
    x += ((int)width/2);
    y += ((int)height/2);

    int nh = -1;
    bool withinToolbar = ToolbarDrawOrHitTest(x, y, false, &nh);
    // They might have clicked within the toolbar, but not on a button.
    if(withinToolbar && nh >= 0) {
        for(int i = 0; SS.GW.menu[i].level >= 0; i++) {
            if(nh == SS.GW.menu[i].id) {
                (SS.GW.menu[i].fn)((GraphicsWindow::MenuId)SS.GW.menu[i].id);
                break;
            }
        }
    }
    return withinToolbar;
}

bool GraphicsWindow::ToolbarDrawOrHitTest(int mx, int my,
                                          bool paint, int *menuHit)
{
    int i;
    int x = 17, y = (int)(height - 52);

    int fudge = 8;
    int h = 32*16 + 3*16 + fudge;
    int aleft = 0, aright = 66, atop = y+16+fudge/2, abot = y+16-h;

    bool withinToolbar =
        (mx >= aleft && mx <= aright && my <= atop && my >= abot);

    if(!paint && !withinToolbar) {
        // This gets called every MouseMove event, so return quickly.
        return false;
    }

    if(paint) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glTranslated(-1, -1, 0);
        glScaled(2.0/width, 2.0/height, 0);
        glDisable(GL_LIGHTING);

        double c = 30.0/255;
        glColor4d(c, c, c, 1.0);
        ssglAxisAlignedQuad(aleft, aright, atop, abot);
    }

    struct {
        bool show;
        const char *str;
    } toolTip = { false, NULL };

    bool leftpos = true;
    for(i = 0; Toolbar[i].iconName; i++) {
        if(Toolbar[i].iconName == SPACER) {
            if(!leftpos) {
                leftpos = true;
                y -= 32;
                x -= 32;
            }
            y -= 16;

            if(paint) {
                // Draw a separator bar in a slightly different color.
                int divw = 30, divh = 2;
                glColor4d(0.17, 0.17, 0.17, 1);
                x += 16;
                y += 24;
                ssglAxisAlignedQuad(x+divw, x-divw, y+divh, y-divh);
                x -= 16;
                y -= 24;
            }

            continue;
        }

        if(Toolbar[i].icon.IsEmpty()) {
            std::string name = ssprintf("icons/graphics-window/%s.png", Toolbar[i].iconName);
            Toolbar[i].icon = LoadPNG(name);
        }

        if(paint) {
            glColor4d(0, 0, 0, 1.0);
            Point2d o = { (double)(x - Toolbar[i].icon.width  / 2),
                          (double)(y - Toolbar[i].icon.height / 2) };
            ssglDrawPixmap(Toolbar[i].icon, o, /*flip=*/true);

            if(toolbarHovered == Toolbar[i].menu ||
               pending.operation == Toolbar[i].menu) {
                // Highlight the hovered or pending item.
                glColor4d(1, 1, 0, 0.3);
                int boxhw = 15;
                ssglAxisAlignedQuad(x+boxhw, x-boxhw, y+boxhw, y-boxhw);
            }

            if(toolbarTooltipped == Toolbar[i].menu) {
                // Display the tool tip for this item; postpone till later
                // so that no one draws over us. Don't need position since
                // that's just wherever the mouse is.
                toolTip.show = true;
                toolTip.str = Toolbar[i].tip;
            }
        } else {
            int boxhw = 16;
            if(mx < (x+boxhw) && mx > (x - boxhw) &&
               my < (y+boxhw) && my > (y - boxhw))
            {
                if(menuHit) *menuHit = Toolbar[i].menu;
            }
        }

        if(leftpos) {
            x += 32;
            leftpos = false;
        } else {
            x -= 32;
            y -= 32;
            leftpos = true;
        }
    }

    if(paint) {
        // Do this last so that nothing can draw over it.
        if(toolTip.show) {
            ssglInitializeBitmapFont();
            std::string str = toolTip.str;

            for(i = 0; SS.GW.menu[i].level >= 0; i++) {
                if(toolbarTooltipped == SS.GW.menu[i].id) {
                    std::string accel = MakeAcceleratorLabel(SS.GW.menu[i].accel);
                    if(!accel.empty()) {
                        str += ssprintf(" (%s)", accel.c_str());
                    }
                    break;
                }
            }

            int tw = str.length() * (SS.TW.CHAR_WIDTH - 1) + 10,
                th = SS.TW.LINE_HEIGHT + 2;

            double ox = toolbarMouseX + 3, oy = toolbarMouseY + 3;
            glLineWidth(1);
            glColor4d(1.0, 1.0, 0.6, 1.0);
            ssglAxisAlignedQuad(ox, ox+tw, oy, oy+th);
            glColor4d(0.0, 0.0, 0.0, 1.0);
            ssglAxisAlignedLineLoop(ox, ox+tw, oy, oy+th);

            glColor4d(0, 0, 0, 1);
            glPushMatrix();
                glTranslated(ox+5, oy+3, 0);
                glScaled(1, -1, 1);
                ssglBitmapText(str, Vector::From(0, 0, 0));
            glPopMatrix();
        }
        ssglDepthRangeLockToFront(false);
    }

    return withinToolbar;
}

void GraphicsWindow::TimerCallback() {
    SS.GW.toolbarTooltipped = SS.GW.toolbarHovered;
    PaintGraphics();
}

