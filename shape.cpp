
//submit mk-colors.perl
#include <typeinfo>
#include <unordered_map>
#include <cmath>
using namespace std;

#include "shape.h"
#include "util.h"

static unordered_map<void*,string> fontname {
    {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
    {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
    {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
    {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
    {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
    {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
    {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};


ostream& operator<< (ostream& out, const vertex& where) {
    out << "(" << where.xpos << "," << where.ypos << ")";
    return out;
}

shape::shape() {
    DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font_, const string& textdata_):
glut_bitmap_font(glut_bitmap_font_), textdata(textdata_) {
    DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {

    DEBUGF ('c', this);
}
polygon::polygon (const vertex_list& vertices_): vertices(vertices_) {

    DEBUGF ('c', this);

}

triangle::triangle(const vertex_list& vertices): polygon(vertices) {
   DEBUGF ('c', this);

}

equilateral::equilateral (GLfloat width):
   triangle({{-width/2, -width/2},
   { 0.0, width/2},{ width/2,-width/2}})
{
   DEBUGF ('c', this << "(" << width << "," << ")");
}

diamond::diamond (GLfloat width, GLfloat height):
   polygon({{0.0, height/2},{-width/2, 0.0},
    {0.0,-height/2},{width/2,0.0}})
{
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
    DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
           polygon({{width, height}, {width, 0}, {0,0},
                    {0, height}}){

   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
    DEBUGF ('c', this);
}

void text::draw (const vertex& center, const rgbcolor& color) const {
    DEBUGF ('d', this << "(" << center << "," << color << ")");

     auto font = glut_bitmap_font;
     glColor3ubv(color.ubvec);
     GLfloat xpos = center.xpos;
     GLfloat ypos = center.ypos;

     glRasterPos2f(xpos,ypos);

     for (auto chr : textdata) glutBitmapCharacter(font, chr);
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {

   glBegin (GL_POLYGON);
   glColor3ubv (color.ubvec);
   const float delta = 2 * M_PI / 32;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float xpos = dimension.xpos/2 * cos (theta) + center.xpos;
      float ypos = dimension.ypos/2 * sin (theta) + center.ypos;
      glVertex2f (xpos, ypos);
   }
   glEnd();
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void polygon::draw(const vertex& center, const rgbcolor& color) const {

   glBegin (GL_POLYGON);
   glColor3ubv (color.ubvec);
   GLfloat xnum,ynum;
   GLfloat xaver,yaver;
   int numbernum = 0;
   for (unsigned int num = 0; num < vertices.size(); num++) {
      xaver += vertices[num].xpos;
      yaver += vertices[num].ypos;
      ++numbernum;
   }

   xaver = xaver / numbernum;
   yaver = yaver / numbernum;
   glBegin(GL_POLYGON);
   glPolygonMode(GL_BACK, GL_LINE);
   for (unsigned int num = 0; num < vertices.size(); num++) {
      xnum = (vertices[num].xpos - xaver) + center.xpos;
      ynum = (vertices[num].ypos - yaver) + center.ypos;
      glVertex2f(xnum, ynum);
   }
   glEnd();
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void ellipse::draw_border (const vertex& center, const rgbcolor& color,
                           const GLfloat& width,
                           const size_t& number) const {
   glLineWidth(width);
   center_number(center,color,number);
   glBegin (GL_LINE_LOOP);
   glColor3ubv(color.ubvec);
   const float delta = 2 * M_PI / 32;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float xpos = dimension.xpos/2 * cos (theta) + center.xpos;
      float ypos = dimension.ypos/2 * sin (theta) + center.ypos;
      glVertex2f (xpos, ypos);
   }
   glEnd();
   DEBUGF ('d', this << "(" << center << "," << color << ")");

}

void polygon::draw_border (const vertex& center, const rgbcolor& color,
                           const GLfloat& width,
                           const size_t& number) const {
   glLineWidth(width);
   glBegin (GL_LINE_LOOP);
   glBegin (GL_POLYGON);
   glColor3ubv (color.ubvec);
   GLfloat xnum,ynum;
   GLfloat xaver,yaver;
   int numbernum = 0;
   for (unsigned int num = 0; num < vertices.size(); num++) {
      xaver += vertices[num].xpos;
      yaver += vertices[num].ypos;
      ++numbernum;
   }

   xaver = xaver / numbernum;
   yaver = yaver / numbernum;

      glBegin(GL_POLYGON);
      glPolygonMode(GL_BACK, GL_LINE);

      for (unsigned int num = 0; num < vertices.size(); num++) {
         xnum = (vertices[num].xpos - xaver) + center.xpos;
         ynum = (vertices[num].ypos - yaver) + center.ypos;
         glVertex2f(xnum, ynum);
      }
      glEnd();
   vertex centernum = {center.xpos,center.ypos};
   center_number(centernum,color,number);
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}
void center_number(const vertex& center, const rgbcolor& color,
                   const size_t& num){
    glColor3ubv (color.ubvec);
    auto ubytes = reinterpret_cast<const GLubyte*>
    (to_string(num).c_str());
    void* font = GLUT_BITMAP_HELVETICA_10;
    glRasterPos2f (center.xpos, center.ypos);
    glutBitmapString (font, ubytes);
    glEnd();
}

void text::draw_border (const vertex& center, const rgbcolor& color,
                        const GLfloat& width,
                        const size_t& number) const {
   glLineWidth(width);
   glBegin (GL_LINE_LOOP);
   glColor3ubv (color.ubvec);
   auto font = reinterpret_cast<const GLubyte*> (textdata.c_str());
   GLfloat length = glutBitmapLength (glut_bitmap_font, font);
   GLfloat height = glutBitmapHeight (glut_bitmap_font);
   glVertex2f (center.xpos - width,
               center.ypos - width);
   glVertex2f (center.xpos - width,
               center.ypos + height + width);
   glVertex2f (center.xpos + length + width,
               center.ypos + height + width);
   glVertex2f (center.xpos + length + width,
               center.ypos - width);
   glEnd();
   vertex num = {(center.xpos + (length /2)),
                     (center.ypos + (height /2))};
   center_number(num,color,number);
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void shape::show (ostream& out) const {
    out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
    shape::show (out);
    out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
    << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
    shape::show (out);
    out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
    shape::show (out);
    out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
    obj.show (out);
    return out;
}
