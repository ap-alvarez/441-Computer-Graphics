#include <iostream>
#include <string>

class Vector3 {
public:
    float x;
    float y;
    float z;

    // Constructor
    Vector3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {
      std::cout << "Construct Vector3" << std::endl;
    }

    // Default constructor
    Vector3() : x(0), y(0), z(0) {
      std::cout << "Construct Vector3" << std::endl;
    }


    // Destructor - called when an object goes out of scope or is destroyed
    ~Vector3() {
        std::cout << "Deconstruct Vector3" << std::endl;
      }
};

Vector3 add(const Vector3& v, const Vector3& v2) {

  Vector3 v3(v.x + v2.x, v.y + v2.y, v.z + v2.z);

  return v3;

}

Vector3 operator+(const Vector3& v, const Vector3& v2) {
  return add(v, v2);
}



std::ostream& operator<<(std::ostream& stream, const Vector3& v) {
    // std::cout is a std::ostream object, just like stream
    // so use stream as if it were cout and output the components of
    // the vector

    stream << v.x << "," << v.y << "," << v.z;
    return stream;
}

int main(int argc, char** argv) {

  //Prompt the user for their name and say hello
  std::cout << "Enter Name:" << std::endl;
  std::string name;
  std::cin >> name;
  std::cout << "Hello " << name << "!" << std::endl;

  //Print out all provided command-line arguments to the terminal
  std::cout << "Command-Line Arguments: " << std::endl;
  for (int i=0;i<argc;i++)
    std::cout << i << ": " << argv[i] << std::endl;

  //Add two vectors and print the result
  Vector3 a(1,2,3);   // allocated to the stack
  Vector3 b(4,5,6);

  Vector3 c = add(a,b);
  std::cout << "Vector3 Add: " << c.x << "," << c.y << "," << c.z << std::endl;

  //Repeat but with operator overloading
  std::cout << "Overloaded Operator +: " << a+b << std::endl;

  //Allocate a Vector3 on the stack initialized to (0,0,0)
  Vector3 v(0,0,0);
  //Set the Y component to 5
  v.y = 5;
  //Output vector to terminal with overloaded << operator
  std::cout << "In stack: " << v << std::endl;

  //Repeat with Vector3 in heap
  Vector3* vp = new Vector3(0,0,0);
  vp->y = 5;
  std::cout << "In heap: " << *vp << std::endl;

  delete vp;

  //Allocate an array of 10 Vector3 objects on the stack
  Vector3 array[10];

  //Allocate an array of 10 Vector3 objects on the heap
  Vector3* vectArray = new Vector3[10];
  //Set the Y component for all objects in heap to 5
  for (int i=0;i<10;i++) {
    vectArray[i].y = 5;
  }
  //Print out all Vector3 objects in heap
  std::cout << "Vector3 Objects in heap: " << std::endl;
  for (int i=0;i<10;i++) {
    std::cout << vectArray[i] << std::endl;
  }

  delete [] vectArray;


}
