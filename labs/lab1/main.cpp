#include <iostream>
#include <string>
#include <cmath>
#include "bitmap_image.hpp"

//Class for storing both position and color information for each point of the triangle
class Point{
public:
    float x;
    float y;
    float r;
    float g;
    float b;

    // Constructor
    Point(float xx, float yy, float rr, float gg, float bb) : x(xx), y(yy), r(rr), g(gg), b(bb) {
      //std::cout << "Construct Point" << std::endl;
    }

    // Default constructor
    Point() : x(0), y(0), r(0), g(0), b(0) {
      //std::cout << "Construct Default Point" << std::endl;
    }

    // Destructor - called when an object goes out of scope or is destroyed
    ~Point() {
        //std::cout << "Deconstruct Point" << std::endl;
      }

    void SetPoint(float xx, float yy, float rr, float gg, float bb) {
      x = xx;
      y = yy;
      r = rr;
      g = gg;
      b = bb;
    }
};

//2D Vector for cartesian coordinates
class Vector2 {
public:
  float x;
  float y;

  //Constructors
  Vector2(float xx, float yy) : x(xx), y(yy) {}
  Vector2() : x(0), y(0) {}

};

//3D Vector for colors and barycentric coordinates
class Vector3 {
public:
  float x;
  float y;
  float z;

  //Constructors
  Vector3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
  Vector3() : x(0), y(0), z(0) {}

  void Scale(float a) {
    x *= a;
    y *= a;
    z *= a;
  }

  void Add(Vector3 v) {
    x += v.x;
    y += v.y;
    z += v.z;
  }

};

//Line for calculating barycentric coordinates
class Line {
public:
  float A;
  float B;
  float C;

  //Constructor
  Line(Vector2 p, Vector2 q) {
    A = (q.y - p.y);
    B = (p.x - q.x);
    C = (A*p.x) + (B*p.y);
  }

  float Calc(Vector2 p) {
    return (A*p.x) + (B*p.y) - C;
  }
};

//Box for representing a bounding box with lower left and upper right coordinates
class Box {
public:
  Vector2 ll;
  Vector2 ur;
  //Constructors
  Box(Vector2* vectors) {
    ll.x = vectors[0].x;
    ll.y = vectors[0].y;
    ur.x = vectors[0].x;
    ur.y = vectors[0].y;

    for (int i = 1; i < 3; i++) {
      if (vectors[i].x < ll.x) {
        ll.x = vectors[i].x;
        std::cout << i << ": " << ll.x << std::endl;
      } else if (vectors[i].x > ur.x) {
        ur.x = vectors[i].x;
      }

      if (vectors[i].y < ll.y) {
        ll.y = vectors[i].y;
      } else if (vectors[i].y > ur.y) {
        ur.y = vectors[i].y;
      }
    }
  }
};

//For representing points (position + color) in the console
std::ostream& operator<<(std::ostream& stream, const Point& p) {
    // std::cout is a std::ostream object, just like stream
    // so use stream as if it were cout and output the components of
    // the vector

    stream << "Point: (" << p.x << "," << p.y << ") Color: (" << p.r << "," << p.g << "," << p.b << ")";
    return stream;
}

//Extracts position vector from point
Vector2 Pos(Point p) {
  Vector2 pos(p.x,p.y);
  return pos;
}

//Extracts color vector from point
Vector3 Color(Point p) {
  Vector3 col(p.r,p.g,p.b);
  return col;
}

//Calculates barycentric coordinate for 2D vector p
Vector3 BaryCoord(Line ab, Line ac, Vector2 b, Vector2 c, Vector2 p) {
  float beta = ac.Calc(p) / ac.Calc(b);
  float gamma = ab.Calc(p) / ab.Calc(c);
  float alpha = 1 - beta - gamma;
  return Vector3(beta, gamma, alpha);
}

//Checks whether the barycentric coordinate is within the triangle
bool CheckBaryCoord(Vector3 coord) {
  if (coord.x < 0 || coord.x > 1 || coord.y < 0 || coord.y > 1 || coord.z < 0 || coord.z > 1) {
    return false;
  }
  return true;
}

//Finds the color for the given barycentric coordinate between the three points provided
Vector3 FindColor(Point* points, Vector3 baryCoord) {
  Vector3 final = Vector3();
  Vector3 col = Color(points[0]);
  col.Scale(baryCoord.x);
  final.Add(col);
  col = Color(points[1]);
  col.Scale(baryCoord.y);
  final.Add(col);
  col = Color(points[2]);
  col.Scale(baryCoord.z);
  final.Add(col);
  return final;
}

//Translates [0,1] (float) to [0,255] (int)
int FloatToColor(float f) {
  return round(255*f);
}

//Draws the given bounding box
void DrawBox(Box box) {

  bitmap_image image(750,750);

  image.set_all_channels(0, 0, 0);

  image_drawer draw(image);

  draw.pen_width(1);
  draw.pen_color(255, 255, 255);
  std::cout << "(" << box.ll.x << "," << box.ll.y << "),(" << box.ur.x << ", " << box.ur.y << ")" << std::endl;
  for (int x = box.ll.x; x < box.ur.x; x++) {
    for (int y = box.ll.y; y < box.ur.y; y++) {
      draw.plot_pixel(x,y);
    }
  }
  image.save_image("../boundingBox.bmp");
}

//Draws a white triangle within the given bounding box
void DrawTriangleBlank(Vector2* points, Box boundingBox) {
  //Draw triangle using barycentric coordinates
  Line ab(points[0],points[1]);
  Line ac(points[0],points[2]);

  bitmap_image image(750,750);
  image.set_all_channels(0, 0, 0);

  image_drawer draw(image);

  draw.pen_width(1);
  draw.pen_color(255, 255, 255);
  for (int x = boundingBox.ll.x; x < boundingBox.ur.x; x++) {
    for (int y = boundingBox.ll.y; y < boundingBox.ur.y; y++) {
      Vector3 coord = BaryCoord(ab, ac, points[1], points[2], Vector2(x,y));
      if (CheckBaryCoord(coord)){
        draw.plot_pixel(x,y);
      }
    }
  }
  image.save_image("../triangleBlank.bmp");
}

//Draws a triangle within the given bounding box, colored using barycentric coordinates as weights
void DrawTriangleColored(Point* points, Box boundingBox) {

  Line ab(Pos(points[0]),Pos(points[1]));
  Line ac(Pos(points[0]),Pos(points[2]));

  bitmap_image image(750,750);
  image.set_all_channels(0, 0, 0);

  image_drawer draw(image);

  draw.pen_width(1);
  for (int x = boundingBox.ll.x; x < boundingBox.ur.x; x++) {
    for (int y = boundingBox.ll.y; y < boundingBox.ur.y; y++) {
      Vector3 coord = BaryCoord(ab, ac, Pos(points[1]), Pos(points[2]), Vector2(x,y));
      if (CheckBaryCoord(coord)){
        Vector3 color = FindColor(points, coord);
        draw.pen_color(FloatToColor(color.x),FloatToColor(color.y),FloatToColor(color.z));
        draw.plot_pixel(x,y);
      }
    }
  }
  image.save_image("../triangleColored.bmp");
}

//Driver
int main(int argc, char** argv) {

  Point points[3];
  Vector2 pos[3];

  //Get triangle info
  std::cout << "Enter 3 points (enter a point as x y r g b):" << std::endl;
  for (int i = 0; i < 3; i++) {
    std::string x, y, r, g, b;
    std::cin >> x >> y >> r >> g >> b;
    points[i] = Point(std::stof(x),std::stof(y),std::stof(r),std::stof(g),std::stof(b));
    pos[i] = Vector2(std::stof(x),std::stof(y));
  }

  //Confirm triangle info
  std::cout << "You entered:" << std::endl;
  for (int i = 0; i < 3; i++) {
    std::cout << points[i] << std::endl;
  }

  //Find bounding box then draw it (ll = lower left, ur = upper right)
  Box boundingBox(pos);
  DrawBox(boundingBox);
  DrawTriangleBlank(pos, boundingBox);
  DrawTriangleColored(points, boundingBox);

  return 0;
}
