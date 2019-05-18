#define OLC_PGE_APPLICATION
#include "olc/olcPixelGameEngine.h"
#include <unistd.h>
#include <math.h>
//#include <png++/png.hpp>

#define SMALL_NUM   0.00000001 // anything that avoids division overflow
// dot product (3D) which allows vector operations in arguments
#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)

struct vec2d {
  double x,y;
  vec2d(){}
  vec2d(int cons){x,y = cons;}
  vec2d(double x, double y) : x(x), y(y) {}
  vec2d operator + (const vec2d& v) const { return vec2d(x+v.x, y+v.y); }
  vec2d operator - (const vec2d& v) const { return vec2d(x-v.x, y-v.y); }
  vec2d operator * (double d) const { return vec2d(x*d, y*d); }
  vec2d operator * (vec2d rhs) const { return vec2d(x*rhs.x, y*rhs.y); }
  vec2d operator / (double d) const { return vec2d(x/d, y/d); }
  vec2d normalize() const {
    double mg = sqrt(x*x + y*y);
    return vec2d(x/mg,y/mg);
  }
  double sum(){return x+y;}
};
struct map {
    int layout[10][10] = {
        {1,1,0,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,0,0,1,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,1,1,0,1,1},
        {1,0,0,0,0,1,0,0,0,1},
        {1,0,0,0,0,1,2,1,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1},
    };
    map(){}
};
/*
struct texture{
    int width, height;
    png::image<png::rgba_pixel> img;
    texture(char *filename) {
        img = png::image<png::rgba_pixel>(filename);
        width = img.get_width();
        height = img.get_height();
    }
};
*/
struct intersection{
    float dist;
    vec2d gridpos;
    vec2d relative_pos;
    vec2d absolute_pos;
    intersection(float indist, vec2d ingridpos, vec2d abspos){
        dist = indist;
        gridpos = ingridpos;
        absolute_pos = abspos;
        relative_pos = abspos-gridpos;
    }
    intersection(int errorint){if(errorint == 0)dist=0;}
};
struct ray {
	float d; // Direction Angle
    vec2d o; // Origin Point
    ray(vec2d io, float id){
        o = io;
        d = id;
    }
    vec2d getpoint(float distance){
        return(vec2d( (distance*cos(d))+o.x, (distance*sin(d))+o.y ));
    }
    intersection findIntersect(map layout, vec2d playerpos, float playerdir){
        float i = 1;
        bool intersect = false;
        while(!intersect){
            vec2d point = getpoint(i);
            int x = ceil(point.x);
            int y = ceil(point.y);
            if(x > 9 || x < 0 || y > 9 || y < 0) return 0;
            if(layout.layout[x][y] == 1){
                intersect = true;
                vec2d delta = point - playerpos;
                float p = (delta.x*cos(playerdir)) + (delta.y*sin(playerdir));
                return(intersection(p, vec2d(x,y), point));
            }
            else if(i > 50) return 0;
            else i += (i/64);
            //if(i < 0.1) i == 10;
        }
    }
};

class CPcastr3D : public olc::PixelGameEngine {
public:
	CPcastr3D(){sAppName = "Raycasting Demo";}
public:
	float fov = 0.4;
    float ang_per_pixel = fov / ScreenWidth();
    map layout; // Initialize Map
    vec2d playerpos = vec2d(3,3);
    float playerdir = 0;
    int pitch = 0;
    int32_t mousex = GetMouseX();
    int32_t mousey = GetMouseY();

    // Load Textures
    //char* brickpath = (char*)"~/C++ Things/raycasting/brick.png";
    //texture brick = texture((char*)"raycasting/brick.png");


	bool OnUserCreate() override
	{
		printf("Started Ray Caster\n");/*  */
        return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
        // Rendering The Frame
        FillRect(0,0,ScreenWidth(), (ScreenHeight()/2)-pitch, olc::Pixel(25, 10, 10)); // ceiling
        FillRect(0,(ScreenHeight()/2)-pitch,ScreenWidth(), ScreenHeight(), olc::Pixel(10, 30, 0)); // floor
		for (int x = 0; x < ScreenWidth(); x++){
            ray pixlray = ray(playerpos, playerdir+(ang_per_pixel*x) - 0.5);
            intersection intersector = pixlray.findIntersect(layout, playerpos, playerdir);
            if(intersector.dist > 0){
                int persp_h = (ScreenHeight()/2) / intersector.dist;
                /*
                int tex_pixel_h = floor(brick.height/(persp_h*2));
                for(int i=0;i<persp_h*2;i++){
                    png::rgba_pixel currentpix;
                    if(intersector.relative_pos.x > 0 && intersector.relative_pos.y < 0.5)
                        currentpix = brick.img.get_pixel(floor(abs(intersector.relative_pos.x * brick.width)), tex_pixel_h*i);
                    else {
                        currentpix = brick.img.get_pixel(floor(abs(intersector.relative_pos.y * brick.width)), tex_pixel_h*i);
                    }
                    Draw(x, (i+ScreenHeight()/2-persp_h)-pitch, olc::Pixel(currentpix.red, currentpix.green, currentpix.blue, currentpix.alpha));
                }*/
                
                DrawLine(x,(ScreenHeight()/2-persp_h)-pitch, x, (ScreenHeight()/2+persp_h)-pitch, olc::Pixel(1/intersector.dist,100/intersector.dist,255/intersector.dist));
            }
        }
		// User Control
        olc::HWButton LEFT = GetKey(olc::Key::A);
        olc::HWButton RIGHT = GetKey(olc::Key::D);
        olc::HWButton UP = GetKey(olc::Key::W);
        olc::HWButton DOWN = GetKey(olc::Key::S);
        olc::HWButton C = GetKey(olc::Key::C);
        if(LEFT.bHeld){
            playerdir -= 0.05;
        }
        else if(RIGHT.bHeld){
            playerdir += 0.05;
        }
        if(UP.bHeld){
            playerpos.x += 0.05*cos(playerdir);
            playerpos.y += 0.05*sin(playerdir);
        }
        else if(DOWN.bHeld){
            playerpos.x -= 0.05*cos(playerdir);
            playerpos.y -= 0.05*sin(playerdir);
        }
        if(C.bPressed) pitch = 0;
        playerdir += 0.005*((GetMouseX()-mousex));
        pitch += (GetMouseY()-mousey);
        mousex = GetMouseX();
        mousey = GetMouseY();
		usleep(9600); // Give the processor some time to breathe
		return true;
	}
};


int main()
{
	CPcastr3D demo;
	if (demo.Construct(720, 480, 3, 3))
		demo.Start();

	return 0;
}
