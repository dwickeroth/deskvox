// Virvo - Virtual Reality Volume Rendering
// Copyright (C) 1999-2003 University of Stuttgart, 2004-2005 Brown University
// Contact: Jurgen P. Schulze, jschulze@ucsd.edu
//
// This file is part of Virvo.
//
// Virvo is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library (see license.txt); if not, write to the
// Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#include "vvplatform.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "vvopengl.h"
#ifdef VV_GLUT
#include <glut.h>
#endif

#ifdef VV_DEBUG_MEMORY
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif

#include "vvtoolshed.h"
#include "vvprintgl.h"
#include "vvdebugmsg.h"

#ifdef HAVE_X11
Display* vvPrintGL::dsp = NULL;
#endif

/** Constructor.
  Builds the bitmap font.
  @param hDC Windows device context
*/
vvPrintGL::vvPrintGL()
  : _consoleOutput(false)
{
  vvDebugMsg::msg(3, "vvPrintGL::vvPrintGL()");

  _fontColor = vvVector4(1.0f, 1.0f, 1.0f, 1.0f);

#ifdef _WIN32
  HFONT font;                                     // Windows Font ID
  HDC   hDC;                                      // Windows device context

  base = glGenLists(96);                          // Storage For 96 Characters

  font = CreateFont(-24,                          // Height Of Font
    0,                                            // Width Of Font
    0,                                            // Angle Of Escapement
    0,                                            // Orientation Angle
    FW_BOLD,                                      // Font Weight
    FALSE,                                        // Italic
    FALSE,                                        // Underline
    FALSE,                                        // Strikeout
    ANSI_CHARSET,                                 // Character Set Identifier
    OUT_TT_PRECIS,                                // Output Precision
    CLIP_DEFAULT_PRECIS,                          // Clipping Precision
    ANTIALIASED_QUALITY,                          // Output Quality
    FF_DONTCARE|DEFAULT_PITCH,                    // Family And Pitch
    TEXT("Courier New"));                         // Font Name

  hDC = wglGetCurrentDC();
  SelectObject(hDC, font);                        // Selects The Font We Want

  wglUseFontBitmaps(hDC, 32, 96, base);           // Builds 96 Characters Starting At Character 32
#elif defined(HAVE_X11)
  if (vvPrintGL::dsp == NULL)
  {
    vvPrintGL::dsp = XOpenDisplay(NULL);
  }
  XFontStruct* font = XLoadQueryFont(vvPrintGL::dsp, "-*-courier-bold-r-normal--20-*-*-*-*-*-*-*");

  if (font)
  {
    const Font id = font->fid;
    uint first = font->min_char_or_byte2;
    uint last = font->max_char_or_byte2;
    base = glGenLists(last + 1);

    if (base)
    {
      glXUseXFont(id, first, last - first + 1, base + first);
    }
  }
#else
  // Don't render to OpenGL window, but to console.
  _consoleOutput = true;
#endif
}

/** Destructor.
  Deletes the bitmap font.
*/
vvPrintGL::~vvPrintGL()
{
  vvDebugMsg::msg(3, "vvPrintGL::~vvPrintGL()");

  glDeleteLists(base, 96);                        // Delete All 96 Characters
}

//----------------------------------------------------------------------------
/** Print a text string to the current screen position.
  The current OpenGL drawing color is used.
  @param x,y text position [0..1]
  @param fmt printf compatible argument format
*/
void vvPrintGL::print(const float x, const float y, const char *fmt, ...)
{

  vvDebugMsg::msg(3, "vvPrintGL::print()");

  va_list ap;                                     // Pointer To List Of Arguments
  char    text[1024];                             // Holds Our String

  if (fmt == NULL)                                // If There's No Text
    return;                                       // Do Nothing

  va_start(ap, fmt);                              // Parses The String For Variables
  vsprintf(text, fmt, ap);                        // And Converts Symbols To Actual Numbers
  va_end(ap);                                     // Results Are Stored In Text

  // make sure that only valid display lists are called
  for(char *p=text; *p; ++p)
  {
      int c = *p;
      if(c < 32 || c >= 128)
          *p = '+';
  }

  if (!_consoleOutput)
  {
    saveGLState();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glColor4f(_fontColor[0], _fontColor[1], _fontColor[2], _fontColor[3]);
    glRasterPos2f(x, y);                            // set text position
  #ifdef _WIN32
    glListBase(base - 32);                          // Sets The Base Character to 32
  #else
    glListBase(base);
  #endif
                                                    // Draws The Display List Text
    glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);

    restoreGLState();
  }
  else
  {
    cerr << text << endl;
  }
}

//----------------------------------------------------------------------------
/// Set the font color.
void vvPrintGL::setFontColor(const vvVector4& fontColor)
{
  _fontColor = fontColor;
}

//----------------------------------------------------------------------------
/// Save GL state for text display.
void vvPrintGL::saveGLState()
{
  vvDebugMsg::msg(3, "vvPrintGL::saveGLState()");

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();

  glPushAttrib(GL_CURRENT_BIT | GL_TRANSFORM_BIT | GL_LIST_BIT);
}

//----------------------------------------------------------------------------
/// Restore GL state to previous state.
void vvPrintGL::restoreGLState()
{
  vvDebugMsg::msg(3, "vvPrintGL::restoreGLState()");

  glPopAttrib();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

//============================================================================
// End of File
//============================================================================